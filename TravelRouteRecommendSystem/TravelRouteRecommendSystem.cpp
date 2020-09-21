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
	vector<string> start_cities(2);
	start_cities[0] = "����";
	start_cities[1] = "����";

	vector<string> arrive_cities(2);
	arrive_cities[0] = (char*)"����";
	arrive_cities[1] = (char*)"������";

	int city_num = 2;

	vector<string> start_time(2);
	start_time[0] = "2020-08-08 00:00";

	vector<string> arrive_time;

	vector<string> vehicle_type(2);
	vehicle_type[0] = (char*)"����";
	vehicle_type[1] = (char*)"��";

	string travel_type = "�������";

	vector<string> transit_type(2);
	transit_type[0] = "����";
	transit_type[1] = "ֱ��";

	string remark = "��Ҫ�ǳ����ʵ�����";

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