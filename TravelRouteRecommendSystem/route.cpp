#include"route.h"

Route vehicleToRoute(Vehicle* vehicle)
{
	Route route;
	route.vehicle_type = (char*)vehicle->getVehicleType().c_str();
	if (route.vehicle_type == AIRPLANE_TYPE)
	{
		route.id = (char*)vehicle->get_vehicle_id().c_str();
		route.plane_type = (char*)vehicle->get_plane_type().c_str();
		route.start_time = (char*)vehicle->get_start_time().c_str();
		route.start_station = (char*)vehicle->get_start_station().c_str();
		route.start_city = (char*)vehicle->get_start_city().c_str();
		route.arrival_time = (char*)vehicle->get_arrival_time().c_str();
		route.arrival_station = (char*)vehicle->get_arrival_station().c_str();
		route.arrival_city = (char*)vehicle->get_arrival_city().c_str();
		route.puntuality_rate = (char*)vehicle->get_puntuality_rate().c_str();
		route.ticket_price = (char*)vehicle->get_ticket_price().c_str();
		route.discount = (char*)vehicle->get_discount().c_str();
		route.other = (char*)vehicle->get_other().c_str();
		route.cost_time = (char*)vehicle->get_cost_time().c_str();
	}
	else if (route.vehicle_type == HSRC_TYPE)
	{
		route.id = (char*)vehicle->get_vehicle_id().c_str();
		route.start_time = (char*)vehicle->get_start_time().c_str();
		route.start_station = (char*)vehicle->get_start_station().c_str();
		route.start_city = (char*)vehicle->get_start_city().c_str();
		route.arrival_time = (char*)vehicle->get_arrival_time().c_str();
		route.arrival_station = (char*)vehicle->get_arrival_station().c_str();
		route.arrival_city = (char*)vehicle->get_arrival_city().c_str();
		route.start_type = (char*)vehicle->get_start_type().c_str();
		route.arrival_type = (char*)vehicle->get_arrival_type().c_str();
		route.second_class_seat_price = (char*)vehicle->get_second_class_seat_price().c_str();
		route.one_class_seat_price = (char*)vehicle->get_one_class_seat_price().c_str();
		route.business_seat_price = (char*)vehicle->get_business_seat_price().c_str();
		route.mileage = (char*)vehicle->getMileage().c_str();
		route.cost_time = (char*)vehicle->get_cost_time().c_str();
	}
	else
	{
		throw MyException(1, "GET_ERROR_VEHICLE_TYPE_WHEN_TO_ROUTE", "´íÎóµÄVEHICLEÀàÐÍ");
	}
	return route;
}
