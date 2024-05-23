#include "socket/Socket.hpp"

// STATIC LOCAL
// STATIC PUBLIC
// STATIC PROTECTED
// STATIC PRIVATE
// PUBLIC
Socket::~Socket()
{
	if (this->pollfd.fd > 0)
	{
		close(this->pollfd.fd);
		this->pollfd.fd = -1;
	}
}
struct pollfd &Socket::getPollFD()
{
	return this->pollfd;
}
// PROTECTED
// PRIVATE
