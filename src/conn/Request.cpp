#include "conn/Request.hpp"

// STATIC LOCAL
static std::vector<std::string> split(const std::string &s, char delimiter)
{
	std::vector<std::string> tokens;
	std::stringstream ss(s);
	std::string token;

	while (std::getline(ss, token, delimiter))
		tokens.push_back(token);

	return tokens;
}
static std::string &rtrim(std::string &str)
{
	size_t end = str.find_last_not_of(" \t\n\r");
	if (end != std::string::npos)
	{
		str.erase(end + 1);
	}
	return str;
}
static std::string &ltrim(std::string &str)
{
	size_t start = str.find_first_not_of(" \t\n\r");
	if (start != std::string::npos)
	{
		str.erase(0, start);
	}
	return str;
}
static std::string &trim(std::string &str)
{
	return ltrim(rtrim(str));
}
static Error readFirstLine(Request *req, std::vector<std::string>::iterator *begin)
{
	std::vector<std::string> splitResult = split(trim(**begin), ' ');

	if (splitResult.size() != 3)
		return makeError("A requisicao possui um erro de formatacao");

	Error err = req->setMethod(splitResult[0]);
	if (err.status == ERROR)
		return err;

	req->setQueryString(splitResult[1]);
	req->setURI(splitResult[1]);
	err = req->setHTTPVersion(splitResult[2]);
	if (err.status == ERROR)
		return err;

	(*begin)++;
	return makeSuccess();
}
static void readHeaders(Request *req, std::vector<std::string>::iterator *begin, std::vector<std::string>::iterator end)
{
	std::vector<std::string> splitResult;

	while ((*begin) != end && (**begin).compare("\r") != 0 && (**begin).compare("") != 0)
	{
		splitResult = split(**begin, ':');
		if (splitResult.size() == 2)
			req->addHeader(splitResult[0], splitResult[1]);
		else if (splitResult.size() > 2)
		{
			std::string valueAux = "";
			for (size_t i = 1; i < splitResult.size(); i++)
			{
				valueAux.append(splitResult[i]);
				if (i + 1 != splitResult.size())
					valueAux.append(":");
			}
			req->addHeader(splitResult[0], valueAux);
		}
		else
			req->addHeader(splitResult[0], "");
		(*begin)++;
	}
}
static void readBody(Request *req, std::string request)
{
	size_t indexBodyStart = request.find("\r\n\r\n");

	if (indexBodyStart != std::string::npos)
	{
		std::string body = request.substr(indexBodyStart + 4);
		req->addInBody(body);
	}
}

