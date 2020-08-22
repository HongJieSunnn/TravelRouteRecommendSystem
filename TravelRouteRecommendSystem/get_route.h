#ifndef GETROUTE_H
#define GETROUTE_H
#include<unordered_map>
#include<queue>
#include<algorithm>
#include"user_requirement.h"
#include"init_mysql.h"
#include"graph.h"
#include"vehicle.h"
//������Ҫ���� ������ȡ����·��
//TODO ��֪��Ҫ����д�ɸ��� Ȼ��ֱ�� ת��ʲô�ļ̳� ����ֱ��дһ��ͨ�÷��� ֧�ָ��ַ�ʽ
using std::unordered_map;
using std::priority_queue;
using std::make_heap;
using std::pop_heap;
using std::push_heap;
using std::sort_heap;

namespace GetRouteNameSpace
{
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
		GET_VEHICLE_SUCCEED
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
		GET_TRANSIT_VEHICLE__GET_FAILED_SECOND_ROUTE_ERROR
	};
	/*
		��ȡ��Ͻ�ͨ���ߵ�״̬ö��
	*/
	enum class GetFixVehicleStatue
	{
		GET_FIX_VEHICLE_FAILED,
		GET_FIX_VEHICLE_SUCCEED
	};
	/*
		��ȡ������ת��ͨ���ߵ�״̬ö��
	*/
	enum class GetAllTransitVehicleStatue
	{

	};
	//ͬ�����͵�lambda���ʽ ���������ȶ��� ʱ������ return false˵�����ǰ���˳�� �Ƚ���ĵ�һ����
	auto sameTypeVehicleGreater_TimeFirst = [](vector<Vehicle*> route1,vector<Vehicle*> route2)-> bool
	{
		return false;
	};

	//ͬ�ַ�ʱ�����ȾͰ�����˳������ 
	auto sameTypeVehicleGreater=[](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		return false;
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

		if (time1 > time2)
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

		if (times_of_price > (times_of_time-0.1))//��Ǯ������ʱ�䱶��-0.1��˵��̫����
			return false;
		return true;
	};

	auto getTimeOfVehicleOneRoute = [](vector<Vehicle*> route)->int
	{
		int size = route.size();
		string type = NULL;
		//ֱ��
		if (size == 1)
		{
			type = route[0]->getVehicleType();
			if (type == HSRC_TYPE)
			{
				return MyTime::costTime(MyTime::stringToMyTime(route[0]->getHSRC().start_time, HH_MM), MyTime::stringToMyTime(route[0]->getHSRC().arrival_time, HH_MM));
			}

			if (type == AIRPLANE_TYPE)
			{
				return MyTime::costTime(MyTime::stringToMyTime(route[0]->getAP().start_time, HH_MM), MyTime::stringToMyTime(route[0]->getAP().arrival_time, HH_MM));
			}
		}
		//��ת
		int res = 0;
		for (int i = 0; i < size-1; i++)
		{
			type = route[i]->getVehicleType();
			if (type == HSRC_TYPE)
			{
				res+=MyTime::costTime(MyTime::stringToMyTime(route[i]->getHSRC().start_time, HH_MM), MyTime::stringToMyTime(route[i]->getHSRC().arrival_time, HH_MM)) +
					MyTime::costTime(MyTime::stringToMyTime(route[i]->getHSRC().arrival_time, HH_MM), MyTime::stringToMyTime(route[i + 1]->getHSRC().start_time, HH_MM)) +
					MyTime::costTime(MyTime::stringToMyTime(route[i + 1]->getHSRC().start_time, HH_MM), MyTime::stringToMyTime(route[i + 1]->getHSRC().arrival_time, HH_MM));
			}

			if (type == AIRPLANE_TYPE)
			{
				res+=MyTime::costTime(MyTime::stringToMyTime(route[i]->getAP().start_time, HH_MM), MyTime::stringToMyTime(route[i]->getAP().arrival_time, HH_MM)) +
					MyTime::costTime(MyTime::stringToMyTime(route[i]->getAP().arrival_time, HH_MM), MyTime::stringToMyTime(route[i + 1]->getAP().start_time, HH_MM)) +
					MyTime::costTime(MyTime::stringToMyTime(route[i + 1]->getAP().start_time, HH_MM), MyTime::stringToMyTime(route[i + 1]->getAP().arrival_time, HH_MM));
			}
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
				res += atof(route[i]->getHSRC().second_class_seat_price.c_str());
			}

			if (type == AIRPLANE_TYPE)
			{
				res += atof(route[i]->getAP().ticket_price.c_str());
			}
		}

		return res;
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
	Graph<string> graph;
	vector<VertexData<string>> vertex_datas;
	vector<pair<VertexData<string>, VertexData<string>>> edges;
	vector<vector<Vehicle*>> weights;
public:
	GetRoute(UserRequirement requirement)
	{
		//TODO:������װdllʱ���԰�UserRequirement requirementŪ��һ��ȫ�ֱ���
		this->requirement = requirement.pretreatUserRequirement();
		
	}

	GetRoute(UserRequirementAfterPretreat requirement)
	{
		//TODO:������װdllʱ���԰�UserRequirement requirementŪ��һ��ȫ�ֱ���
		this->requirement = requirement;
	}
	~GetRoute()
	{

	}

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
		��ȡת���ĺ��� ���Ұѷ���������д��weights
	* 
	*/
	GetRouteNameSpace::GetTransitVehicleStatue getTransitVehicleInforSecondRoute(int now_index, string start_city_name, Vehicle* first_route);

	/*
		��ȡ��ϵĽ�ͨ������Ϣ
	* sql_query:Ҫִ�е�sql�������
	*/
	GetRouteNameSpace::GetFixVehicleStatue getFixVehicleInfor(int now_index, vector<string> sql_query);

	/*
		��ȡ������ת�Ľ�ͨ������Ϣ
	* sql_query:Ҫִ�е�sql�������
	*/
	GetRouteNameSpace::GetFixVehicleStatue getAllTransitVehicleInfor(int now_index, vector<string> sql_query);

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
		��ȡtable_name(����start_city)
	* now_index:��õ�ǰ���л��н�ͨ��������
	*/
	string getTableName(int now_index);

	/*
		��ȡtable_name(���ݴ�����ַ���)
	* now_index:��ǰ�±�
	* city_name:������
	*/
	string getTableName(int now_index,string city_name);

	/*
		ͨ��requirement����ú��ʵ�ORDER���
	* now_index:��õ�ǰ���л��н�ͨ��������
	*/
	string getOrderSentence(int now_index);

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
		��ȡSQL������ݴ���getXxxxVehicleInfor������
	* now_index:��ǰ�±�
	*/
	vector<string> getSQLQueryVector(int now_index);
};

#endif // !GETROUTE_H