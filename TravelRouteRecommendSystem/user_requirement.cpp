#include "user_requirement.h"
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
		TIME_FIRST
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
	*/
	enum PretreatStatus
	{
		PRETREAT_FAILED,
		PRETREAT_SUCCEED,
		PRETREAT_REMARK_FAILED,
		PRETREAT_TRAVEL_TYPE_FAILED
	};
	enum PretreatRemarkStatus
	{
		PRETREAT_REMARK_FAILED,
		PRETREAT_REMARK_SUCCEED,
		NO_REMARK,
		NO_MATCH_REMARK,//备注里没有包含符合定义的那些操作的文字(例如备注打了个sb 那么不执行任何操作) 备注却不为空
		REMARK_CHANGE_TRIP_STATUE_TO_URGENT,
		REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT,
		REMARK_FAMILIIES_INCLUDE_THE_OLD_OR_CHILD,
		REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES,
		REMARK_CHEAP_PRICE,
		REMARK_COMFORTABLE_EXPERIENCE,
	};
	enum PretreatTravelTypeStatus
	{
		PRETREAT_TRAVELTYPE_FAILED_ERROR_TYPE,
		PRETREAT_TRAVELTYPE_SUCCEED,
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
		requirement.start_cities[i] = this->start_cities[i];
		requirement.arrive_cities[i] = this->arrive_cities[i];
	}
}

PretreatStatue UserRequirement::pretreatTime(UserRequirementAfterPretreat& requirement)
{
	
}

PretreatStatue UserRequirement::timeToMyTimeAndIntoRequirementAfterPreTreat(UserRequirementAfterPretreat& requirement)
{
	if (this->start_time != "" && this->start_time != nullptr)
	{
		requirement.start_time = MyTime::stringToMyTime(this->start_time, YYYY_MM_DD_HH_MM);
	}
	if (this->arrive_time != "" && this->start_time != nullptr)
	{
		requirement.arrive_time = MyTime::stringToMyTime(this->arrive_time, YYYY_MM_DD_HH_MM);
	}
}

PretreatStatue UserRequirement::assignWeights(UserRequirementAfterPretreat& requirement)
{
	
}

PretreatStatue UserRequirement::pretreatTravelType(UserRequirementAfterPretreat& requirement)
{
	if (this->travel_type == "商务出差")
	{
		requirement.travelType = BUSINESS_TRIP;
	}
	else if (this->travel_type == "个人出游")
	{
		requirement.travelType = PERSONAL_TRIP;
	}
	else if (this->travel_type == "朋友出游")
	{
		requirement.travelType = WITH_FRIEND_TRIP;
	}
	else if (this->travel_type == "家庭出游")
	{
		requirement.travelType = WITH_FAMILIES_TRIP;
	}
	else if (this->travel_type == "上学")
	{
		requirement.travelType = TO_SCHOOL_TRIP;
	}
	else if (this->travel_type == "紧急出行")
	{
		requirement.travelType = URGENT_TRIP;
	}
	else if (this->travel_type == "商务出差|紧急出行")
	{
		requirement.travelType = URGENT_BUSINESS_TRIP;
	}
	else if (this->travel_type == "上学|紧急出行")
	{
		requirement.travelType = URGENT_TO_SCHOOL_TRIP;
	}
	else
	{
		return PRETREAT_TRAVELTYPE_FAILED_ERROR_TYPE;
	}
	return PRETREAT_TRAVELTYPE_SUCCEED;
}

PretreatStatue UserRequirement::pretreatVehicleExperience(UserRequirementAfterPretreat& requirement)
{
	return PretreatStatue();
}

PretreatStatue UserRequirement::pretreatTransitType(UserRequirementAfterPretreat& requirement)
{
	return PretreatStatue();
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
			return REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT;//出游不能变成紧急 要不就选紧急出行
		return REMARK_CHANGE_TRIP_STATUE_TO_URGENT;
	}

	if (remark_str.find("老人") != string::npos ||
		remark_str.find("孩") != string::npos )
	{
		if (requirement.travelType == WITH_FAMILIES_TRIP)
		{
			return REMARK_FAMILIIES_INCLUDE_THE_OLD_OR_CHILD;
		}
		else
			return REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES;
	}

	if (remark_str.find("便宜") != string::npos)
	{
		return REMARK_CHEAP_PRICE;
	}
	if (remark_str.find("舒") != string::npos)
	{
		return REMARK_COMFORTABLE_EXPERIENCE;
	}
	return NO_MATCH_REMARK;
}

PretreatStatue UserRequirement::dealPretreatRemarkStatue(PretreatStatue statue_code)
{
	switch (statue_code)
	{
	case NO_REMARK:
		return PRETREAT_REMARK_SUCCEED;
		break;
	case NO_MATCH_REMARK:
		//TODO 后面肯定要改的 实际上就是把要返回给后端的结果的备注添加上一些信息
		RouteResult::remark = (char*)"对不起 暂时无法理解您的备注哦";
		return PRETREAT_REMARK_SUCCEED;
		break;
	case REMARK_CHANGE_TRIP_STATUE_TO_URGENT:
	case REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT:
		//TODO 后面肯定要改的 实际上就是把要返回给后端的结果的备注添加上一些信息
		RouteResult::remark = (char*)"再紧急也要注意安全哦";
		return PRETREAT_REMARK_SUCCEED;
		break;
	case REMARK_FAMILIIES_INCLUDE_THE_OLD_OR_CHILD:
		//TODO 后面肯定要改的 实际上就是把要返回给后端的结果的备注添加上一些信息
		RouteResult::remark = (char*)"祝一家人出游开心哦";
		return PRETREAT_REMARK_SUCCEED;
		break;
	case REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES:
		//TODO 后面肯定要改的 实际上就是把要返回给后端的结果的备注添加上一些信息
		RouteResult::remark = (char*)"下次和家人一起出游时再添加这些备注吧~";
		return PRETREAT_REMARK_SUCCEED;
		break;
	case REMARK_CHEAP_PRICE:
	case REMARK_COMFORTABLE_EXPERIENCE:
		return PRETREAT_REMARK_SUCCEED;
		break;
	}
	return PRETREAT_REMARK_FAILED;
}
