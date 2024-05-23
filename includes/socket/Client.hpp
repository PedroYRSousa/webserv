#ifndef CLIENT_HPP
#define CLIENT_HPP

// Importes com ""
#include "Log.hpp"
#include "Error.hpp"
#include "Socket.hpp"
#include "Server.hpp"
#include "conn/Request.hpp"
#include "conn/Response.hpp"

// Importes com <>

// Defines
// #ifndef NAME
// #define NAME
// #endif // NAME

class Client : public Socket
{
public:
	Client(int fd, Server *s);
	~Client();
	Error init();
	bool isServer();
	S_Response *getResponse();
	Request *getRequest();
	void digestRequest();
	void readRequest(std::string reqRaw);
	bool getIsDigesting();
	void clearRequest();
	void addRetry();
	int getRetry();

protected:
private:
	Server *s;
	S_Response *res;
	Request *req;

	bool digesting;
	int retries;

	void resolveRequest();
};

#endif // CLIENT_HPP
