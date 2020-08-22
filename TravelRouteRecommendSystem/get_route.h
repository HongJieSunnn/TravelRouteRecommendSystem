#ifndef GETROUTE_H
#define GETROUTE_H
#include<unordered_map>
#include<queue>
#include<algorithm>
#include"user_requirement.h"
#include"init_mysql.h"
#include"graph.h"
#include"vehicle.h"
//这是主要的类 用来获取最优路径
//TODO 不知道要把它写成父类 然后直达 转车什么的继承 还是直接写一个通用方法 支持各种方式
using std::unordered_map;
using std::priority_queue;
using std::make_heap;
using std::pop_heap;
using std::push_heap;
using std::sort_heap;

namespace GetRouteNameSpace
{
	/*
		获取路线的状态枚举
	*/
	enum class GetRouteStatue
	{
		
	};
	/*
		获取交通工具的状态枚举
	*/
	enum class GetVehicleStatue
	{
		GET_VEHICLE_FAILED,
		GET_VEHICLE_SUCCEED
	};
	/*
		获取直达交通工具的状态枚举
	*/
	enum class GetDirectVehicleStatue
	{
		GET_DIRECT_VEHICLE_FAILED,
		GET_DIRECT_VEHICLE_SUCCEED,
		SELECT_RESULT_EMPTY
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
		GET_TRANSIT_VEHICLE__GET_FAILED_SECOND_ROUTE_ERROR
	};
	/*
		获取混合交通工具的状态枚举
	*/
	enum class GetFixVehicleStatue
	{
		GET_FIX_VEHICLE_FAILED,
		GET_FIX_VEHICLE_SUCCEED
	};
	/*
		获取任意中转交通工具的状态枚举
	*/
	enum class GetAllTransitVehicleStatue
	{

	};
	//同种类型的lambda表达式 用来给优先队列 时间优先 return false说明就是按照顺序 先进入的第一个出
	auto sameTypeVehicleGreater_TimeFirst = [](vector<Vehicle*> route1,vector<Vehicle*> route2)-> bool
	{
		return false;
	};

	//同种非时间优先就按进入顺序排序 
	auto sameTypeVehicleGreater=[](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		return false;
	};

	//不同种时间优先 适用于 all_vehicle
	auto differentTypeVehicleGreater_TimeFirst = [](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		float time1 = getTimeOfVehicleOneRoute(route1);
		float time2 = getTimeOfVehicleOneRoute(route2);

		float price1 = getPriceOfVehicleOneRoute(route1);
		float price2 = getPriceOfVehicleOneRoute(route2);
		//时间大的除以时间小的 就是倍数 时间小的乘上倍数 比较平衡一点
		float times = price1 > price2 ? (price1 / price2) : (price2 / price1);//倍数

		time1 > time2 ? time2 *= times : time1 *= times;

		if (time1 > time2)
			return true;
		return false;
	};
	//适用于 all_vehicle
	auto differentTypeVehicleGreater = [](vector<Vehicle*> route1, vector<Vehicle*> route2)->bool
	{
		float time1 = getTimeOfVehicleOneRoute(route1);
		float time2 = getTimeOfVehicleOneRoute(route2);

		float price1 = getPriceOfVehicleOneRoute(route1);
		float price2 = getPriceOfVehicleOneRoute(route2);

		float times_of_price= price1 > price2 ? (price1 / price2) : (price2 / price1);//价钱倍数
		float times_of_time= time1 > time2 ? (time1 / time2) : (time2 / time1);//时间倍数

		if (times_of_price > (times_of_time-0.1))//价钱倍数比时间倍数-0.1大说明太贵了
			return false;
		return true;
	};

	auto getTimeOfVehicleOneRoute = [](vector<Vehicle*> route)->int
	{
		int size = route.size();
		string type = NULL;
		//直达
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
		//中转
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
	GetRoute 获取路线 实际上就是获取两个城市间的交通工具
* requirement:预处理后的需求
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
		//TODO:后续封装dll时可以把UserRequirement requirement弄成一个全局变量
		this->requirement = requirement.pretreatUserRequirement();
		
	}

	GetRoute(UserRequirementAfterPretreat requirement)
	{
		//TODO:后续封装dll时可以把UserRequirement requirement弄成一个全局变量
		this->requirement = requirement;
	}
	~GetRoute()
	{

	}

	/*
		getVechileInfor 通过预处理后的需求获取交通工具(也就是获取路线)
	* 
	*/
	GetRouteNameSpace::GetVehicleStatue getVechileInfor();

	//我感觉把中转类型大区分就好了其实 至于用什么交通工具、价格、时间这些的操作都是类似的

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
		获取转车的后半段 并且把符合条件的写入weights
	* 
	*/
	GetRouteNameSpace::GetTransitVehicleStatue getTransitVehicleInforSecondRoute(int now_index, string start_city_name, Vehicle* first_route);

	/*
		获取混合的交通工具信息
	* sql_query:要执行的sql语句数组
	*/
	GetRouteNameSpace::GetFixVehicleStatue getFixVehicleInfor(int now_index, vector<string> sql_query);

	/*
		获取任意中转的交通工具信息
	* sql_query:要执行的sql语句数组
	*/
	GetRouteNameSpace::GetFixVehicleStatue getAllTransitVehicleInfor(int now_index, vector<string> sql_query);

	/*
		根据requirement里的条件合成where
	* now_index:requirement中 例如 start_cities 是数组 那么需要传入当前遍历到的那组城市下标 才能获得当前的城市
	* second_route:是否是中转的第二跳路布尔变量 默认为false 用来区分是否要使用arrival_time
	*/
	unordered_map<string, string> getWhereSentenceKeyValue(int now_index);

	/*
		根据vehicle里的条件合成where(用于转车的后半段) 并且符合条件的写入weights
	* 
	*/
	unordered_map<string, string> getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index,Vehicle* vehicle);

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
	string getTableName(int now_index,string city_name);

	/*
		通过requirement来获得合适的ORDER语句
	* now_index:获得当前城市还有交通工具类型
	*/
	string getOrderSentence(int now_index);

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
		获得转车的第二段路线 例如 福州-南京可以分解成 福州-杭州(getSQLQuery) 然后该方法获得 杭州-福州 在getTransitVehicleInfor中使用 
		因为需要先对前半段做SQL查询 才能知道后半段的起始城市
	* 
	*/
	string getSQLQuerySecondRouteOfTrans(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence);

	/*
		获取SQL语句数据传入getXxxxVehicleInfor方法中
	* now_index:当前下标
	*/
	vector<string> getSQLQueryVector(int now_index);
};

#endif // !GETROUTE_H