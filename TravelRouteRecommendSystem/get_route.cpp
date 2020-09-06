#include "get_route.h"

using namespace GetRouteNameSpace;

GetRouteNameSpace::CreateGraphStatue GetRoute::createGraph()
{
	int cities_num = requirement.start_cities.size();
	
	this->graph = Graph<string, vector<vector<Vehicle*>>>(cities_num + 1, cities_num);
	if (graph.createGraph(this->vertex_datas, edges, weights) != OperateGraphStatue::CREATE_GRAPH_SUCCEED)
	{
		return CreateGraphStatue::CREATE_GRAPH_FAILED;
	}

	if (!sortWeights())
	{
		return CreateGraphStatue::CREATE_GRAPH_FAILED;
	}

	return CreateGraphStatue::CREATE_GRAPH_SUCCEED;
}

bool GetRoute::sortWeights()
{
	for (int i = 0; i < weights.size(); i++)
	{
		if (this->requirement.timeType == UserRequirementNamespace::TIME_FIRST)
		{
			if (this->requirement.vehicleType[i] == UserRequirementNamespace::ALL_VEHICLE)
			{
				sort(weights[i].begin(), weights[i].end(),differentTypeVehicleGreater_TimeFirst);
			}
			else
			{
				sort(weights[i].begin(), weights[i].end(), sameTypeVehicleGreater_TimeFirst);
			}
		}
		else
		{
			if (this->requirement.vehicleType[i] == UserRequirementNamespace::ALL_VEHICLE)
			{
				sort(weights[i].begin(), weights[i].end(), differentTypeVehicleGreater);
			}
			else
			{
				sort(weights[i].begin(), weights[i].end(), sameTypeVehicleGreater);
			}
		}
	}
	return true;
}

GetRouteNameSpace::GetVehicleStatue GetRoute::getVechileInfor()
{
	int cities_num = requirement.start_cities.size();

	for (int i = 0; i < cities_num; i++)
	{
		vertex_datas[i]=(this->requirement.start_cities[i]);//��ʼ���з���ͼ�ĵ���
		vertex_datas[i+1]=(this->requirement.arrive_cities[i]);//������з���ͼ�ĵ���
		edges[i]={ this->requirement.start_cities[i],this->requirement.arrive_cities[i] };//��ӱ�
		switch (this->requirement.transitType[i])
		{
		case UserRequirementNamespace::DIRECT:
			if (getDirectVehicleInfor(i, getSQLQueryVector(i)) != GetDirectVehicleStatue::GET_DIRECT_VEHICLE_SUCCEED)
			{
				return GetVehicleStatue::GET_DIRECT_VEHICLE_FAILED;
			}
			break;
		case UserRequirementNamespace::TRANS:
			if (getTransitVehicleInfor(i, getSQLQueryVector(i)) != GetTransitVehicleStatue::GET_TRANSIT_VEHICLE_SUCCEED)
			{
				return GetVehicleStatue::GET_TRANSIT_VEHICLE_FAILED;
			}
			break;
		case UserRequirementNamespace::FIX_TRANS:
			if (getFixVehicleInfor(i, getSQLQueryVector(i)) != GetFixVehicleStatue::GET_FIX_VEHICLE_SUCCEED)
			{
				return GetVehicleStatue::GET_FIX_VEHICLE_FAILED;
			}
			break;
		case UserRequirementNamespace::ALL_TRANSIT:
			if (getAllTransitVehicleInfor(i) != GetAllTransitVehicleStatue::GET_ALL_TRANSIT_VEHICLE_SUCCEED)
			{
				return GetVehicleStatue::GET_ALL_TRANSIT_VEHICLE_FAILED;
			}
			break;
		}
	}


	return GetVehicleStatue::GET_VEHICLE_SUCCEED;
}

GetRouteNameSpace::GetDirectVehicleStatue GetRoute::getDirectVehicleInfor(int now_index,vector<string> sql_query)
{
	for (int i = 0; i < sql_query.size(); i++)
	{
		MYSQL_RES* res = InitMySQL::execSQLToGetResult(sql_query[i]);
		int rows_count = res->row_count;
		if (rows_count == 0)
			return GetDirectVehicleStatue::SELECT_RESULT_EMPTY;

		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(res);
		vector<Vehicle*> vehicle(1);
		vector<vector<Vehicle*>> temp_weights = vector<vector<Vehicle*>>(rows_count>10?10:rows_count, vector<Vehicle*>(1));
		int j = 0;
		while (row != NULL)
		{
			switch (this->requirement.vehicleType[now_index])
			{
			case UserRequirementNamespace::AIRPLANE:
				vehicle[0]=new AirPlane
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11], row[12]
				);
				break;
			case UserRequirementNamespace::HSRC:
				vehicle[0]=new HSRC
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11],
					row[12], row[13]
				);
				break;
			case UserRequirementNamespace::ALL_VEHICLE:
				if (sql_query[i].find("��") != string::npos)
				{
					vehicle[0] = new HSRC
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11],
						row[12], row[13]
					);
				}
				else
				{
					vehicle[0] = new AirPlane
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11], row[12]
					);
					break;
				}
				break;
			}
			temp_weights[j]=vehicle;
			j++;
			row = mysql_fetch_row(res);
			if (j >= 10)//���20
			{
				break;
			}
		}
		weights[now_index].insert(weights[now_index].end(),temp_weights.begin(),temp_weights.end());
	}
	return GetDirectVehicleStatue::GET_DIRECT_VEHICLE_SUCCEED;
}

