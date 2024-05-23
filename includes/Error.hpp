#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>

#ifndef ERROR
#define ERROR -1
#endif // ERROR

#ifndef SUCCESS
#define SUCCESS 0
#endif // SUCCESS

struct Error
{
	int status;
	std::string message;
};

Error makeSuccess();
Error makeError(const std::string &message);

#endif // ERROR_HPP
