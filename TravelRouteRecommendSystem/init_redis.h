#ifndef INITREDIS_H
#define INITREDIS_H
//��ʼ��һ��hiredis���� ��������ĳЩ������Ҫ�õ�redis��дʱ����
#include"route_result.h"
#include"user_requirement.h"
class InitRedis
{
public:
	InitRedis();
	//��redis���ȡ��go��˴�����������
	//���뵽�û�����ṹ�� ���ظ��û����� �Ա�����Ƽ�
	UserRequirement getInforFromGoByRedis();

	//��c++��д���ݵ�redis���Թ�goʹ��
	//����redis->type
	//TODO ������ܻỻ�ɱ�ķ���ֵ����
	int putInforIntoRedis();
};

#endif // !INITREDIS_H