#include "get_route.h"
namespace
{
	//往FirstRouteDivideById中判断优先级并且添加键值对 适用于飞机
	void addKeyValueOfFirstRouteDivideByIdAP(string start_city, unordered_map<string, Vehicle*>& first_route_divede_by_id, const MYSQL_ROW& row)
	{
		string start_city_name_of_second = row[8];
		if (start_city_name_of_second == start_city)
		{
			return;
		}
		//如果该班次已存在
		if (first_route_divede_by_id.find(row[0]) != first_route_divede_by_id.end())
		{
			string station_name = row[7];
			int now_priority_level = InitRedis::getPriorityLevel(station_name);
			int old_priority_level = InitRedis::getPriorityLevel(first_route_divede_by_id[row[0]]->get_arrival_station());
			if (now_priority_level != -1 && now_priority_level != 0)
			{
				if (now_priority_level > old_priority_level)//如果新的优先级更大
				{
					first_route_divede_by_id[row[0]] = new AirPlane
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11], row[12]
					);
				}
			}
		}
		else//如果不存在 直接赋值
		{
			first_route_divede_by_id[row[0]] = new AirPlane
			(
				row[0], row[1], row[2], row[3], row[4], row[5],
				row[6], row[7], row[8], row[9], row[10], row[11], row[12]
			);
		}
	}
	//往FirstRouteDivideById中判断优先级并且添加键值对 适用于火车
	void addKeyValueOfFirstRouteDivideByIdHSRC(string start_city,unordered_map<string, Vehicle*>& first_route_divede_by_id, const MYSQL_ROW& row)
	{
		string start_city_name_of_second = row[9];
		if (start_city_name_of_second ==start_city)//同一个城市站点 就不要转了！
		{
			return;
		}
		if (first_route_divede_by_id.find(row[0]) != first_route_divede_by_id.end())
		{
			int now_priority_level = InitRedis::getPriorityLevel(row[8]);
			int old_priority_level = InitRedis::getPriorityLevel(first_route_divede_by_id[row[0]]->get_arrival_station());
			if (now_priority_level != -1 && now_priority_level != 0)
			{
				if (now_priority_level > old_priority_level)//如果新的优先级更大
				{
					first_route_divede_by_id[row[0]] = new HSRC
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11],
						row[12], row[13]
					);
				}
				else if (now_priority_level == old_priority_level)//可能出现同一个train_id 终点站一样 但是出发站不一样的情况
				{
					int now_start_station_priority_level = InitRedis::getPriorityLevel(row[3]);
					int old_start_station_priority_level = InitRedis::getPriorityLevel(first_route_divede_by_id[row[0]]->get_start_station());
					if (now_start_station_priority_level > old_start_station_priority_level)//如果新的优先级更大
					{
						first_route_divede_by_id[row[0]] = new HSRC
						(
							row[0], row[1], row[2], row[3], row[4], row[5],
							row[6], row[7], row[8], row[9], row[10], row[11],
							row[12], row[13]
						);
					}
				}
			}
		}
		else
		{
			first_route_divede_by_id[row[0]] = new HSRC
			(
				row[0], row[1], row[2], row[3], row[4], row[5],
				row[6], row[7], row[8], row[9], row[10], row[11],
				row[12], row[13]
			);
		}
	}
}
using namespace GetRouteNameSpace;

