#include<iostream>
#include<hiredis.h>
#include<string>
#include "init_redis.h"
#include"route_result.h"
#include"user_requirement.h"
using std::string;
namespace RedisStatus
{
	//��ʼ��rediscontextʱ�Ĵ������
	//��redis_context->err
	using RedisContextErr = int;
	//��дredisʱ���ص����� Ϊ6ʱ ������д����ERROR
	//��redis_reply->type
	using RedisReplyType = int;

	/*
		�����ʼ��redisʵ��ʱ�Ĵ���
		*statue_code �������
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
		�����ȡreplyʱ�Ĵ���
		*statue_code �������
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
