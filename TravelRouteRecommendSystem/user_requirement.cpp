#include "user_requirement.h"

using namespace UserRequirementNamespace;

//备注需求数组
vector<PretreatRemarkStatus> remark_needs_status_code;

UserRequirementAfterPretreat UserRequirement::pretreatUserRequirement()
{
	UserRequirementAfterPretreat requirement;
	/*
		预处理顺序:
			1. 先根据用户需求的出行类型以及备注得到最符合的出行类型枚举值
			2. 把需要的城市录入到需求中
			3. 根据出行类型枚举值以及用户给定的出行时间的状态(来回时间给没给)来推荐最好的出行时间
			4. 
	*/
	if ((PretreatStatus)pretreatCities(requirement) == PretreatStatus::PRETREAT_CITIES_FAILED)
	{
		throw MyException(2, "CITIES_EMPTY", "出发城市或到达城市为空");
	}

	if ((PretreatStatus)pretreatTime(requirement) == PretreatStatus::PRETREAT_TIME_FAILED)
	{
		throw MyException(2,"PRETREAT_TIME_FAILED","预处理时间时出现错误");
	}

	if ((PretreatStatus)dealPretreatTravelTypeStatue(pretreatTravelType(requirement)) == PretreatStatus::PRETREAT_TRAVELTYPE_FAILED)
	{
		throw MyException(2, "PRETREAT_TRAVELTYPE_FAILED", "预处理出游方式时出现错误");
	}

	if ((PretreatStatus)dealPretreatRemarkStatue(pretreatRemark(requirement)) == PretreatStatus::PRETREAT_REMARK_FAILED)
	{
		throw MyException(2, "PRETREAT_REMARK_FAILED", "预处理备注时出现错误");
	}

	if ((PretreatStatus)dealPretreatVehicleExperienceStatue(pretreatVehicleExperience(requirement)) == PretreatStatus::PRETREAT_VEHICLE_EXPERIENCE_FAILED)
	{
		throw MyException(2, "PRETREAT_VEHICLE_EXPERIENCE_FAILED", "预处理交通体验时出现错误");
	}

	if ((PretreatStatus)dealPretreatTransitTypeStatue(pretreatTransitType(requirement)) == PretreatStatus::PRETREAT_TRANSIT_TYPE_FAILED_FINALL)
	{
		throw MyException(2, "PRETREAT_TRANSIT_TYPE_FAILED_FINALL", "预处理中转方式时出现错误");
	}

	if ((PretreatStatus)dealPretreatDistancesStatue(pretreatDistances(requirement)) == PretreatStatus::PRETREAT_DISTANCES_ERROR_FINALL)
	{
		throw MyException(2, "PRETREAT_DISTANCES_ERROR_FINALL", "预处理距离时出现错误");
	}
	return requirement;
}

PretreatStatue UserRequirement::pretreatCities(UserRequirementAfterPretreat& requirement)
{
	requirement.start_cities = vector<string>(city_num);
	requirement.arrive_cities = vector<string>(city_num);

	for (int i = 0; i < this->city_num; i++)//city_num 是城市组数
	{
		if (start_cities[i].empty() || arrive_cities[i].empty())//不能为空
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
		return PRETREAT_TIME_FAILED;
	}
	return PRETREAT_TIME_SUCCEED;
}

PretreatStatue UserRequirement::timeToMyTimeAndIntoRequirementAfterPreTreat(UserRequirementAfterPretreat& requirement)
{
	if ((this->start_time.empty()) && (this->arrive_time.empty()))//时间都没给
	{
		return PRETREAT_TIME_NO_BOTH_TIME;
	}
	requirement.start_time = vector<MyTime>(city_num);
	requirement.arrive_time = vector<MyTime>(city_num);
	if (!this->start_time.empty())//若出发时间不为空
	{
		for (int i = 0; i < city_num; i++)
		{
			if (!start_time[i].empty())
			{
				requirement.start_time[i] = MyTime::stringToMyTime(this->start_time[i], YYYY_MM_DD_HH_MM);
			}
			else
			{
				requirement.start_time[i] = MyTime();
			}
		}
	}
	else
	{
		for (int i = 0; i < city_num; i++)
		{
			requirement.start_time[i] = MyTime();
		}
		return PRETREAT_TIME_NO_START_TIME;
	}

	if (!this->arrive_time.empty())//若到达时间不为空
	{
		for (int i = 0; i < city_num; i++)
		{
			if (!arrive_time[i].empty())
			{
				requirement.arrive_time[i] = MyTime::stringToMyTime(this->arrive_time[i], YYYY_MM_DD_HH_MM);
			}
			else
			{
				requirement.arrive_time[i] = MyTime();
			}
		}
	}
	else
	{
		for (int i = 0; i < city_num; i++)
		{
			requirement.arrive_time[i] = MyTime();
		}
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
		throw MyException(2,"PRETREAT_TIME_NO_START_TIME","后端没给出开始时间");
	case PRETREAT_TIME_NO_ARRIVLE_TIME:
	case PRETREAT_TIME_INTO_REQUIREMENT_SUCCEED:
		return PRETREAT_TIME_SUCCEED;
		break;
	}
	return PRETREAT_TIME_FAILED;
}

