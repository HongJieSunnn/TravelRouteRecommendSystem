#include "init_redis.h"
redisContext* InitRedis::redis_context = nullptr;
redisReply* InitRedis::redis_reply = nullptr;

InitRedis::InitRedis()
{
}

RedisContextErr InitRedis::initRedis()
{
	redis_context= redisConnect("127.0.0.1", 6379);
	if (redis_context != NULL && redis_context->err)//因为redis_context->err是redis_context的一个成员 所以即使初始化错误 redis_context也不会为NULL
	{
		std::cout << redis_context->errstr << "\n";
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

int InitRedis::getPriorityLevel(string key)
{
	string redis_command = "GET ";
	redis_command.append(key);
	int type = getRedisReply(redis_command);
	if (type != 6&&type==3)
	{
		return redis_reply->integer;
	}
	else
	{
		return -1;//获取错误
	}
}

UserRequirement InitRedis::getInforFromGoByRedis()
{
	return UserRequirement();
}

int InitRedis::putInforIntoRedis()
{
	return 0;
}

bool RedisStatus::dealInitRedisError(RedisContextErr statue_code)
{
	switch (statue_code)
	{
	case REDIS_ERR_IO:
		std::cout << "REDIS_ERR_IO_FAILD" << "\n";
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

bool RedisStatus::dealGetRedisReplyError(RedisReplyType statue_code)
{
	if (statue_code == REDIS_REPLY_ERROR)
	{
		std::cout << "REDIS_REPLY_ERROR" << "\n";
		return false;
	}
	return true;
}
