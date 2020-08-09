#include "user_requirement.h"

using namespace UserRequirementNamespace;

//��ע��������
vector<PretreatRemarkStatus> remark_needs_status_code;

UserRequirementAfterPretreat UserRequirement::pretreatUserRequirement()
{
	UserRequirementAfterPretreat requirement;
	/*
		Ԥ����˳��:
			1. �ȸ����û�����ĳ��������Լ���ע�õ�����ϵĳ�������ö��ֵ
			2. ����Ҫ�ĳ���¼�뵽������
			3. ���ݳ�������ö��ֵ�Լ��û������ĳ���ʱ���״̬(����ʱ���û��)���Ƽ���õĳ���ʱ��
			4. 
	*/
	if ((PretreatStatus)pretreatCities(requirement) == PretreatStatus::PRETREAT_CITIES_FAILED)
	{
		throw "Ԥ�������ʱ���ִ���";
	}

	if ((PretreatStatus)pretreatTime(requirement) == PretreatStatus::PRETREAT_TIME_FAILED)
	{
		throw "Ԥ����ʱ��ʱ���ִ���";
	}

	if ((PretreatStatus)dealPretreatTravelTypeStatue(pretreatTravelType(requirement)) == PretreatStatus::PRETREAT_TRAVELTYPE_FAILED)
	{
		throw "Ԥ������η�ʽʱ���ִ���";
	}

	if ((PretreatStatus)dealPretreatRemarkStatue(pretreatRemark(requirement)) == PretreatStatus::PRETREAT_REMARK_FAILED)
	{
		throw "Ԥ����עʱ���ִ���";
	}

	if ((PretreatStatus)dealPretreatVehicleExperienceStatue(pretreatVehicleExperience(requirement)) == PretreatStatus::PRETREAT_VEHICLE_EXPERIENCE_FAILED)
	{
		throw "Ԥ����ͨ����ʱ���ִ���";
	}

	if ((PretreatStatus)dealPretreatTransitTypeStatue(pretreatTransitType(requirement)) == PretreatStatus::PRETREAT_transit_TYPE_FAILED_FINALL)
	{
		throw "Ԥ������ת��ʽʱ���ִ���";
	}

	return requirement;
}

PretreatStatue UserRequirement::pretreatCities(UserRequirementAfterPretreat& requirement)
{
	requirement.start_cities = vector<string>(city_num);
	requirement.arrive_cities = vector<string>(city_num);

	for (int i = 0; i < this->city_num; i++)//city_num �ǳ�������
	{
		if (!start_cities[i] || !arrive_cities[i])//����Ϊ��
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
		//ֱ�ӷ��ش�������Ϣ����������
		return PRETREAT_TIME_FAILED;
	}
	return PRETREAT_TIME_SUCCEED;
}

