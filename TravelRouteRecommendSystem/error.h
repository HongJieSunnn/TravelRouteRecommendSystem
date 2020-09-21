#ifndef ERROR_H
#define ERROR_H
class Error
{
public:
	int level;
	char* error_code;
	char* what;
};
#endif