PretreatStatue UserRequirement::pretreatTravelType(UserRequirementAfterPretreat& requirement)
{
	if (this->travel_type.empty()||this->travel_type=="任意")
	{
		this->travel_type = (char*)"个人出游";
	}

	//种类已经可以决定推荐的时间、价格。。类型了 只不过可能会根据备注而改变
	if (this->travel_type == "商务出差")
	{
		requirement.travelType = BUSINESS_TRIP;
		requirement.timeType = TIME_BETTER;
		requirement.priceType = PRICE_REGARDLESS;
	}
	else if (this->travel_type == "个人出游")
	{
		requirement.travelType = PERSONAL_TRIP;
		requirement.timeType = TIME_REGARDLESS;
		requirement.priceType = PRICE_AFFORDABLE;
	}
	else if (this->travel_type == "朋友出游")
	{
		requirement.travelType = WITH_FRIEND_TRIP;
		requirement.timeType = TIME_REGARDLESS;
		requirement.priceType = PRICE_AFFORDABLE;
	}
	else if (this->travel_type == "家庭出游")
	{
		requirement.travelType = WITH_FAMILIES_TRIP;
		requirement.timeType = TIME_BETTER;
		requirement.priceType = PRICE_AFFORDABLE;
	}
	else if (this->travel_type == "上学")
	{
		requirement.travelType = TO_SCHOOL_TRIP;
		requirement.timeType = TIME_REGARDLESS;
		requirement.priceType = PRICE_AFFORDABLE;
	}
	else if (this->travel_type == "紧急出行")
	{
		requirement.travelType = URGENT_TRIP;
		requirement.timeType = TIME_FIRST;
		requirement.priceType = PRICE_REGARDLESS;
	}
	else if (this->travel_type == "商务出差|紧急出行")
	{
		requirement.travelType = URGENT_BUSINESS_TRIP;
		requirement.timeType = TIME_FIRST;
		requirement.priceType = PRICE_REGARDLESS;
	}
	else if (this->travel_type == "上学|紧急出行")
	{
		requirement.travelType = URGENT_TO_SCHOOL_TRIP;
		requirement.timeType = TIME_FIRST;
		requirement.priceType = PRICE_REGARDLESS;
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
		throw MyException(2,"PRETREAT_TRAVELTYPE_FAILED_ERROR_TYPE","出游方式(travel_type)错误,不存在这种方式");
	case PRETREAT_TRAVELTYPE_SUCCEED:
		return PRETREAT_TRAVELTYPE_SUCCEED;
		break;
	}
	return PRETREAT_TRAVELTYPE_FAILED;
}

