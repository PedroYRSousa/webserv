#ifndef RESPONSE_HPP
#define RESPONSE_HPP

// Importes com ""
#include "Log.hpp"
#include "Conn.hpp"
#include "Error.hpp"

// Importes com <>

// Defines
// #ifndef NAME
// #define NAME
// #endif // NAME

class Response : public Conn
{
public:
	static std::map<int, std::string> statusString;

	Response();
	Response(S_Response resStruct);
	~Response();

	void setStatus(int status);
	int getStatus(void);
	std::string dump(bool withBody);

protected:
private:
	int status;
};

#endif // RESPONSE_HPP
