#ifndef GETROUTE_H
#define GETROUTE_H
#include<unordered_map>
#include<queue>
#include<algorithm>
#include<thread>
#include<mutex>
#include<map>
#include"user_requirement.h"
#include"init_mysql.h"
#include"vehicle.h"
#include"init_redis.h"
#include"route.h"
//������Ҫ���� ������ȡ����·��
//TODO ��֪��Ҫ����д�ɸ��� Ȼ��ֱ�� ת��ʲô�ļ̳� ����ֱ��дһ��ͨ�÷��� ֧�ָ��ַ�ʽ
using std::unordered_map;
using std::priority_queue;
using std::thread;
using std::map;
using std::ref;
using std::make_heap;
using std::pop_heap;
using std::push_heap;
using std::sort_heap;
using std::sort;
using std::to_string;

namespace GetRouteNameSpace
{
	enum class GetRouteResultStatue
	{
		GET_ROUTE_RESULT_FAILED,
		GET_ROUTE_RESULT_SUCCEED,
		SORT_WEIGHTS_FAILED
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
		SELECT_RESULT_EMPTY,
		GET_RES_FAILED
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
		SELECT_RESULT_NO_TABLE_EXIST,
		GET_RES_FAILED,
		GOT_THIRDTY_WEIGHTS
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
		GET_TRANSIT_VEHICLE__GET_FAILED_SECOND_ROUTE_ERROR,
		GET_RES_FAILED
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

