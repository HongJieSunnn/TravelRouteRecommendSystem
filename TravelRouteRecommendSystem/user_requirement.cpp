#include "user_requirement.h"
#include"my_time.h"
#include"route_result.h"
#include<vector>
#include<string>
using std::vector;
using std::string;

namespace UserRequirementNamespace
{
	//备注需求数组
	vector<PretreatRemarkStatus> remark_needs_status_code;

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
		ONE_CLASS_TICKET=1,
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
		ALL=1,
		HSRC,
		AIRPLANE,
	};
	/*
		出行方式 即限定的几种
	*/
	enum TravelTypeEnum
	{
		BUSINESS_TRIP=1,
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
		DIRECT=1,
		TRANS,
		FIX_TRANS,
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
}
using namespace UserRequirementNamespace;

UserRequirementAfterPretreat UserRequirement::pretreatUserRequirement() noexcept
{
	UserRequirementAfterPretreat requirement;
	/*
		预处理顺序:
			1. 先根据用户需求的出行类型以及备注得到最符合的出行类型枚举值
			2. 把需要的城市录入到需求中
			3. 根据出行类型枚举值以及用户给定的出行时间的状态(来回时间给没给)来推荐最好的出行时间
			4. 
	*/

}

PretreatStatue UserRequirement::pretreatCities(UserRequirementAfterPretreat& requirement)
{
	requirement.start_cities = vector<string>(city_num);
	requirement.arrive_cities = vector<string>(city_num);

	for (int i = 0; i < this->city_num; i++)//city_num 是城市组数
	{
		if (!start_cities[i] || !arrive_cities[i])//不能为空
		{
			return PRETREAT_CITIES_FAILED;
		}
		requirement.start_cities[i] = this->start_cities[i];
		requirement.arrive_cities[i] = this->arrive_cities[i];
	}
	return PRETREAT_CITIES_SUCCEED;
}

PretreatStatue UserRequirement::pretreatTime(UserRequirementAfterPretreat& requirement)
{
	if (!dealPretreatTimeStatue(timeToMyTimeAndIntoRequirementAfterPreTreat(requirement)))
	{
		//直接返回带错误信息的请求给后端
	}
	return PRETREAT_TIME_SUCCEED;
}

PretreatStatue UserRequirement::timeToMyTimeAndIntoRequirementAfterPreTreat(UserRequirementAfterPretreat& requirement)
{
	if ((this->start_time == "" || this->start_time == nullptr) && (this->arrive_time == "" || this->start_time == nullptr))//时间都没给
	{
		return PRETREAT_TIME_NO_BOTH_TIME;
	}

	if (this->start_time != "" && this->start_time != nullptr)//若出发时间不为空
	{
		requirement.start_time = MyTime::stringToMyTime(this->start_time, YYYY_MM_DD_HH_MM);
	}
	else
	{
		return PRETREAT_TIME_NO_START_TIME;
	}

	if (this->arrive_time != "" && this->start_time != nullptr)//若到达时间不为空
	{
		requirement.arrive_time = MyTime::stringToMyTime(this->arrive_time, YYYY_MM_DD_HH_MM);
	}
	else
	{
		return PRETREAT_TIME_NO_ARRIVLE_TIME;
	}

	return PRETREAT_TIME_INTO_REQUIREMENT_SUCCEED;
}

PretreatStatue UserRequirement::dealPretreatTimeStatue(PretreatStatue statue_code)
{
	switch (statue_code)
	{
	case PRETREAT_TIME_NO_BOTH_TIME:
	case PRETREAT_TIME_NO_START_TIME:
		RouteResult::error = (char*)"出发时间空错误";
		return PRETREAT_TIME_FAILED;
		break;
	case PRETREAT_TIME_NO_ARRIVLE_TIME:
	case PRETREAT_TIME_INTO_REQUIREMENT_SUCCEED:
		return PRETREAT_TIME_SUCCEED;
		break;
	}
}

PretreatStatue UserRequirement::pretreatTravelType(UserRequirementAfterPretreat& requirement)
{
	if (this->travel_type == "商务出差")
	{
		requirement.travelType = BUSINESS_TRIP;
		requirement.timeType = TIME_BETTER;
	}
	else if (this->travel_type == "个人出游")
	{
		requirement.travelType = PERSONAL_TRIP;
		requirement.timeType = TIME_REGARDLESS;
	}
	else if (this->travel_type == "朋友出游")
	{
		requirement.travelType = WITH_FRIEND_TRIP;
		requirement.timeType = TIME_REGARDLESS;
	}
	else if (this->travel_type == "家庭出游")
	{
		requirement.travelType = WITH_FAMILIES_TRIP;
		requirement.timeType = TIME_BETTER;
	}
	else if (this->travel_type == "上学")
	{
		requirement.travelType = TO_SCHOOL_TRIP;
		requirement.timeType = TIME_REGARDLESS;
	}
	else if (this->travel_type == "紧急出行")
	{
		requirement.travelType = URGENT_TRIP;
		requirement.timeType = TIME_FIRST;
	}
	else if (this->travel_type == "商务出差|紧急出行")
	{
		requirement.travelType = URGENT_BUSINESS_TRIP;
		requirement.timeType = TIME_FIRST;
	}
	else if (this->travel_type == "上学|紧急出行")
	{
		requirement.travelType = URGENT_TO_SCHOOL_TRIP;
		requirement.timeType = TIME_FIRST;
	}
	else
	{
		return PRETREAT_TRAVELTYPE_FAILED_ERROR_TYPE;
	}
	return PRETREAT_TRAVELTYPE_SUCCEED;
}

