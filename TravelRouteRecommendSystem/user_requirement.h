#ifndef USERREQUIREMENT_H
#define USERREQUIREMENT_H
#include"my_time.h"
#include"route_result.h"
#include<vector>
#include<string>
using std::vector;
using std::string;

namespace UserRequirementNamespace
{
	//regardless:不考虑 也就是可以接受任何推荐的

	/*
		时间权重枚举
	*/
	enum TimeWeightEnum
	{
		TIME_REGARDLESS,
		TIME_FIRST,
		TIME_BETTER
	};
	/*
		价格权重枚举
	*/
	enum PriceWeightEnum
	{
		PRICE_REGARDLESS,
		PRICE_CHEAP,
		PRICE_AFFORDABLE,
		PRICE_EXPENSIVE
	};
	/*
		票种类 通过出行类型分配
		飞机只有一种票价 就把它当做二等座
	*/
	enum TicketTypeEnum
	{
		ONE_CLASS_TICKET = 1,
		SECOND_CLASS_TICKET,
		BUSINESS_CLASS_TICKET,
		ALL_CLASS_TICKET//所有类型都展示 用户自己选

		//把所有票价都放进去 但是排放的顺序不同 例如速度优先其它不管 就把二等座优先推荐
	};
	/*
		交通工具枚举
	*/
	enum VehicleTypeEnum
	{
		ALL_VEHICLE = 1,
		HSRC,
		AIRPLANE,
	};
	/*
		出行方式 即限定的几种
	*/
	enum TravelTypeEnum
	{
		BUSINESS_TRIP = 1,
		PERSONAL_TRIP,
		WITH_FRIEND_TRIP,
		WITH_FAMILIES_TRIP,
		TO_SCHOOL_TRIP,
		URGENT_TRIP,
		URGENT_BUSINESS_TRIP,
		URGENT_TO_SCHOOL_TRIP
	};
	/*
		中转类型 即
			1. 直达
			2. 中转
			3. 混合中转
	*/
	enum TransitTypeEnum
	{
		DIRECT = 1,
		TRANS,
		FIX_TRANS,
		ALL_TRANSIT//包含直达和中转 但是不包含FIX
		/*
		* tip:
		* 若用户选择DIRECT 但是没有直达 那么就提示 没有直达
		* 而如果是ALL_TRANSIT 才需要如果没直达就弄转车 否则全部显示
		*/
	};
	/*
		预处理状态码 方便后续操作
		这个返回成功或失败 每个对象的具体类型有对应的枚举
	*/
	enum PretreatStatus
	{
		PRETREAT_FAILED,
		PRETREAT_SUCCEED,
		PRETREAT_CITIES_FAILED,
		PRETREAT_CITIES_SUCCEED,
		PRETREAT_TIME_FAILED,
		PRETREAT_TIME_SUCCEED,
		PRETREAT_REMARK_FAILED,
		PRETREAT_REMARK_SUCCEED,
		PRETREAT_TRAVELTYPE_FAILED,
		PRETREAT_TRAVELTYPE_SUCCEED,
		PRETREAT_VEHICLE_EXPERIENCE_FAILED,
		PRETREAT_VEHICLE_EXPERIENCE_SUCCEED,
		PRETREAT_transit_TYPE_FAILED_FINALL,
		PRETREAT_transit_TYPE_SUCCEED_FINALL,
	};
	enum PretreatTimeStatus
	{
		PRETREAT_TIME_INTO_REQUIREMENT_ERROR,
		PRETREAT_TIME_INTO_REQUIREMENT_SUCCEED,
		PRETREAT_TIME_NO_BOTH_TIME,
		PRETREAT_TIME_NO_START_TIME,
		PRETREAT_TIME_NO_ARRIVLE_TIME,
	};
	enum PretreatRemarkStatus
	{
		NO_REMARK,
		NO_MATCH_REMARK,//备注里没有包含符合定义的那些操作的文字(例如备注打了个sb 那么不执行任何操作) 备注却不为空
		REMARK_CHANGE_TRIP_STATUE_TO_URGENT,
		REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT,
		REMARK_FAMILIIES_INCLUDE_THE_OLD_OR_CHILD,
		REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES,
		REMARK_CHEAP_PRICE,
		REMARK_COMFORTABLE_EXPERIENCE,
		REMARK_PRETREAT_END
	};
	enum PretreatRemarkNeeds
	{
		PRETREAT_REMARK_NEEDS_ERROR,
		PRETREAT_REMARK_NEEDS_SUCCEED,
		PRETREAT_REMARK_NEEDS_EMPTY,
	};
	enum PretreatTravelTypeStatus
	{
		PRETREAT_TRAVELTYPE_FAILED_ERROR_TYPE,
	};
	enum PretreatVehicleExperienceStatus
	{
		PRETREAT_VEHICLE_TYPE_EMPTY_TYPE,
		PRETREAT_VEHICLE_TYPE_ERROR_TYPE,
		PRETREAT_VEHICLE_TYPE_SUCCEED,
		PRETREAT_TICKET_TYPE_ERROR,
		PRETREAT_TICKET_TYPE_SUCCEED,
	};
	enum PretreatTransitTypeStatus
	{
		PRETREAT_transit_TYPE_ERROR,
		PRETREAT_transit_TYPE_SUCCEED,
		PRETREAT_transit_TYPE_ERROR_TYPE,
		PRETREAT_transit_TYPE_ERROR_FIX_ONLY_WHEN_VEHICLE_ALL,
	};

};

