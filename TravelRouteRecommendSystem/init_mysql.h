#ifndef INITMYSQL_H
#define INITMYSQL_H
#include<mysql.h>
//由于该项目需要使用到mysql数据库 所以需要初始化一个全局的mysql对象用来对数据库进行操作
//在main函数里调用构造函数初始化数据库 然后就可以直接使用
//是用静态成员变量还是全局变量没想好
//用例
//int main()
//{
//	InitMySQL();
//}
//
class InitMySQL
{
	
public:
	InitMySQL();
	
	//初始化数据库实例的静态成员变量 这样数据库只需要初始化一次就可以在其它任何地方使用
	static bool initGlobleMySQL();

	//~InitMySQL();
};

#endif // !INITMYSQL_H