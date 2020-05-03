#ifndef INITREDIS_H
#define INITREDIS_H

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
public:
	InitRedis();

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