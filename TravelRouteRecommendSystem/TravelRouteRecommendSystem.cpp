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

int main()
{
	initDB();//初始化数据库
	InitRedis::initRedis();//初始化redis

	//新建一个UserRequirement对象测试一下
	char** start_cities = new char* [2];
	start_cities[0] = (char*)"福州";
	start_cities[1] = (char*)"南京";

	char** arrive_cities = new char* [2];
	arrive_cities[0] = (char*)"南京";
	arrive_cities[1] = (char*)"北京";

	int city_num = 2;

	char* start_time = (char*)"2020-08-08 00:00";

	char* arrive_time = nullptr;

	char* vehicle_type = (char*)"任意";

	char* travel_type = (char*)"商务出差";

	char* transist_type = (char*)"直达";

	char* remark = (char*)"我要非常舒适的体验";

	int* distances = new int[2];
	distances[0] = 600;
	distances[1] = 500;

	UserRequirement user_requirement
	(
		start_cities,
		arrive_cities,
		city_num,
		start_time,
		arrive_time,
		vehicle_type,
		travel_type,
		transist_type,
		distances,
		remark
	);
	user_requirement.pretreatUserRequirement();
	cout << "ok";
}

void initDB()
{
	
	if (!DBStatus::dealInitMySQLError(InitMySQL::initGlobleMySQL("cdb-j6k4d9vs.bj.tencentcdb.com", "user", "asdf3485", "tourism-recommend-sys-trafficimfor", 10255, "gbk")))
	{
		cout << "初始化数据库错误\n";
		exit(0);
	}
	else
	{
		cout << "初始化数据库成功\n\n";
		system("cls");
	}
}