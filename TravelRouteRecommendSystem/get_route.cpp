#include "get_route.h"

using namespace GetRouteNameSpace;

GetRouteNameSpace::GetVehicleStatue GetRoute::getVechileInfor()
{
    bool get_infor_failed = false;//��������������ȻԤ������� ��ֻ�ǳ������� ���ܻ�����һЩ����ᵼ������������ ����û��ֱ�� ��ô��Ҫ����ȥ��ȡ��ת...
    bool requirement_changed = false;//ֻ������ı��� ���ǲŻ�����ȥ���� ������Ȼ��ȡʧ�� ������������û�иı�
	bool transit_can_change_tag = false;
	int cities_num = requirement.start_cities.size();

	do
	{
		get_infor_failed = false;
		requirement_changed = false;
		for (int i = 0; i < cities_num; i++)
		{
			vertex_datas.push_back(this->requirement.start_cities[i]);//��ʼ���з���ͼ�ĵ���
			vertex_datas.push_back(this->requirement.arrive_cities[i]);//������з���ͼ�ĵ���
			edges.push_back({ this->requirement.start_cities[i],this->requirement.arrive_cities[i] });//��ӱ�
			switch (this->requirement.transitType[i])
			{
			case UserRequirementNamespace::DIRECT:
				getDirectVehicleInfor(i,getSQLQueryVector(i));
				break;
			case UserRequirementNamespace::TRANS:
				break;
			case UserRequirementNamespace::FIX_TRANS:
				break;
			case UserRequirementNamespace::ALL_TRANSIT:
				
				break;
			}
		}
	} while (get_infor_failed && requirement_changed);
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
		weights = vector<vector<Vehicle*>>(rows_count, vector<Vehicle*>(1));
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
			}
			weights[i]=vehicle;
			row = mysql_fetch_row(res);
		}
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
		//ÿ�ζ�ȡ��һ�е�ǰ��� ֱ�Ӷ�ȡ���к��� ���Ұѷ��������Ķ��ŵ�weighs��
		while (row != NULL)
		{
			Vehicle* first_route;//��һ��·
			string start_city_name;
			//ǰ���
			switch (this->requirement.vehicleType[now_index])
			{
			case UserRequirementNamespace::AIRPLANE:
				first_route = (new AirPlane
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11], row[12]
				));
				start_city_name = first_route->getAP().start_city;
				break;
			case UserRequirementNamespace::HSRC:
				first_route = (new HSRC
				(
					row[0], row[1], row[2], row[3], row[4], row[5],
					row[6], row[7], row[8], row[9], row[10], row[11],
					row[12], row[13]
				));
				start_city_name = first_route->getHSRC().start_city;
				break;
			}
			if (getTransitVehicleInforSecondRoute(now_index, start_city_name, first_route) != GetTransitVehicleStatue::GET_ONE_SECOND_ROUTE_INFOR_SUCCEED)
			{
				return GetTransitVehicleStatue::GET_TRANSIT_VEHICLE__GET_FAILED_SECOND_ROUTE_ERROR;
			}
		}
		
	}
	return GetTransitVehicleStatue::GET_TRANSIT_VEHICLE_SUCCEED;
}

