#include "get_route.h"

using namespace GetRouteNameSpace;

GetRouteNameSpace::GetVehicleStatue GetRoute::getVechileInfor()
{
    bool get_infor_failed = false;//��������������ȻԤ������� ��ֻ�ǳ������� ���ܻ�����һЩ����ᵼ������������ ����û��ֱ�� ��ô��Ҫ����ȥ��ȡ��ת...
    bool requirement_changed = false;//ֻ������ı��� ���ǲŻ�����ȥ���� ������Ȼ��ȡʧ�� ������������û�иı�
	int cities_num = requirement.start_cities.size();

	do
	{
		for (int i = 0; i < cities_num; i++)
		{

		}
	} while (get_infor_failed && requirement_changed);
	return GetVehicleStatue::GET_VEHICLE_SUCCEED;
}

GetRouteNameSpace::GetDirectVehicleStatue GetRoute::getDirectVehicleInfor()
{
	for (int i = 0; i < requirement.start_cities.size(); i++)
	{
		
	}
	

	return GetDirectVehicleStatue::GET_DIRECT_VEHICLE_SUCCEED;
}

GetRouteNameSpace::GettransitVehicleStatue GetRoute::getTransitVehicleInfor()
{
	return GetRouteNameSpace::GettransitVehicleStatue();
}

GetRouteNameSpace::GetFixVehicleStatue GetRoute::getFixVehicleInfor()
{
	return GetRouteNameSpace::GetFixVehicleStatue();
}

unordered_map<string, string> GetRoute::getWhereSentenceKeyValue(int now_index)
{
	unordered_map<string, string> where_sentence;
	where_sentence.insert({ "start_city",requirement.start_cities[now_index] });
	where_sentence.insert({ "arrival_city",requirement.arrive_cities[now_index] });
	where_sentence.insert({ "start_time",requirement.start_time.myTimeToStringByInt(HH_MM) });
	if (requirement.arrive_time.format != 0)
	{
		where_sentence.insert({ "arrival_time",requirement.arrive_time.myTimeToStringByInt(HH_MM) });
	}
	return where_sentence;
}

string GetRoute::getTableName(int now_index)
{
	{
		string now_start_city = requirement.start_cities[now_index];
		UserRequirementNamespace::VehicleTypeEnum now_vehicle_type = requirement.vehicleType[now_index];

		switch (now_vehicle_type)
		{
		case UserRequirementNamespace::HSRC:
			now_start_city.append("��·");
			break;
		case UserRequirementNamespace::AIRPLANE:
			now_start_city.append("����");
			break;
		}
		return now_start_city;
	}
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

	return sql_query;
}
