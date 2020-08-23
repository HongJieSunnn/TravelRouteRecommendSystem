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
	initDB();//��ʼ�����ݿ�
	initRedis();//��ʼ��redis

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

	char** vehicle_type=new char*[2];
	vehicle_type[0] = (char*)"����";
	vehicle_type[1] = (char*)"��";

	char* travel_type = (char*)"�������";

	char** transit_type=new char*[2];
	transit_type[0] = (char*)"����";
	transit_type[1] = (char*)"ֱ��";

	char* remark = (char*)"��Ҫ�ǳ����ʵ�����";

	int* distances = new int[2]{950,1050};

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

	GetRoute get_route(user_requirement);
	get_route.getVechileInfor();
	get_route.createGraph();
	cout << "ok";

	delete[] start_cities;
	delete[] arrive_cities;
	delete[] vehicle_type;
	delete[] transit_type;
	delete[] distances;
}

void initDB()
{
	/*string pwd;
	cout << "���������ݿ�����:";
	cin >> pwd;*/
	if (!DBStatus::dealInitMySQLError(InitMySQL::initGlobleMySQL("cdb-j6k4d9vs.bj.tencentcdb.com", "user", "asdf3485", "tourism-recommend-sys-trafficimfor", 10255, "gbk")))
	{
		cout << "��ʼ�����ݿ����\n";
		exit(0);
	}
	else
	{
		cout << "��ʼ�����ݿ�ɹ�\n\n";
		cout << "��ǰ����ʱ��:" << clock() << "ms" << "\n\n";
	}
}

void initRedis()
{
	if (!dealInitRedisError(InitRedis::initRedis()))
	{
		cout << "��ʼ��Redis����\n";
		exit(0);
	}
	else
	{
		cout << "��ʼ��Redis�ɹ�\n\n";
		cout << "��ǰ����ʱ��:" << clock() <<"ms"<< "\n\n";
	}
}