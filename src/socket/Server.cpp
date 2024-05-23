#include "socket/Server.hpp"

// STATIC LOCAL
// STATIC PUBLIC
// STATIC PROTECTED
// STATIC PRIVATE
// PUBLIC
Server::Server(S_Server s, int index)
{
	this->opt = 1;
	this->pollfd.events = POLLIN;
	this->pollfd.revents = 0;
	this->pollfd.fd = 0;
	this->port = s.port;
	if (this->port <= 1024)
		Log::warn << "A porta menor que 1024 necessita de permissão de root" << Log::eof;
	this->addrlen = sizeof(this->address);
	this->serverStruct = s;
	this->indexServerStruct = index;
}
Server::~Server()
{
}
Error Server::init()
{
	Log::info << "Iniciando um servidor" << Log::eof;

	if ((this->pollfd.fd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
		return makeError("socket error: " + std::string(strerror(errno)));

	Log::debug << "Porta: " << this->port << " FD: " << this->pollfd.fd << Log::eof;

	if (setsockopt(this->pollfd.fd, SOL_SOCKET, (SO_REUSEADDR | SO_REUSEPORT), &opt, sizeof(opt)) <= ERROR)
		return makeError("setsockopt error: " + std::string(strerror(errno)));

	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons(this->port);

	if (bind(this->pollfd.fd, (struct sockaddr *)&this->address, sizeof(this->address)) <= ERROR)
		return makeError("bind error: " + std::string(strerror(errno)));

	if (listen(this->pollfd.fd, -1) <= ERROR)
		return makeError("listen error: " + std::string(strerror(errno)));

	return makeSuccess();
}
bool Server::isServer()
{
	return true;
}
struct sockaddr_in &Server::getAddress()
{
	return this->address;
}
struct sockaddr *Server::getSockAddress()
{
	return (struct sockaddr *)&(this->address);
}
socklen_t *Server::getSockLenAddress()
{
	return (socklen_t *)&(this->addrlen);
}
S_Server &Server::getStruct()
{
	return this->serverStruct;
}
int Server::getIndexServerStruct()
{
	return this->indexServerStruct;
}
int Server::getPort()
{
	return this->port;
}
// PROTECTED
// PRIVATE