PretreatStatue UserRequirement::pretreatVehicleExperience(UserRequirementAfterPretreat& requirement)
{
	/*
		处理交通工具类型
	*/

	requirement.vehicleType = vector<VehicleType>(this->city_num);
	requirement.transitType = vector <TransitType>(this->city_num);
	//后端传回来时 vehicle_type必须不为空 即使用户给空 后端要自己加上任意
	for (int i = 0; i < this->city_num; i++)
	{
		if (this->vehicle_type[i] == "任意")
		{
			requirement.vehicleType[i] = ALL_VEHICLE;
		}
		else if (this->vehicle_type[i] == "火车")
		{
			requirement.vehicleType[i] = UserRequirementNamespace::HSRC;
		}
		else if (this->vehicle_type[i] == "航空")
		{
			requirement.vehicleType[i] = AIRPLANE;
		}
		else
		{
			return PRETREAT_VEHICLE_TYPE_ERROR_TYPE;
		}
	}

	/*
			根据requirement.priceType来决定ticket类型
		*/
	switch (requirement.priceType)
	{
	case PRICE_REGARDLESS:
		switch (requirement.timeType)
		{
		case TIME_FIRST:
			requirement.ticketType = ALL_CLASS_TICKET;
			break;
		case TIME_REGARDLESS:
		case TIME_BETTER:
			requirement.ticketType = SECOND_CLASS_TICKET;
			break;
		}
		return PRETREAT_TICKET_TYPE_SUCCEED;
		break;
	case PRICE_AFFORDABLE:
	case PRICE_CHEAP:
		requirement.ticketType = SECOND_CLASS_TICKET;
		return PRETREAT_TICKET_TYPE_SUCCEED;
		break;
	case PRICE_EXPENSIVE:
		requirement.ticketType = BUSINESS_CLASS_TICKET;
		return PRETREAT_TICKET_TYPE_SUCCEED;
		break;
	default:
		return PRETREAT_TICKET_TYPE_ERROR;
		break;
	}

	return PRETREAT_TICKET_TYPE_ERROR;
}

PretreatStatue UserRequirement::dealPretreatVehicleExperienceStatue(PretreatStatue statue_code)
{
	switch (statue_code)
	{
	case PRETREAT_VEHICLE_TYPE_EMPTY_TYPE:
		throw MyException(2, "PRETREAT_VEHICLE_TYPE_EMPTY_TYPE", "后端没有传递交通工具类型(vehicle_type)");
	case PRETREAT_VEHICLE_TYPE_ERROR_TYPE:
		throw MyException(2, "PRETREAT_VEHICLE_TYPE_ERROR_TYPE", "后端传递了错误的交通工具类型(vehicle_type)");
	case PRETREAT_TICKET_TYPE_ERROR:
		throw MyException(1, "PRETREAT_TICKET_TYPE_ERROR", "priceType错误(处理travelType时发生了意外的错误)");
	case PRETREAT_TICKET_TYPE_SUCCEED:
	case PRETREAT_VEHICLE_TYPE_SUCCEED:
		return PRETREAT_VEHICLE_EXPERIENCE_SUCCEED;
	default:
		return PRETREAT_VEHICLE_EXPERIENCE_FAILED;
		break;
	}
}

PretreatStatue UserRequirement::pretreatTransitType(UserRequirementAfterPretreat& requirement)
{
	for (int i = 0; i < this->city_num; i++)
	{
		if (this->transit_type[i].empty())
		{
			this->transit_type[i] = (char*)"直达";
		}

		if (this->transit_type[i] == "直达")
		{
			requirement.transitType[i] = DIRECT;
		}
		else if (this->transit_type[i] == "中转")
		{
			requirement.transitType[i] = TRANS;
		}
		else if (this->transit_type[i] == "混合中转")
		{
			//只有交通工具指定为ALL才能混合中转
			if (requirement.vehicleType[i] == ALL_VEHICLE)
				requirement.transitType[i] = FIX_TRANS;
			else
				return PRETREAT_TRANSIT_TYPE_ERROR_FIX_ONLY_WHEN_VEHICLE_ALL;
		}
		else if (this->transit_type[i] == "任意")
		{
			requirement.transitType[i] = ALL_TRANSIT;
		}
		else
		{
			return PRETREAT_TRANSIT_TYPE_ERROR_TYPE;
		}
	}

	return PretreatTransitTypeStatus::PRETREAT_TRANSIT_TYPE_SUCCEED;
}