PretreatStatue UserRequirement::dealPretreatTravelTypeStatue(PretreatStatue statue_code)
{
	switch (statue_code)
	{
	case PRETREAT_TRAVELTYPE_FAILED_ERROR_TYPE:
		return PRETREAT_TRAVELTYPE_FAILED;
		break;
	case PRETREAT_TRAVELTYPE_SUCCEED:
		return PRETREAT_TRAVELTYPE_SUCCEED;
		break;
	}
}

PretreatStatue UserRequirement::pretreatVehicleExperience(UserRequirementAfterPretreat& requirement)
{
	if (!this->vehicle_type)
	{
		return PRETREAT_VEHICLE_TYPE_EMPTY_TYPE;
	}

	if (this->vehicle_type == "任意")
	{
		requirement.vehicleType = ALL;
	}
	else if (this->vehicle_type == "铁路")
	{
		requirement.vehicleType = HSRC;
	}
	else if (this->vehicle_type == "航空")
	{
		requirement.vehicleType = AIRPLANE;
	}
	else
	{
		return PRETREAT_VEHICLE_TYPE_ERROR_TYPE;
	}
}

PretreatStatue UserRequirement::pretreatTransitType(UserRequirementAfterPretreat& requirement)
{
	
}

PretreatStatue UserRequirement::pretreatRemark(UserRequirementAfterPretreat& requirement)
{
	string remark_str = this->remark;
	if (remark_str.empty())
		return NO_REMARK;
	//备注若包含紧急 且出行方式为出差或上学
	if (remark_str.find("立刻") != string::npos ||
		remark_str.find("马上") != string::npos ||
		remark_str.find("急") != string::npos||
		remark_str.find("赶") != string::npos||
		remark_str.find("紧") != string::npos||
		remark_str.find("快") != string::npos)
	{
		if (requirement.travelType == BUSINESS_TRIP)
		{
			requirement.travelType = URGENT_BUSINESS_TRIP;
		}
		else if (requirement.travelType == TO_SCHOOL_TRIP)
		{
			requirement.travelType = URGENT_TO_SCHOOL_TRIP;
		}
		else
			return REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT;//出游不能变成紧急 要不就选紧急出行 出游备注紧急 则直接返回 并且提示用户出行不能变为紧急 所以我们无法根据您的备注来推荐哦
		remark_needs_status_code.push_back(REMARK_CHANGE_TRIP_STATUE_TO_URGENT);
	}

	if (remark_str.find("老人") != string::npos ||
		remark_str.find("孩") != string::npos )
	{
		if (requirement.travelType == WITH_FAMILIES_TRIP)
		{
			remark_needs_status_code.push_back(REMARK_FAMILIIES_INCLUDE_THE_OLD_OR_CHILD);
		}
		else
			return REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES;//和出游不能变成紧急类似 直接忽略备注 并且返回提示
	}

	if (remark_str.find("便宜") != string::npos||
		remark_str.find("实惠")!=string::npos)
	{
		remark_needs_status_code.push_back(REMARK_CHEAP_PRICE);
	}
	if (remark_str.find("舒") != string::npos||
		remark_str.find("贵")!=string::npos)
	{
		remark_needs_status_code.push_back(REMARK_COMFORTABLE_EXPERIENCE);
	}
	if (remark_needs_status_code.empty())
		return NO_MATCH_REMARK;
	return REMARK_PRETREAT_END;
}

PretreatStatue UserRequirement::dealPretreatRemarkStatue(PretreatStatue statue_code)
{
	switch (statue_code)
	{
	case NO_REMARK:
	case REMARK_PRETREAT_END:
		return PRETREAT_REMARK_SUCCEED;
		break;
	case NO_MATCH_REMARK:
		//TODO 后面肯定要改的 实际上就是把要返回给后端的结果的备注添加上一些信息
		RouteResult::remark = (char*)"对不起 暂时无法理解您的备注哦";
		return PRETREAT_REMARK_FAILED;
		break;
	case REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT:
		//TODO 后面肯定要改的 实际上就是把要返回给后端的结果的备注添加上一些信息
		RouteResult::remark = (char*)"出游不能变为紧急 所以我们无法根据您的备注来推荐哦";
		return PRETREAT_REMARK_FAILED;
		break;
	case REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES:
		//TODO 后面肯定要改的 实际上就是把要返回给后端的结果的备注添加上一些信息
		RouteResult::remark = (char*)"下次和家人一起出游时再添加这些备注吧~";
		return PRETREAT_REMARK_FAILED;
		break;
	}
	return PretreatStatus::PRETREAT_REMARK_FAILED;
}
