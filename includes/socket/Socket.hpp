#ifndef SOCKET_HPP
#define SOCKET_HPP

// Importes com ""
#include "Log.hpp"
#include "Error.hpp"

// Importes com <>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>

// Defines
// #ifndef NAME
// #define NAME
// #endif // NAME

class Socket
{
public:
	virtual ~Socket();

	virtual Error init() = 0;
	virtual bool isServer() = 0;

	struct pollfd &getPollFD();

protected:
	struct pollfd pollfd;

private:
};

#endif // SOCKET_HPP
