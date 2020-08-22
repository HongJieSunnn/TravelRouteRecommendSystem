#include "my_time.h"

int MyTime::costTime(MyTime time)
{
	//+1天 国内航班应该没有超过24小时的吧
	if (format == HH_MM&&time.hour<this->hour)
		time.day += 1;
	tm start_tm_time=myTimeToTmTime(*this);
	tm end_tm_time=myTimeToTmTime(time);
	
	double seconds = difftime(mktime(&start_tm_time), mktime(&end_tm_time));
	return static_cast<int>(seconds / 60);//返回分钟
}

tm MyTime::myTimeToTmTime(MyTime time)
{
	tm temp;
	temp.tm_year = (time.year-1900)>0?(time.year - 1900):(1900-time.year);
	temp.tm_mon = time.month-1;//tm中mon [0-11]
	temp.tm_mday = time.day;
	temp.tm_hour = time.hour;
	temp.tm_min = time.minute;
	return temp;
}

int MyTime::costTime(MyTime start_time, MyTime end_time)
{
	return end_time.costTime(start_time);
}

MyTime MyTime::stringToMyTime(string time,int time_format)
{
	MyTime temp_mytime;
	temp_mytime.format = time_format;
	switch (time_format)
	{
	case YYYY_MM_DD_HH_MM:
		if (conformTimeFormatOrNot(time, YYYY_MM_DD_HH_MM))
		{
			temp_mytime.year = atoi(time.substr(0, 4).c_str());
			temp_mytime.month= atoi(time.substr(5, 2).c_str());
			temp_mytime.day = atoi(time.substr(8, 2).c_str());
			temp_mytime.hour = atoi(time.substr(11, 2).c_str());
			temp_mytime.minute = atoi(time.substr(14, 2).c_str());
		}
		break;
	case HH_MM:
		if (conformTimeFormatOrNot(time, HH_MM))
		{
			temp_mytime.year =1900;
			temp_mytime.month = 0;
			temp_mytime.day = 1;
			temp_mytime.hour = atoi(time.substr(0, 2).c_str());
			temp_mytime.minute = atoi(time.substr(2, 2).c_str());
		}
		break;
	}
	
	return temp_mytime;
}

int MyTime::getCostTimeByString(string cost_time)
{
	return atoi(cost_time.c_str());
}

bool MyTime::conformTimeFormatOrNot(string time,int time_format)
{
	int length = time.length();
	if (!length)
		return false;
	switch (time_format)
	{
	case YYYY_MM_DD_HH_MM:
		if (length == 16)
		{
			if ((time[4] == '.'|| time[4] == '-') && (time[7] == '.'|| time[7] == '-')&& time[10] == ' ' && time[13] == ':')//不放在这里面可能会overflow
				return true;
		}
		break;
	case HH_MM:
		if (length == 5)
		{
			if (time[2] == ':')
				return true;
		}
		break;
	}
	return false;
}

string MyTime::myTimeToStringByInt(int time_format)
{
	string res="";

	switch (time_format)
	{
	case YYYY_MM_DD_HH_MM:
		res.append(to_string(this->year));
		res.append("-");
		res.append(to_string(this->month));
		res.append("-");
		res.append(to_string(this->day));
		res.append(" ");
		if (this->hour < 10)
		{
			res.append("0");
		}
		res.append(to_string(this->hour));
		res.append(":");
		if (this->minute < 10)
		{
			res.append("0");
		}
		res.append(to_string(this->minute));
		break;
	case HH_MM:
		res = "";
		if (this->hour < 10)
		{
			res.append("0");
		}
		res.append(to_string(this->hour));
		res.append(":");
		if (this->minute < 10)
		{
			res.append("0");
		}
		res.append(to_string(this->minute));
		break;
	}
	return res;
}