// STATIC PUBLIC
Error Request::parser(std::string request, Request *out)
{
	std::vector<std::string> splitResult = split(request, '\n');
	std::vector<std::string>::iterator begin = splitResult.begin();

	if (splitResult.size() <= 0)
		return makeError("A requisicao possui um erro de formatacao");

	(*out).raw = request;
	Error err = readFirstLine(out, &begin);
	if (err.status == ERROR)
		return err;
	readHeaders(out, &begin, splitResult.end());

	if (out->headers.find("expect") != out->headers.end())
		return makeError("Header expect não é suportado");
	if (out->headers.find("content-type") != out->headers.end() && out->headers["content-type"].find("multipart/form-data") != std::string::npos)
		return makeError("multipart/form-data não é suportado");

	readBody(out, request);

	return makeSuccess();
}
// STATIC PROTECTED
// STATIC PRIVATE
// PUBLIC
Request::Request()
{
}
Request::~Request()
{
}
void Request::setQueryString(std::string URI)
{
	if (URI.find("?") == std::string::npos)
		return;

	this->queryString = URI.substr(URI.find("?"));
}
void Request::setURI(std::string URI)
{
	if (URI.find("?") == std::string::npos)
	{
		this->uri = trim(URI);
	}
	else
	{
		URI = std::string(URI, URI.find("?"));
		this->uri = trim(URI);
	}
}
Error Request::setMethod(std::string method)
{
	method = trim(method);

	this->method = method;

	return makeSuccess();
}
std::string Request::getURI(void)
{
	return this->uri;
}
std::string Request::getMethod(void)
{
	return this->method;
}
Error Request::getHeader(std::string key, std::string *value)
{
	if (this->headers.find(key) == this->headers.end())
		return makeError("Key nao encontrado em headers");

	(*value) = this->headers[key];
	return makeSuccess();
}
std::string Request::getQueryString(void)
{
	return this->queryString;
}
std::string Request::dump(bool withBody)
{
	std::stringstream ss;

	// POST /legendary-4b0e1/us-central1/captions HTTP/1.1
	ss << this->method << " " << this->uri << " " << this->httpVersion << "\r\n";
	//> Host: 127.0.0.1:5001
	//> User-Agent: insomnia/2023.5.8
	//> Content-Type: application/json
	//> Authorization: Bearer eyJhbGciOiJub25lIiwidHlwIjoiSldUIn0.eyJlbWFpbCI6InlhZ29zb3VzYTI1MTJAZ21haWwuY29tIiwiZW1haWxfdmVyaWZpZWQiOmZhbHNlLCJhdXRoX3RpbWUiOjE2OTY4NjYwMjMsInVzZXJfaWQiOiJiOEhZbWZ0TWJMUFFFSUMwR0d2bDdDNVB4aGFxIiwiZmlyZWJhc2UiOnsiaWRlbnRpdGllcyI6eyJlbWFpbCI6WyJ5YWdvc291c2EyNTEyQGdtYWlsLmNvbSJdfSwic2lnbl9pbl9wcm92aWRlciI6InBhc3N3b3JkIn0sImlhdCI6MTY5Njg2NjAyMywiZXhwIjoxNjk2ODY5NjIzLCJhdWQiOiJsZWdlbmRhcnktNGIwZTEiLCJpc3MiOiJodHRwczovL3NlY3VyZXRva2VuLmdvb2dsZS5jb20vbGVnZW5kYXJ5LTRiMGUxIiwic3ViIjoiYjhIWW1mdE1iTFBRRUlDMEdHdmw3QzVQeGhhcSJ9.
	//> Accept: */*
	//> Content-Length: 14234
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

#ifdef TEST_MODE
#include <gtest/gtest.h>
TEST(parseRequest, DeveRealizarOParser)
{
	try
	{
		Request req;
		std::string request = "GET /exemplo.html HTTP/1.1\nHost: www.exemplo.com\n\nTesteA";
		Error err = Request::parser(request, &req);
		EXPECT_EQ(err.status, SUCCESS);
		EXPECT_EQ(err.message, "");
		EXPECT_EQ(req.getMethod(), "GET");
		EXPECT_EQ(req.getURI(), "/exemplo.html");
		EXPECT_EQ(req.getHTTPVersion(), "HTTP/1.1");
		std::string value = "";
		err = req.getHeader("Host", &value);
		EXPECT_EQ(err.status, SUCCESS);
		EXPECT_EQ(err.message, "");
		EXPECT_EQ(value, "www.exemplo.com");
		value = "";
		err = req.getHeader("Host123", &value);
		EXPECT_EQ(err.status, ERROR);
		EXPECT_EQ(err.message, "Key nao encontrado em headers");
		EXPECT_EQ(value, "");
		EXPECT_EQ(req.getBody(), "TesteA");
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}
TEST(parseRequest, NãoDeveRealizarOParserMetodoNãoPermitido)
{
	try
	{
		Request req;
		std::string request = "HEAD /exemplo.html HTTP/1.1\nHost: www.exemplo.com\n\nTesteA";
		Error err = Request::parser(request, &req);
		EXPECT_EQ(err.status, ERROR);
		EXPECT_EQ(err.message, "Method nao permitido");

		request = "DELETE /exemplo.html HTTP/1.1\nHost: www.exemplo.com\n\nTesteA";
		err = Request::parser(request, &req);
		EXPECT_EQ(err.status, ERROR);
		EXPECT_EQ(err.message, "Method nao permitido");

		request = "OPTIONS /exemplo.html HTTP/1.1\nHost: www.exemplo.com\n\nTesteA";
		err = Request::parser(request, &req);
		EXPECT_EQ(err.status, ERROR);
		EXPECT_EQ(err.message, "Method nao permitido");
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}
TEST(parseRequest, NãoDeveRealizarOParserRequestInvalida)
{
	try
	{
		Request req;
		std::string request = "GET";
		Error err = Request::parser(request, &req);
		EXPECT_EQ(err.status, ERROR);
		EXPECT_EQ(err.message, "A requisicao possui um erro de formatacao");

		request = "GET /exemplo.html";
		err = Request::parser(request, &req);
		EXPECT_EQ(err.status, ERROR);
		EXPECT_EQ(err.message, "A requisicao possui um erro de formatacao");

		request = "";
		err = Request::parser(request, &req);
		EXPECT_EQ(err.status, ERROR);
		EXPECT_EQ(err.message, "A requisicao possui um erro de formatacao");
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}
#endif