GetRouteNameSpace::GetTransitVehicleStatue GetRoute::getTransitVehicleInfor(int now_index, vector<string> sql_query)
{
	for (int i = 0; i < sql_query.size(); i++)
	{
		MYSQL_RES* res = InitMySQL::execSQLToGetResult(sql_query[i]);
		int rows_count = res->row_count;
		if (rows_count == 0)
			return GetTransitVehicleStatue::SELECT_RESULT_NO_FIRST_ROUTE;

		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(res);
		//��ϣ�� ���ǵڶ���·��ʼ��վ�� Ҳ���ǵ�һ��·��arrival_station ����������� ֱ�Ӳ�ѯʱ��������Ǹ� Ȼ�������������Ӧ�ĵ�һ��· ���ҿ��Կ����߳� ֱ�Ӷ�����վ��ͬʱ��
		unordered_map<string, vector<Vehicle*>> first_route_divide_by_station;
		//���ս�� Ҳ���Ƕ������·�ļ���
		vector<vector<Vehicle*>> temp_weights;
		while (row != NULL)
		{
			//ǰ���
			switch (this->requirement.vehicleType[now_index])
			{
			case UserRequirementNamespace::AIRPLANE:
				first_route_divide_by_station[row[7]].emplace_back(new AirPlane
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11], row[12]
				));
				break;
			case UserRequirementNamespace::HSRC:
				first_route_divide_by_station[row[8]].emplace_back(new HSRC
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11],
					row[12], row[13]
				));
				break;
			case UserRequirementNamespace::ALL_VEHICLE:
				if (sql_query[i].find("��") != string::npos)
				{
					first_route_divide_by_station[row[8]].emplace_back(new HSRC
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11],
						row[12], row[13]
					));
				}
				else
				{
					first_route_divide_by_station[row[7]].emplace_back(new AirPlane
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11], row[12]
					));
					break;
				}
				break;
			}
			row = mysql_fetch_row(res);
		}
		int size = first_route_divide_by_station.size();
		int async_times = size / 5;//5�̸߳�ͬʱ���еĴ���
		int sync_times = size % 5;//һ���߳�һ���߳�ִ�� ��ʵ���൱��ͬ��ִ����
		auto routes_one_station = first_route_divide_by_station.begin();
		for (int j=0;j<async_times;j++)
		{
			thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, temp_weights);
			thread thread_2(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread thread_3(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread thread_4(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread thread_5(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);

			thread_1.join();
			thread_2.join();
			thread_3.join();
			thread_4.join();
			thread_5.join();
		}
		switch (sync_times)
		{
		case 1:
			{
				thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, temp_weights);
				thread_1.join();
				break;
			}
		case 2:
			{
				thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, temp_weights);
				thread thread_2(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
				thread_1.join();
				thread_2.join();
				break;
			}
		case 3:
			{
				thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, temp_weights);
				thread thread_2(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
				thread thread_3(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
				thread_1.join();
				thread_2.join();
				thread_3.join();
				break;
			}
		case 4:
			{
				thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, temp_weights);
				thread thread_2(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
				thread thread_3(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
				thread thread_4(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
				thread_1.join();
				thread_2.join();
				thread_3.join();
				thread_4.join();
				break;
			}
		}
		weights[now_index] = temp_weights;
	}
	return GetTransitVehicleStatue::GET_TRANSIT_VEHICLE_SUCCEED;
}

GetRouteNameSpace::GetFixVehicleStatue GetRoute::getFixVehicleInfor(int now_index, vector<string> sql_query)
{
	for (int i = 0; i < sql_query.size(); i++)
	{
		MYSQL_RES* res = InitMySQL::execSQLToGetResult(sql_query[i]);
		int rows_count = res->row_count;
		if (rows_count == 0)
			return GetFixVehicleStatue::SELECT_RESULT_NO_FIRST_ROUTE;

		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(res);
		//��ϣ�� ���ǵڶ���·��ʼ��վ�� Ҳ���ǵ�һ��·��arrival_station ����������� ֱ�Ӳ�ѯʱ��������Ǹ� Ȼ�������������Ӧ�ĵ�һ��· ���ҿ��Կ����߳� ֱ�Ӷ�����վ��ͬʱ��
		unordered_map<string, vector<Vehicle*>> first_route_divide_by_station;
		//���ս�� Ҳ���Ƕ������·�ļ���
		vector<vector<Vehicle*>> temp_weights;
		while (row != NULL)
		{
			//ǰ���
			switch (this->requirement.vehicleType[now_index])
			{
			case UserRequirementNamespace::AIRPLANE:
				first_route_divide_by_station[row[7]].emplace_back(new AirPlane
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11], row[12]
				));
				break;
			case UserRequirementNamespace::HSRC:
				first_route_divide_by_station[row[8]].emplace_back(new HSRC
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11],
					row[12], row[13]
				));
				break;
			case UserRequirementNamespace::ALL_VEHICLE:
				if (sql_query[i].find("��") != string::npos)
				{
					first_route_divide_by_station[row[8]].emplace_back(new HSRC
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11],
						row[12], row[13]
					));
				}
				else
				{
					first_route_divide_by_station[row[7]].emplace_back(new AirPlane
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11], row[12]
					));
					break;
				}
				break;
			}
			row = mysql_fetch_row(res);
		}
		int size = first_route_divide_by_station.size();
		int async_times = size / 5;//5�̸߳�ͬʱ���еĴ���
		int sync_times = size % 5;//һ���߳�һ���߳�ִ�� ��ʵ���൱��ͬ��ִ����
		auto routes_one_station = first_route_divide_by_station.begin();
		for (int j = 0; j < async_times; j++)
		{
			thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, temp_weights);
			thread thread_2(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread thread_3(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread thread_4(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread thread_5(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);

			thread_1.join();
			thread_2.join();
			thread_3.join();
			thread_4.join();
			thread_5.join();
		}
		switch (sync_times)
		{
		case 1:
		{
			thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, temp_weights);
			thread_1.join();
			break;
		}
		case 2:
		{
			thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, temp_weights);
			thread thread_2(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread_1.join();
			thread_2.join();
			break;
		}
		case 3:
		{
			thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, temp_weights);
			thread thread_2(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread thread_3(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread_1.join();
			thread_2.join();
			thread_3.join();
			break;
		}
		case 4:
		{
			thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, temp_weights);
			thread thread_2(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread thread_3(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread thread_4(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station++->second, temp_weights);
			thread_1.join();
			thread_2.join();
			thread_3.join();
			thread_4.join();
			break;
		}
		}
		weights[now_index] = temp_weights;
	}
	return GetFixVehicleStatue::GET_FIX_VEHICLE_SUCCEED;
}

GetRouteNameSpace::GetTransitVehicleStatue GetRoute::getTransitVehicleInforSecondRoute(int now_index, vector<Vehicle*> first_route, vector<vector<Vehicle*>>& temp_weights)
{
	sort(first_route.begin(), first_route.end(), 
		[](Vehicle* vehicle1,Vehicle* vehicle2)->bool
		{
			if (vehicle1->get_arrival_time() < vehicle2->get_arrival_time())
			{
				return true;
			}
			return false;
		});//����arrival_time(���ڶ��ε�start_time)������ Ȼ�����С���Ǹ��������ݿ��ѯ�ͺ���

	//��Ϊͬһ��first_route���ϵ���վһ��һ�� ������all_vehicle first_routeҲ������ַɻ��ͻ�һ����� ��Ϊվ������һ����
	UserRequirementNamespace::VehicleTypeEnum vehicle_type = first_route[0]->getVehicleType() == HSRC_TYPE ? UserRequirementNamespace::HSRC : UserRequirementNamespace::AIRPLANE;
	string start_city_name = first_route[0]->get_arrival_city();

	string sql = getSQLQuerySecondRouteOfTrans(
		now_index,
		vector<string>(),
		getTableName(start_city_name, vehicle_type),
		getWhereSentenceKeyValueOfSecondRouteOfTrans(now_index, first_route[0], vehicle_type),
		vehicle_type
	);
	//����
	MYSQL_RES* res_second = InitMySQL::execSQLToGetResult(sql);//��ѯ���ݿ�
	if (res_second == nullptr)
	{
		return GetTransitVehicleStatue::SELECT_RESULT_NO_TABLE_EXIST;
	}
	int rows_count_second = res_second->row_count;
	if (rows_count_second == 0)
		return GetTransitVehicleStatue::SELECT_RESULT_NO_SECOND_ROUTE;
	MYSQL_ROW row_second = NULL;
	row_second = mysql_fetch_row(res_second);
	vector<Vehicle*> second_route;
	while (row_second != NULL)
	{
		switch (vehicle_type)
		{
		case UserRequirementNamespace::AIRPLANE:
			second_route.emplace_back(new AirPlane
			(
				row_second[0], row_second[1], row_second[2], row_second[3], row_second[4], row_second[5],
				row_second[6], row_second[7], row_second[8], row_second[9], row_second[10], row_second[11], row_second[12]
			));
			break;
		case UserRequirementNamespace::HSRC:
			second_route.emplace_back(new HSRC
			(
				row_second[0], row_second[1], row_second[2], row_second[3], row_second[4], row_second[5],
				row_second[6], row_second[7], row_second[8], row_second[9], row_second[10], row_second[11],
				row_second[12], row_second[13]
			));
			break;
		}
		row_second = mysql_fetch_row(res_second);
	}
	//д����������µĶ���ѭ�����Ӷȶ���n^2 ����д�����һЩ�о� ���ÿ����Լ��ڴ��ͷŵ����⣨����ʹ������ָ�� �����������е��鷳��

	for (auto one_of_second_route : second_route)
	{
		for (auto one_of_first_route : first_route)
		{
			if (one_of_first_route->get_arrival_time() < one_of_second_route->get_start_time())//�ڶ��γ���ʱ�����ڵ�һ�ε���ʱ��
			{
				if (MyTime::costTime(MyTime::stringToMyTime(one_of_first_route->get_arrival_time(), HH_MM), MyTime::stringToMyTime(one_of_second_route->get_start_time(), HH_MM)) < 20)//ʱ������20��������
				{
					temp_weights.emplace_back(vector<Vehicle>{one_of_first_route, one_of_second_route});
				}
			}
		}
	}
	return GetTransitVehicleStatue::GET_ONE_SECOND_ROUTE_INFOR_SUCCEED;
}

GetRouteNameSpace::GetAllTransitVehicleStatue GetRoute::getAllTransitVehicleInfor(int now_index)
{
	UserRequirementAfterPretreat temp_requirement = this->requirement;
	temp_requirement.transitType[now_index] = UserRequirementNamespace::DIRECT;
	GetRoute temp_obj1(temp_requirement);
	temp_requirement.transitType[now_index] = UserRequirementNamespace::TRANS;
	GetRoute temp_obj2(temp_requirement);

	temp_obj1.getDirectVehicleInfor(now_index, temp_obj1.getSQLQueryVector(now_index));
	temp_obj2.getTransitVehicleInfor(now_index, temp_obj2.getSQLQueryVector(now_index));

	if (!temp_obj1.getWeights().empty())
	{
		auto temp_weights = temp_obj1.getWeights()[now_index];
		this->weights[now_index].insert(weights[now_index].end(), temp_weights.begin(), temp_weights.end());
	}
	else
	{
		return GetAllTransitVehicleStatue::DIRECT_EMPTY;
	}
	if (!temp_obj1.getWeights().empty())
	{
		auto temp_weights = temp_obj2.getWeights()[now_index];
		this->weights[now_index].insert(weights[now_index].end(), temp_weights.begin(), temp_weights.end());
	}
	else
	{
		return GetAllTransitVehicleStatue::TRANSIT_EMPTY;
	}
	return GetAllTransitVehicleStatue::GET_ALL_TRANSIT_VEHICLE_SUCCEED;
}

unordered_map<string, string> GetRoute::getWhereSentenceKeyValue(int now_index)
{
	unordered_map<string, string> where_sentence;
	where_sentence.insert({ "start_city",requirement.start_cities[now_index] });
	where_sentence.insert({ "arrival_city",requirement.arrive_cities[now_index] });
	where_sentence.insert({ "start_time",requirement.start_time.myTimeToStringByInt(HH_MM) });
	if (requirement.arrive_time.format != 0&& requirement.vehicleType[now_index] != UserRequirementNamespace::TRANS&& requirement.vehicleType[now_index] != UserRequirementNamespace::FIX_TRANS)
	{
		if (requirement.start_time.day < requirement.arrive_time.day)
		{
			requirement.arrive_time.hour += (requirement.arrive_time.day - requirement.start_time.day) * 24;
		}
		where_sentence.insert({ "arrival_time",requirement.arrive_time.myTimeToStringByInt(HH_MM) });
	}
	if (requirement.transitType[now_index] == UserRequirementNamespace::TRANS)
	{
		where_sentence.insert({ "mileage",to_string(requirement.distances[now_index]*0.65) });//��

		int minute_of_all = requirement.distances[now_index] / 800.0*60.0*0.65;
		int hour = minute_of_all / 60;
		int minute = minute_of_all % 60;
		string cost_time=to_string(hour).append("Сʱ").append(to_string(minute)).append("����");
		where_sentence.insert({ "cost_time",cost_time });
	}
	return where_sentence;
}

unordered_map<string, string> GetRoute::getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index,Vehicle* vehicle)
{
	unordered_map<string, string> where_sentence;
	//��ת�ܲ��ܻ�����һ��վȥ�� ������Ҫstart��վ��
	if(this->requirement.vehicleType[now_index]==UserRequirementNamespace::HSRC)
		where_sentence.insert({ "start_station",vehicle->get_arrival_station() });
	else if(this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
		where_sentence.insert({ "start_airport",vehicle->get_arrival_station() });

	where_sentence.insert({ "arrival_city",requirement.arrive_cities[now_index] });
	where_sentence.insert({ "start_time",vehicle->get_arrival_time() });
	if (requirement.arrive_time.format != 0)
	{
		if (requirement.start_time.day < requirement.arrive_time.day)
		{
			requirement.arrive_time.hour += (requirement.arrive_time.day - requirement.start_time.day) * 24;
		}
		where_sentence.insert({ "arrival_time",requirement.arrive_time.myTimeToStringByInt(HH_MM) });
	}
	where_sentence.insert({ "mileage",to_string(requirement.distances[now_index] * 0.60) });//��

	int minute_of_all = requirement.distances[now_index] / 800.0 * 60.0 * 0.65;
	int hour = minute_of_all / 60;
	int minute = minute_of_all % 60;
	string cost_time = to_string(hour).append("Сʱ").append(to_string(minute)).append("����");
	where_sentence.insert({ "cost_time",cost_time });
	return where_sentence;
}

unordered_map<string, string> GetRoute::getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index,Vehicle* vehicle, UserRequirementNamespace::VehicleTypeEnum vehicle_type)
{
	unordered_map<string, string> where_sentence;
	//��ת�ܲ��ܻ�����һ��վȥ�� ������Ҫstart��վ��
	if (vehicle_type == UserRequirementNamespace::HSRC)
		where_sentence.insert({ "start_station",vehicle->get_arrival_station() });
	else if (vehicle_type == UserRequirementNamespace::AIRPLANE)
		where_sentence.insert({ "start_airport",vehicle->get_arrival_station() });

	where_sentence.insert({ "arrival_city",requirement.arrive_cities[now_index] });
	where_sentence.insert({ "start_time",vehicle->get_arrival_time() });
	if (requirement.arrive_time.format != 0)
	{
		if (requirement.start_time.day < requirement.arrive_time.day)
		{
			requirement.arrive_time.hour += (requirement.arrive_time.day - requirement.start_time.day) * 24;
		}
		where_sentence.insert({ "arrival_time",requirement.arrive_time.myTimeToStringByInt(HH_MM) });
	}
	where_sentence.insert({ "mileage",to_string(requirement.distances[now_index] * 0.65) });//��

	int minute_of_all = requirement.distances[now_index] / 800.0 * 60.0 * 0.65;
	int hour = minute_of_all / 60;
	int minute = minute_of_all % 60;
	string cost_time = to_string(hour).append("Сʱ").append(to_string(minute)).append("����");
	where_sentence.insert({ "cost_time",cost_time });
	return where_sentence;
}

