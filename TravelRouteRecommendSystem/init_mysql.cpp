#include<iostream>
#include<mysql.h>
#include<string>
#include "init_mysql.h"
using std::string;
namespace DBStatus
{
	
	using InitStatue = int;
	//初始化时的状态枚举
	enum InitStatus
	{
		INIT_MYSQL_FAILED=0,
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
		CLOSE_FAILED=0,
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
	bool dealInitMySQLError(InitStatue statue_code)
	{
		switch (statue_code)
		{
		case DBStatus::INIT_MYSQL_FAILED:
			std::cout << INIT_MYSQL_FAILED_STR << "\n";
			return false;
			break;
		case DBStatus::INIT_MYSQL_LIBRARY_FAILED:
			std::cout << INIT_MYSQL_LIBRARY_FAILED_STR << "\n";
			return false;
			break;
		case DBStatus::INIT_DB_OBJECT_FAILED:
			std::cout << INIT_DB_OBJECT_FAILED_STR << "\n";
			return false;
			break;
		case DBStatus::INIT_MYSQL_OPITION_FAILED:
			std::cout << INIT_MYSQL_OPITION_FAILED_STR << "\n";
			return false;
			break;
		}
		return true;
	}
	
	/*
		处理关闭mysql时的错误
		*statue_code 关闭的状态码
	*/
	bool dealCloseSQLError(CloseStatue statue_code)
	{
		if (statue_code == CLOSE_FAILED)
		{
			std::cout << CLOSE_FAILED_STR << "\n";
			return false;
		}
		return true;
	}
}
using namespace DBStatus;

InitMySQL::InitMySQL()
{
}

InitStatue InitMySQL::initGlobleMySQL(string host, string user_name, string password, string database_name, int port, string character_set = "gbk")
{
	if (mysql_library_init(0, NULL, NULL) != 0)
	{
		return INIT_MYSQL_LIBRARY_FAILED;
	}
	if (mysql_init(&db) == NULL)
	{
		return INIT_DB_OBJECT_FAILED;
	}
	if (mysql_options(&db, MYSQL_SET_CHARSET_NAME, character_set.c_str()) != 0)
	{
		return INIT_MYSQL_OPITION_FAILED;
	}
	if (mysql_real_connect(&db, host.c_str(), user_name.c_str(), password.c_str(), database_name.c_str(), port, NULL, 0) != NULL)
	{
		return INIT_MYSQL_SUCCEED;
	}
	return INIT_MYSQL_FAILED;
}

CloseStatue InitMySQL::closeGlobleMySQL()
{
	
	if (&db)
	{
		mysql_close(&db);
		return CLOSE_SUCCEED;
	}
	return CLOSE_FAILED;
}

int InitMySQL::resetGlobleMySQL(string host, string user_name, string password, string database_name, int port, string character_set = "gbk")
{
	int close_statu= closeGlobleMySQL();
	if (close_statu==CLOSE_FAILED)
		return close_statu;
	
	int init_statu=initGlobleMySQL(host, user_name, password, database_name, port, character_set);
	if (init_statu != INIT_MYSQL_SUCCEED)
		return init_statu;
}

MYSQL* InitMySQL::DB()
{
	return &db;
}

MYSQL_RES* InitMySQL::execSQLToGetResult(string SQL)
{
	//每次执行都要把它赋值成nullptr
	//否则若此次结果为空 会返回上次结果
	result = nullptr;
	if (mysql_query(&db, SQL.c_str())==0)
	{
		result = mysql_store_result(&db);
	}
	return result;
}
