#include "my_exception.h"

int MyException::getLevel() const
{
	return this->level_;
}

std::string MyException::getErrorCode() const
{
	return this->error_code_;
}
