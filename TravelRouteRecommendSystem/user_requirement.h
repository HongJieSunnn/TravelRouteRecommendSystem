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
	//regardless:������ Ҳ���ǿ��Խ����κ��Ƽ���

	/*
		ʱ��Ȩ��ö��
	*/
	enum TimeWeightEnum
	{
		TIME_REGARDLESS,
		TIME_FIRST,
		TIME_BETTER
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
		ONE_CLASS_TICKET = 1,
		SECOND_CLASS_TICKET,
		BUSINESS_CLASS_TICKET,
		ALL_CLASS_TICKET//�������Ͷ�չʾ �û��Լ�ѡ

		//������Ʊ�۶��Ž�ȥ �����ŷŵ�˳��ͬ �����ٶ������������� �ͰѶ����������Ƽ�
	};
	/*
		��ͨ����ö��
	*/
	enum VehicleTypeEnum
	{
		ALL_VEHICLE = 1,
		HSRC,
		AIRPLANE,
	};
	/*
		���з�ʽ ���޶��ļ���
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
		��ת���� ��
			1. ֱ��
			2. ��ת
			3. �����ת
	*/
	enum TransitTypeEnum
	{
		DIRECT = 1,
		TRANS,
		FIX_TRANS,
		ALL_TRANSIT//����ֱ�����ת ���ǲ�����FIX
		/*
		* tip:
		* ���û�ѡ��DIRECT ����û��ֱ�� ��ô����ʾ û��ֱ��
		* �������ALL_TRANSIT ����Ҫ���ûֱ���Ūת�� ����ȫ����ʾ
		*/
	};
	/*
		Ԥ����״̬�� �����������
		������سɹ���ʧ�� ÿ������ľ��������ж�Ӧ��ö��
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
		NO_MATCH_REMARK,//��ע��û�а������϶������Щ����������(���籸ע���˸�sb ��ô��ִ���κβ���) ��עȴ��Ϊ��
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
	�ýṹ��������Ԥ�������û�����	һ�����һ���Ƽ� ������Ҫ����
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
	������������洢�û����� ����Ԥ��������
	��c#��������ݻ�ŵ�������
	����c#string �� c++stringת�����鷳(string��char���������Elem �����������ַ�������char* ����ʹ����ת��string)
	*start_cities:��������(s) ����ж��������� ���縣��->�Ͼ�->���� ��������Ϊ���� �Ͼ�(ֻҪ�������յ��� ���ǳ�������)
	*arrive_cities:������� �����ж��
	*city_num:�������� ���м������ �������г���-1
	*start_time:����ʱ�� �������� Ĭ��Ϊ��ǰʱ��һСʱ��(TODO:���ߵ���api��õ�ǰ��ַ����վ�����ʱ��+��Сʱ(�鷳 Ӧ�ò���ʵ��)) ����ɺ�˲���
	*arrive_time:�����ĵ���ʱ��
		!*	ʱ���ʽ����Ϊ yyyy.mm.dd hh:mm|hh:mm	  *!(ǰһ�����hh:mm=00:00 ˵��û��ʱ��� ��Ҫ�Ƽ� ����˵���Ƿ������ߵ���ʱ����00:00)
		!*	����ֻ������ʱ�� ����ֻ������ʱ�� ���߶�����	 *!
	*vehicle_type:�������ؾ� �������д ͨ��travel_type���ж�
	*travel_type:���з�ʽ �һ�ָ��ֻ���޶��ļ���
		1. �������
		2. ���˳���
		3. ���ѳ���
		4. ��ͥ����:�豸ע���ˡ�С������ͥ��Ա����״����(����ע����������С�� ����Ҫ���� ʱ��� �Դ�����Ȩ��)
		5. ��ѧ:�������ĳ��ʱ�������и����ͺ��� ������ѡ���з�ʽ ����ͨ������
		6. ��������:��������������Ҫ���������� ֱ�������ʱ����콻ͨ��ʽ
		��������������к���ѧ���������
	*transit_type:��ת��ʽ ����ָ�� һ��Ĭ�������Ƽ�ֱ��
	*distances:ͨ�����õ�ͼapi��ȡÿ��������֮��ֱ�߾��� �Ӷ��жϽ�ͨ�����Ƽ�
	*remark:��ע
*/

