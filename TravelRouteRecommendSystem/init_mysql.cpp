#include<mysql.h>
#include "init_mysql.h"
namespace DBStatus
{
	//初始化时的状态枚举
	enum InitStatus
	{
		INIT_MYSQL_FAILED=0,
		INIT_MYSQL_SUCCEED = 1,
		INIT_MYSQL_LIBRARY_FAILED,
		INIT_DB_OBJECT_FAILED,
		INIT_MYSQL_OPITION_FAILED,
	};
	//关闭数据库时的状态枚举
	enum CloseStatus
	{
		CLOSE_FAILED=0,
		CLOSE_SUCCEED
	};

	enum CURDStatus
	{

	};
}
using namespace DBStatus;

InitMySQL::InitMySQL()
{
}

int InitMySQL::initGlobleMySQL(string host, string user_name, string password, string database_name, int port, string character_set = "gbk")
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

int InitMySQL::closeGlobleMySQL()
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
