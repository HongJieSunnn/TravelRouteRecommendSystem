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
		vertex_datas[i]=(this->requirement.start_cities[i]);//开始城市放入图的点中
		vertex_datas[i+1]=(this->requirement.arrive_cities[i]);//到达城市放入图的点中
		edges[i]={ this->requirement.start_cities[i],this->requirement.arrive_cities[i] };//添加边
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
				if (sql_query[i].find("火车") != string::npos)
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
			if (j >= 10)//最多20
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
		//哈希表 键是第二段路开始的站点 也就是第一段路的arrival_station 用这个来分类 直接查询时间最早的那个 然后再来分配给对应的第一段路 并且可以开多线程 直接对所有站点同时读
		unordered_map<string, vector<Vehicle*>> first_route_divide_by_station;
		//最终结果 也就是多个两段路的集合
		vector<vector<Vehicle*>> temp_weights;
		while (row != NULL)
		{
			//前半段
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
				if (sql_query[i].find("火车") != string::npos)
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
		int async_times = size / 5;//5线程个同时进行的次数
		int sync_times = size % 5;//一个线程一个线程执行 其实就相当于同步执行了
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
		//哈希表 键是第二段路开始的站点 也就是第一段路的arrival_station 用这个来分类 直接查询时间最早的那个 然后再来分配给对应的第一段路 并且可以开多线程 直接对所有站点同时读
		unordered_map<string, vector<Vehicle*>> first_route_divide_by_station;
		//最终结果 也就是多个两段路的集合
		vector<vector<Vehicle*>> temp_weights;
		while (row != NULL)
		{
			//前半段
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
				if (sql_query[i].find("火车") != string::npos)
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
		int async_times = size / 5;//5线程个同时进行的次数
		int sync_times = size % 5;//一个线程一个线程执行 其实就相当于同步执行了
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
		});//按照arrival_time(即第二段的start_time)来排序 然后对最小的那个进行数据库查询就好了

	//因为同一个first_route集合到达站一定一样 即便是all_vehicle first_route也不会出现飞机和火车一起出现 因为站点名不一样的
	UserRequirementNamespace::VehicleTypeEnum vehicle_type = first_route[0]->getVehicleType() == HSRC_TYPE ? UserRequirementNamespace::HSRC : UserRequirementNamespace::AIRPLANE;
	string start_city_name = first_route[0]->get_arrival_city();

	string sql = getSQLQuerySecondRouteOfTrans(
		now_index,
		vector<string>(),
		getTableName(start_city_name, vehicle_type),
		getWhereSentenceKeyValueOfSecondRouteOfTrans(now_index, first_route[0], vehicle_type),
		vehicle_type
	);
	//后半段
	MYSQL_RES* res_second = InitMySQL::execSQLToGetResult(sql);//查询数据库
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
	//写在上面或者新的二重循环复杂度都是n^2 但是写外面好一些感觉 不用考虑自己内存释放的问题（除非使用智能指针 但是那样又有点麻烦）

	for (auto one_of_second_route : second_route)
	{
		for (auto one_of_first_route : first_route)
		{
			if (one_of_first_route->get_arrival_time() < one_of_second_route->get_start_time())//第二段出发时间晚于第一段到达时间
			{
				if (MyTime::costTime(MyTime::stringToMyTime(one_of_first_route->get_arrival_time(), HH_MM), MyTime::stringToMyTime(one_of_second_route->get_start_time(), HH_MM)) < 20)//时间间隔差20分钟往上
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
		where_sentence.insert({ "mileage",to_string(requirement.distances[now_index]*0.65) });//火车

		int minute_of_all = requirement.distances[now_index] / 800.0*60.0*0.65;
		int hour = minute_of_all / 60;
		int minute = minute_of_all % 60;
		string cost_time=to_string(hour).append("小时").append(to_string(minute)).append("分钟");
		where_sentence.insert({ "cost_time",cost_time });
	}
	return where_sentence;
}

unordered_map<string, string> GetRoute::getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index,Vehicle* vehicle)
{
	unordered_map<string, string> where_sentence;
	//中转总不能还跑另一个站去吧 所以需要start的站点
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
	where_sentence.insert({ "mileage",to_string(requirement.distances[now_index] * 0.60) });//火车

	int minute_of_all = requirement.distances[now_index] / 800.0 * 60.0 * 0.65;
	int hour = minute_of_all / 60;
	int minute = minute_of_all % 60;
	string cost_time = to_string(hour).append("小时").append(to_string(minute)).append("分钟");
	where_sentence.insert({ "cost_time",cost_time });
	return where_sentence;
}

