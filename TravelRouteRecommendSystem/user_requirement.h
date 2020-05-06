#ifndef USERREQUIREMENT_H
#define USERREQUIREMENT_H
namespace
{
	using TimeWeightType = int;
	using TimeWeight = int;
	using PriceWeightType = int;
	using PriceWeight = int;
	using TicketType = int;
	using VehicleType = int;
	using TravelType = int;
	using TransitType = int;
	using PretreatStatue = int;
}
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

	*distances:通过调用地图api获取每两个城市之间直线距离 从而判断交通工具推荐
	*remark:备注

*/
class UserRequirement
{
	char** start_cities;
	char** arrive_cities;
	int city_num;
	char* start_time;
	char* arrive_time;
	char* vehicle_type;
	char* travel_type;
	int* distances;
	char* remark;

public:
	/*
		预处理用户需求 得到推荐的方式(例如时间 中转方式 钱等等) 更方便后续使用
	*/
	UserRequirementAfterPretreat pretreatUserRequirement() noexcept;

	/*
		预处理城市 包括开始城市和结束城市
		*requirement:最后要被返回的那个预处理过的用户需求
	*/
	PretreatStatue pretreatCities(UserRequirementAfterPretreat& requirement);

	/*
		预处理时间 包括开始时间和到达时间
			- 必须有一个不为空 如果用户传来的为全空 后端要把start_time赋值成当前时间
			- 用户可以只给天 不给时和分 那么默认为0 若都为0 说明出发的具体时间点需要推荐
			- 需要我通过预处理给这个乘客推荐出适合他的时间
	*/
	PretreatStatue pretreatTime(UserRequirementAfterPretreat& requirement);
	//把时间从string转换成MyTime并且录入到预处理后的需求中
	PretreatStatue timeToMyTimeAndIntoRequirementAfterPreTreat(UserRequirementAfterPretreat& requirement);

	/*
		分配权重的函数
	*/
	PretreatStatue assignWeights(UserRequirementAfterPretreat& requirement);

	/*
		根据其它需求分析是否需要预处理出行方式
			例如 如果出差备注写紧急 那么就改成紧急出差
	*/
	PretreatStatue pretreatTravelType(UserRequirementAfterPretreat& requirement);

	/*
		预处理交通工具体验 即交通工具类型和票类型(座位类型 不同座位体验肯定不同)
	*/
	PretreatStatue pretreatVehicleExperience(UserRequirementAfterPretreat& requirement);

	/*
		预处理中转方式 其实直达是最方便的 除非用户指定 否则一般不推荐转车的 除非没有直达
	*/
	PretreatStatue pretreatTransitType(UserRequirementAfterPretreat& requirement);

	/*
		预处理备注
	*/
	PretreatStatue pretreatRemark(UserRequirementAfterPretreat& requirement);
	//处理预处理备注状态码的函数
	PretreatStatue dealPretreatRemarkStatue(PretreatStatue statue_code);
};

/*
	该结构用来储存预处理后的用户需求	一组城市一个推荐 所以需要数组
*/
struct UserRequirementAfterPretreat
{
	vector<string> start_cities;
	vector<string> arrive_cities;
	MyTime start_time;
	MyTime arrive_time;
	vector<TimeWeight> timeWeight;
	vector<PriceWeight> priceWeight;
	vector<TicketType> ticketType;
	VehicleType vehicleType;
	TravelType travelType;
	TransitType transitType;
};
#endif // !USERREQUIREMENT_H