namespace
{
	using TimeType = UserRequirementNamespace::TimeWeightEnum;
	using PriceType = UserRequirementNamespace::PriceWeightEnum;
	using TicketType = UserRequirementNamespace::TicketTypeEnum;
	using VehicleType = UserRequirementNamespace::VehicleTypeEnum;
	using TravelType = UserRequirementNamespace::TravelTypeEnum;
	using TransitType = UserRequirementNamespace::TransitTypeEnum;
	using PretreatStatue = int;
}

/*
	该结构用来储存预处理后的用户需求	一组城市一个推荐 所以需要数组
*/
struct UserRequirementAfterPretreat
{
	vector<string> start_cities;
	vector<string> arrive_cities;
	MyTime start_time;
	MyTime arrive_time;
	TravelType travelType;
	TimeType timeType;
	PriceType priceType;
	TicketType ticketType;
	vector<VehicleType> vehicleType;
	vector <TransitType> transitType;
};

/*
	这个类是用来存储用户需求 并且预处理数据
	从c#传入的数据会放到这里面
	由于c#string 和 c++string转换很麻烦(string里char被定义成了Elem 所以这里面字符串都用char* 后续使用再转成string)
	*start_cities:出发城市(s) 如果有多个到达城市 例如福州->南京->北京 出发城市为福州 南京(只要不是最终到达 都是出发城市)
	*arrive_cities:到达城市 可以有多个
	*city_num:城市数量 即有几组城市 就是所有城市-1
	*start_time:出发时间 若无输入 默认为当前时间一小时后(TODO:或者调用api获得当前地址到车站的最短时间+半小时(麻烦 应该不会实现)) 这个由后端操作
	*arrive_time:期望的到达时间
		!*	时间格式必须为 yyyy.mm.dd hh:mm|hh:mm	  *!(前一个里的hh:mm=00:00 说明没给时间点 需要推荐 后面说明是发车或者到达时间是00:00)
		!*	可以只给出发时间 或者只给到达时间 或者都不给	 *!
	*vehicle_type:出发的载具 如果不填写 通过travel_type来判断
	*travel_type:出行方式 我会指定只有限定的几种
		1. 商务出差
		2. 个人出游
		3. 朋友出游
		4. 家庭出游:需备注老人、小孩、家庭成员身体状况等(不备注当做有老人小孩 即需要舒适 时间短 以此配置权重)
		5. 上学:这个就在某段时间内所有给它就好了 这个最好选出行方式 或者通过距离
		6. 紧急出行:例如家里出事了需要紧急出发的 直接找最近时间最快交通方式
		但是允许紧急出行和上学、出差组合
	*transit_type:中转方式 若无指定 一般默认优先推荐直达
	*distances:通过调用地图api获取每两个城市之间直线距离 从而判断交通工具推荐
	*remark:备注
*/

