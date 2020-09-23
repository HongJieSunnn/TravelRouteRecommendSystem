#include"route.h"
#pragma warning(disable:4996)
void vehicleToRoute(Vehicle* vehicle,Route& route)
{
	auto vehicle_type = vehicle->getVehicleType();
	route.vehicle_type = new char[vehicle_type.length()];
	strcpy(route.vehicle_type, vehicle_type.c_str());
	if (route.vehicle_type == AIRPLANE_TYPE)
	{
		auto id= vehicle->get_vehicle_id();
		auto plane_type= vehicle->get_plane_type();
		auto start_time = vehicle->get_start_time();
		auto start_station = vehicle->get_start_station();
		auto start_city = vehicle->get_start_city();
		auto arrival_time = vehicle->get_arrival_time();
		auto arrival_station = vehicle->get_arrival_station();;
		auto arrival_city = vehicle->get_arrival_city();
		auto puntuality_rate = vehicle->get_puntuality_rate();
		auto ticket_price = vehicle->get_ticket_price();
		auto discount = vehicle->get_discount();
		auto other = vehicle->get_other();
		auto cost_time = vehicle->get_cost_time();

		route.id = new char[id.length()];
		route.start_time = new char[start_time.length()];
		route.start_station = new char[start_station.length()];
		route.start_city = new char[start_city.length()];
		route.arrival_time = new char[arrival_time.length()];
		route.arrival_station = new char[arrival_station.length()];
		route.arrival_city = new char[arrival_city.length()];
		route.plane_type = new char[plane_type.length()];
		route.puntuality_rate = new char[puntuality_rate.length()];
		route.ticket_price = new char[ticket_price.length()];
		route.discount = new char[discount.length()];
		route.other = new char[other.length()];
		route.cost_time = new char[cost_time.length()];

		strcpy(route.id, id.c_str());
		strcpy(route.plane_type, plane_type.c_str());
		strcpy(route.start_time, start_time.c_str());
		strcpy(route.start_station, start_station.c_str());
		strcpy(route.start_city, start_city.c_str());
		strcpy(route.arrival_time, arrival_time.c_str());
		strcpy(route.arrival_station, arrival_station.c_str());
		strcpy(route.arrival_city, arrival_city.c_str());
		strcpy(route.puntuality_rate, puntuality_rate.c_str());
		strcpy(route.ticket_price, ticket_price.c_str());
		strcpy(route.discount, discount.c_str());
		strcpy(route.other, other.c_str());
		strcpy(route.cost_time, cost_time.c_str());
	}
	else if (route.vehicle_type == HSRC_TYPE)
	{
		auto id = vehicle->get_vehicle_id();
		auto start_time = vehicle->get_start_time();
		auto start_station = vehicle->get_start_station();
		auto start_city = vehicle->get_start_city();
		auto arrival_time = vehicle->get_arrival_time();
		auto arrival_station = vehicle->get_arrival_station();;
		auto arrival_city = vehicle->get_arrival_city();
		auto start_type = vehicle->get_start_type();
		auto arrival_type = vehicle->get_arrival_type();
		auto second_class_seat_price = vehicle->get_second_class_seat_price();
		auto one_class_seat_price = vehicle->get_one_class_seat_price();
		auto business_seat_price = vehicle->get_business_seat_price();
		auto mileage = vehicle->getMileage();
		auto cost_time = vehicle->get_cost_time();

		route.id = new char[id.length()];
		route.start_time = new char[start_time.length()];
		route.start_station = new char[start_station.length()];
		route.start_city = new char[start_city.length()];
		route.arrival_time = new char[arrival_time.length()];
		route.arrival_station = new char[arrival_station.length()];
		route.arrival_city = new char[arrival_city.length()];
		route.start_type = new char[start_type.length()];
		route.arrival_type = new char[arrival_type.length()];
		route.second_class_seat_price = new char[second_class_seat_price.length()];
		route.one_class_seat_price = new char[one_class_seat_price.length()];
		route.business_seat_price = new char[business_seat_price.length()];
		route.mileage = new char[mileage.length()];
		route.cost_time = new char[cost_time.length()];

		strcpy(route.id, id.c_str());
		strcpy(route.start_time, start_time.c_str());
		strcpy(route.start_station, start_station.c_str());
		strcpy(route.start_city, start_city.c_str());
		strcpy(route.arrival_time, arrival_time.c_str());
		strcpy(route.arrival_station, arrival_station.c_str());
		strcpy(route.arrival_city, arrival_city.c_str());
		strcpy(route.start_type, start_type.c_str());
		strcpy(route.arrival_type, arrival_type.c_str());
		strcpy(route.second_class_seat_price, second_class_seat_price.c_str());
		strcpy(route.one_class_seat_price, one_class_seat_price.c_str());
		strcpy(route.business_seat_price, business_seat_price.c_str());
		strcpy(route.mileage, mileage.c_str());
		strcpy(route.cost_time, cost_time.c_str());
	}
	else
	{
		throw MyException(1, "GET_ERROR_VEHICLE_TYPE_WHEN_VEHICLE_TO_ROUTE", "´íÎóµÄVEHICLEÀàÐÍ");
	}
}
