#include<iostream>
#include<hiredis.h>
#include<string>
#include "init_redis.h"
#include"route_result.h"
#include"user_requirement.h"
using std::string;

InitRedis::InitRedis()
{
}

RedisContextErr InitRedis::initRedis()
{
	redis_context= redisConnect("127.0.0.1", 6379);
	if (redis_context != NULL && redis_context->err)//因为redis_context->err是redis_context的一个成员 所以即使初始化错误 redis_context也不会为NULL
	{
		return redis_context->err;
	}
	//0即为没错
	return 0;
}

RedisReplyType InitRedis::getRedisReply(string redis_command)
{
	redis_reply=(redisReply*)redisCommand(redis_context, redis_command.c_str());
	return redis_reply->type;//若为6即出错 其它即为获得的结果数据的类型 后续通过函数dealRedisRelyErr来处理错误
}

UserRequirement InitRedis::getInforFromGoByRedis()
{
	return UserRequirement();
}

int InitRedis::putInforIntoRedis()
{
	return 0;
}
