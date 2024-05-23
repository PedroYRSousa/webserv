#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Log.hpp"
#include "Conn.hpp"
#include "Error.hpp"

class Request : public Conn
{
public:
	Request();
	~Request();

	static Error parser(std::string request, Request *out);

	void setQueryString(std::string URI);
	void setURI(std::string URI);
	Error setMethod(std::string method);
	std::string getQueryString(void);
	std::string getURI(void);
	std::string getMethod(void);
	Error getHeader(std::string key, std::string *value);

	std::string dump(bool withBody);

protected:
private:
	std::string uri;
	std::string method;
	std::string queryString;
};

#endif // REQUEST_HPP