GetRouteNameSpace::GetTransitVehicleStatue GetRoute::getTransitVehicleInforSecondRoute(int now_index,string start_city_name, Vehicle* first_route)
{
	//����
	MYSQL_RES* res_second = InitMySQL::execSQLToGetResult(getSQLQuerySecondRouteOfTrans(
		now_index,
		vector<string>(),
		getTableName(now_index, start_city_name),
		getWhereSentenceKeyValueOfSecondRouteOfTrans(now_index, first_route)
	));//��ѯ���ݿ�

	int rows_count_second = res_second->row_count;
	if (rows_count_second == 0)
		return GetTransitVehicleStatue::SELECT_RESULT_NO_SECOND_ROUTE;
	MYSQL_ROW row_second = NULL;
	row_second = mysql_fetch_row(res_second);
	while (row_second != NULL)
	{
		vector<Vehicle*> route(2);
		route[0] = first_route;
		switch (this->requirement.vehicleType[now_index])
		{
		case UserRequirementNamespace::AIRPLANE:
			route[1] = (new AirPlane
			(
				row_second[0], row_second[1], row_second[2], row_second[3], row_second[4], row_second[5],
				row_second[6], row_second[7], row_second[8], row_second[9], row_second[10], row_second[11], row_second[12]
			));
			if (MyTime::costTime(MyTime::stringToMyTime(route[0]->getAP().arrival_time, HH_MM), MyTime::stringToMyTime(route[1]->getAP().start_time, HH_MM)) < 25)//����ȴ�ʱ��С��25����
			{
				row_second = mysql_fetch_row(res_second);
				continue;
			}
			else
			{
				weights.push_back(route);
			}
			break;
		case UserRequirementNamespace::HSRC:
			route[1] = (new HSRC
			(
				row_second[0], row_second[1], row_second[2], row_second[3], row_second[4], row_second[5],
				row_second[6], row_second[7], row_second[8], row_second[9], row_second[10], row_second[11],
				row_second[12], row_second[13]
			));
			if (MyTime::costTime(MyTime::stringToMyTime(route[0]->getHSRC().arrival_time, HH_MM), MyTime::stringToMyTime(route[1]->getHSRC().start_time, HH_MM)) < 15)//����ȴ�ʱ��С��15����
			{
				row_second = mysql_fetch_row(res_second);
				continue;
			}
			else
			{
				weights.push_back(route);
			}
			break;
		}
		row_second = mysql_fetch_row(res_second);
	}
	return GetTransitVehicleStatue::GET_ONE_SECOND_ROUTE_INFOR_SUCCEED;
}

GetRouteNameSpace::GetFixVehicleStatue GetRoute::getFixVehicleInfor(int now_index, vector<string> sql_query)
{
	return GetRouteNameSpace::GetFixVehicleStatue();
}

GetRouteNameSpace::GetFixVehicleStatue GetRoute::getAllTransitVehicleInfor(int now_index, vector<string> sql_query)
{
	return GetRouteNameSpace::GetFixVehicleStatue();
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
	return where_sentence;
}

unordered_map<string, string> GetRoute::getWhereSentenceKeyValueOfSecondRouteOfTrans(int now_index,Vehicle* vehicle)
{
	unordered_map<string, string> where_sentence;
	//��ת�ܲ��ܻ�����һ��վȥ�� ������Ҫstart��վ��
	if(this->requirement.vehicleType[now_index]==UserRequirementNamespace::HSRC)
		where_sentence.insert({ "start_station",vehicle->getHSRC().start_station });
	else if(this->requirement.vehicleType[now_index] == UserRequirementNamespace::AIRPLANE)
		where_sentence.insert({ "start_airport",vehicle->getAP().start_airport });

	where_sentence.insert({ "arrival_city",requirement.arrive_cities[now_index] });
	where_sentence.insert({ "start_time",requirement.start_time.myTimeToStringByInt(HH_MM) });
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
		now_start_city.append("��");
		break;
	case UserRequirementNamespace::AIRPLANE:
		now_start_city.append("����");
		break;
	}
	return now_start_city;
}

string GetRoute::getTableName(int now_index,string city_name)
{
	switch (this->requirement.vehicleType[now_index])
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

	sql_query.append(getOrderSentence(now_index));

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
			temp_get_route_instance_1.getSQLQuery(now_index, vector<string>(), getTableName(now_index), getWhereSentenceKeyValue(now_index)),
			temp_get_route_instance_2.getSQLQuery(now_index, vector<string>(), getTableName(now_index), getWhereSentenceKeyValue(now_index))
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
