#ifndef INITMYSQL_H
#define INITMYSQL_H
#include<mysql.h>
//���ڸ���Ŀ��Ҫʹ�õ�mysql���ݿ� ������Ҫ��ʼ��һ��ȫ�ֵ�mysql�������������ݿ���в���
//��main��������ù��캯����ʼ�����ݿ� Ȼ��Ϳ���ֱ��ʹ��
//���þ�̬��Ա��������ȫ�ֱ���û���
//����
//int main()
//{
//	InitMySQL();
//}
//
class InitMySQL
{
	
public:
	InitMySQL();
	
	//��ʼ�����ݿ�ʵ���ľ�̬��Ա���� �������ݿ�ֻ��Ҫ��ʼ��һ�ξͿ����������κεط�ʹ��
	static bool initGlobleMySQL();

	//~InitMySQL();
};

#endif // !INITMYSQL_H