#ifndef MY_TIME_H
#define MY_TIME_H
#include<string>
#include<time.h>
using std::string;
using std::to_string;
namespace
{
	enum TimeFormatEnum
	{
		YYYY_MM_DD_HH_MM = 1,
		HH_MM
	};
}

/*
	时间结构 把年月日时分分开 并提供函数计算两个MyTime间的时间(分钟)
	cost_time的字符串形式必须是 分钟数
*/
class MyTime
{
public:
	//创建空的对象
	MyTime()
	{
		this->format = 0;//为0说明，空的，也就是没有给出时间
	}
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int format;
	/*
		计算两个时间间的间隔 返回单位分钟
		*time:要计算的时间点
	Tip:this.time - time
	*/
	int costTime(MyTime time);

	/*
		把MyTime类型转换成tm类型以便计算
	*/
	tm myTimeToTmTime(MyTime time);

	/*
		costTime的一个重载形式 需要两个时间
		*start_time:开始时间
		*end_time:结束时间
	Tip:end_time-start_time
	*/
	static int costTime(MyTime start_time, MyTime end_time);

	/*
		把时间字符串转成MyTime结构
		*time:yyyy.mm.dd hh:mm格式的字符串/hh:mm格式
	*/
	static MyTime stringToMyTime(string time, int time_format);

	/*
		通过数据库里的cost_time字段获得消耗时间
		*cost_time:消耗时间的字符表示
	*/
	static int getCostTimeByString(string cost_time);

	/*
		判断时间是否符合时间格式
		*time:时间的字符表示
		不会判断符号间是否为数字 因为默认必须是 其他地方会处理好
	*/
	static bool conformTimeFormatOrNot(string time,int time_format);

	/*
	*根据MyTime的成员变量返回时间的字符串 可以指定返回的格式
	*/
	string myTimeToStringByInt(int time_format);
};

#endif // !MY_TIME_H