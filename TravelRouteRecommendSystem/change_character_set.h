#ifndef CHANGECHARACTERSET_H
#define CHANGECHARACTERSET_H
//һ���� ����ת���ַ��� ��Ϊgo��utf-8��c++��gkb ������Ҫת���ַ��� ������ȷ��������
/*��C#��C++��ϱ�� ͨ��p/invoke ��������Ҫ�õ����������� ����c#��utf16 ���Կ��Է�����*/
#include <string>
using std::string;
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
