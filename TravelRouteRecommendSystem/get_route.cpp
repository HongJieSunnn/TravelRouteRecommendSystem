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
		//每次读取完一行的前半段 直接读取所有后半段 并且把符合条件的都放到weighs中
		vector<vector<Vehicle*>> temp_weights;
		while (row != NULL)
		{
			Vehicle* first_route=new Vehicle();//第一段路
			string start_city_name;
			//前半段
			switch (this->requirement.vehicleType[now_index])
			{
			case UserRequirementNamespace::AIRPLANE:
				first_route = (new AirPlane
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11], row[12]
				));
				break;
			case UserRequirementNamespace::HSRC:
				first_route = (new HSRC
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11],
					row[12], row[13]
				));
				break;
			case UserRequirementNamespace::ALL_VEHICLE:
				if (sql_query[i].find("火车") != string::npos)
				{
					first_route = new HSRC
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11],
						row[12], row[13]
					);
				}
				else
				{
					first_route = new AirPlane
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11], row[12]
					);
					break;
				}
				break;
			}
			start_city_name = first_route->get_arrival_city();
			auto second_route_statue = getTransitVehicleInforSecondRoute(now_index, start_city_name, first_route, temp_weights);
			if (second_route_statue != GetTransitVehicleStatue::GET_ONE_SECOND_ROUTE_INFOR_SUCCEED)
			{
				if (second_route_statue == GetTransitVehicleStatue::SELECT_RESULT_NO_SECOND_ROUTE|| second_route_statue == GetTransitVehicleStatue::SELECT_RESULT_NO_TABLE_EXIST)
				{
					row = mysql_fetch_row(res);
					delete first_route;
					continue;
				}
				return GetTransitVehicleStatue::GET_TRANSIT_VEHICLE__GET_FAILED_SECOND_ROUTE_ERROR;
			}
			if (temp_weights.size() >= 20)
			{
				delete first_route;
				break;
			}
			row = mysql_fetch_row(res);
			delete first_route;
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
		//每次读取完一行的前半段 直接读取所有后半段 并且把符合条件的都放到weighs中
		vector<vector<Vehicle*>> temp_weights;
		while (row != NULL)
		{
			Vehicle* first_route = new Vehicle();//第一段路
			string start_city_name;
			//前半段
			switch (this->requirement.vehicleType[now_index])
			{
			case UserRequirementNamespace::AIRPLANE:
				first_route = (new AirPlane
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11], row[12]
				));
				break;
			case UserRequirementNamespace::HSRC:
				first_route = (new HSRC
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11],
					row[12], row[13]
				));
				break;
			case UserRequirementNamespace::ALL_VEHICLE:
				if (sql_query[i].find("火车") != string::npos)
				{
					first_route = new HSRC
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11],
						row[12], row[13]
					);
				}
				else
				{
					first_route = new AirPlane
					(
						row[0], row[1], row[2], row[3], row[4], row[5],
						row[6], row[7], row[8], row[9], row[10], row[11], row[12]
					);
					break;
				}
				break;
			}
			start_city_name = first_route->get_arrival_city();
			auto second_route_statue = getTransitVehicleInforSecondRoute(now_index, start_city_name, first_route, temp_weights);
			if (second_route_statue != GetTransitVehicleStatue::GET_ONE_SECOND_ROUTE_INFOR_SUCCEED)
			{
				if (second_route_statue == GetTransitVehicleStatue::SELECT_RESULT_NO_SECOND_ROUTE || second_route_statue == GetTransitVehicleStatue::SELECT_RESULT_NO_TABLE_EXIST)
				{
					row = mysql_fetch_row(res);
					delete first_route;
					continue;
				}
				return GetFixVehicleStatue::GET_TRANSIT_VEHICLE__GET_FAILED_SECOND_ROUTE_ERROR;
			}
			if (temp_weights.size() >= 20)
			{
				delete first_route;
				break;
			}
			row = mysql_fetch_row(res);
			delete first_route;
		}
		weights[now_index] = temp_weights;
	}
	return GetFixVehicleStatue::GET_FIX_VEHICLE_SUCCEED;
}

GetRouteNameSpace::GetTransitVehicleStatue GetRoute::getTransitVehicleInforSecondRoute(int now_index, string start_city_name, Vehicle* first_route, vector<vector<Vehicle*>>& temp_weights)
{
	UserRequirementNamespace::VehicleTypeEnum vehicle_type = first_route->getVehicleType() == HSRC_TYPE ? UserRequirementNamespace::HSRC : UserRequirementNamespace::AIRPLANE;

	string sql = getSQLQuerySecondRouteOfTrans(
		now_index,
		vector<string>(),
		getTableName(start_city_name, vehicle_type),
		getWhereSentenceKeyValueOfSecondRouteOfTrans(now_index, first_route, vehicle_type),
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
	
	while (row_second != NULL)
	{
		vector<Vehicle*> route(2);
		route[0] = first_route;
		switch (vehicle_type)
		{
		case UserRequirementNamespace::AIRPLANE:
			route[1] = (new AirPlane
			(
				row_second[0], row_second[1], row_second[2], row_second[3], row_second[4], row_second[5],
				row_second[6], row_second[7], row_second[8], row_second[9], row_second[10], row_second[11], row_second[12]
			));
			if (MyTime::costTime(MyTime::stringToMyTime(route[0]->get_arrival_time(), HH_MM), MyTime::stringToMyTime(route[1]->get_start_time(), HH_MM)) < 25)//如果等待时间小于25分钟
			{
				row_second = mysql_fetch_row(res_second);
				continue;
			}
			else
			{
				temp_weights.push_back(route);
			}
			break;
		case UserRequirementNamespace::HSRC:
			route[1] = (new HSRC
			(
				row_second[0], row_second[1], row_second[2], row_second[3], row_second[4], row_second[5],
				row_second[6], row_second[7], row_second[8], row_second[9], row_second[10], row_second[11],
				row_second[12], row_second[13]
			));
			if (MyTime::costTime(MyTime::stringToMyTime(route[0]->get_arrival_time(), HH_MM), MyTime::stringToMyTime(route[1]->get_start_time(), HH_MM)) < 15)//如果等待时间小于15分钟
			{
				row_second = mysql_fetch_row(res_second);
				continue;
			}
			else
			{
				temp_weights.push_back(route);
			}
			break;
		}
		row_second = mysql_fetch_row(res_second);
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
		where_sentence.insert({ "mileage",to_string(requirement.distances[now_index]) });
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
	string sql_query = "SELECT ";

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

	sql_query.append(getOrderSentence(now_index));

	return sql_query;
}

string GetRoute::getSQLQuerySecondRouteOfTrans(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence)
{
	string sql_query = "SELECT ";

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
	string sql_query = "SELECT ";

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
