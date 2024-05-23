#include "lib.hpp"

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
static std::string itoa(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}
static std::vector<std::string> split(const std::string &s, char delimiter)
{
	std::vector<std::string> tokens;
	std::stringstream ss(s);
	std::string token;

	while (std::getline(ss, token, delimiter))
		tokens.push_back(token);

	return tokens;
}
static void addHeaders(S_Response &response, std::string cgiResponse, size_t bodyInit)
{
	std::string headers = cgiResponse.substr(0, bodyInit);
	std::vector<std::string> headersSplit = split(headers, '\n');
	for (std::vector<std::string>::iterator it = headersSplit.begin(); it != headersSplit.end(); it++)
	{
		std::vector<std::string> keyValue = split(headers, ':');
		std::string value = "";
		for (std::vector<std::string>::iterator it = (keyValue.begin() + 1); it != keyValue.end(); it++)
		{
			value.append((*it));
			if ((it + 1) != keyValue.end())
				value.append(":");
		}

		response.header_fields[keyValue[0]] = trim(value);
	}
}
static std::vector<std::string> getEnvs(S_Request request, S_Location location)
{
	std::string method = "";
	std::vector<std::string> envs(5);
	if (request.method == POST)
		method = "POST";
	if (request.method == GET)
		method = "GET";
	if (request.method == DELETE)
		method = "DELETE";

	envs[0] = "REQUEST_METHOD=" + method;
	envs[1] = "QUERY_STRING=" + request.queryString;
	envs[2] = "CONTENT_TYPE=text/html";
	envs[3] = "CONTENT_LENGTH=" + itoa(request.body.length());
	// if (headers.count("user-agent") > 0)
	// envs[5] = "HTTP_USER_AGENT=" + headers["user-agent"];
	envs[4] = "DOCUMENT_ROOT=" + location.host_directory;
	// envs[7] = "SCRIPT_FILENAME=" + resource.substr(1);
	// envs[8] = "SCRIPT_NAME=" + resource.substr(resource.find_last_of('/') + 1);
	// envs[9] = "REDIRECT_STATUS=200";
	// envs[10] = "REQUEST_BODY=" + request.getBody();

	return envs;
}
static std::string getPath(S_Request request)
{
	return request.path;
}
static void cgiExec(int _pipe[2], S_Request request, S_Location location)
{
	std::vector<std::string> envs = getEnvs(request, location);
	std::string pathBinary = "/usr/bin/python3";
	std::string pathFile = getPath(request);

	char const *argv[] = {pathBinary.c_str(), pathFile.c_str(), NULL};
	char const *envp[] = {envs[0].c_str(), envs[1].c_str(), envs[2].c_str(), envs[3].c_str(), envs[4].c_str(), NULL};

	if (dup2(_pipe[0], STDIN_FILENO) < 0)
	{
		Log::error << std::string(strerror(errno)) << pathBinary << Log::eof;
		exit(1);
	} // Erro no dup
	if (close(_pipe[0]) < 0)
	{
		Log::error << std::string(strerror(errno)) << pathBinary << Log::eof;
		exit(1);
	} // Erro no close
	if (dup2(_pipe[1], STDOUT_FILENO) < 0)
	{
		Log::error << std::string(strerror(errno)) << pathBinary << Log::eof;
		exit(1);
	} // Erro no dup
	if (close(_pipe[1]) < 0)
	{
		Log::error << std::string(strerror(errno)) << pathBinary << Log::eof;
		exit(1);
	} // Erro no close

	if (access(pathBinary.c_str(), X_OK) < 0)
	{
		Log::error << std::string(strerror(errno)) << pathBinary << Log::eof;
		exit(1);
	}
	if (access(pathFile.c_str(), X_OK) < 0)
	{
		Log::error << std::string(strerror(errno)) << pathFile << Log::eof;
		exit(1);
	}

	if (execve(pathBinary.c_str(), (char *const *)argv, (char *const *)envp) < 0)
	{
		Log::error << "execve: " << std::string(strerror(errno)) << pathFile << Log::eof;
		exit(1);
	}

	exit(0);
}
static S_Response cgiSchedule(int _pid, int _pipe[2], S_Request request, S_Location location)
{
	(void)request;
	(void)location;
	int ret = 1;
	S_Response res;

	size_t timeout = 1000;

	if (close(_pipe[1]) < 0)
		throw CgiClosePipeError();

	pid_t pid = _pid;
	struct timeval startTime;
	gettimeofday(&startTime, NULL);

	while (true)
	{
		int __stat_loc = 0;
		pid_t result = waitpid(pid, &__stat_loc, WNOHANG);

		if (__stat_loc != 0)
			throw CgiExecuteError();

		if (result == ERROR)
			throw CgiWaitpidError();

		if (result != 0)
			break;

		struct timeval currentTime;
		gettimeofday(&currentTime, NULL);
		unsigned int elapsedTime = (currentTime.tv_sec - startTime.tv_sec) * 1000 + (currentTime.tv_usec - startTime.tv_usec) / 1000;
		if (elapsedTime >= timeout)
		{
			kill(pid, SIGTERM);
			throw CgiTimeoutError();
		}
		usleep(1000);
	}

	int const bufsize = 1048576; // 1 Mega
	char buffer[bufsize + 1];

	size_t bodyInit = 0;
	std::string cgiResponse;
	while (ret)
	{
		ret = read(_pipe[0], buffer, bufsize);
		if (ret < 0)
			throw CgiReadError();
		buffer[ret] = '\0';

		cgiResponse.append(std::string(buffer, ret));
	}

	if (close(_pipe[0]) < 0)
		throw CgiClosePipeError();

	bodyInit = cgiResponse.find("\r\n\r\n");
	if (bodyInit != std::string::npos)
	{
		res.body.append(cgiResponse.substr(bodyInit + 5));
		addHeaders(res, cgiResponse, bodyInit);
	}
	else
		addHeaders(res, cgiResponse, bodyInit);

	res.status_code = 200;
	return res;
}
S_Response runCGI(S_Response &response, S_Request request, S_Location location)
{
	int _pid = 0;
	int _pipe[2];

	if (pipe(_pipe) == ERROR)
		throw CgiPipeCreateError();

	if (request.method == POST)
	{
		request.body = encode(request.body);

		if (request.body.length() >= 65536)
			throw PayloadTooLarge();

		if (write(_pipe[1], request.body.c_str(), request.body.length()) <= ERROR)
			throw CgiWriteError();
	}

	_pid = fork();
	if (_pid == 0)
		cgiExec(_pipe, request, location);
	else if (_pid > 0)
		response = cgiSchedule(_pid, _pipe, request, location);
	else
		throw CgiForkCreateError();

	return response;
}

