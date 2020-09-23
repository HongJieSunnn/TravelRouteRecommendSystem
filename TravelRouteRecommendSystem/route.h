#ifndef ROUTE_H
#define ROUTE_H
#include"vehicle.h"
#include"my_exception.h"
struct Route
{
public:
	char* vehicle_type;
	char* id;
	char* plane_type;
	char* start_time;
	char* start_station;
	char* start_city;
	char* arrival_time;
	char* arrival_station;
	char* arrival_city;
	char* puntuality_rate;
	char* ticket_price;
	char* discount;
	char* other;
	char* cost_time;
	char* start_type;
	char* arrival_type;
	char* second_class_seat_price;
	char* one_class_seat_price;
	char* business_seat_price;
	char* mileage;
};
void vehicleToRoute(Vehicle* vehicle,Route& route);
#endif // !ROUTE_H