string GetRoute::getTableName(int now_index)
{
	string now_start_city = requirement.start_cities[now_index];
	UserRequirementNamespace::VehicleTypeEnum now_vehicle_type = requirement.vehicleType[now_index];

	switch (now_vehicle_type)
	{
	case UserRequirementNamespace::HSRC:
		now_start_city.append("��");
		break;
	case UserRequirementNamespace::AIRPLANE:
		now_start_city.append("����");
		break;
	}
	return now_start_city;
}

string GetRoute::getTableName(string city_name,UserRequirementNamespace::VehicleTypeEnum vehicle)
{
	switch (vehicle)
	{
	case UserRequirementNamespace::HSRC:
		city_name.append("��");
		break;
	case UserRequirementNamespace::AIRPLANE:
		city_name.append("����");
		break;
	}
	return city_name;
}

string GetRoute::getOrderSentence(int now_index)
{
	string sql_query = " ORDER BY ";

	switch (requirement.timeType)
	{
	case UserRequirementNamespace::TIME_REGARDLESS:
		switch (requirement.priceType)
		{
		case UserRequirementNamespace::PRICE_AFFORDABLE:
		case UserRequirementNamespace::PRICE_REGARDLESS://ʱ�� �۸��������
			if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,second_class_seat_price;");
			}
			else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,ticket_price;");
			}
			break;
		case UserRequirementNamespace::PRICE_CHEAP://�Ȱ��۸������
			if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
			{
				sql_query.append("second_class_seat_price,cost_time;");
			}
			else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("ticket_price,cost_time;");
			}
			break;
		case UserRequirementNamespace::PRICE_EXPENSIVE:
			if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
			{
				sql_query.append("second_class_seat_price DESC,cost_time;");
			}
			else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("ticket_price DESC,cost_time;");
			}
			break;
		}
		break;
	case UserRequirementNamespace::TIME_FIRST:
		switch (requirement.priceType)
		{
		case UserRequirementNamespace::PRICE_REGARDLESS://ʱ�� �۸��������
			sql_query.append("cost_time,start_time;");
			break;
		case UserRequirementNamespace::PRICE_CHEAP://�Ȱ��۸������
			if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,second_class_seat_price,start_time;");
			}
			else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,ticket_price,start_time;");
			}
			break;
		case UserRequirementNamespace::PRICE_AFFORDABLE:
			if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,start_time,second_class_seat_price;");
			}
			else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,start_time,ticket_price;");
			}
			break;
		case UserRequirementNamespace::PRICE_EXPENSIVE:
			if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,second_class_seat_price DESC,start_time;");
			}
			else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,ticket_price DESC,start_time;");
			}
			break;
		}
		break;
	case UserRequirementNamespace::TIME_BETTER:
		switch (requirement.priceType)
		{
		case UserRequirementNamespace::PRICE_REGARDLESS://ʱ�� �۸��������
			sql_query.append("cost_time;");
			break;
		case UserRequirementNamespace::PRICE_CHEAP://�Ȱ��۸������
			if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
			{
				sql_query.append("second_class_seat_price,cost_time;");
			}
			else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("ticket_price,cost_time;");
			}
			break;
		case UserRequirementNamespace::PRICE_AFFORDABLE:
			if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,second_class_seat_price;");
			}
			else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,ticket_price;");
			}
			break;
		case UserRequirementNamespace::PRICE_EXPENSIVE:
			if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
			{
				sql_query.append("second_class_seat_price DESC,cost_time;");
			}
			else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("ticket_price DESC,cost_time;");
			}
			break;
		}
		break;
	}
	return sql_query;
}

