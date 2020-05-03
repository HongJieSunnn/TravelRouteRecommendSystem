#include<iostream>
#include<hiredis.h>
#include<string>
#include "init_redis.h"
#include"route_result.h"
#include"user_requirement.h"
using std::string;
namespace RedisStatus
{
	//初始化rediscontext时的错误代码
	//即redis_context->err
	using RedisContextErr = int;
	//读写redis时返回的类型 为6时 即读或写出现ERROR
	//即redis_reply->type
	using RedisReplyType = int;

	/*
		处理初始化redis实例时的错误
		*statue_code 错误代码
	*/
	bool dealInitRedisError(RedisContextErr statue_code)
	{
		switch (statue_code)
		{
		case REDIS_ERR_IO:
			std::cout << "REDIS_ERR_IO_FAILD"<<"\n";
			return false;
			break;
		case REDIS_ERR_EOF:
			std::cout << "REDIS_ERR_EOF_FAILD" << "\n";
			return false;
			break;
		case REDIS_ERR_PROTOCOL:
			std::cout << "REDIS_ERR_PROTOCOL_FAILD" << "\n";
			return false;
			break;
		case REDIS_ERR_OOM:
			std::cout << "REDIS_ERR_OOM_FAILD" << "\n";
			return false;
			break;
		case REDIS_ERR_OTHER:
			std::cout << "REDIS_ERR_OTHER_FAILD" << "\n";
			return false;
			break;
		}
		return true;
	}
	/*
		处理获取reply时的错误
		*statue_code 错误代码
	*/
	bool dealGetRedisReplyError(RedisReplyType statue_code)
	{
		if (statue_code == REDIS_REPLY_ERROR)
		{
			std::cout << "REDIS_REPLY_ERROR" << "\n";
			return false;
		}
		return true;
	}
}
using namespace RedisStatus;

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
