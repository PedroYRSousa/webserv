#include "socket/Client.hpp"

// STATIC LOCAL
static int getMethodIndex(std::string method)
{
	if (method.compare("GET") == 0)
		return GET;
	if (method.compare("POST") == 0)
		return POST;
	if (method.compare("DELETE") == 0)
		return DELETE;

	return ERROR;
}
// STATIC PUBLIC
// STATIC PROTECTED
// STATIC PRIVATE
// PUBLIC
Client::Client(int fd, Server *s)
{
	this->retries = 0;
	this->digesting = false;
	this->s = s;
	this->pollfd.fd = fd;
	this->pollfd.revents = 0;
	this->pollfd.events = POLLIN | POLLOUT;
	this->req = NULL;
	this->res = NULL;
}
Client::~Client()
{
	if (this->req != NULL)
		delete this->req;
	if (this->res != NULL)
		delete this->res;
}
Error Client::init()
{
	int flags = -1;

	if ((flags = fcntl(this->pollfd.fd, F_GETFL, 0)) == ERROR)
		return makeError("fcntl get flags error: " + std::string(strerror(errno)));

	if (fcntl(this->pollfd.fd, F_SETFL, flags | O_NONBLOCK) == ERROR)
		return makeError("fcntl set flags error: " + std::string(strerror(errno)));

	return makeSuccess();
}
bool Client::isServer()
{
	return false;
}
S_Response *Client::getResponse()
{
	return this->res;
}
Request *Client::getRequest()
{
	return this->req;
}
void Client::digestRequest()
{
	this->digesting = true;

	if (this->res)
		delete this->res;

	if (this->getRequest() == NULL && this->getRetry() > 100)
	{
		this->res = new S_Response();
		this->res->status_code = 408;
	}
	else if (this->req == NULL)
	{
		this->res = new S_Response();
		this->res->status_code = 500;
	}
	else
	{
		S_Server serverStruct = this->s->getStruct();
		std::string host;

		Log::debug << this->req->dump(this->req->getBody().length() <= 1000) << Log::eof;

		Error err = this->req->getHeader("host", &host);
		if (err.status == ERROR)
		{
			Log::error << "Erro no header. Deve-se ter um host" << Log::eof;
			this->res = new S_Response();
			this->res->status_code = 400;
			return;
		}

		host = std::string(host, 0, host.find(":"));

		if (serverStruct.server_name.size() <= 0)
		{
			this->resolveRequest();
			// Vai resolver ou negar?
		}
		for (std::vector<std::string>::iterator it = serverStruct.server_name.begin(); it != serverStruct.server_name.end(); it++)
		{
			if ((*it).compare(host) == 0)
			{
				this->resolveRequest();
				break;
			}
		}
		if (this->res == NULL)
		{
			this->res = new S_Response();
			this->res->status_code = 404;
		}
	}
}
void Client::readRequest(std::string reqRaw)
{
	if (this->res != NULL)
		return;

	if (this->req == NULL)
	{
		this->req = new Request();
		Error err = Request::parser(reqRaw, this->req);
		if (err.status == ERROR)
		{
			Log::error << err.message << Log::eof;

			if (this->res)
				delete this->res;

			this->res = new S_Response();
			this->res->status_code = 400;
			return;
		}
	}
	else
		this->req->addInBody(reqRaw);
}
bool Client::getIsDigesting()
{
	return this->digesting;
}
void Client::clearRequest()
{
	if (this->req)
		delete this->req;

	this->req = NULL;
}
void Client::addRetry()
{
	this->retries = this->retries + 1;
}
int Client::getRetry()
{
	return this->retries;
}
// PROTECTED
// PRIVATE
void Client::resolveRequest()
{
	S_Request reqStruct;
	int index = this->s->getIndexServerStruct();

	reqStruct.server_number = index;
	reqStruct.body = this->req->getBody();
	reqStruct.header_fields = this->req->getAllHeaders();
	reqStruct.method = getMethodIndex(this->req->getMethod());
	if (reqStruct.method == ERROR)
	{
		this->res = new S_Response();
		this->res->status_code = 405;
		return;
	}
	reqStruct.path = this->req->getURI();
	reqStruct.queryString = this->req->getQueryString();

	S_Response resStruct = run(reqStruct);

	this->res = new S_Response();
	this->res->body = resStruct.body;
	this->res->header_fields = resStruct.header_fields;
	this->res->server_number = resStruct.server_number;
	this->res->status_code = resStruct.status_code;
}
