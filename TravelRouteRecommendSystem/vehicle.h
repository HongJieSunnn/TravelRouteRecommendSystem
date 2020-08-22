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

	virtual AirPlane getAP()
	{
		return AirPlane();
	}
	virtual HSRC getHSRC()
	{
		return HSRC();
	}
	virtual string get_vehicle_id()
	{
		return "";
	};
	virtual string get_plane_type()
	{
		return "";
	};
	virtual string get_start_time()
	{
		return "";
	};
	virtual string get_start_station()
	{
		return "";
	};
	virtual string get_start_city()
	{
		return "";
	};
	virtual string get_arrival_time()
	{
		return "";
	};
	virtual string get_arrival_station()
	{
		return "";
	};
	virtual string get_arrival_city()
	{
		return "";
	};
	virtual string get_puntuality_rate()
	{
		return "";
	};
	virtual string get_ticket_price()
	{
		return "";
	};
	virtual string get_discount()
	{
		return "";
	};
	virtual string get_other()
	{
		return "";
	};
	virtual string get_cost_time()
	{
		return "";
	};
	virtual string get_start_type()
	{
		return "";
	};
	virtual string get_arrival_type()
	{
		return "";
	};
	virtual string get_second_class_seat_price()
	{
		return "";
	};
	virtual string get_one_class_seat_price()
	{
		return "";
	};
	virtual string get_business_seat_price()
	{
		return "";
	};
};

class AirPlane:public Vehicle
{
public:
	AirPlane() :Vehicle(AIRPLANE_TYPE)
	{
	}

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

	AirPlane getAP()
	{
		AirPlane temp_ap;
		temp_ap.plane_id = this->plane_id;
		temp_ap.plane_type = this->plane_type;
		temp_ap.start_time = this->start_time;
		temp_ap.start_airport = this->start_airport;
		temp_ap.start_city = this->start_city;
		temp_ap.arrival_time = this->arrival_time;
		temp_ap.arrival_airport = this->arrival_airport;
		temp_ap.arrival_city = this->arrival_city;
		temp_ap.puntuality_rate = this->puntuality_rate;
		temp_ap.ticket_price = this->ticket_price;
		temp_ap.discount = this->discount;
		temp_ap.other = this->other;
		temp_ap.cost_time = this->cost_time;
		return temp_ap;
	}
	string get_vehicle_id();
	string get_plane_type();
	string get_start_time();
	string get_start_station();
	string get_start_city();
	string get_arrival_time();
	string get_arrival_station();
	string get_arrival_city();
	string get_puntuality_rate();
	string get_ticket_price();
	string get_discount();
	string get_other();
	string get_cost_time();
};

class HSRC:public Vehicle
{
public:
	HSRC() :Vehicle(HSRC_TYPE)
	{
	}

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
		string business_seat_price,
		string mileage
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
		this->mileage = mileage;
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
	string mileage;

	HSRC getHSRC()
	{
		HSRC temp_hsrc;
		temp_hsrc.car_id = this->car_id;
		temp_hsrc.start_type = this->start_type;
		temp_hsrc.start_time = this->start_time;
		temp_hsrc.start_station = this->start_station;
		temp_hsrc.start_city = this->start_city;
		temp_hsrc.cost_time = this->cost_time;
		temp_hsrc.arrival_type = this->arrival_type;
		temp_hsrc.arrival_time = this->arrival_time;
		temp_hsrc.arrival_station = this->arrival_station;
		temp_hsrc.arrival_city = this->arrival_city;
		temp_hsrc.second_class_seat_price = this->second_class_seat_price;
		temp_hsrc.one_class_seat_price = this->one_class_seat_price;
		temp_hsrc.business_seat_price = this->business_seat_price;
		return temp_hsrc;
	}
	string get_vehicle_id();
	string get_start_type();
	string get_start_time();
	string get_start_station();
	string get_start_city();
	string get_arrival_type();
	string get_arrival_time();
	string get_arrival_station();
	string get_arrival_city();
	string get_second_class_seat_price();
	string get_one_class_seat_price();
	string get_business_seat_price();
	string get_cost_time();
};

inline string AirPlane::get_vehicle_id()
{
	return this->plane_id;
}
inline string AirPlane::get_plane_type()
{
	return this->plane_type;
}
inline string AirPlane::get_start_time()
{
	return this->start_time;
}
inline string AirPlane::get_start_station()
{
	return this->start_airport;
}
inline string AirPlane::get_start_city()
{
	return this->start_city;
}
inline string AirPlane::get_arrival_time()
{
	return arrival_time;
}
inline string AirPlane::get_arrival_station()
{
	return this->arrival_airport;
}
inline string AirPlane::get_arrival_city()
{
	return this->arrival_city;
}
inline string AirPlane::get_puntuality_rate()
{
	return this->puntuality_rate;
}
inline string AirPlane::get_ticket_price()
{
	return this->ticket_price;
}
inline string AirPlane::get_discount()
{
	return this->discount;
}
inline string AirPlane::get_other()
{
	return this->other;
}
inline string AirPlane::get_cost_time()
{
	return this->cost_time;
}

inline string HSRC::get_vehicle_id()
{
	return this->car_id;
}
inline string HSRC::get_start_type()
{
	return this->start_type;
}
inline string HSRC::get_start_time()
{
	return this->start_time;
}
inline string HSRC::get_start_station()
{
	return this->start_station;
}
inline string HSRC::get_start_city()
{
	return this->start_city;
}
inline string HSRC::get_arrival_type()
{
	return this->arrival_type;
}
inline string HSRC::get_arrival_time()
{
	return this->arrival_time;
}
inline string HSRC::get_arrival_station()
{
	return this->arrival_station;
}
inline string HSRC::get_arrival_city()
{
	return this->arrival_city;
}
inline string HSRC::get_second_class_seat_price()
{
	return this->second_class_seat_price;
}
inline string HSRC::get_one_class_seat_price()
{
	return this->one_class_seat_price;
}
inline string HSRC::get_business_seat_price()
{
	return this->business_seat_price;
}
inline string HSRC::get_cost_time()
{
	return this->cost_time;
}

#endif // !VEHICLE_H