GetRouteResultStatue GetRoute::getRouteResults(RouteResult*& route_result)
{
	if (!sortWeights())
	{
		return GetRouteResultStatue::SORT_WEIGHTS_FAILED;
	}
	int how_many_city_nums = this->weights.size();//城市组数
	for (int i = 0; i < how_many_city_nums; i++)
	{
		if (weights[i].size() == 0)
		{
			string error = "没有可推荐线路(其中第";
			throw std::exception(error.append(to_string(i+1)).append("段路没有结果)").c_str());
		}
	}
	route_result = new RouteResult[weights.size()];
	for (int i = 0; i < weights.size(); i++)
	{
		route_result[i].route = new Vehicle * *[weights[i].size()];
		for (int j = 0; j < weights[i].size(); j++)
		{
			route_result[i].route[j] = new Vehicle * [weights[i][j].size()];
			for (int k = 0; k < weights[i][j].size(); k++)
			{
				route_result[i].route[j][k] = this->weights[i][j][k];
			}
		}
	}
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
		if (res == nullptr)
		{
			return GetDirectVehicleStatue::GET_RES_FAILED;
		}
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
			if (j >= 15)//最多15
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
		if (res == nullptr)
		{
			return GetTransitVehicleStatue::GET_RES_FAILED;
		}
		int rows_count = res->row_count;
		if (rows_count == 0)
			return GetTransitVehicleStatue::SELECT_RESULT_NO_FIRST_ROUTE;

		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(res);
		//哈希表 键是第二段路开始的站点 也就是第一段路的arrival_station 用这个来分类 直接查询时间最早的那个 然后再来分配给对应的第一段路 并且可以开多线程 直接对所有站点同时读
		//通过站点的优先级排序
		map<string, vector<Vehicle*>,MapCmpOfFirstRouteDivideByStation> first_route_divide_by_station;
		//哈希表 键是train_id 值是second_route的start_station 这个表用来使相同的train_id只出现一次 例如 福州——连江 连江-南京 和 福州-罗源 罗源-南京是同一班车 那么我们选取优先级高的那个
		unordered_map<string, Vehicle*> first_route_divede_by_id;
		//最终结果 也就是多个两段路的集合
		vector<vector<Vehicle*>> temp_weights;

		string start_city = this->requirement.start_cities[now_index];
		while (row != NULL)
		{
			//前半段
			switch (this->requirement.vehicleType[now_index])
			{
			case UserRequirementNamespace::AIRPLANE:
				addKeyValueOfFirstRouteDivideByIdAP(start_city,first_route_divede_by_id, row);
				break;
			case UserRequirementNamespace::HSRC:
				addKeyValueOfFirstRouteDivideByIdHSRC(start_city, first_route_divede_by_id, row);
				break;
			case UserRequirementNamespace::ALL_VEHICLE:
				if (sql_query[i].find("火车") != string::npos)
				{
					addKeyValueOfFirstRouteDivideByIdHSRC(start_city, first_route_divede_by_id, row);
				}
				else
				{
					addKeyValueOfFirstRouteDivideByIdAP(start_city, first_route_divede_by_id, row);
				}
				break;
			}
			row = mysql_fetch_row(res);
		}
		//遍历 first_route_divede_by_id 把数据添加到 first_route_divede_by_station中
		for (const auto& train : first_route_divede_by_id)
		{
			first_route_divide_by_station[train.second->get_arrival_station()].push_back(train.second);
		}
		int size = first_route_divide_by_station.size();
		auto routes_one_station = first_route_divide_by_station.begin();
		
		for (int j=0;j<size;j++)
		{
			thread thread_1(&GetRoute::getTransitVehicleInforSecondRoute, this, now_index, routes_one_station->second, ref(temp_weights));
			thread_1.join();

			if (temp_weights.size() == 30)
			{
				break;
			}
			routes_one_station++;
		}
		weights[now_index].insert(weights[now_index].end(),temp_weights.begin(),temp_weights.end());
	}
	return GetTransitVehicleStatue::GET_TRANSIT_VEHICLE_SUCCEED;
}

