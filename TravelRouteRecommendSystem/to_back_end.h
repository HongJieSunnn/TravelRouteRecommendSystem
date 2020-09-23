#ifndef TO_BACK_END_H
#define TO_BACK_END_H
#include"route_result.h"
#include"user_requirement.h"
#include"get_route.h"
#include"my_exception.h"
/*
获取推荐路线函数 C#调用该函数
* requirement:C#传过来的需求
* routes:结果
* error:错误信息给C#
* remark:备注
* weight_size:有几个城市组
* first_route_size:第一个城市组的线路数
* second_route_size:第二个城市组的线路数
* first_route_of_one_size_array:第一个城市组具体线路的条数(直达1 转车2)
* second_route_of_one_size_array：第二个城市组具体线路的条数(直达1 转车2)
*/
extern "C" __declspec(dllexport)void GetRecommendations(UserRequirementFromCSharp requirement, RouteResult * &routes, char*& error,char*&remark,
	int& weight_size, int& first_route_size, int& second_route_size, int*& first_route_of_one_size_array, int*& second_route_of_one_size_array);

/*
获取一组城市推荐路线函数 C#调用该函数
* 
*/
extern "C" __declspec(dllexport)void GetRecommendationsOneGroup(UserRequirementFromCSharp requirement, RouteResult* & routes, int& level,char*& error_code,char*& error, char*& remark, int& first_route_size, int*& first_route_of_one_size_array);
/*
释放空间 
* 参数看上一个
* 可能需要把每个vehicle* 都释放 
*/
extern "C" __declspec(dllexport)void FreeMemory(RouteResult * &routes, int weight_size, int first_route_size, int second_route_size);

extern "C" __declspec(dllexport)void FreeMemoryOneGroup(RouteResult* &routes, int first_route_size);
#endif // !TOBACKEND_H