	auto getTimeOfVehicleOneRoute = [](vector<Vehicle*> route)->double
	{
		auto size = route.size();
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

	auto getPriceOfVehicleOneRoute = [](vector<Vehicle*> route)->double
	{
		double res = 0;
		string type = "";
		for (int i = 0; i < route.size(); i++)
		{
			type = route[i]->getVehicleType();

			if (type == HSRC_TYPE)
			{
				res += atof(route[i]->get_second_class_seat_price().c_str());
			}

			if (type == AIRPLANE_TYPE)
			{
				string ticket_price = route[i]->get_ticket_price().substr(1);
				auto index_of_QI = ticket_price.find("��");
				ticket_price = ticket_price.substr(0, index_of_QI);
				string discount_rate_str = route[i]->get_discount().substr(6,3);
				double discount_rate = atof(discount_rate_str.c_str());
				res += atof(ticket_price.c_str())/ (discount_rate/10);
			}
		}

		return res;
	};

	//��ͬ��ʱ������ ������ all_vehicle
	auto differentTypeVehicleGreater_TimeFirst = [](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		double time1 = getTimeOfVehicleOneRoute(route1);
		double time2 = getTimeOfVehicleOneRoute(route2);

		double price1 = getPriceOfVehicleOneRoute(route1);
		double price2 = getPriceOfVehicleOneRoute(route2);
		double times = price1 / price2;//����
		if (time1 < time2)//���ʱ��1С��ʱ��2 ��ô1Ϊ�ɻ�2Ϊ�� ��ô�����Ƚϵ�ʱ��ΪС��ʱ��˱����ٳ�2(��2���ҹ涨��) ��������С��ʱ���4��
		{
			time1 *= times * 2;
		}
		else if (time1 > time2)//���ʱ��1����ʱ��2 ˵��1Ϊ�� ��ô��ʱҪ���Ա����������ܱȽ� ���ǳ�����Ϊ������ʱ��С��1 �������ô��ʱ��С4��
		{
			time1 *= (times / 2);
		}

		int start_time_hour1 = MyTime::stringToMyTime(route1[0]->get_start_time(), HH_MM).hour;
		int start_time_hour2 = MyTime::stringToMyTime(route2[0]->get_start_time(), HH_MM).hour;
		if (start_time_hour1 >= 7 && start_time_hour1 <= 16)//�������ʱ����8�㵽16��֮�� ���������ȽϺ� ��ô���ǰ�time1��ȥ��������֮һ �Ӷ���ø��ߵ�Ȩ ���time2Ҳ��ȥ�������֮һ ��ô��С��Ȼ��û��һ��
		{
			time1 -= time1 / 5;
		}
		if (start_time_hour2 >= 7 && start_time_hour2 <= 16)
		{
			time2 -= time2 / 5;
		}

		if (time1 < time2)
			return true;
		return false;
	};
	//������ all_vehicle include fix
	auto differentTypeVehicleGreater = [](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		double time1 = getTimeOfVehicleOneRoute(route1);
		double time2 = getTimeOfVehicleOneRoute(route2);

		int start_time_hour1 = MyTime::stringToMyTime(route1[0]->get_start_time(), HH_MM).hour;
		int start_time_hour2 = MyTime::stringToMyTime(route2[0]->get_start_time(), HH_MM).hour;
		if (start_time_hour1 >= 7 && start_time_hour1 <= 16)//�������ʱ����8�㵽16��֮�� ���������ȽϺ� ��ô���ǰ�time1��ȥ��������֮һ �Ӷ���ø��ߵ�Ȩ ���time2Ҳ��ȥ�������֮һ ��ô��С��Ȼ��û��һ��
		{
			time1 -= time1 / 5;
		}
		if (start_time_hour2 >= 7 && start_time_hour2 <= 16)
		{
			time2 -= time2 / 5;
		}

		double price1 = getPriceOfVehicleOneRoute(route1);
		double price2 = getPriceOfVehicleOneRoute(route2);

		return (time1 + price1) < (time2 + price2);//ֱ�ӱȽ�ʱ��+Ǯ ��Ȼ��������ɻ��ô��ۺ��Ǯ�ͺܲ�ƽ�� ���Էɻ���price�Ǵ���ǰ��price
	};
	//ͬ�����͵�lambda���ʽ ���������ȶ��� ʱ������ return false˵�����ǰ���˳�� �Ƚ���ĵ�һ����
	auto sameTypeVehicleGreater_TimeFirst = [](vector<Vehicle*> route1,vector<Vehicle*> route2)-> bool
	{
		double time1 = getTimeOfVehicleOneRoute(route1);
		double time2 = getTimeOfVehicleOneRoute(route2);
		int start_time_hour1 = MyTime::stringToMyTime(route1[0]->get_start_time(), HH_MM).hour;
		int start_time_hour2 = MyTime::stringToMyTime(route2[0]->get_start_time(), HH_MM).hour;
		if (start_time_hour1 >= 7 && start_time_hour1 <= 16)//�������ʱ����8�㵽16��֮�� ���������ȽϺ� ��ô���ǰ�time1��ȥ��������֮һ �Ӷ���ø��ߵ�Ȩ ���time2Ҳ��ȥ�������֮һ ��ô��С��Ȼ��û��һ��
		{
			time1 -= time1 / 5;
		}
		if (start_time_hour2 >= 7 && start_time_hour2 <= 16)
		{
			time2 -= time2 / 5;
		}

		return time1 < time2;
	};
	//ͬ�ַ�ʱ�����ȾͰ�����˳������ 
	auto sameTypeVehicleGreater=[](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		return differentTypeVehicleGreater(route1,route2);
	};
	//�ṹMapCmpOfFirstRouteDivideByStation ���������() Ŀ������Ϊmap�ıȽϺ���
	struct MapCmpOfFirstRouteDivideByStation
	{
		bool operator()(const string& key1, const string& key2) const
		{
			if (InitRedis::getPriorityLevel(key1) > InitRedis::getPriorityLevel(key2))
				return true;
			return false;
		}
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
	vector<vector<vector<Vehicle*>>> weights;
	std::mutex mu;
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
	*/
	unordered_map<string, string> getWhereSentenceKeyValue(int now_index);

	/*
		����vehicle��������ϳ�where(����ת���ĺ���) ���ҷ���������д��weights
		������ all_vehicle��ʱ��
	*
	*/
	unordered_map<string, string> getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index, Vehicle* vehicle, UserRequirementNamespace::VehicleTypeEnum vehicle_type);

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
		���ת���ĵڶ���·�� ������ all_vehicle��ʱ��
	*
	*/
	string getSQLQuerySecondRouteOfTrans(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence, UserRequirementNamespace::VehicleTypeEnum vehicle_type);

	/*
		��ȡSQL������ݴ���getXxxxVehicleInfor������
	* now_index:��ǰ�±�
	*/
	vector<string> getSQLQueryVector(int now_index);

	/*
		��ȡ�������м�������� �Ӷ�ȷ��ת���ĵ�һ�ε���תվ ���Ҫ�����С
	*
	*/
	string getMaxMileage(string start_city, string arrival_city);
public:
	GetRoute(UserRequirement requirement)
	{
		this->requirement = requirement.pretreatUserRequirement();
		weights.resize(this->requirement.start_cities.size());
	}

	GetRoute(UserRequirementAfterPretreat requirement)
	{
		this->requirement = requirement;
		weights.resize(this->requirement.start_cities.size());
	}
	~GetRoute()
	{

	}

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
	������·�������
	* route_result:RouteResult��������� ���������ء�RouteResult����
	*/
	GetRouteNameSpace::GetRouteResultStatue getRouteResults(RouteResult*& route_result);

	/*
	������·������� һ�����
	* route_result:RouteResult ���������ء�RouteResult����
	*/
	GetRouteNameSpace::GetRouteResultStatue getRouteResultsOneGroup(RouteResult*& route_result);

	/*
		����weighs ��Ϊall_transitҪ�½��������� �Ӷ�Ҳ��������Ӧ��weights��д ��ֱ��getд�õ� Ȼ��ƴһ��ͺ���
	*
	*/
	vector<vector<vector<Vehicle*>>> getWeights();
};

#endif // !GETROUTE_H