//��ʹ�û���Щ����û���� ���Ǻ�˸�C++ʱ ��Ҫ��û������Ĭ�ϲ�ȫ ��ΪC#��������Щ
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
		Ԥ�����û����� �õ��Ƽ��ķ�ʽ(����ʱ�� ��ת��ʽ Ǯ�ȵ�) ���������ʹ��
	*/
	UserRequirementAfterPretreat pretreatUserRequirement();
protected:
	/*
		Ԥ������� ������ʼ���кͽ�������
		*requirement:���Ҫ�����ص��Ǹ�Ԥ��������û�����
	*/
	PretreatStatue pretreatCities(UserRequirementAfterPretreat& requirement);

	/*
		Ԥ����ʱ�� �õ�ʱ��������ʱ������(����)��������ν���Ƽ�
			- ������һ����Ϊ�� ����û�������Ϊȫ�� ���Ҫ��start_time��ֵ�ɵ�ǰʱ��
			- �û�����ֻ���� ����ʱ�ͷ� ��ôĬ��Ϊ0 ����Ϊ0 ˵�������ľ���ʱ�����Ҫ�Ƽ�
			- Ԥ�����޷��Ƽ�������ʱ�� ��Ҫ�����Ƽ�ʱ���ܵõ��Ƽ��ľ���ʱ���
	*/
	PretreatStatue pretreatTime(UserRequirementAfterPretreat& requirement);
	//��ʱ���stringת����MyTime����¼�뵽Ԥ������������
	PretreatStatue timeToMyTimeAndIntoRequirementAfterPreTreat(UserRequirementAfterPretreat& requirement);

	/*
		����Ԥ����ʼʱ��ͳ���ʱ��״̬��ĺ���
	*/
	PretreatStatue dealPretreatTimeStatue(PretreatStatue statue_code);

	/*
		����Ȩ�صĺ���
		*������ û��Ҫ����Ȩ��
		����sql���sort˳���sort�и���������ʲô����
	*/
	//PretreatStatue assignWeights(UserRequirementAfterPretreat& requirement);

	/*
		����������������Ƿ���ҪԤ������з�ʽ
			���� ������עд���� ��ô�͸ĳɽ�������
	*/
	PretreatStatue pretreatTravelType(UserRequirementAfterPretreat& requirement);

	/*
		����Ԥ������з�ʽ״̬��ĺ���
	*/
	PretreatStatue dealPretreatTravelTypeStatue(PretreatStatue statue_code);
	
	/*
		Ԥ����ͨ�������� ����ͨ�������ͺ�Ʊ����(��λ���� ��ͬ��λ����϶���ͬ)
	*/
	PretreatStatue pretreatVehicleExperience(UserRequirementAfterPretreat& requirement);
	/*
		����Ԥ����ͨ����״̬��
	*/
	PretreatStatue dealPretreatVehicleExperienceStatue(PretreatStatue statue_code);

	/*
		Ԥ������ת��ʽ ��ʵֱ�������� �����û�ָ�� ����һ�㲻�Ƽ�ת���� ����û��ֱ��
	*/
	PretreatStatue pretreatTransitType(UserRequirementAfterPretreat& requirement);
	/*
		����Ԥ������ת��ʽ״̬��
	*/
	PretreatStatue dealPretreatTransitTypeStatue(PretreatStatue statue_code);

	/*
		Ԥ����ע
	*/
	PretreatStatue pretreatRemark(UserRequirementAfterPretreat& requirement);
	/*
	����Ԥ����ע״̬��ĺ���
	*/
	PretreatStatue dealPretreatRemarkStatue(PretreatStatue statue_code);

	/*
	����Ԥ����ע����ĺ���
	*/
	PretreatStatue toDealPretreatRemarkNeeds(UserRequirementAfterPretreat& requirement);
};
#endif // !USERREQUIREMENT_H