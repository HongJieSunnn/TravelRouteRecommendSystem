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
	initDB();//��ʼ�����ݿ�
	InitRedis::initRedis();//��ʼ��redis

	//�½�һ��UserRequirement�������һ��
	char** start_cities = new char* [2];
	start_cities[0] = (char*)"����";
	start_cities[1] = (char*)"�Ͼ�";

	char** arrive_cities = new char* [2];
	arrive_cities[0] = (char*)"�Ͼ�";
	arrive_cities[1] = (char*)"����";

	int city_num = 2;

	char* start_time = (char*)"2020-08-08 00:00";

	char* arrive_time = nullptr;

	char* vehicle_type = (char*)"����";

	char* travel_type = (char*)"�������";

	char* transist_type = (char*)"ֱ��";

	char* remark = (char*)"��Ҫ�ǳ����ʵ�����";

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
		cout << "��ʼ�����ݿ����\n";
		exit(0);
	}
	else
	{
		cout << "��ʼ�����ݿ�ɹ�\n\n";
		system("cls");
	}
}