S_Response run(S_Request request)
{
	S_Response response;
	response.server_number = request.server_number;
	std::string content_type;

	request.pathPure = request.path;

	try
	{
		// Decidir a location
		S_Location location = findLocation(request.server_number, request.path);

		// Verifica se tem redirect
		if (hasRedirection(location))
		{
			// verifica metodos
			checkIfMethodIsAllowed(request.method, location);

			Log::debug << "Redirection no path: " << request.pathPure << " para: " << location.redirection << Log::eof;
			response.status_code = 302;
			response.header_fields["Location"] = location.redirection;
			return response;
		}

		// verifica metodos
		checkIfMethodIsAllowed(request.method, location);

		// verifica se tem body_size_limit
		checkBodySizeLimit(location, request.body, request.method);

		// Monta o Path verdadero - Host_directory + S_Request path
		request.path = buildFinalPath(location, request);

		// Limitado a execução a py
		if (isCGI(location, request))
		{

			// Verifica se é arquivo e se existe
			checkFileExist(request.path);

			// verifica se tem acesso ao arquivo
			// Erro de falta de acesso
			checkReadPermission(request.path);

			Log::debug << "CGI no path: " << request.pathPure << Log::eof;
			return runCGI(response, request, location);
		}

		// GET? POST? DELETE?
		if (request.method & GET)
		{
			getResource(request, response, location);
		}
		else
		{
			if (!location.accept_files)
				throw LocactionAcceptFileError();

			if (request.method & POST)
			{
				struct stat buffer;

				// Verifica se é arquivo e se existe
				// verifica se tem acesso ao arquivo
				// Erro de falta de acesso
				if (stat(request.path.c_str(), &buffer) == 0)
				{
					checkReadPermission(request.path);
					checkWritePermission(request.path);
				}

				postResource(request, response, location);
			}
			else if (request.method & DELETE)
			{
				// Verifica se é arquivo e se existe
				checkFileExist(request.path);

				// verifica se tem acesso ao arquivo
				// Erro de falta de acesso
				checkReadPermission(request.path);
				checkWritePermission(request.path);

				deleteResource(request, response);
			}
		}
	}
	catch (const LocationNotFoundException &e)
	{
		response.status_code = 404;
		Log::error << e.what() << " " << request.path << Log::eof;
		return response;
	}
	catch (const ServerNotFound &e)
	{
		response.status_code = 404;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const MethodNotAllowed &e)
	{
		response.status_code = 405;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const PayloadTooLarge &e)
	{
		response.status_code = 413;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const ResourceNotFound &e)
	{
		response.status_code = 404;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const ForbiddenAccess &e)
	{
		response.status_code = 403;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const InternalAccessError &e)
	{
		response.status_code = 500;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const InternalOpenError &e)
	{
		response.status_code = 404;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const ReadFileError &e)
	{
		response.status_code = 500;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const InternalDeleteError &e)
	{
		response.status_code = 500;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const InternalOpenFile &e)
	{
		response.status_code = 500;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const IsNotADirectory &e)
	{
		response.status_code = 500;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const CgiPipeCreateError &e)
	{
		response.status_code = 500;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const CgiWriteError &e)
	{
		response.status_code = 500;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const CgiForkCreateError &e)
	{
		response.status_code = 500;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const CgiClosePipeError &e)
	{
		response.status_code = 500;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const CgiWaitpidError &e)
	{
		response.status_code = 500;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const CgiExecuteError &e)
	{
		response.status_code = 500;
		return response;
	}
	catch (const CgiTimeoutError &e)
	{
		response.status_code = 408;
		Log::error << e.what() << Log::eof;
		return response;
	}
	catch (const LocactionAcceptFileError &e)
	{
		response.status_code = 403;
		Log::error << e.what() << Log::eof;
		return response;
	}

	return response;
}
