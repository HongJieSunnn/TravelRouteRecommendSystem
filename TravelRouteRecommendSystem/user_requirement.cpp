#include "user_requirement.h"
#include"my_time.h"
#include"route_result.h"
#include<vector>
#include<string>
using std::vector;
using std::string;

namespace UserRequirementNamespace
{
	//regardless:������ Ҳ���ǿ��Խ����κ��Ƽ���

	/*
		ʱ��Ȩ��ö��
	*/
	enum TimeWeightEnum
	{
		TIME_REGARDLESS,
		TIME_FIRST
	};
	/*
		�۸�Ȩ��ö��
	*/
	enum PriceWeightEnum
	{
		PRICE_REGARDLESS,
		PRICE_CHEAP,
		PRICE_AFFORDABLE,
		PRICE_EXPENSIVE
	};
	/*
		Ʊ���� ͨ���������ͷ���
		�ɻ�ֻ��һ��Ʊ�� �Ͱ�������������
	*/
	enum TicketTypeEnum
	{
		ONE_CLASS_TICKET=1,
		SECOND_CLASS_TICKET,
		BUSINESS_CLASS_TICKET,
	};
	/*
		��ͨ����ö��
	*/
	enum VehicleTypeEnum
	{
		ALL=1,
		HSRC,
		AIRPLANE,
	};
	/*
		���з�ʽ ���޶��ļ���
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
		��ת���� ��
			1. ֱ��
			2. ��ת
			3. �����ת
	*/
	enum TransitTypeEnum
	{
		DIRECT=1,
		TRANS,
		FIX_TRANS,
	};
	/*
		Ԥ����״̬�� �����������
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
		NO_MATCH_REMARK,//��ע��û�а������϶������Щ����������(���籸ע���˸�sb ��ô��ִ���κβ���) ��עȴ��Ϊ��
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
		Ԥ����˳��:
			1. �ȸ����û�����ĳ��������Լ���ע�õ�����ϵĳ�������ö��ֵ
			2. ����Ҫ�ĳ���¼�뵽������
			3. ���ݳ�������ö��ֵ�Լ��û������ĳ���ʱ���״̬(����ʱ���û��)���Ƽ���õĳ���ʱ��
			4. 
	*/

}

PretreatStatue UserRequirement::pretreatCities(UserRequirementAfterPretreat& requirement)
{
	requirement.start_cities = vector<string>(city_num);
	requirement.arrive_cities = vector<string>(city_num);

	for (int i = 0; i < this->city_num; i++)//city_num �ǳ�������
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
	if (this->travel_type == "�������")
	{
		requirement.travelType = BUSINESS_TRIP;
	}
	else if (this->travel_type == "���˳���")
	{
		requirement.travelType = PERSONAL_TRIP;
	}
	else if (this->travel_type == "���ѳ���")
	{
		requirement.travelType = WITH_FRIEND_TRIP;
	}
	else if (this->travel_type == "��ͥ����")
	{
		requirement.travelType = WITH_FAMILIES_TRIP;
	}
	else if (this->travel_type == "��ѧ")
	{
		requirement.travelType = TO_SCHOOL_TRIP;
	}
	else if (this->travel_type == "��������")
	{
		requirement.travelType = URGENT_TRIP;
	}
	else if (this->travel_type == "�������|��������")
	{
		requirement.travelType = URGENT_BUSINESS_TRIP;
	}
	else if (this->travel_type == "��ѧ|��������")
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
			return REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT;//���β��ܱ�ɽ��� Ҫ����ѡ��������
		return REMARK_CHANGE_TRIP_STATUE_TO_URGENT;
	}

	if (remark_str.find("����") != string::npos ||
		remark_str.find("��") != string::npos )
	{
		if (requirement.travelType == WITH_FAMILIES_TRIP)
		{
			return REMARK_FAMILIIES_INCLUDE_THE_OLD_OR_CHILD;
		}
		else
			return REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES;
	}

	if (remark_str.find("����") != string::npos)
	{
		return REMARK_CHEAP_PRICE;
	}
	if (remark_str.find("��") != string::npos)
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
		//TODO ����϶�Ҫ�ĵ� ʵ���Ͼ��ǰ�Ҫ���ظ���˵Ľ���ı�ע�����һЩ��Ϣ
		RouteResult::remark = (char*)"�Բ��� ��ʱ�޷�������ı�עŶ";
		return PRETREAT_REMARK_SUCCEED;
		break;
	case REMARK_CHANGE_TRIP_STATUE_TO_URGENT:
	case REMARK_CANNOT_CHANGE_TRIP_STATUE_TO_URGENT:
		//TODO ����϶�Ҫ�ĵ� ʵ���Ͼ��ǰ�Ҫ���ظ���˵Ľ���ı�ע�����һЩ��Ϣ
		RouteResult::remark = (char*)"�ٽ���ҲҪע�ⰲȫŶ";
		return PRETREAT_REMARK_SUCCEED;
		break;
	case REMARK_FAMILIIES_INCLUDE_THE_OLD_OR_CHILD:
		//TODO ����϶�Ҫ�ĵ� ʵ���Ͼ��ǰ�Ҫ���ظ���˵Ľ���ı�ע�����һЩ��Ϣ
		RouteResult::remark = (char*)"ףһ���˳��ο���Ŷ";
		return PRETREAT_REMARK_SUCCEED;
		break;
	case REMARK_FAMLIIES_INCLUDE_THE_OLD_OR_CHILD_BUT_NOT_WITH_FAMILIES:
		//TODO ����϶�Ҫ�ĵ� ʵ���Ͼ��ǰ�Ҫ���ظ���˵Ľ���ı�ע�����һЩ��Ϣ
		RouteResult::remark = (char*)"�´κͼ���һ�����ʱ�������Щ��ע��~";
		return PRETREAT_REMARK_SUCCEED;
		break;
	case REMARK_CHEAP_PRICE:
	case REMARK_COMFORTABLE_EXPERIENCE:
		return PRETREAT_REMARK_SUCCEED;
		break;
	}
	return PRETREAT_REMARK_FAILED;
}
