#ifndef GETROUTE_H
#define GETROUTE_H
#include<unordered_map>
#include<queue>
#include<algorithm>
#include<thread>
#include"user_requirement.h"
#include"init_mysql.h"
#include"graph.h"
#include"graph.cpp"
#include"vehicle.h"
//������Ҫ���� ������ȡ����·��
//TODO ��֪��Ҫ����д�ɸ��� Ȼ��ֱ�� ת��ʲô�ļ̳� ����ֱ��дһ��ͨ�÷��� ֧�ָ��ַ�ʽ
using std::unordered_map;
using std::priority_queue;
using std::thread;
using std::make_heap;
using std::pop_heap;
using std::push_heap;
using std::sort_heap;
using std::sort;
using std::to_string;

namespace GetRouteNameSpace
{
	/*
		����ͼʱ��״̬ö��
	*/
	enum class CreateGraphStatue
	{
		CREATE_GRAPH_FAILED,
		CREATE_GRAPH_SUCCEED
	};
	/*
		��ȡ·�ߵ�״̬ö��
	*/
	enum class GetRouteStatue
	{
		
	};
	/*
		��ȡ��ͨ���ߵ�״̬ö��
	*/
	enum class GetVehicleStatue
	{
		GET_VEHICLE_FAILED,
		GET_VEHICLE_SUCCEED,
		GET_DIRECT_VEHICLE_FAILED,
		GET_TRANSIT_VEHICLE_FAILED,
		GET_FIX_VEHICLE_FAILED,
		GET_ALL_TRANSIT_VEHICLE_FAILED
	};
	/*
		��ȡֱ�ｻͨ���ߵ�״̬ö��
	*/
	enum class GetDirectVehicleStatue
	{
		GET_DIRECT_VEHICLE_FAILED,
		GET_DIRECT_VEHICLE_SUCCEED,
		SELECT_RESULT_EMPTY
	};
	/*
		��ȡ��ת��ͨ���ߵ�״̬ö��
	*/
	enum class GetTransitVehicleStatue
	{
		GET_TRANSIT_VEHICLE_FAILED,
		GET_TRANSIT_VEHICLE_SUCCEED,
		SELECT_RESULT_NO_FIRST_ROUTE,
		SELECT_RESULT_NO_SECOND_ROUTE,
		GET_ONE_SECOND_ROUTE_INFOR_SUCCEED,
		GET_TRANSIT_VEHICLE__GET_FAILED_SECOND_ROUTE_ERROR,
		SELECT_RESULT_NO_TABLE_EXIST
	};
	/*
		��ȡ��Ͻ�ͨ���ߵ�״̬ö��
	*/
	enum class GetFixVehicleStatue
	{
		GET_FIX_VEHICLE_FAILED,
		GET_FIX_VEHICLE_SUCCEED,
		SELECT_RESULT_NO_FIRST_ROUTE,
		SELECT_RESULT_NO_SECOND_ROUTE,
		GET_ONE_SECOND_ROUTE_INFOR_SUCCEED,
		GET_TRANSIT_VEHICLE__GET_FAILED_SECOND_ROUTE_ERROR
	};
	/*
		��ȡ������ת��ͨ���ߵ�״̬ö��
	*/
	enum class GetAllTransitVehicleStatue
	{
		GET_ALL_TRANSIT_VEHICLE_FAILED,
		GET_ALL_TRANSIT_VEHICLE_SUCCEED,
		DIRECT_EMPTY,
		TRANSIT_EMPTY
	};

	auto getTimeOfVehicleOneRoute = [](vector<Vehicle*> route)->int
	{
		int size = route.size();
		string type;
		//ֱ��
		if (size == 1)
		{
			type = route[0]->getVehicleType();
			return MyTime::costTime(MyTime::stringToMyTime(route[0]->get_start_time(), HH_MM), MyTime::stringToMyTime(route[0]->get_arrival_time(), HH_MM));
		}
		//��ת
		int res = 0;
		for (int i = 0; i < size - 1; i++)
		{
			type = route[i]->getVehicleType();
			res += MyTime::costTime(MyTime::stringToMyTime(route[i]->get_start_time(), HH_MM), MyTime::stringToMyTime(route[i]->get_arrival_time(), HH_MM)) +
				MyTime::costTime(MyTime::stringToMyTime(route[i]->get_arrival_time(), HH_MM), MyTime::stringToMyTime(route[i + 1]->get_start_time(), HH_MM)) +
				MyTime::costTime(MyTime::stringToMyTime(route[i + 1]->get_start_time(), HH_MM), MyTime::stringToMyTime(route[i + 1]->get_arrival_time(), HH_MM));
		}
		return res;
	};

