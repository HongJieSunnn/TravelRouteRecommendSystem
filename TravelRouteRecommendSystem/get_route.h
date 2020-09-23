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
//这是主要的类 用来获取最优路径
//TODO 不知道要把它写成父类 然后直达 转车什么的继承 还是直接写一个通用方法 支持各种方式
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
		获取交通工具的状态枚举
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
		获取直达交通工具的状态枚举
	*/
	enum class GetDirectVehicleStatue
	{
		GET_DIRECT_VEHICLE_FAILED,
		GET_DIRECT_VEHICLE_SUCCEED,
		SELECT_RESULT_EMPTY,
		GET_RES_FAILED
	};
	/*
		获取中转交通工具的状态枚举
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
		获取混合交通工具的状态枚举
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
		获取任意中转交通工具的状态枚举
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
		//直达
		if (size == 1)
		{
			type = route[0]->getVehicleType();
			return MyTime::costTime(MyTime::stringToMyTime(route[0]->get_start_time(), HH_MM), MyTime::stringToMyTime(route[0]->get_arrival_time(), HH_MM));
		}
		//中转
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
				auto index_of_QI = ticket_price.find("起");
				ticket_price = ticket_price.substr(0, index_of_QI);
				string discount_rate_str = route[i]->get_discount().substr(6,3);
				double discount_rate = atof(discount_rate_str.c_str());
				res += atof(ticket_price.c_str())/ (discount_rate/10);
			}
		}

		return res;
	};

	//不同种时间优先 适用于 all_vehicle
	auto differentTypeVehicleGreater_TimeFirst = [](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		double time1 = getTimeOfVehicleOneRoute(route1);
		double time2 = getTimeOfVehicleOneRoute(route2);

		double price1 = getPriceOfVehicleOneRoute(route1);
		double price2 = getPriceOfVehicleOneRoute(route2);
		double times = price1 / price2;//倍数
		if (time1 < time2)//如果时间1小于时间2 那么1为飞机2为火车 那么用来比较的时间为小的时间乘倍数再乘2(乘2是我规定的) 这里是让小的时间大4倍
		{
			time1 *= times * 2;
		}
		else if (time1 > time2)//如果时间1大于时间2 说明1为火车 那么此时要乘以倍数除二才能比较 都是乘是因为倍数此时会小于1 这里是让大的时间小4倍
		{
			time1 *= (times / 2);
		}

		int start_time_hour1 = MyTime::stringToMyTime(route1[0]->get_start_time(), HH_MM).hour;
		int start_time_hour2 = MyTime::stringToMyTime(route2[0]->get_start_time(), HH_MM).hour;
		if (start_time_hour1 >= 7 && start_time_hour1 <= 16)//如果出发时间在8点到16点之间 这个点出发比较好 那么我们把time1减去自身的五分之一 从而获得更高的权 如果time2也减去自身五分之一 那么大小仍然和没减一样
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
	//适用于 all_vehicle include fix
	auto differentTypeVehicleGreater = [](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		double time1 = getTimeOfVehicleOneRoute(route1);
		double time2 = getTimeOfVehicleOneRoute(route2);

		int start_time_hour1 = MyTime::stringToMyTime(route1[0]->get_start_time(), HH_MM).hour;
		int start_time_hour2 = MyTime::stringToMyTime(route2[0]->get_start_time(), HH_MM).hour;
		if (start_time_hour1 >= 7 && start_time_hour1 <= 16)//如果出发时间在8点到16点之间 这个点出发比较好 那么我们把time1减去自身的五分之一 从而获得更高的权 如果time2也减去自身五分之一 那么大小仍然和没减一样
		{
			time1 -= time1 / 5;
		}
		if (start_time_hour2 >= 7 && start_time_hour2 <= 16)
		{
			time2 -= time2 / 5;
		}

		double price1 = getPriceOfVehicleOneRoute(route1);
		double price2 = getPriceOfVehicleOneRoute(route2);

		return (time1 + price1) < (time2 + price2);//直接比较时间+钱 当然这里如果飞机用打折后的钱就很不平衡 所以飞机的price是打折前的price
	};
	//同种类型的lambda表达式 用来给优先队列 时间优先 return false说明就是按照顺序 先进入的第一个出
	auto sameTypeVehicleGreater_TimeFirst = [](vector<Vehicle*> route1,vector<Vehicle*> route2)-> bool
	{
		double time1 = getTimeOfVehicleOneRoute(route1);
		double time2 = getTimeOfVehicleOneRoute(route2);
		int start_time_hour1 = MyTime::stringToMyTime(route1[0]->get_start_time(), HH_MM).hour;
		int start_time_hour2 = MyTime::stringToMyTime(route2[0]->get_start_time(), HH_MM).hour;
		if (start_time_hour1 >= 7 && start_time_hour1 <= 16)//如果出发时间在8点到16点之间 这个点出发比较好 那么我们把time1减去自身的五分之一 从而获得更高的权 如果time2也减去自身五分之一 那么大小仍然和没减一样
		{
			time1 -= time1 / 5;
		}
		if (start_time_hour2 >= 7 && start_time_hour2 <= 16)
		{
			time2 -= time2 / 5;
		}

		return time1 < time2;
	};
	//同种非时间优先就按进入顺序排序 
	auto sameTypeVehicleGreater=[](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		return differentTypeVehicleGreater(route1,route2);
	};
	//结构MapCmpOfFirstRouteDivideByStation 重载运算符() 目的是作为map的比较函数
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
	GetRoute 获取路线 实际上就是获取两个城市间的交通工具
* requirement:预处理后的需求
*/
class GetRoute
{
private:
	UserRequirementAfterPretreat requirement;
	vector<vector<vector<Vehicle*>>> weights;
	std::mutex mu;
	/*
		给weight排序
	*
	*/
	bool sortWeights();