string GetRoute::getOrderSentence(int now_index, UserRequirementNamespace::VehicleTypeEnum vehicle)
{
	string sql_query = " ORDER BY ";

	switch (requirement.timeType)
	{
	case UserRequirementNamespace::TIME_REGARDLESS:
		switch (requirement.priceType)
		{
		case UserRequirementNamespace::PRICE_AFFORDABLE:
		case UserRequirementNamespace::PRICE_REGARDLESS://ʱ�� �۸��������
			if (vehicle == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,second_class_seat_price;");
			}
			else if (vehicle == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,ticket_price;");
			}
			break;
		case UserRequirementNamespace::PRICE_CHEAP://�Ȱ��۸������
			if (vehicle == UserRequirementNamespace::HSRC)
			{
				sql_query.append("second_class_seat_price,cost_time;");
			}
			else if (vehicle == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("ticket_price,cost_time;");
			}
			break;
		case UserRequirementNamespace::PRICE_EXPENSIVE:
			if (vehicle == UserRequirementNamespace::HSRC)
			{
				sql_query.append("second_class_seat_price DESC,cost_time;");
			}
			else if (vehicle == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("ticket_price DESC,cost_time;");
			}
			break;
		}
		break;
	case UserRequirementNamespace::TIME_FIRST:
		switch (requirement.priceType)
		{
		case UserRequirementNamespace::PRICE_REGARDLESS://ʱ�� �۸��������
			sql_query.append("cost_time,start_time;");
			break;
		case UserRequirementNamespace::PRICE_CHEAP://�Ȱ��۸������
			if (vehicle == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,second_class_seat_price,start_time;");
			}
			else if (vehicle == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,ticket_price,start_time;");
			}
			break;
		case UserRequirementNamespace::PRICE_AFFORDABLE:
			if (vehicle == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,start_time,second_class_seat_price;");
			}
			else if (vehicle == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,start_time,ticket_price;");
			}
			break;
		case UserRequirementNamespace::PRICE_EXPENSIVE:
			if (vehicle == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,second_class_seat_price DESC,start_time;");
			}
			else if (vehicle == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,ticket_price DESC,start_time;");
			}
			break;
		}
		break;
	case UserRequirementNamespace::TIME_BETTER:
		switch (requirement.priceType)
		{
		case UserRequirementNamespace::PRICE_REGARDLESS://ʱ�� �۸��������
			sql_query.append("cost_time;");
			break;
		case UserRequirementNamespace::PRICE_CHEAP://�Ȱ��۸������
			if (vehicle == UserRequirementNamespace::HSRC)
			{
				sql_query.append("second_class_seat_price,cost_time;");
			}
			else if (vehicle == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("ticket_price,cost_time;");
			}
			break;
		case UserRequirementNamespace::PRICE_AFFORDABLE:
			if (vehicle == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,second_class_seat_price;");
			}
			else if (vehicle == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,ticket_price;");
			}
			break;
		case UserRequirementNamespace::PRICE_EXPENSIVE:
			if (vehicle == UserRequirementNamespace::HSRC)
			{
				sql_query.append("second_class_seat_price DESC,cost_time;");
			}
			else if (vehicle == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("ticket_price DESC,cost_time;");
			}
			break;
		}
		break;
	}


	return sql_query;
}

