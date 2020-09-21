#ifndef MY_EXCEPTION_H
#define MY_EXCEPTION_H
#include<iostream>
/*
自定义的异常类
* level_:错误级别
*	1级：重大错误 一般是执行C++代码时出现的一些错误 例如说初始化Redis失败等
*	2级: 数据错误 一般是后端给C++数据时出现的空输入、不符合需求的输入等
*	3级: 查询错误 一般是执行数据库查询时出现没有结果而无法给出推荐
* error_code_:statue_code的string形式
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