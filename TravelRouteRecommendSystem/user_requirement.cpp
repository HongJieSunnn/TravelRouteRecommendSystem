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
		throw MyException(2, "CITIES_EMPTY", "�������л򵽴����Ϊ��");
	}

	if ((PretreatStatus)pretreatTime(requirement) == PretreatStatus::PRETREAT_TIME_FAILED)
	{
		throw MyException(2,"PRETREAT_TIME_FAILED","Ԥ����ʱ��ʱ���ִ���");
	}

	if ((PretreatStatus)dealPretreatTravelTypeStatue(pretreatTravelType(requirement)) == PretreatStatus::PRETREAT_TRAVELTYPE_FAILED)
	{
		throw MyException(2, "PRETREAT_TRAVELTYPE_FAILED", "Ԥ������η�ʽʱ���ִ���");
	}

	if ((PretreatStatus)dealPretreatRemarkStatue(pretreatRemark(requirement)) == PretreatStatus::PRETREAT_REMARK_FAILED)
	{
		throw MyException(2, "PRETREAT_REMARK_FAILED", "Ԥ����עʱ���ִ���");
	}

	if ((PretreatStatus)dealPretreatVehicleExperienceStatue(pretreatVehicleExperience(requirement)) == PretreatStatus::PRETREAT_VEHICLE_EXPERIENCE_FAILED)
	{
		throw MyException(2, "PRETREAT_VEHICLE_EXPERIENCE_FAILED", "Ԥ����ͨ����ʱ���ִ���");
	}

	if ((PretreatStatus)dealPretreatTransitTypeStatue(pretreatTransitType(requirement)) == PretreatStatus::PRETREAT_TRANSIT_TYPE_FAILED_FINALL)
	{
		throw MyException(2, "PRETREAT_TRANSIT_TYPE_FAILED_FINALL", "Ԥ������ת��ʽʱ���ִ���");
	}

	if ((PretreatStatus)dealPretreatDistancesStatue(pretreatDistances(requirement)) == PretreatStatus::PRETREAT_DISTANCES_ERROR_FINALL)
	{
		throw MyException(2, "PRETREAT_DISTANCES_ERROR_FINALL", "Ԥ�������ʱ���ִ���");
	}
	return requirement;
}

PretreatStatue UserRequirement::pretreatCities(UserRequirementAfterPretreat& requirement)
{
	requirement.start_cities = vector<string>(city_num);
	requirement.arrive_cities = vector<string>(city_num);

	for (int i = 0; i < this->city_num; i++)//city_num �ǳ�������
	{
		if (start_cities[i].empty() || arrive_cities[i].empty())//����Ϊ��
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
	if ((this->start_time.empty()) && (this->arrive_time.empty()))//ʱ�䶼û��
	{
		return PRETREAT_TIME_NO_BOTH_TIME;
	}
	requirement.start_time = vector<MyTime>(city_num);
	requirement.arrive_time = vector<MyTime>(city_num);
	if (!this->start_time.empty())//������ʱ�䲻Ϊ��
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

	if (!this->arrive_time.empty())//������ʱ�䲻Ϊ��
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
		throw MyException(2,"PRETREAT_TIME_NO_START_TIME","���û������ʼʱ��");
	case PRETREAT_TIME_NO_ARRIVLE_TIME:
	case PRETREAT_TIME_INTO_REQUIREMENT_SUCCEED:
		return PRETREAT_TIME_SUCCEED;
		break;
	}
	return PRETREAT_TIME_FAILED;
}

PretreatStatue UserRequirement::pretreatTravelType(UserRequirementAfterPretreat& requirement)
{
	if (this->travel_type.empty()||this->travel_type=="����")
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
		throw MyException(2,"PRETREAT_TRAVELTYPE_FAILED_ERROR_TYPE","���η�ʽ(travel_type)����,���������ַ�ʽ");
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
		else if (this->vehicle_type[i] == "��")
		{
			requirement.vehicleType[i] = UserRequirementNamespace::HSRC;
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
		throw MyException(2, "PRETREAT_VEHICLE_TYPE_EMPTY_TYPE", "���û�д��ݽ�ͨ��������(vehicle_type)");
	case PRETREAT_VEHICLE_TYPE_ERROR_TYPE:
		throw MyException(2, "PRETREAT_VEHICLE_TYPE_ERROR_TYPE", "��˴����˴���Ľ�ͨ��������(vehicle_type)");
	case PRETREAT_TICKET_TYPE_ERROR:
		throw MyException(1, "PRETREAT_TICKET_TYPE_ERROR", "priceType����(����travelTypeʱ����������Ĵ���)");
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
				return PRETREAT_TRANSIT_TYPE_ERROR_FIX_ONLY_WHEN_VEHICLE_ALL;
		}
		else if (this->transit_type[i] == "����")
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
		throw MyException(2, "PRETREAT_TRANSIT_TYPE_ERROR_TYPE", "��ת�����ַ�������");
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
		{
			requirement.remark = "���β��ܱ�Ϊ���� ���������޷��������ı�ע���Ƽ�Ŷ";
			return REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT;//���β��ܱ�ɽ��� Ҫ����ѡ�������� ���α�ע���� ��ֱ�ӷ��� ������ʾ�û����в��ܱ�Ϊ���� ���������޷��������ı�ע���Ƽ�Ŷ
		}
			
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
		{
			requirement.remark = "�´κͼ���һ�����ʱ�������Щ��ע��~";
			return REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES;//�ͳ��β��ܱ�ɽ������� ֱ�Ӻ��Ա�ע ���ҷ�����ʾ
		}
			
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
	{
		requirement.remark = "�Բ��� ��ʱ�޷�������ı�עŶ";
		return NO_MATCH_REMARK;
	}
		

	PretreatRemarkNeeds pretreat_remark_needs_status = (PretreatRemarkNeeds)toDealPretreatRemarkNeeds(requirement);
	switch (pretreat_remark_needs_status)
	{
	case PRETREAT_REMARK_NEEDS_ERROR:
		throw MyException(3, "PRETREAT_REMARK_NEEDS_ERROR", "�Բ��𣬸������ı�ע���������������޷��ж���������Ҫ��Ļ��Ǳ��˵ļ۸�");
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
