#include "lib.hpp"

// std::vector<S_Server> servers;

static std::string itoa(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}

std::map<std::string, std::string> getStatusCodeInfoTable()
{
	std::map<std::string, std::string> status_codes;

	status_codes["100"] = "Continue";
	status_codes["101"] = "Switching Protocols";
	status_codes["102"] = "Processing";
	status_codes["200"] = "OK";
	status_codes["201"] = "Created";
	status_codes["202"] = "Accepted";
	status_codes["203"] = "Non-Authoritative Information";
	status_codes["204"] = "No Content";
	status_codes["205"] = "Reset Content";
	status_codes["206"] = "Partial Content";
	status_codes["207"] = "Multi-Status";
	status_codes["208"] = "Already Reported";
	status_codes["226"] = "IM Used";
	status_codes["300"] = "Multiple Choices";
	status_codes["301"] = "Moved Permanently";
	status_codes["302"] = "Found";
	status_codes["303"] = "See Other";
	status_codes["304"] = "Not Modified";
	status_codes["305"] = "Use Proxy";
	status_codes["307"] = "Temporary Redirect";
	status_codes["308"] = "Permanent Redirect";
	status_codes["400"] = "Bad Request";
	status_codes["401"] = "Unauthorized";
	status_codes["402"] = "Payment Required";
	status_codes["403"] = "Forbidden";
	status_codes["404"] = "Not Found";
	status_codes["405"] = "Method Not Allowed";
	status_codes["406"] = "Not Acceptable";
	status_codes["407"] = "Proxy Authentication Required";
	status_codes["408"] = "Request Timeout";
	status_codes["409"] = "Conflict";
	status_codes["410"] = "Gone";
	status_codes["411"] = "Length Required";
	status_codes["412"] = "Precondition Failed";
	status_codes["413"] = "Payload Too Large";
	status_codes["414"] = "URI Too Long";
	status_codes["415"] = "Unsupported Media Type";
	status_codes["416"] = "Range Not Satisfiable";
	status_codes["417"] = "Expectation Failed";
	status_codes["418"] = "I'm a teapot";
	status_codes["421"] = "Misdirected Request";
	status_codes["422"] = "Unprocessable Entity";
	status_codes["423"] = "Locked";
	status_codes["424"] = "Failed Dependency";
	status_codes["425"] = "Too Early";
	status_codes["426"] = "Upgrade Required";
	status_codes["428"] = "Precondition Required";
	status_codes["429"] = "Too Many Requests";
	status_codes["431"] = "Request Header Fields Too Large";
	status_codes["451"] = "Unavailable For Legal Reasons";
	status_codes["500"] = "Internal S_Server Error";
	status_codes["501"] = "Not Implemented";
	status_codes["502"] = "Bad Gateway";
	status_codes["503"] = "Service Unavailable";
	status_codes["504"] = "Gateway Timeout";
	status_codes["505"] = "HTTP Version Not Supported";
	status_codes["506"] = "Variant Also Negotiates";
	status_codes["507"] = "Insufficient Storage";
	status_codes["508"] = "Loop Detected";
	status_codes["510"] = "Not Extended";
	status_codes["511"] = "Network Authentication Required";

	return (status_codes);
}

std::string getTimeCString()
{
	std::string date;
	char buf[1000];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	date = buf;
	return date;
}

std::string getErrorFile(std::string fileLocation)
{
	std::stringstream buffer;

	std::ifstream file;
	file.open(fileLocation.c_str());
	if (!file.is_open())
	{
		Log::error << fileLocation << Log::eof;
		throw ErrorFileNotFoundError();
	}
	buffer << file.rdbuf();
	return (buffer.str());
}

std::string getStandardErrorLocation(std::string error)
{
	std::stringstream buffer;

	buffer << ERROR_FOLDER << error << ".html";
	return (buffer.str());
}

void getErrorBody(S_Response &response)
{
	std::stringstream ss;
	ss << response.status_code;
	std::string error_location = "";

	if (response.status_code == 408)
		return;

	error_location = servers[response.server_number].custom_error[itoa(response.status_code)];

	try
	{
		if (error_location == "")
		{
			response.body = getErrorFile(getStandardErrorLocation(itoa(response.status_code)));
		}
		else
			response.body = getErrorFile(error_location);
		response.header_fields["Content-type"] = "text/html";
	}
	catch (const ErrorFileNotFoundError &e)
	{
		Log::error << e.what() << Log::eof;
		response.status_code = 500;
	}
	catch (const std::exception &e)
	{
		response.status_code = 500;
	}
}

int checkIfError(std::string code_string)
{
	int status_code = std::atoi(code_string.c_str());
	return (status_code >= 400 && status_code < 600);
}

void processHeaders(S_Response response, std::stringstream &buffer)
{
	std::map<std::string, std::string>::iterator end, it;
	end = response.header_fields.end();
	it = response.header_fields.begin();
	for (; it != end; it++)
	{
		buffer << it->first << ": " << it->second << "\n";
	}
	buffer << "\r\n";
}

void makeHeaders(S_Response &response)
{
	std::map<std::string, std::string> status_codes_info = getStatusCodeInfoTable();
	std::stringstream buffer;

	response.header_fields["Date"] = getTimeCString();
	response.header_fields["Connection"] = "Close";
	response.header_fields["Server"] = "-*{FORMA42}*-";
	if (response.body != "")
	{
		buffer << response.body.size();
		response.header_fields["Content-Length"] = buffer.str();
	}
}

std::string generateOutMessage(S_Response response)
{
	std::stringstream buffer;
	std::map<std::string, std::string> status_codes_info = getStatusCodeInfoTable();

	if (checkIfError(itoa(response.status_code)))
		getErrorBody(response);

	makeHeaders(response);

	buffer << "HTTP/1.1 "
		   << response.status_code << ' ' << status_codes_info[itoa(response.status_code)] << '\n';
	processHeaders(response, buffer);
	buffer << response.body;
	return (buffer.str());
}
