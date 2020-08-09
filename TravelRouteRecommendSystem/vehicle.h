#ifndef VEHICLE_H
#define VEHICLE_H
#include<iostream>
#include<string>
using std::string;
//该类用来表示交通工具对象 用来储存一些交通工具的信息
//这是个父类 具体的交通工具要继承它
namespace 
{
	const string ERROR_VEHICLE_TYPE = "ERROR_TYPE";
	const string AIRPLANE_TYPE = "AIRPLANE_TYPE";
	const string HSRC_TYPE = "HSRC_TYPE";
}
/*	
	交通工具类 基类
*vehicle_type:交通工具类型 当fix类型时 Vechile会动态多态 所以用这个来判断究竟是哪个子类
*/
class Vehicle
{
private:
	string vehicle_type;
public:
	Vehicle(string vehicle_type= ERROR_VEHICLE_TYPE) :vehicle_type(vehicle_type)
	{
	};

	/*
		返回当前类型
	*/
	string getVehicleType()
	{
		return this->vehicle_type;
	}
};

class AirPlane:public Vehicle
{
public:
	AirPlane
	(
		string plane_id,
		string plane_type,
		string start_time,
		string start_airport,
		string start_city,
		string arrival_time,
		string arrival_airport,
		string arrival_city,
		string puntuality_rate,
		string ticket_price,
		string discount,
		string other,
		string cost_time
	) :Vehicle(AIRPLANE_TYPE)
	{
		this->plane_id = plane_id;
		this->plane_type = plane_type;
		this->start_time = start_time;
		this->start_airport = start_airport;
		this->start_city = start_city;
		this->arrival_time = arrival_time;
		this->arrival_airport = arrival_airport;
		this->arrival_city = arrival_city;
		this->puntuality_rate = puntuality_rate;
		this->ticket_price = ticket_price;
		this->discount = discount;
		this->other = other;
		this->cost_time = cost_time;
	};
	~AirPlane()
	{
	};
	//要用直接访问
	string plane_id;
	string plane_type;
	string start_time;
	string start_airport;
	string start_city;
	string arrival_time;
	string arrival_airport;
	string arrival_city;
	string puntuality_rate;
	string ticket_price;
	string discount;
	string other;
	string cost_time;
};

class HSRC:public Vehicle
{
public:
	HSRC
	(
		string car_id,
		string start_type,
		string start_time,
		string start_station,
		string start_city,
		string cost_time,
		string arrival_type,
		string arrival_time,
		string arrival_station,
		string arrival_city,
		string second_class_seat_price,
		string one_class_seat_price,
		string business_seat_price
	):Vehicle(HSRC_TYPE)
	{
		this->car_id = car_id;
		this->start_type = start_type;
		this->start_time = start_time;
		this->start_station = start_station;
		this->start_city = start_city;
		this->arrival_time = arrival_time;
		this->arrival_type = arrival_type;
		this->arrival_city = arrival_city;
		this->arrival_station = arrival_station;
		this->second_class_seat_price = second_class_seat_price;
		this->one_class_seat_price = one_class_seat_price;
		this->business_seat_price = one_class_seat_price;
		this->cost_time = cost_time;
	};
	~HSRC()
	{
	};
	string car_id;
	string start_type;
	string start_time;
	string start_station;
	string start_city;
	string cost_time;
	string arrival_type;
	string arrival_time;
	string arrival_station;
	string arrival_city;
	string second_class_seat_price;
	string one_class_seat_price;
	string business_seat_price;

};

#endif // !VEHICLE_H