string GetRoute::getSQLQuery(int now_index,vector<string> columns, string table_name, unordered_map<string, string> where_sentence)
{
	string sql_query = "SELECT DISTINCT ";

	if (columns.size())
	{
		for (int i = 0; i < columns.size(); i++)
		{
			sql_query.append(columns[i]);
			if (i != columns.size() - 1)
			{
				sql_query.append(",");
			}
		}
	}
	else
	{
		sql_query.append("*");
	}

	sql_query.append(" FROM ");
	sql_query.append(table_name);
	sql_query.append(" WHERE ");

	sql_query.append("start_city=");
	sql_query.append(InitMySQL::toSQLString(where_sentence["start_city"]));

	sql_query.append(" AND ");

	if (requirement.transitType[now_index] == UserRequirementNamespace::TransitTypeEnum::DIRECT)
	{
		sql_query.append("arrival_city=");
	}
	else
	{
		sql_query.append("arrival_city!=");
	}
	sql_query.append(InitMySQL::toSQLString(where_sentence["arrival_city"]));

	sql_query.append(" AND ");

	sql_query.append("start_time>=");
	sql_query.append(InitMySQL::toSQLString(where_sentence["start_time"]));

	if (where_sentence.find("arrival_time") != where_sentence.end())
	{
		sql_query.append(" AND ");

		sql_query.append("arrival_time<=");
		sql_query.append(InitMySQL::toSQLString(where_sentence["arrival_time"]));
	}
	
	if (where_sentence.find("mileage") != where_sentence.end()&& table_name.find("��")!=string::npos)
	{
		sql_query.append(" AND ");

		sql_query.append("mileage<");
		sql_query.append(InitMySQL::toSQLString(where_sentence["mileage"]));
	}

	if (where_sentence.find("cost_time") != where_sentence.end() && table_name.find("����") != string::npos)
	{
		sql_query.append(" AND ");

		sql_query.append("cost_time<");
		sql_query.append(InitMySQL::toSQLString(where_sentence["cost_time"]));
	}

	sql_query.append(getOrderSentence(now_index));

	return sql_query;
}