PretreatStatue UserRequirement::timeToMyTimeAndIntoRequirementAfterPreTreat(UserRequirementAfterPretreat& requirement)
{
	if ((this->start_time == "" || this->start_time == nullptr) && (this->arrive_time == "" || this->start_time == nullptr))//ʱ�䶼û��
	{
		return PRETREAT_TIME_NO_BOTH_TIME;
	}

	if (this->start_time != "" && this->start_time != nullptr)//������ʱ�䲻Ϊ��
	{
		requirement.start_time = MyTime::stringToMyTime(this->start_time, YYYY_MM_DD_HH_MM);
	}
	else
	{
		requirement.start_time = MyTime();
		return PRETREAT_TIME_NO_START_TIME;
	}

	if (this->arrive_time != "" && this->arrive_time != nullptr)//������ʱ�䲻Ϊ��
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
		RouteResult::error = (char*)"����ʱ��մ���";
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
		this->travel_type = (char*)"���˳���";
	}

	//�����Ѿ����Ծ����Ƽ���ʱ�䡢�۸񡣡������� ֻ�������ܻ���ݱ�ע���ı�
	if (this->travel_type == "�������")
	{
		requirement.travelType = BUSINESS_TRIP;
		requirement.timeType = TIME_BETTER;
		requirement.priceType = PRICE_REGARDLESS;
	}
	else if (this->travel_type == "���˳���")
	{
		requirement.travelType = PERSONAL_TRIP;
		requirement.timeType = TIME_REGARDLESS;
		requirement.priceType = PRICE_AFFORDABLE;
	}
	else if (this->travel_type == "���ѳ���")
	{
		requirement.travelType = WITH_FRIEND_TRIP;
		requirement.timeType = TIME_REGARDLESS;
		requirement.priceType = PRICE_AFFORDABLE;
	}
	else if (this->travel_type == "��ͥ����")
	{
		requirement.travelType = WITH_FAMILIES_TRIP;
		requirement.timeType = TIME_BETTER;
		requirement.priceType = PRICE_AFFORDABLE;
	}
	else if (this->travel_type == "��ѧ")
	{
		requirement.travelType = TO_SCHOOL_TRIP;
		requirement.timeType = TIME_REGARDLESS;
		requirement.priceType = PRICE_AFFORDABLE;
	}
	else if (this->travel_type == "��������")
	{
		requirement.travelType = URGENT_TRIP;
		requirement.timeType = TIME_FIRST;
		requirement.priceType = PRICE_REGARDLESS;
	}
	else if (this->travel_type == "�������|��������")
	{
		requirement.travelType = URGENT_BUSINESS_TRIP;
		requirement.timeType = TIME_FIRST;
		requirement.priceType = PRICE_REGARDLESS;
	}
	else if (this->travel_type == "��ѧ|��������")
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
		����ͨ��������
	*/

	requirement.vehicleType = vector<VehicleType>(this->city_num);
	requirement.transitType = vector <TransitType>(this->city_num);
	//��˴�����ʱ vehicle_type���벻Ϊ�� ��ʹ�û����� ���Ҫ�Լ���������
	for (int i = 0; i < this->city_num; i++)
	{
		if (this->vehicle_type[i] == "����")
		{
			requirement.vehicleType[i] = ALL_VEHICLE;
		}
		else if (this->vehicle_type[i] == "��·")
		{
			requirement.vehicleType[i] = HSRC;
		}
		else if (this->vehicle_type[i] == "����")
		{
			requirement.vehicleType[i] = AIRPLANE;
		}
		else
		{
			return PRETREAT_VEHICLE_TYPE_ERROR_TYPE;
		}
	}

	/*
			����requirement.priceType������ticket����
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
		//TODO:ֱ�ӽ��� ���غ�˱�ʾû��ѡ��ͨ����(һ�㴦������������� �������������ζ�ź�˴��������ǿյ�)
		return PRETREAT_VEHICLE_EXPERIENCE_FAILED;
		break;
	case PRETREAT_VEHICLE_TYPE_ERROR_TYPE:
		//TODO:ֱ�ӽ��� ���غ��˵��ͨ�������ʹ���
		return PRETREAT_VEHICLE_EXPERIENCE_FAILED;
		break;
	case PRETREAT_TICKET_TYPE_ERROR:
		//TODO:priceType����
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
			this->transit_type[i] = (char*)"ֱ��";
		}

		if (this->transit_type[i] == "ֱ��")
		{
			requirement.transitType[i] = DIRECT;
		}
		else if (this->transit_type[i] == "��ת")
		{
			requirement.transitType[i] = TRANS;
		}
		else if (this->transit_type[i] == "�����ת")
		{
			//ֻ�н�ͨ����ָ��ΪALL���ܻ����ת
			if (requirement.vehicleType[i] == ALL_VEHICLE)
				requirement.transitType[i] = FIX_TRANS;
			else
				return PRETREAT_transit_TYPE_ERROR_FIX_ONLY_WHEN_VEHICLE_ALL;
		}
		else if (this->transit_type[i] == "����")
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
		//TODO:��ת�����ַ�������
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
	//��ע���������� �ҳ��з�ʽΪ�������ѧ
	if (remark_str.find("����") != string::npos ||
		remark_str.find("����") != string::npos ||
		remark_str.find("��") != string::npos||
		remark_str.find("��") != string::npos||
		remark_str.find("��") != string::npos||
		remark_str.find("��") != string::npos)
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
			return REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT;//���β��ܱ�ɽ��� Ҫ����ѡ�������� ���α�ע���� ��ֱ�ӷ��� ������ʾ�û����в��ܱ�Ϊ���� ���������޷��������ı�ע���Ƽ�Ŷ
		remark_needs_status_code.push_back(REMARK_CHANGE_TRIP_STATUE_TO_URGENT);
	}

	if (remark_str.find("����") != string::npos ||
		remark_str.find("��") != string::npos )
	{
		if (requirement.travelType == WITH_FAMILIES_TRIP)
		{
			remark_needs_status_code.push_back(REMARK_FAMILIIES_INCLUDE_THE_OLD_OR_CHILD);
		}
		else
			return REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES;//�ͳ��β��ܱ�ɽ������� ֱ�Ӻ��Ա�ע ���ҷ�����ʾ
	}

	if (remark_str.find("����") != string::npos||
		remark_str.find("ʵ��")!=string::npos)
	{
		remark_needs_status_code.push_back(REMARK_CHEAP_PRICE);
	}
	if (remark_str.find("��") != string::npos||
		remark_str.find("��")!=string::npos)
	{
		remark_needs_status_code.push_back(REMARK_COMFORTABLE_EXPERIENCE);
	}

	if (remark_needs_status_code.empty())
		return NO_MATCH_REMARK;

	PretreatRemarkNeeds pretreat_remark_needs_status = (PretreatRemarkNeeds)toDealPretreatRemarkNeeds(requirement);
	switch (pretreat_remark_needs_status)
	{
	case PRETREAT_REMARK_NEEDS_ERROR:
		//TODO:ֱ�ӷ�����Ϣ�����
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
		//TODO ����϶�Ҫ�ĵ� ʵ���Ͼ��ǰ�Ҫ���ظ���˵Ľ���ı�ע�����һЩ��Ϣ
		RouteResult::remark = (char*)"�Բ��� ��ʱ�޷�������ı�עŶ";
		return PRETREAT_REMARK_FAILED;
		break;
	case REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT:
		//TODO ����϶�Ҫ�ĵ� ʵ���Ͼ��ǰ�Ҫ���ظ���˵Ľ���ı�ע�����һЩ��Ϣ
		RouteResult::remark = (char*)"���β��ܱ�Ϊ���� ���������޷��������ı�ע���Ƽ�Ŷ";
		return PRETREAT_REMARK_FAILED;
		break;
	case REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES:
		//TODO ����϶�Ҫ�ĵ� ʵ���Ͼ��ǰ�Ҫ���ظ���˵Ľ���ı�ע�����һЩ��Ϣ
		RouteResult::remark = (char*)"�´κͼ���һ�����ʱ�������Щ��ע��~";
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
				RouteResult::remark = (char*)"�Բ��������޷��ж���������Ҫ��Ļ��Ǳ��˵ļ۸�";
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
				RouteResult::remark = (char*)"�Բ��������޷��ж���������Ҫ��Ļ��Ǳ��˵ļ۸�";
				return PRETREAT_REMARK_NEEDS_ERROR;
			}
			break;
		}
	}
	return PRETREAT_REMARK_NEEDS_SUCCEED;
}
