#ifndef CHANGECHARACTERSET_H
#define CHANGECHARACTERSET_H
#include<string>
using std::string;
//һ���� ����ת���ַ��� ��Ϊgo��utf-8��c++��gkb ������Ҫת���ַ��� ������ȷ��������
class ChangeCharacterSet
{
public:
	//GbkתUTF8 ����UTF�ַ� Ҳ����goĬ�ϵ��ַ�
	//*src_str Gbk�ַ�
	//��̬���� ����Ҫ����
	static string GbkToUtf8(const char* src_str);

	//UTF8תGbk ����Gbk�ַ� Ҳ����c++Ĭ�ϵ��ַ�
	//@ src_str UTF8�ַ�
	//��̬���� ����Ҫ����
	static string Utf8ToGbk(const char* src_str);
};

#endif // !CHANGECHARACTERSET_H
