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
	if (redis_context != NULL && redis_context->err)//��Ϊredis_context->err��redis_context��һ����Ա ���Լ�ʹ��ʼ������ redis_contextҲ����ΪNULL
	{
		return redis_context->err;
	}
	//0��Ϊû��
	return 0;
}

RedisReplyType InitRedis::getRedisReply(string redis_command)
{
	redis_reply=(redisReply*)redisCommand(redis_context, redis_command.c_str());
	return redis_reply->type;//��Ϊ6������ ������Ϊ��õĽ�����ݵ����� ����ͨ������dealRedisRelyErr���������
}

UserRequirement InitRedis::getInforFromGoByRedis()
{
	return UserRequirement();
}

int InitRedis::putInforIntoRedis()
{
	return 0;
}
