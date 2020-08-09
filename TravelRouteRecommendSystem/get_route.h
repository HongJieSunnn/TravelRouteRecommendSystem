#ifndef GETROUTE_H
#define GETROUTE_H
#include<unordered_map>
#include"user_requirement.h"
#include"init_mysql.h"
//������Ҫ���� ������ȡ����·��
//TODO ��֪��Ҫ����д�ɸ��� Ȼ��ֱ�� ת��ʲô�ļ̳� ����ֱ��дһ��ͨ�÷��� ֧�ָ��ַ�ʽ
using std::unordered_map;

namespace GetRouteNameSpace
{
	/*
		��ȡ·�ߵ�״̬ö��
	*/
	enum class GetRouteStatue
	{
		
	};
	/*
		��ȡ��ͨ���ߵ�״̬ö��
	*/
	enum class GetVehicleStatue
	{
		GET_VEHICLE_FAILED,
		GET_VEHICLE_SUCCEED
	};
	/*
		��ȡֱ�ｻͨ���ߵ�״̬ö��
	*/
	enum class GetDirectVehicleStatue
	{
		GET_DIRECT_VEHICLE_FAILED,
		GET_DIRECT_VEHICLE_SUCCEED
	};
	/*
		��ȡ��ת��ͨ���ߵ�״̬ö��
	*/
	enum class GettransitVehicleStatue
	{
		GET_transit_VEHICLE_FAILED,
		GET_transit_VEHICLE_SUCCEED
	};
	/*
		��ȡ��Ͻ�ͨ���ߵ�״̬ö��
	*/
	enum class GetFixVehicleStatue
	{
		GET_FIX_VEHICLE_FAILED,
		GET_FIX_VEHICLE_SUCCEED
	};
}
/*
	GetRoute ��ȡ·�� ʵ���Ͼ��ǻ�ȡ�������м�Ľ�ͨ����
* requirement:Ԥ����������
*/
class GetRoute
{
private:
	UserRequirementAfterPretreat requirement;
public:
	GetRoute(UserRequirement requirement)
	{
		//TODO:������װdllʱ���԰�UserRequirement requirementŪ��һ��ȫ�ֱ���
		this->requirement = requirement.pretreatUserRequirement();
	}
	~GetRoute()
	{

	}

	/*
		getVechileInfor ͨ��Ԥ�����������ȡ��ͨ����(Ҳ���ǻ�ȡ·��)
	* 
	*/
	GetRouteNameSpace::GetVehicleStatue getVechileInfor();

	//�Ҹо�����ת���ʹ����־ͺ�����ʵ ������ʲô��ͨ���ߡ��۸�ʱ����Щ�Ĳ����������Ƶ�

	/*
		��ȡֱ��Ľ�ͨ������Ϣ
	* 
	*/
	GetRouteNameSpace::GetDirectVehicleStatue getDirectVehicleInfor();

	/*
		��ȡ��ת�Ľ�ͨ������Ϣ
	*
	*/
	GetRouteNameSpace::GettransitVehicleStatue getTransitVehicleInfor();

	/*
		��ȡ��ϵĽ�ͨ������Ϣ
	*
	*/
	GetRouteNameSpace::GetFixVehicleStatue getFixVehicleInfor();

	/*
		����requirement��������ϳ�where
	* now_index:requirement�� ���� start_cities ������ ��ô��Ҫ���뵱ǰ����������������±� ���ܻ�õ�ǰ�ĳ���
	*/
	unordered_map<string, string> getWhereSentenceKeyValue(int now_index);

	/*
		��ȡtable_name(����start_city)
	* now_index:��õ�ǰ���л��н�ͨ��������
	*/
	string getTableName(int now_index);

	/**
		���������ϳ�SQL���
	* now_index:��ǰ�±�(�ô��ж�arrive_city��=����!=)
	* columns:�� Ĭ���� * columns.size()==0��ΪĬ��
	* table_name:����
	* where_sentence:where��� ��һ��hash��ֵ�� ��Ϊ���ڸ���Ŀ where�ľ���ôЩ���� ����õ� �ͼ��Ͼ����������û�õ� ���ں���� is not null�ͺ���
	*	Tip:
	*		���ڻ���һЩʲôorder ���߸���ת����ת���жϵ��ڻ��ǲ����� ֱ�ӷ���requirement��Ա���жϾͺ���
	*/
	string getSQLQuery(int now_index, vector<string> columns, string table_name, unordered_map<string, string> where_sentence);
};

#endif // !GETROUTE_H