	auto getPriceOfVehicleOneRoute = [](vector<Vehicle*> route)->float
	{
		float res = 0;
		string type = NULL;
		for (int i = 0; i < route.size(); i++)
		{
			type = route[i]->getVehicleType();

			if (type == HSRC_TYPE)
			{
				res += atof(route[i]->get_second_class_seat_price().c_str());
			}

			if (type == AIRPLANE_TYPE)
			{
				res += atof(route[i]->get_ticket_price().c_str());
			}
		}

		return res;
	};

	//ͬ�����͵�lambda���ʽ ���������ȶ��� ʱ������ return false˵�����ǰ���˳�� �Ƚ���ĵ�һ����
	auto sameTypeVehicleGreater_TimeFirst = [](vector<Vehicle*> route1,vector<Vehicle*> route2)-> bool
	{
		return true;
	};

	//ͬ�ַ�ʱ�����ȾͰ�����˳������ 
	auto sameTypeVehicleGreater=[](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		return true;
	};

	//��ͬ��ʱ������ ������ all_vehicle
	auto differentTypeVehicleGreater_TimeFirst = [](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		float time1 = getTimeOfVehicleOneRoute(route1);
		float time2 = getTimeOfVehicleOneRoute(route2);

		float price1 = getPriceOfVehicleOneRoute(route1);
		float price2 = getPriceOfVehicleOneRoute(route2);
		//ʱ���ĳ���ʱ��С�� ���Ǳ��� ʱ��С�ĳ��ϱ��� �Ƚ�ƽ��һ��
		float times = price1 > price2 ? (price1 / price2) : (price2 / price1);//����

		time1 > time2 ? time2 *= times : time1 *= times;

		if (time1 < time2)
			return true;
		return false;
	};
	//������ all_vehicle
	auto differentTypeVehicleGreater = [](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		float time1 = getTimeOfVehicleOneRoute(route1);
		float time2 = getTimeOfVehicleOneRoute(route2);

		float price1 = getPriceOfVehicleOneRoute(route1);
		float price2 = getPriceOfVehicleOneRoute(route2);

		float times_of_price= price1 > price2 ? (price1 / price2) : (price2 / price1);//��Ǯ����
		float times_of_time= time1 > time2 ? (time1 / time2) : (time2 / time1);//ʱ�䱶��

		if (times_of_price < (times_of_time-0.1))//��Ǯ������ʱ�䱶��-0.1��˵��̫����
			return false;
		return true;
	};
}
/*
	GetRoute ��ȡ·�� ʵ���Ͼ��ǻ�ȡ�������м�Ľ�ͨ����
* requirement:Ԥ����������
*/
class GetRoute
{
private:
	UserRequirementAfterPretreat requirement;
	Graph<string, vector<vector<Vehicle*>>> graph;
	vector<VertexData<string>> vertex_datas;
	vector<pair<VertexData<string>, VertexData<string>>> edges;
	vector<vector<vector<Vehicle*>>> weights;
public:
	GetRoute(UserRequirement requirement)
	{
		//TODO:������װdllʱ���԰�UserRequirement requirementŪ��һ��ȫ�ֱ���
		this->requirement = requirement.pretreatUserRequirement();

		int cities_num = this->requirement.start_cities.size();
		vertex_datas = vector<string>(cities_num + 1);
		edges = vector<pair<VertexData<string>, VertexData<string>>>(cities_num);
		weights.resize(cities_num);
	}

	GetRoute(UserRequirementAfterPretreat requirement)
	{
		//TODO:������װdllʱ���԰�UserRequirement requirementŪ��һ��ȫ�ֱ���
		this->requirement = requirement;
		int cities_num = requirement.start_cities.size();
		vertex_datas = vector<string>(cities_num + 1);
		edges = vector<pair<VertexData<string>, VertexData<string>>>(cities_num);
		weights.resize(cities_num);
	}
	~GetRoute()
	{

	}

	/*
		����ͼ���Ұѱ�����
	* 
	*/
	GetRouteNameSpace::CreateGraphStatue createGraph();

	/*
		��weight����
	* 
	*/
	bool sortWeights();

	/*
		getVechileInfor ͨ��Ԥ�����������ȡ��ͨ����(Ҳ���ǻ�ȡ·��)
	* 
	*/
	GetRouteNameSpace::GetVehicleStatue getVechileInfor();

	//�Ҹо�����ת���ʹ����־ͺ�����ʵ ������ʲô��ͨ���ߡ��۸�ʱ����Щ�Ĳ����������Ƶ�

	/*
		��ȡֱ��Ľ�ͨ������Ϣ
	* sql_query:Ҫִ�е�sql�������
	*/
	GetRouteNameSpace::GetDirectVehicleStatue getDirectVehicleInfor(int now_index, vector<string> sql_query);

	/*
		��ȡ��ת�Ľ�ͨ������Ϣ
	* sql_query:Ҫִ�е�sql�������
	*/
	GetRouteNameSpace::GetTransitVehicleStatue getTransitVehicleInfor(int now_index, vector<string> sql_query);

