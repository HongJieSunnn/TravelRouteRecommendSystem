#ifndef MY_EXCEPTION_H
#define MY_EXCEPTION_H
#include<iostream>
/*
�Զ�����쳣��
* level_:���󼶱�
*	1�����ش���� һ����ִ��C++����ʱ���ֵ�һЩ���� ����˵��ʼ��Redisʧ�ܵ�
*	2��: ���ݴ��� һ���Ǻ�˸�C++����ʱ���ֵĿ����롢����������������
*	3��: ��ѯ���� һ����ִ�����ݿ��ѯʱ����û�н�����޷������Ƽ�
* error_code_:statue_code��string��ʽ
*/
class MyException :public std::exception
{
private:
	int level_;
	std::string error_code_;
public:
	MyException(int level, std::string error_code, std::string what) :exception(what.c_str())
	{
		this->level_ = level;
		this->error_code_ = error_code_;
	}

	int getLevel() const;

	std::string getErrorCode() const;
};
#endif // !MY_EXCEPTION_H