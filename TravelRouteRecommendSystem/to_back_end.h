#ifndef TO_BACK_END_H
#define TO_BACK_END_H
#include"route_result.h"
#include"user_requirement.h"
#include"get_route.h"
#include"my_exception.h"
/*
��ȡ�Ƽ�·�ߺ��� C#���øú���
* requirement:C#������������
* routes:���
* error:������Ϣ��C#
* remark:��ע
* weight_size:�м���������
* first_route_size:��һ�����������·��
* second_route_size:�ڶ������������·��
* first_route_of_one_size_array:��һ�������������·������(ֱ��1 ת��2)
* second_route_of_one_size_array���ڶ��������������·������(ֱ��1 ת��2)
*/
extern "C" __declspec(dllexport)void GetRecommendations(UserRequirementFromCSharp requirement, RouteResult * &routes, char*& error,char*&remark,
	int& weight_size, int& first_route_size, int& second_route_size, int*& first_route_of_one_size_array, int*& second_route_of_one_size_array);

/*
��ȡһ������Ƽ�·�ߺ��� C#���øú���
* 
*/
extern "C" __declspec(dllexport)void GetRecommendationsOneGroup(UserRequirementFromCSharp requirement, RouteResult* & routes, int& level,char*& error_code,char*& error, char*& remark, int& first_route_size, int*& first_route_of_one_size_array);
/*
�ͷſռ� 
* ��������һ��
* ������Ҫ��ÿ��vehicle* ���ͷ� 
*/
extern "C" __declspec(dllexport)void FreeMemory(RouteResult * &routes, int weight_size, int first_route_size, int second_route_size);

extern "C" __declspec(dllexport)void FreeMemoryOneGroup(RouteResult* &routes, int first_route_size);
#endif // !TOBACKEND_H
