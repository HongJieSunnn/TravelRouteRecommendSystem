#ifndef CHANGECHARACTERSET_H
#define CHANGECHARACTERSET_H
//一个类 用来转换字符集 因为go是utf-8而c++是gkb 所以需要转换字符集 才能正确交互数据
/*用C#和C++混合编程 通过p/invoke 根本不需要用到这两个函数 而且c#是utf16 所以可以废弃了*/
#include <string>
using std::string;
class ChangeCharacterSet
{
public:
	//Gbk转UTF8 返回UTF字符 也就是go默认的字符
	//*src_str Gbk字符
	//静态函数 不需要对象
	static string GbkToUtf8(const char* src_str);

	//UTF8转Gbk 返回Gbk字符 也就是c++默认的字符
	//@ src_str UTF8字符
	//静态函数 不需要对象
	static string Utf8ToGbk(const char* src_str);
};

#endif // !CHANGECHARACTERSET_H