string GetRoute::getSQLQuerySecondRouteOfTrans(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence)
{
	string sql_query = "SELECT DISTINCT ";

	if (columns.size())
	{
		for (int i = 0; i < columns.size(); i++)
		{
			sql_query.append(columns[i]);
			if (i != columns.size() - 1)
			{
				sql_query.append(",");
			}
		}
	}
	else
	{
		sql_query.append("*");
	}

	sql_query.append(" FROM ");
	if (this->requirement.transitType[now_index] == UserRequirementNamespace::FIX_TRANS)//����ǻ����ת ����ȡ����ֻ�����requirement.vehicleType����ȡ ���Ի����תʱ Ҫ�ѱ����ĳ���һ��
	{
		int start_index_train = table_name.find("��");
		int start_index_airplane = table_name.find("����");
		if (start_index_train !=string::npos)
		{
			table_name.replace(start_index_train,4, "����");
		}
		else if (start_index_airplane != string::npos)
		{
			table_name.replace(start_index_airplane, 4, "��");
		}
	}
	sql_query.append(table_name);
	sql_query.append(" WHERE ");

	if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
	{
		sql_query.append("start_station=");
		sql_query.append(InitMySQL::toSQLString(where_sentence["start_station"]));
	}
	else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
	{
		sql_query.append("start_airport=");
		sql_query.append(InitMySQL::toSQLString(where_sentence["start_airport"]));
	}

	sql_query.append(" AND ");

	sql_query.append("arrival_city=");
	sql_query.append(InitMySQL::toSQLString(where_sentence["arrival_city"]));

	sql_query.append(" AND ");

	sql_query.append("start_time>=");
	sql_query.append(InitMySQL::toSQLString(where_sentence["start_time"]));

	if (where_sentence.find("arrival_time") != where_sentence.end())
	{
		sql_query.append(" AND ");

		sql_query.append("arrival_time<=");
		sql_query.append(InitMySQL::toSQLString(where_sentence["arrival_time"]));
	}
	if (where_sentence.find("mileage") != where_sentence.end() && requirement.vehicleType[now_index]==UserRequirementNamespace::HSRC)
	{
		sql_query.append(" AND ");

		sql_query.append("mileage<");
		sql_query.append(InitMySQL::toSQLString(where_sentence["mileage"]));
	}

	if (where_sentence.find("cost_time") != where_sentence.end() && requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
	{
		sql_query.append(" AND ");

		sql_query.append("cost_time<");
		sql_query.append(InitMySQL::toSQLString(where_sentence["cost_time"]));
	}

	if(this->requirement.vehicleType[now_index]==UserRequirementNamespace::ALL_VEHICLE)
	{
		if (table_name.find("��") != string::npos)
		{
			sql_query.append(getOrderSentence(now_index,UserRequirementNamespace::HSRC));
		}
		else if (table_name.find("����") != string::npos)
		{
			sql_query.append(getOrderSentence(now_index, UserRequirementNamespace::AIRPLANE));
		}
	}
	else
	{
		sql_query.append(getOrderSentence(now_index));
	}

	return sql_query;
}

string GetRoute::getSQLQuerySecondRouteOfTrans(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence, UserRequirementNamespace::VehicleTypeEnum vehicle_type)
{
	string sql_query = "SELECT DISTINCT ";

	if (columns.size())
	{
		for (int i = 0; i < columns.size(); i++)
		{
			sql_query.append(columns[i]);
			if (i != columns.size() - 1)
			{
				sql_query.append(",");
			}
		}
	}
	else
	{
		sql_query.append("*");
	}

	sql_query.append(" FROM ");
	sql_query.append(table_name);
	sql_query.append(" WHERE ");

	if (vehicle_type == UserRequirementNamespace::HSRC)
	{
		sql_query.append("start_station=");
		sql_query.append(InitMySQL::toSQLString(where_sentence["start_station"]));
	}
	else if (vehicle_type == UserRequirementNamespace::AIRPLANE)
	{
		sql_query.append("start_airport=");
		sql_query.append(InitMySQL::toSQLString(where_sentence["start_airport"]));
	}

	sql_query.append(" AND ");

	sql_query.append("arrival_city=");
	sql_query.append(InitMySQL::toSQLString(where_sentence["arrival_city"]));

	sql_query.append(" AND ");

	sql_query.append("start_time>=");
	sql_query.append(InitMySQL::toSQLString(where_sentence["start_time"]));

	if (where_sentence.find("arrival_time") != where_sentence.end())
	{
		sql_query.append(" AND ");

		sql_query.append("arrival_time<=");
		sql_query.append(InitMySQL::toSQLString(where_sentence["arrival_time"]));
	}
	if (where_sentence.find("mileage") != where_sentence.end() && vehicle_type==UserRequirementNamespace::HSRC)
	{
		sql_query.append(" AND ");

		sql_query.append("mileage<");
		sql_query.append(InitMySQL::toSQLString(where_sentence["mileage"]));
	}

	if (where_sentence.find("cost_time") != where_sentence.end() && vehicle_type==UserRequirementNamespace::AIRPLANE)
	{
		sql_query.append(" AND ");

		sql_query.append("cost_time<");
		sql_query.append(InitMySQL::toSQLString(where_sentence["cost_time"]));
	}
	sql_query.append(getOrderSentence(now_index, vehicle_type));

	return sql_query;
}

vector<string> GetRoute::getSQLQueryVector(int now_index)
{
	if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::ALL_VEHICLE)
	{
		UserRequirementAfterPretreat temp_requirement1 = this->requirement;
		UserRequirementAfterPretreat temp_requirement2 = this->requirement;
		temp_requirement1.vehicleType[now_index] = UserRequirementNamespace::HSRC;
		temp_requirement2.vehicleType[now_index] = UserRequirementNamespace::AIRPLANE;
		GetRoute temp_get_route_instance_1(temp_requirement1);
		GetRoute temp_get_route_instance_2(temp_requirement2);

		return vector<string>
		{
			temp_get_route_instance_1.getSQLQuery(now_index, vector<string>(), temp_get_route_instance_1.getTableName(now_index), temp_get_route_instance_1.getWhereSentenceKeyValue(now_index)),
			temp_get_route_instance_2.getSQLQuery(now_index, vector<string>(), temp_get_route_instance_2.getTableName(now_index), temp_get_route_instance_2.getWhereSentenceKeyValue(now_index))
		};
	}
	else
	{
		return vector<string>
		{
			getSQLQuery(now_index, vector<string>(), getTableName(now_index), getWhereSentenceKeyValue(now_index))
		};
	}
}

vector<vector<vector<Vehicle*>>> GetRoute::getWeights()
{
	return this->weights;
}

string GetRoute::getMaxMileage(string start_city, string arrival_city)
{
	string sql = "SELECT MAX(mileage) FROM ";
	sql.append(start_city).append("��");
	sql.append(" WHERE arrival_city='").append(arrival_city).append("';");
	MYSQL_RES* res = InitMySQL::execSQLToGetResult(sql);
	if (res == NULL)
		return "";
	MYSQL_ROW row = mysql_fetch_row(res);
	return row[0];
}
