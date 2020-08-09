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
		throw "预处理城市时出现错误";
	}

	if ((PretreatStatus)pretreatTime(requirement) == PretreatStatus::PRETREAT_TIME_FAILED)
	{
		throw "预处理时间时出现错误";
	}

	if ((PretreatStatus)dealPretreatTravelTypeStatue(pretreatTravelType(requirement)) == PretreatStatus::PRETREAT_TRAVELTYPE_FAILED)
	{
		throw "预处理出游方式时出现错误";
	}

	if ((PretreatStatus)dealPretreatRemarkStatue(pretreatRemark(requirement)) == PretreatStatus::PRETREAT_REMARK_FAILED)
	{
		throw "预处理备注时出现错误";
	}

	if ((PretreatStatus)dealPretreatVehicleExperienceStatue(pretreatVehicleExperience(requirement)) == PretreatStatus::PRETREAT_VEHICLE_EXPERIENCE_FAILED)
	{
		throw "预处理交通体验时出现错误";
	}

	if ((PretreatStatus)dealPretreatTransitTypeStatue(pretreatTransitType(requirement)) == PretreatStatus::PRETREAT_transit_TYPE_FAILED_FINALL)
	{
		throw "预处理中转方式时出现错误";
	}

	return requirement;
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
		return PRETREAT_TIME_FAILED;
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
		requirement.start_time = MyTime();
		return PRETREAT_TIME_NO_START_TIME;
	}

	if (this->arrive_time != "" && this->arrive_time != nullptr)//若到达时间不为空
	{
		requirement.arrive_time = MyTime::stringToMyTime(this->arrive_time, YYYY_MM_DD_HH_MM);
	}
	else
	{
		requirement.arrive_time = MyTime();
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
	return PRETREAT_TIME_FAILED;
}

PretreatStatue UserRequirement::pretreatTravelType(UserRequirementAfterPretreat& requirement)
{
	if (this->travel_type == "" || this->travel_type == nullptr)
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
		return PRETREAT_TRAVELTYPE_FAILED;
		break;
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
		else if (this->vehicle_type[i] == "铁路")
		{
			requirement.vehicleType[i] = HSRC;
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
		//TODO:直接结束 返回后端表示没有选择交通工具(一般处理过不会出现这个 出现这个代码意味着后端传过来的是空的)
		return PRETREAT_VEHICLE_EXPERIENCE_FAILED;
		break;
	case PRETREAT_VEHICLE_TYPE_ERROR_TYPE:
		//TODO:直接结束 返回后端说交通工具类型错误
		return PRETREAT_VEHICLE_EXPERIENCE_FAILED;
		break;
	case PRETREAT_TICKET_TYPE_ERROR:
		//TODO:priceType错误
		return PRETREAT_VEHICLE_EXPERIENCE_FAILED;
		break;
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
		if (this->transit_type[i] == "" || this->transit_type[i] == nullptr)
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
				return PRETREAT_transit_TYPE_ERROR_FIX_ONLY_WHEN_VEHICLE_ALL;
		}
		else if (this->transit_type[i] == "任意")
		{
			requirement.transitType[i] = ALL_TRANSIT;
		}
		else
		{
			return PRETREAT_transit_TYPE_ERROR_TYPE;
		}
	}

	return PretreatTransitTypeStatus::PRETREAT_transit_TYPE_SUCCEED;
}

PretreatStatue UserRequirement::dealPretreatTransitTypeStatue(PretreatStatue statue_code)
{
	switch (statue_code)
	{
	case PRETREAT_transit_TYPE_ERROR_TYPE:
		//TODO:中转类型字符串错误
		return PRETREAT_transit_TYPE_FAILED_FINALL;
		break;
	case PretreatTransitTypeStatus::PRETREAT_transit_TYPE_SUCCEED:
		return PretreatStatus::PRETREAT_transit_TYPE_SUCCEED_FINALL;
		break;
	case PRETREAT_transit_TYPE_ERROR_FIX_ONLY_WHEN_VEHICLE_ALL:
		return PRETREAT_transit_TYPE_SUCCEED_FINALL;
	default:
		return PRETREAT_transit_TYPE_FAILED_FINALL;
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

	PretreatRemarkNeeds pretreat_remark_needs_status = (PretreatRemarkNeeds)toDealPretreatRemarkNeeds(requirement);
	switch (pretreat_remark_needs_status)
	{
	case PRETREAT_REMARK_NEEDS_ERROR:
		//TODO:直接返回信息给后端
		return PRETREAT_REMARK_FAILED;
		break;
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
				RouteResult::remark = (char*)"对不起，我们无法判断您究竟是要贵的还是便宜的价格";
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
				RouteResult::remark = (char*)"对不起，我们无法判断您究竟是要贵的还是便宜的价格";
				return PRETREAT_REMARK_NEEDS_ERROR;
			}
			break;
		}
	}
	return PRETREAT_REMARK_NEEDS_SUCCEED;
}
