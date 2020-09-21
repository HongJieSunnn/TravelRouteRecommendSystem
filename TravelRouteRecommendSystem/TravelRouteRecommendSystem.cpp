#include<iostream>
#include<string>
#include"init_mysql.h"
#include"init_redis.h"
#include"change_character_set.h"
#include"get_route.h"
#include"graph.h"
#include"my_time.h"
#include"route_result.h"
#include"user_requirement.h"
#include"vehicle.h"

using std::string;
using std::cout;
using std::cin;
void initDB();
void initRedis();

int main()
{
	initDB();//初始化数据库
	initRedis();//初始化redis

	//新建一个UserRequirement对象测试一下
	vector<string> start_cities(2);
	start_cities[0] = "福州";
	start_cities[1] = "北京";

	vector<string> arrive_cities(2);
	arrive_cities[0] = (char*)"北京";
	arrive_cities[1] = (char*)"哈尔滨";

	int city_num = 2;

	vector<string> start_time(2);
	start_time[0] = "2020-08-08 00:00";

	vector<string> arrive_time;

	vector<string> vehicle_type(2);
	vehicle_type[0] = (char*)"任意";
	vehicle_type[1] = (char*)"火车";

	string travel_type = "商务出差";

	vector<string> transit_type(2);
	transit_type[0] = "任意";
	transit_type[1] = "直达";

	string remark = "我要非常舒适的体验";

	vector<int> distances({ 1800,1200 });

	

	RouteResult* route_result;
	try
	{
		UserRequirement user_requirement
		(
			start_cities,
			arrive_cities,
			city_num,
			start_time,
			arrive_time,
			vehicle_type,
			travel_type,
			transit_type,
			distances,
			remark
		);
		UserRequirementAfterPretreat temp = user_requirement.pretreatUserRequirement();
		GetRoute get_route(temp);
		get_route.getRouteResults(route_result);
	}
	catch (const std::exception& exc)
	{
		cout << exc.what();
	}
	
	cout << "ok";
}

void initDB()
{
	/*string pwd;
	cout << "请输入数据库密码:";
	cin >> pwd;*/
	if (!DBStatus::dealInitMySQLError(InitMySQL::initGlobleMySQL("cdb-j6k4d9vs.bj.tencentcdb.com", "user", "asdf3485", "tourism-recommend-sys-trafficimfor", 10255, "gbk")))
	{
		cout << "初始化数据库错误\n";
		exit(0);
	}
	else
	{
		cout << "初始化数据库成功\n\n";
		cout << "当前消耗时间:" << clock() << "ms" << "\n\n";
	}
}

void initRedis()
{
	if (!dealInitRedisError(InitRedis::initRedis()))
	{
		cout << "初始化Redis错误\n";
		exit(0);
	}
	else
	{
		cout << "初始化Redis成功\n\n";
		cout << "当前消耗时间:" << clock() <<"ms"<< "\n\n";
	}
}