#ifndef INITREDIS_H
#define INITREDIS_H
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
/*
	��ʼ��һ��hiredis�����Լ�����ĳЩ������Ҫ�õ�redis�Ķ�д����
	����
	int main()
	{
		InitRedis::initRedis();
	}
*/
class InitRedis
{
private:
	//�൱��һ��redisʵ��
	static redisContext* redis_context;
	//��ö�redisʵ��ִ��redis���Ľ��
	static redisReply* redis_reply;
	//�޷���������
	InitRedis();
public:
	
	/*
		��ʼ����̬����redis_context
		����ֵ��RedisContextErr ʵ����int 
	*/
	static RedisContextErr initRedis();

	/*
		ͨ��redis�����ý�� ���ҷŵ���̬����redis_reply�� Ȼ��ͨ�������������������õ�����
		����RedisReplyType ʵ����int
		*redis_command:������redis����
	*/
	static RedisReplyType getRedisReply(string redis_command);

	/*
		��redis���ȡ��go��˴�����������
		���뵽�û�����ṹ�� ���ظ��û����� �Ա�����Ƽ�
	*/
	static UserRequirement getInforFromGoByRedis();

	/*
		��c++��д���ݵ�redis���Թ�go(�������������)ʹ��
		����RedisReplyType
	*/
	RedisReplyType putInforIntoRedis();
};

#endif // !INITREDIS_H