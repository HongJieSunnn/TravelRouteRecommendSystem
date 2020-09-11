#ifndef INITREDIS_H
#define INITREDIS_H
#include<iostream>
#include<hiredis.h>
#include<string>
#include"route_result.h"
#include"user_requirement.h"
using std::string;

#pragma comment(lib, "Win32_Interop.lib")
#pragma comment(lib, "hiredis.lib")

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
	bool dealInitRedisError(RedisContextErr statue_code);
	/*
		�����ȡreplyʱ�Ĵ���
		*statue_code �������
	*/
	bool dealGetRedisReplyError(RedisReplyType statue_code);
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
	* ��ȡ���ȼ�
	* table_name:��Ҫ��ȡ���ȼ��ı���
	*/
	static int getPriorityLevel(string key);

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