GetRouteNameSpace::GetFixVehicleStatue GetRoute::getFixVehicleInfor(int now_index, vector<string> sql_query)
{
	auto statue = getTransitVehicleInfor(now_index, sql_query);
	switch (statue)
	{
	case GetRouteNameSpace::GetTransitVehicleStatue::GET_TRANSIT_VEHICLE_FAILED:
		return GetFixVehicleStatue::GET_FIX_VEHICLE_FAILED;
		break;
	case GetRouteNameSpace::GetTransitVehicleStatue::GET_TRANSIT_VEHICLE_SUCCEED:
		return GetFixVehicleStatue::GET_FIX_VEHICLE_SUCCEED;
		break;
	case GetRouteNameSpace::GetTransitVehicleStatue::SELECT_RESULT_NO_FIRST_ROUTE:
		return GetFixVehicleStatue::SELECT_RESULT_NO_FIRST_ROUTE;
		break;
	case GetRouteNameSpace::GetTransitVehicleStatue::SELECT_RESULT_NO_SECOND_ROUTE:
		return GetFixVehicleStatue::SELECT_RESULT_NO_SECOND_ROUTE;
		break;
	case GetRouteNameSpace::GetTransitVehicleStatue::GET_ONE_SECOND_ROUTE_INFOR_SUCCEED:
		return GetFixVehicleStatue::GET_ONE_SECOND_ROUTE_INFOR_SUCCEED;
		break;
	case GetRouteNameSpace::GetTransitVehicleStatue::GET_TRANSIT_VEHICLE__GET_FAILED_SECOND_ROUTE_ERROR:
		return GetFixVehicleStatue::GET_TRANSIT_VEHICLE__GET_FAILED_SECOND_ROUTE_ERROR;
		break;
	case GetRouteNameSpace::GetTransitVehicleStatue::GET_RES_FAILED:
		return GetFixVehicleStatue::GET_RES_FAILED;
		break;
	}
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
	UserRequirementNamespace::VehicleTypeEnum vehicle_type;
	if (this->requirement.transitType[now_index] != UserRequirementNamespace::FIX_TRANS)
	{
		vehicle_type = first_route[0]->getVehicleType() == HSRC_TYPE ? UserRequirementNamespace::HSRC : UserRequirementNamespace::AIRPLANE;
	}
	else//如果是FIX类型 取和传入的不同的类型
	{
		vehicle_type = first_route[0]->getVehicleType() == HSRC_TYPE ? UserRequirementNamespace::AIRPLANE : UserRequirementNamespace::HSRC;
	}
	string start_city_name = first_route[0]->get_arrival_city();

	string sql = getSQLQuerySecondRouteOfTrans(
		now_index,
		vector<string>(),
		getTableName(start_city_name, vehicle_type),
		getWhereSentenceKeyValueOfSecondRouteOfTrans(now_index, first_route[0], vehicle_type),
		vehicle_type
	);
	//后半段
	mu.lock();
	MYSQL_RES* res_second = InitMySQL::execSQLToGetResult(sql);//查询数据库
	mu.unlock();
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
				if (MyTime::costTime(MyTime::stringToMyTime(one_of_first_route->get_arrival_time(), HH_MM), MyTime::stringToMyTime(one_of_second_route->get_start_time(), HH_MM)) > 20&&
					MyTime::costTime(MyTime::stringToMyTime(one_of_first_route->get_arrival_time(), HH_MM), MyTime::stringToMyTime(one_of_second_route->get_start_time(), HH_MM)) < 120)
					//时间间隔差20分钟往上 120分钟往下
				{
					if (temp_weights.size() <= 30)
					{
						temp_weights.emplace_back(vector<Vehicle*>{one_of_first_route, one_of_second_route});
					}
					else
					{
						return GetTransitVehicleStatue::GOT_THIRDTY_WEIGHTS;
					}
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
	where_sentence.insert({ "start_time",requirement.start_time[now_index].myTimeToStringByInt(HH_MM) });
	if (requirement.arrive_time[now_index].format != 0 && requirement.vehicleType[now_index] != UserRequirementNamespace::TRANS && requirement.vehicleType[now_index] != UserRequirementNamespace::FIX_TRANS)
	{
		if (requirement.start_time[now_index].day < requirement.arrive_time[now_index].day)
		{
			requirement.arrive_time[now_index].hour += (requirement.arrive_time[now_index].day - requirement.start_time[now_index].day) * 24;
		}
		where_sentence.insert({ "arrival_time",requirement.arrive_time[now_index].myTimeToStringByInt(HH_MM) });
	}
	if (requirement.transitType[now_index] == UserRequirementNamespace::TRANS)
	{
		where_sentence.insert({ "mileage",to_string(requirement.distances[now_index] * 0.65) });//火车
		string repitive_train_id = "train_id NOT IN (SELECT train_id FROM ";
		where_sentence.insert({ "train_id",repitive_train_id.
			append(getTableName(requirement.start_cities[now_index],UserRequirementNamespace::HSRC)).append(" WHERE arrival_city=").
			append(InitMySQL::toSQLString(requirement.arrive_cities[now_index]).append(")")) });

		int minute_of_all = requirement.distances[now_index] / 800.0 * 60.0 * 0.65;
		int hour = minute_of_all / 60;
		int minute = minute_of_all % 60;
		string cost_time = to_string(hour).append("小时").append(to_string(minute)).append("分钟");
		where_sentence.insert({ "cost_time",cost_time });
	}
	return where_sentence;
}

unordered_map<string, string> GetRoute::getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index, Vehicle* vehicle, UserRequirementNamespace::VehicleTypeEnum vehicle_type)
{
	unordered_map<string, string> where_sentence;
	if (this->requirement.transitType[now_index] == UserRequirementNamespace::FIX_TRANS)
	{
		where_sentence.insert({ "start_city",vehicle->get_arrival_city() });
	}
	else
	{
		where_sentence.insert({ "start_station",vehicle->get_arrival_station() });
	}

	where_sentence.insert({ "arrival_city",requirement.arrive_cities[now_index] });
	where_sentence.insert({ "start_time",vehicle->get_arrival_time() });
	if (requirement.arrive_time[now_index].format != 0)
	{
		if (requirement.start_time[now_index].day < requirement.arrive_time[now_index].day)
		{
			requirement.arrive_time[now_index].hour += (requirement.arrive_time[now_index].day - requirement.start_time[now_index].day) * 24;
		}
		where_sentence.insert({ "arrival_time",requirement.arrive_time[now_index].myTimeToStringByInt(HH_MM) });
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

	if (where_sentence.find("train_id") != where_sentence.end() && table_name.find("火车") != string::npos)
	{
		sql_query.append(" AND ").append(where_sentence["train_id"]);
	}

	sql_query.append(getOrderSentence(now_index));

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
		if (this->requirement.transitType[now_index] == UserRequirementNamespace::FIX_TRANS)
		{
			sql_query.append("start_city=");
			sql_query.append(InitMySQL::toSQLString(where_sentence["start_city"]));
		}
		else
		{
			sql_query.append("start_station=");
			sql_query.append(InitMySQL::toSQLString(where_sentence["start_station"]));
		}
	}
	else if (vehicle_type == UserRequirementNamespace::AIRPLANE)
	{
		if (this->requirement.transitType[now_index] == UserRequirementNamespace::FIX_TRANS)
		{
			sql_query.append("start_city=");
			sql_query.append(InitMySQL::toSQLString(where_sentence["start_city"]));
		}
		else
		{
			sql_query.append("start_airport=");
			sql_query.append(InitMySQL::toSQLString(where_sentence["start_station"]));
		}
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