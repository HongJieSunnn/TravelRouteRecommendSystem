#ifndef INITREDIS_H
#define INITREDIS_H
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
/*
	初始化一个hiredis对象以及后续某些操作需要用到redis的读写操作
	用例
	int main()
	{
		InitRedis::initRedis();
	}
*/
class InitRedis
{
private:
	//相当于一个redis实例
	static redisContext* redis_context;
	//获得对redis实例执行redis语句的结果
	static redisReply* redis_reply;
	//无法创建对象
	InitRedis();
public:
	
	/*
		初始化静态变量redis_context
		返回值是RedisContextErr 实际是int 
	*/
	static RedisContextErr initRedis();

	/*
		通过redis命令获得结果 并且放到静态变量redis_reply中 然后通过其它方法具体操作获得的数据
		返回RedisReplyType 实际是int
		*redis_command:完整的redis命令
	*/
	static RedisReplyType getRedisReply(string redis_command);

	/*
		从redis里获取从go后端传过来的数据
		填入到用户需求结构里 返回该用户需求 以便后续推荐
	*/
	static UserRequirement getInforFromGoByRedis();

	/*
		从c++里写数据到redis里以供go(或其它后端语言)使用
		返回RedisReplyType
	*/
	RedisReplyType putInforIntoRedis();
};

#endif // !INITREDIS_H