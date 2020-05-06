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

	*distances:ͨ�����õ�ͼapi��ȡÿ��������֮��ֱ�߾��� �Ӷ��жϽ�ͨ�����Ƽ�
	*remark:��ע

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
		Ԥ�����û����� �õ��Ƽ��ķ�ʽ(����ʱ�� ��ת��ʽ Ǯ�ȵ�) ���������ʹ��
	*/
	UserRequirementAfterPretreat pretreatUserRequirement() noexcept;

	/*
		Ԥ������� ������ʼ���кͽ�������
		*requirement:���Ҫ�����ص��Ǹ�Ԥ��������û�����
	*/
	PretreatStatue pretreatCities(UserRequirementAfterPretreat& requirement);

	/*
		Ԥ����ʱ�� ������ʼʱ��͵���ʱ��
			- ������һ����Ϊ�� ����û�������Ϊȫ�� ���Ҫ��start_time��ֵ�ɵ�ǰʱ��
			- �û�����ֻ���� ����ʱ�ͷ� ��ôĬ��Ϊ0 ����Ϊ0 ˵�������ľ���ʱ�����Ҫ�Ƽ�
			- ��Ҫ��ͨ��Ԥ���������˿��Ƽ����ʺ�����ʱ��
	*/
	PretreatStatue pretreatTime(UserRequirementAfterPretreat& requirement);
	//��ʱ���stringת����MyTime����¼�뵽Ԥ������������
	PretreatStatue timeToMyTimeAndIntoRequirementAfterPreTreat(UserRequirementAfterPretreat& requirement);

	/*
		����Ȩ�صĺ���
	*/
	PretreatStatue assignWeights(UserRequirementAfterPretreat& requirement);

	/*
		����������������Ƿ���ҪԤ������з�ʽ
			���� ������עд���� ��ô�͸ĳɽ�������
	*/
	PretreatStatue pretreatTravelType(UserRequirementAfterPretreat& requirement);

	/*
		Ԥ����ͨ�������� ����ͨ�������ͺ�Ʊ����(��λ���� ��ͬ��λ����϶���ͬ)
	*/
	PretreatStatue pretreatVehicleExperience(UserRequirementAfterPretreat& requirement);

	/*
		Ԥ������ת��ʽ ��ʵֱ�������� �����û�ָ�� ����һ�㲻�Ƽ�ת���� ����û��ֱ��
	*/
	PretreatStatue pretreatTransitType(UserRequirementAfterPretreat& requirement);

	/*
		Ԥ����ע
	*/
	PretreatStatue pretreatRemark(UserRequirementAfterPretreat& requirement);
	//����Ԥ����ע״̬��ĺ���
	PretreatStatue dealPretreatRemarkStatue(PretreatStatue statue_code);
};

/*
	�ýṹ��������Ԥ�������û�����	һ�����һ���Ƽ� ������Ҫ����
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