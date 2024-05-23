#include "conn/Response.hpp"

// STATIC LOCAL
static void initMapOfStatus(void)
{
	Response::statusString[100] = "Continue";
	Response::statusString[101] = "Switching Protocols";
	Response::statusString[102] = "Processing";
	Response::statusString[103] = "Early Hints";

	Response::statusString[200] = "OK";
	Response::statusString[201] = "Created";
	Response::statusString[202] = "Accepted";
	Response::statusString[203] = "Non-Authoritative Information";
	Response::statusString[204] = "No Content";
	Response::statusString[205] = "Reset Content";
	Response::statusString[206] = "Partial Content";
	Response::statusString[207] = "Multi-Status";
	Response::statusString[208] = "Already Reported";
	Response::statusString[226] = "IM Used";

	Response::statusString[300] = "Multiple Choices";
	Response::statusString[301] = "Moved Permanently";
	Response::statusString[302] = "Found";
	Response::statusString[303] = "See Other";
	Response::statusString[304] = "Not Modified";
	Response::statusString[305] = "Use Proxy";
	Response::statusString[306] = "unused";
	Response::statusString[307] = "Temporary Redirect";
	Response::statusString[308] = "Permanent Redirect";

	Response::statusString[400] = "Bad Request";
	Response::statusString[401] = "Unauthorized";
	Response::statusString[402] = "Payment Required";
	Response::statusString[403] = "Forbidden";
	Response::statusString[404] = "Not Found";
	Response::statusString[405] = "Method Not Allowed";
	Response::statusString[406] = "Not Acceptable";
	Response::statusString[407] = "Proxy Authentication Required";
	Response::statusString[408] = "Request Timeout";
	Response::statusString[409] = "Conflict";
	Response::statusString[410] = "Gone";
	Response::statusString[411] = "Length Required";
	Response::statusString[412] = "Precondition Failed";
	Response::statusString[413] = "Payload Too Large";
	Response::statusString[414] = "URI Too Long";
	Response::statusString[415] = "Unsupported Media Type";
	Response::statusString[416] = "Range Not Satisfiable";
	Response::statusString[417] = "Expectation Failed";
	Response::statusString[418] = "I'm a teapot";
	Response::statusString[421] = "Misdirected Request";
	Response::statusString[422] = "Unprocessable Content";
	Response::statusString[423] = "Locked";
	Response::statusString[424] = "Failed Dependency";
	Response::statusString[425] = "Too Early";
	Response::statusString[426] = "Upgrade Required";
	Response::statusString[428] = "Precondition Required";
	Response::statusString[429] = "Too Many Requests";
	Response::statusString[431] = "Request Header Fields Too Large";
	Response::statusString[451] = "Unavailable For Legal Reasons";

	Response::statusString[500] = "Internal Server Error";
	Response::statusString[501] = "Not Implemented";
	Response::statusString[502] = "Bad Gateway";
	Response::statusString[503] = "Service Unavailable";
	Response::statusString[504] = "Gateway Timeout";
	Response::statusString[505] = "HTTP Version Not Supported";
	Response::statusString[506] = "Variant Also Negotiates";
	Response::statusString[507] = "Insufficient Storage";
	Response::statusString[508] = "Loop Detected";
	Response::statusString[510] = "Not Extended";
	Response::statusString[511] = "Network Authentication Required";
}
static std::string getStatusString(int status)
{
	if (Response::statusString.size() <= 0)
		initMapOfStatus();

	std::string statusString = "";
	try
	{
		statusString = Response::statusString.at(status);
	}
	catch (const std::exception &e)
	{
		(void)e;
		statusString = "";
	}
	return statusString;
}
// STATIC PUBLIC
std::map<int, std::string> Response::statusString;
// STATIC PROTECTED
// STATIC PRIVATE
// PUBLIC
Response::Response()
{
	this->status = 200;
	this->httpVersion = "HTTP/1.1";
	this->addHeader("connection", "Close");
}
Response::~Response()
{
}
void Response::setStatus(int status)
{
	this->status = status;
}
int Response::getStatus(void)
{
	return this->status;
}
std::string Response::dump(bool withBody)
{
	std::stringstream ss;

	// HTTP/1.1 200 OK
	ss << this->httpVersion << " " << this->status << " " << getStatusString(status) << "\r\n";
	//< x-powered-by: Express
	//< vary: Origin
	//< content-type: application/json; charset=utf-8
	//< content-length: 8219
	//< etag: W/"201b-tRl5+9UScHapTqzwvLFb3ei8+Is"
	//< date: Tue, 03 Oct 2023 18:26:25 GMT
	//< connection: close
	for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
		ss << (*it).first << ": " << (*it).second << "\r\n";
	// BODY
	ss << "\r\n";
	if (this->body.size() > 0 && withBody)
		ss << this->body << "\r\n";

	return ss.str();
}
// PROTECTED
// PRIVATE