unordered_map<string, string> GetRoute::getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index,Vehicle* vehicle, UserRequirementNamespace::VehicleTypeEnum vehicle_type)
{
	unordered_map<string, string> where_sentence;
	//中转总不能还跑另一个站去吧 所以需要start的站点
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
	where_sentence.insert({ "mileage",to_string(requirement.distances[now_index] * 0.65) });//火车

	int minute_of_all = requirement.distances[now_index] / 800.0 * 60.0 * 0.65;
	int hour = minute_of_all / 60;
	int minute = minute_of_all % 60;
	string cost_time = to_string(hour).append("小时").append(to_string(minute)).append("分钟");
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
		now_start_city.append("火车");
		break;
	case UserRequirementNamespace::AIRPLANE:
		now_start_city.append("航班");
		break;
	}
	return now_start_city;
}

string GetRoute::getTableName(string city_name,UserRequirementNamespace::VehicleTypeEnum vehicle)
{
	switch (vehicle)
	{
	case UserRequirementNamespace::HSRC:
		city_name.append("火车");
		break;
	case UserRequirementNamespace::AIRPLANE:
		city_name.append("航班");
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
		case UserRequirementNamespace::PRICE_REGARDLESS://时间 价格都最低优先
			if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,second_class_seat_price;");
			}
			else if (this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,ticket_price;");
			}
			break;
		case UserRequirementNamespace::PRICE_CHEAP://先按价格低排序
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
		case UserRequirementNamespace::PRICE_REGARDLESS://时间 价格都最低优先
			sql_query.append("cost_time,start_time;");
			break;
		case UserRequirementNamespace::PRICE_CHEAP://先按价格低排序
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
		case UserRequirementNamespace::PRICE_REGARDLESS://时间 价格都最低优先
			sql_query.append("cost_time;");
			break;
		case UserRequirementNamespace::PRICE_CHEAP://先按价格低排序
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
		case UserRequirementNamespace::PRICE_REGARDLESS://时间 价格都最低优先
			if (vehicle == UserRequirementNamespace::HSRC)
			{
				sql_query.append("cost_time,second_class_seat_price;");
			}
			else if (vehicle == UserRequirementNamespace::AIRPLANE)
			{
				sql_query.append("cost_time,ticket_price;");
			}
			break;
		case UserRequirementNamespace::PRICE_CHEAP://先按价格低排序
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
		case UserRequirementNamespace::PRICE_REGARDLESS://时间 价格都最低优先
			sql_query.append("cost_time,start_time;");
			break;
		case UserRequirementNamespace::PRICE_CHEAP://先按价格低排序
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
		case UserRequirementNamespace::PRICE_REGARDLESS://时间 价格都最低优先
			sql_query.append("cost_time;");
			break;
		case UserRequirementNamespace::PRICE_CHEAP://先按价格低排序
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
	
	if (where_sentence.find("mileage") != where_sentence.end()&& table_name.find("火车")!=string::npos)
	{
		sql_query.append(" AND ");

		sql_query.append("mileage<");
		sql_query.append(InitMySQL::toSQLString(where_sentence["mileage"]));
	}

	if (where_sentence.find("cost_time") != where_sentence.end() && table_name.find("航班") != string::npos)
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
	if (this->requirement.transitType[now_index] == UserRequirementNamespace::FIX_TRANS)//如果是混合中转 而获取表名只会根据requirement.vehicleType来获取 所以混合中转时 要把表名改成另一种
	{
		int start_index_train = table_name.find("火车");
		int start_index_airplane = table_name.find("航班");
		if (start_index_train !=string::npos)
		{
			table_name.replace(start_index_train,4, "航班");
		}
		else if (start_index_airplane != string::npos)
		{
			table_name.replace(start_index_airplane, 4, "火车");
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
		if (table_name.find("火车") != string::npos)
		{
			sql_query.append(getOrderSentence(now_index,UserRequirementNamespace::HSRC));
		}
		else if (table_name.find("航班") != string::npos)
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
	sql.append(start_city).append("火车");
	sql.append(" WHERE arrival_city='").append(arrival_city).append("';");
	MYSQL_RES* res = InitMySQL::execSQLToGetResult(sql);
	if (res == NULL)
		return "";
	MYSQL_ROW row = mysql_fetch_row(res);
	return row[0];
}
