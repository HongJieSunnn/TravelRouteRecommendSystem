#ifndef INITMYSQL_H
#define INITMYSQL_H
#include<mysql.h>
#include<iostream>
#include<string>
using std::string;
namespace DBStatus
{

	using InitStatue = int;
	//初始化时的状态枚举
	enum InitStatus
	{
		INIT_MYSQL_FAILED = 0,
		INIT_MYSQL_SUCCEED = 1,
		INIT_MYSQL_LIBRARY_FAILED,
		INIT_DB_OBJECT_FAILED,
		INIT_MYSQL_OPITION_FAILED,
	};
	const string INIT_MYSQL_FAILED_STR = "INIT_MYSQL_FAILED";
	const string INIT_MYSQL_LIBRARY_FAILED_STR = "INIT_MYSQL_LIBRARY_FAILED";
	const string INIT_DB_OBJECT_FAILED_STR = "INIT_DB_OBJECT_FAILED";
	const string INIT_MYSQL_OPITION_FAILED_STR = "INIT_MYSQL_OPITION_FAILED";

	using CloseStatue = int;
	//关闭数据库时的状态枚举
	enum CloseStatus
	{
		CLOSE_FAILED = 0,
		CLOSE_SUCCEED
	};
	const string CLOSE_FAILED_STR = "CLOSE_MYSQL_FAILED";

	enum CURDStatus
	{

	};
	/*
		处理初始化mysql时的错误 只输出错误代码 不输出正确
		*statue_code 初始化的状态码 为INIT_MYSQL_SUCCEED return true 其余false
	*/
	bool dealInitMySQLError(InitStatue statue_code);

	/*
		处理关闭mysql时的错误
		*statue_code 关闭的状态码
	*/
	bool dealCloseSQLError(CloseStatue statue_code);
}
using namespace DBStatus;
/*
由于该项目需要使用到mysql数据库 所以需要初始化一个全局的mysql对象用来对数据库进行操作
在main函数里调用构造函数初始化数据库 然后就可以直接使用
是用静态成员变量还是全局变量没想好
用例
int main()
{
	InitMySQL::initGlobleMySQL("127.0.0.1","root","password","database_name",3306,*"gbk"(可省略)*);
}
*/
class InitMySQL
{
private:
	//mysql实例
	static MYSQL db;
	//sql语句执行结果实例
	static MYSQL_RES* result;
	/*
		关闭当前的db 可以和resetGlobleMySQL配合使用
	*/
	static CloseStatue closeGlobleMySQL();
public:
	InitMySQL();
	/*
		初始化数据库实例的静态成员变量 这样数据库只需要初始化一次就可以在其它任何地方使用
		*host mysql地址
		*user_name 登陆账户
		*password 密码
		*database_name 要打开的那个数据库名称
		*port 端口号
		*character_set 字符集 c++默认 gbk 而数据库一般默认utf-8 不设置会出现乱码
	*/
	static InitStatue initGlobleMySQL(string host, string user_name, string password, string database_name,int port,string character_set="gbk");

	/*
		重新设置mysql实例
	*/
	static int resetGlobleMySQL(string host, string user_name, string password, string database_name, int port, string character_set = "gbk");

	/*
		其他地方获取db实例
		返回db句柄的指针
	*/
	static MYSQL* DB();

	/*
		通过SQL语句获得执行结果
		返回静态变量 result 
		这样可以避免返回临时指针问题，也可以减少空间浪费
	*/
	static MYSQL_RES* execSQLToGetResult(string SQL);

	//~InitMySQL();
};

#endif // !INITMYSQL_H