//即使用户有些需求没给出 但是后端给C++时 就要把没给的用默认补全 因为C#操作方便些
class UserRequirement
{
	char** start_cities;
	char** arrive_cities;
	int city_num;
	char* start_time;
	char* arrive_time;
	char* travel_type;
	char** vehicle_type;
	char** transit_type;
	int* distances;
	char* remark;

public:
	UserRequirement() 
	{
	};
	UserRequirement(char** start_cities, char** arrive_cities, int city_num, char* start_time, char* arrive_time, char** vehicle_type, char* travel_type, char** transit_type, int* distances, char* remark) :
		start_cities(start_cities),
		arrive_cities(arrive_cities),
		city_num(city_num),
		start_time(start_time),
		arrive_time(arrive_time),
		vehicle_type(vehicle_type),
		travel_type(travel_type),
		transit_type(transit_type),
		distances(distances),
		remark(remark)
	{
	}
	/*
		预处理用户需求 得到推荐的方式(例如时间 中转方式 钱等等) 更方便后续使用
	*/
	UserRequirementAfterPretreat pretreatUserRequirement();
protected:
	/*
		预处理城市 包括开始城市和结束城市
		*requirement:最后要被返回的那个预处理过的用户需求
	*/
	PretreatStatue pretreatCities(UserRequirementAfterPretreat& requirement);

	/*
		预处理时间 得到时间类型是时间优先(紧急)还是无所谓看推荐
			- 必须有一个不为空 如果用户传来的为全空 后端要把start_time赋值成当前时间
			- 用户可以只给天 不给时和分 那么默认为0 若都为0 说明出发的具体时间点需要推荐
			- 预处理无法推荐出具体时间 需要后续推荐时才能得到推荐的具体时间点
	*/
	PretreatStatue pretreatTime(UserRequirementAfterPretreat& requirement);
	//把时间从string转换成MyTime并且录入到预处理后的需求中
	PretreatStatue timeToMyTimeAndIntoRequirementAfterPreTreat(UserRequirementAfterPretreat& requirement);

	/*
		处理预处理开始时间和出发时间状态码的函数
	*/
	PretreatStatue dealPretreatTimeStatue(PretreatStatue statue_code);

	/*
		分配权重的函数
		*已弃用 没必要分配权重
		按照sql语句sort顺序和sort列个数来决定什么优先
	*/
	//PretreatStatue assignWeights(UserRequirementAfterPretreat& requirement);

	/*
		根据其它需求分析是否需要预处理出行方式
			例如 如果出差备注写紧急 那么就改成紧急出差
	*/
	PretreatStatue pretreatTravelType(UserRequirementAfterPretreat& requirement);

	/*
		处理预处理出行方式状态码的函数
	*/
	PretreatStatue dealPretreatTravelTypeStatue(PretreatStatue statue_code);
	
	/*
		预处理交通工具体验 即交通工具类型和票类型(座位类型 不同座位体验肯定不同)
	*/
	PretreatStatue pretreatVehicleExperience(UserRequirementAfterPretreat& requirement);
	/*
		处理预处理交通体验状态码
	*/
	PretreatStatue dealPretreatVehicleExperienceStatue(PretreatStatue statue_code);

	/*
		预处理中转方式 其实直达是最方便的 除非用户指定 否则一般不推荐转车的 除非没有直达
	*/
	PretreatStatue pretreatTransitType(UserRequirementAfterPretreat& requirement);
	/*
		处理预处理中转方式状态码
	*/
	PretreatStatue dealPretreatTransitTypeStatue(PretreatStatue statue_code);

	/*
		预处理备注
	*/
	PretreatStatue pretreatRemark(UserRequirementAfterPretreat& requirement);
	/*
	处理预处理备注状态码的函数
	*/
	PretreatStatue dealPretreatRemarkStatue(PretreatStatue statue_code);

	/*
	处理预处理备注需求的函数
	*/
	PretreatStatue toDealPretreatRemarkNeeds(UserRequirementAfterPretreat& requirement);
};
#endif // !USERREQUIREMENT_H