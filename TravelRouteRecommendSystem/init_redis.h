#ifndef INITREDIS_H
#define INITREDIS_H
//初始化一个hiredis对象 用来后续某些操作需要用到redis读写时操作
#include"route_result.h"
#include"user_requirement.h"
class InitRedis
{
public:
	InitRedis();
	//从redis里获取从go后端传过来的数据
	//填入到用户需求结构里 返回该用户需求 以便后续推荐
	UserRequirement getInforFromGoByRedis();

	//从c++里写数据到redis里以供go使用
	//返回redis->type
	//TODO 后面可能会换成别的返回值类型
	int putInforIntoRedis();
};

#endif // !INITREDIS_H