	/*
		getVechileInfor 通过预处理后的需求获取交通工具(也就是获取路线)
	*
	*/
	GetRouteNameSpace::GetVehicleStatue getVechileInfor();

	/*
		获取混合的交通工具信息
	* sql_query:要执行的sql语句数组
	*/
	GetRouteNameSpace::GetFixVehicleStatue getFixVehicleInfor(int now_index, vector<string> sql_query);

	/*
		获取转车的后半段 并且把符合条件的写入weights
		FIX_TRANSIT和TRANSIT通用(获取sql时不同)
	* first_route:第一段路相同arrival_station的集合
	*/
	GetRouteNameSpace::GetTransitVehicleStatue getTransitVehicleInforSecondRoute(int now_index, vector<Vehicle*> first_route, vector<vector<Vehicle*>>& temp_weights);

	/*
		获取任意中转的交通工具信息
	*
	*/
	GetRouteNameSpace::GetAllTransitVehicleStatue getAllTransitVehicleInfor(int now_index);

	/*
		根据requirement里的条件合成where
	* now_index:requirement中 例如 start_cities 是数组 那么需要传入当前遍历到的那组城市下标 才能获得当前的城市
	*/
	unordered_map<string, string> getWhereSentenceKeyValue(int now_index);

	/*
		根据vehicle里的条件合成where(用于转车的后半段) 并且符合条件的写入weights
		适用于 all_vehicle的时候
	*
	*/
	unordered_map<string, string> getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index, Vehicle* vehicle, UserRequirementNamespace::VehicleTypeEnum vehicle_type);

	/*
		获取table_name(根据start_city)
	* now_index:获得当前城市还有交通工具类型
	*/
	string getTableName(int now_index);

	/*
		获取table_name(根据传入的字符串)
	* now_index:当前下标
	* city_name:城市名
	*/
	string getTableName(string city_name, UserRequirementNamespace::VehicleTypeEnum vehicle);

	/*
		通过requirement来获得合适的ORDER语句
	* now_index:获得当前城市还有交通工具类型
	*/
	string getOrderSentence(int now_index);

	/*
		适用于all__transit且all_vehicle时获取order信息
	*
	*/
	string getOrderSentence(int now_index, UserRequirementNamespace::VehicleTypeEnum);

	/**
		根据条件合成SQL语句
	* now_index:当前下标(用此判断arrive_city是=还是!=)
	* columns:列 默认是 * columns.size()==0即为默认
	* table_name:表名
	* where_sentence:where语句 是一个hash键值对 因为对于该项目 where的就那么些东西 如果用到 就加上具体的条件，没用到 就在后面跟 is not null就好了
	*	Tip:
	*		至于还有一些什么order 或者根据转车不转车判断等于还是不等于 直接访问requirement成员来判断就好了
	*/
	string getSQLQuery(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence);

	/*
		获得转车的第二段路线 适用于 all_vehicle的时候
	*
	*/
	string getSQLQuerySecondRouteOfTrans(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence, UserRequirementNamespace::VehicleTypeEnum vehicle_type);

	/*
		获取SQL语句数据传入getXxxxVehicleInfor方法中
	* now_index:当前下标
	*/
	vector<string> getSQLQueryVector(int now_index);

	/*
		获取两个城市间的最大里程 从而确定转车的第一段的中转站 里程要比这个小
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
		获取直达的交通工具信息
	* sql_query:要执行的sql语句数组
	*/
	GetRouteNameSpace::GetDirectVehicleStatue getDirectVehicleInfor(int now_index, vector<string> sql_query);

	/*
		获取中转的交通工具信息
	* sql_query:要执行的sql语句数组
	*/
	GetRouteNameSpace::GetTransitVehicleStatue getTransitVehicleInfor(int now_index, vector<string> sql_query);

	/*
	返回线路结果数组
	* route_result:RouteResult数组的引用 用来“返回”RouteResult数组
	*/
	GetRouteNameSpace::GetRouteResultStatue getRouteResults(RouteResult*& route_result);

	/*
	返回线路结果数组 一组城市
	* route_result:RouteResult 用来“返回”RouteResult数组
	*/
	GetRouteNameSpace::GetRouteResultStatue getRouteResultsOneGroup(RouteResult*& route_result);

	/*
		返回weighs 因为all_transit要新建两个对象 从而也就是往对应的weights中写 我直接get写好的 然后拼一起就好了
	*
	*/
	vector<vector<vector<Vehicle*>>> getWeights();
};

#endif // !GETROUTE_H