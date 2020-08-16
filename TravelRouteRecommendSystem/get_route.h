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
		GET_DIRECT_VEHICLE_SUCCEED
	};
	/*
		获取中转交通工具的状态枚举
	*/
	enum class GettransitVehicleStatue
	{
		GET_transit_VEHICLE_FAILED,
		GET_transit_VEHICLE_SUCCEED
	};
	/*
		获取混合交通工具的状态枚举
	*/
	enum class GetFixVehicleStatue
	{
		GET_FIX_VEHICLE_FAILED,
		GET_FIX_VEHICLE_SUCCEED
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
	VertexData<string>* vertex_datas;
	pair<VertexData<string>, VertexData<string>>* edges;
	priority_queue<Vehicle> weights;
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
	* 
	*/
	GetRouteNameSpace::GetDirectVehicleStatue getDirectVehicleInfor(string sql_query);

	/*
		获取中转的交通工具信息
	*
	*/
	GetRouteNameSpace::GettransitVehicleStatue getTransitVehicleInfor(string sql_query);

	/*
		获取混合的交通工具信息
	*
	*/
	GetRouteNameSpace::GetFixVehicleStatue getFixVehicleInfor(string sql_query);

	/*
		根据requirement里的条件合成where
	* now_index:requirement中 例如 start_cities 是数组 那么需要传入当前遍历到的那组城市下标 才能获得当前的城市
	*/
	unordered_map<string, string> getWhereSentenceKeyValue(int now_index);

	/*
		获取table_name(根据start_city)
	* now_index:获得当前城市还有交通工具类型
	*/
	string getTableName(int now_index);

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
};

#endif // !GETROUTE_H