	/*
		��ȡ��ϵĽ�ͨ������Ϣ
	* sql_query:Ҫִ�е�sql�������
	*/
	GetRouteNameSpace::GetFixVehicleStatue getFixVehicleInfor(int now_index, vector<string> sql_query);

	/*
		��ȡת���ĺ��� ���Ұѷ���������д��weights
		FIX_TRANSIT��TRANSITͨ��(��ȡsqlʱ��ͬ)
	* first_route:��һ��·��ͬarrival_station�ļ���
	*/
	GetRouteNameSpace::GetTransitVehicleStatue getTransitVehicleInforSecondRoute(int now_index, vector<Vehicle*> first_route, vector<vector<Vehicle*>>& temp_weights);

	/*
		��ȡ������ת�Ľ�ͨ������Ϣ
	* 
	*/
	GetRouteNameSpace::GetAllTransitVehicleStatue getAllTransitVehicleInfor(int now_index);

	/*
		����requirement��������ϳ�where
	* now_index:requirement�� ���� start_cities ������ ��ô��Ҫ���뵱ǰ����������������±� ���ܻ�õ�ǰ�ĳ���
	* second_route:�Ƿ�����ת�ĵڶ���·�������� Ĭ��Ϊfalse ���������Ƿ�Ҫʹ��arrival_time
	*/
	unordered_map<string, string> getWhereSentenceKeyValue(int now_index);

	/*
		����vehicle��������ϳ�where(����ת���ĺ���) ���ҷ���������д��weights
	* 
	*/
	unordered_map<string, string> getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index,Vehicle* vehicle);

	/*
		����vehicle��������ϳ�where(����ת���ĺ���) ���ҷ���������д��weights
		������ all_transit+all_vehicle��ʱ��
	*
	*/
	unordered_map<string, string> getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index, Vehicle* vehicle,UserRequirementNamespace::VehicleTypeEnum vehicle_type);

	/*
		��ȡtable_name(����start_city)
	* now_index:��õ�ǰ���л��н�ͨ��������
	*/
	string getTableName(int now_index);

	/*
		��ȡtable_name(���ݴ�����ַ���)
	* now_index:��ǰ�±�
	* city_name:������
	*/
	string getTableName(string city_name, UserRequirementNamespace::VehicleTypeEnum vehicle);

	/*
		ͨ��requirement����ú��ʵ�ORDER���
	* now_index:��õ�ǰ���л��н�ͨ��������
	*/
	string getOrderSentence(int now_index);

	/*
		������all__transit��all_vehicleʱ��ȡorder��Ϣ
	* 
	*/
	string getOrderSentence(int now_index, UserRequirementNamespace::VehicleTypeEnum);

	/**
		���������ϳ�SQL���
	* now_index:��ǰ�±�(�ô��ж�arrive_city��=����!=)
	* columns:�� Ĭ���� * columns.size()==0��ΪĬ��
	* table_name:����
	* where_sentence:where��� ��һ��hash��ֵ�� ��Ϊ���ڸ���Ŀ where�ľ���ôЩ���� ����õ� �ͼ��Ͼ����������û�õ� ���ں���� is not null�ͺ���
	*	Tip:
	*		���ڻ���һЩʲôorder ���߸���ת����ת���жϵ��ڻ��ǲ����� ֱ�ӷ���requirement��Ա���жϾͺ���
	*/
	string getSQLQuery(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence);

	/*
		���ת���ĵڶ���·�� ���� ����-�Ͼ����Էֽ�� ����-����(getSQLQuery) Ȼ��÷������ ����-���� ��getTransitVehicleInfor��ʹ�� 
		��Ϊ��Ҫ�ȶ�ǰ�����SQL��ѯ ����֪�����ε���ʼ����
	* 
	*/
	string getSQLQuerySecondRouteOfTrans(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence);

	/*
		���ת���ĵڶ���·�� ������ all_transit+all_vehicle��ʱ��
	*
	*/
	string getSQLQuerySecondRouteOfTrans(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence,UserRequirementNamespace::VehicleTypeEnum vehicle_type);

	/*
		��ȡSQL������ݴ���getXxxxVehicleInfor������
	* now_index:��ǰ�±�
	*/
	vector<string> getSQLQueryVector(int now_index);

	/*
		����weighs ��Ϊall_transitҪ�½��������� �Ӷ�Ҳ��������Ӧ��weights��д ��ֱ��getд�õ� Ȼ��ƴһ��ͺ���
	* 
	*/
	vector<vector<vector<Vehicle*>>> getWeights();

	/*
		��ȡ�������м�������� �Ӷ�ȷ��ת���ĵ�һ�ε���תվ ���Ҫ�����С
	* 
	*/
	string getMaxMileage(string start_city,string arrival_city);
};

#endif // !GETROUTE_H