PretreatStatue UserRequirement::dealPretreatTransitTypeStatue(PretreatStatue statue_code)
{
	switch (statue_code)
	{
	case PRETREAT_TRANSIT_TYPE_ERROR_TYPE:
		throw MyException(2, "PRETREAT_TRANSIT_TYPE_ERROR_TYPE", "中转类型字符串错误");
	case PretreatTransitTypeStatus::PRETREAT_TRANSIT_TYPE_SUCCEED:
	case PRETREAT_TRANSIT_TYPE_ERROR_FIX_ONLY_WHEN_VEHICLE_ALL:
		return PRETREAT_TRANSIT_TYPE_SUCCEED_FINALL;
	default:
		return PRETREAT_TRANSIT_TYPE_FAILED_FINALL;
		break;
	}
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
		{
			requirement.remark = "出游不能变为紧急 所以我们无法根据您的备注来推荐哦";
			return REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT;//出游不能变成紧急 要不就选紧急出行 出游备注紧急 则直接返回 并且提示用户出行不能变为紧急 所以我们无法根据您的备注来推荐哦
		}
			
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
		{
			requirement.remark = "下次和家人一起出游时再添加这些备注吧~";
			return REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES;//和出游不能变成紧急类似 直接忽略备注 并且返回提示
		}
			
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
	{
		requirement.remark = "对不起 暂时无法理解您的备注哦";
		return NO_MATCH_REMARK;
	}
		

	PretreatRemarkNeeds pretreat_remark_needs_status = (PretreatRemarkNeeds)toDealPretreatRemarkNeeds(requirement);
	switch (pretreat_remark_needs_status)
	{
	case PRETREAT_REMARK_NEEDS_ERROR:
		throw MyException(3, "PRETREAT_REMARK_NEEDS_ERROR", "对不起，根据您的备注和其它需求我们无法判断您究竟是要贵的还是便宜的价格");
	case PRETREAT_REMARK_NEEDS_SUCCEED:
	case PRETREAT_REMARK_NEEDS_EMPTY:
		return REMARK_PRETREAT_END;
		break;
	}
	return PRETREAT_REMARK_FAILED;
}

PretreatStatue UserRequirement::dealPretreatRemarkStatue(PretreatStatue statue_code)
{

	switch (statue_code)
	{
	case NO_REMARK:
	case REMARK_PRETREAT_END:
	case NO_MATCH_REMARK:
	case REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT:
	case REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES:
		return PRETREAT_REMARK_SUCCEED;
		break;
	}
	return PRETREAT_REMARK_FAILED;
}

PretreatStatue UserRequirement::toDealPretreatRemarkNeeds(UserRequirementAfterPretreat& requirement)
{
	if (remark_needs_status_code.empty())
		return PRETREAT_REMARK_NEEDS_EMPTY;
	bool change_to_expensive = false;
	bool change_to_cheap = false;
	for (int i = 0; i < remark_needs_status_code.size(); i++)
	{
		switch (remark_needs_status_code[i])
		{
		case REMARK_CHANGE_TRIP_STATUE_TO_URGENT:
			requirement.timeType = TIME_FIRST;
			requirement.priceType = PRICE_REGARDLESS;
			break;
		case REMARK_FAMILIIES_INCLUDE_THE_OLD_OR_CHILD:
			requirement.priceType = PRICE_REGARDLESS;
			break;
		case REMARK_CHEAP_PRICE:
			if (!change_to_expensive)
			{
				requirement.priceType = PRICE_CHEAP;
				change_to_cheap = true;
			}
			else
			{
				return PRETREAT_REMARK_NEEDS_ERROR;
			}
			break;
		case REMARK_COMFORTABLE_EXPERIENCE:
			if (!change_to_cheap)
			{
				requirement.priceType = PRICE_EXPENSIVE;
				change_to_expensive = true;
			}
			else
			{
				return PRETREAT_REMARK_NEEDS_ERROR;
			}
			break;
		}
	}
	return PRETREAT_REMARK_NEEDS_SUCCEED;
}

PretreatStatue UserRequirement::dealPretreatDistancesStatue(PretreatStatue statue_code)
{
	if (statue_code != PRETREAT_DISTANCES_SUCCEED)
	{
		return PRETREAT_DISTANCES_ERROR_FINALL;
	}
	return PRETREAT_DISTANCES_SUCCEED_FINALL;
}

PretreatStatue UserRequirement::pretreatDistances(UserRequirementAfterPretreat& requirement)
{
	if (this->distances.empty())
		return PRETREAT_DISTANCES_ERROR;
	requirement.distances = vector<int>(city_num);
	for (int i = 0; i < this->city_num; i++)
	{
		requirement.distances[i] = distances[i];
	}
	return PRETREAT_DISTANCES_SUCCEED;
}
