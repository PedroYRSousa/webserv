#include "lib.hpp"
#include "dirListing.hpp"

static std::vector<std::string> split(const std::string &s, char delimiter)
{
	std::vector<std::string> tokens;
	std::stringstream ss(s);
	std::string token;

	while (std::getline(ss, token, delimiter))
		tokens.push_back(token);

	return tokens;
}

std::string intToString(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void setHeaders(const S_Location &location, const S_Request &request, S_Response &response)
{

	if (location.directory_listing)
		response.header_fields["Content-Type"] = "text/html";
	else
		response.header_fields["Content-Type"] = getContentType(request.path);
	response.status_code = 200;
}

void getResource(S_Request &request, S_Response &response, S_Location &location)
{
	std::string extension;

	// Verifica se existe e se é diretorio
	if (isDirectory(request.path))
	{
		// verifica config de DIRECTORY LISTING
		if (location.directory_listing)
		{
			DirListing dir(request, location.host_directory);
			response.body = dir.getPageString();
			response.header_fields["Content-Length"] = intToString(response.body.size());

			// seta headers - fazer func
			response.header_fields["Content-Type"] = "text/html";
			response.status_code = 200;

			return;
		}
		else
		{
			if (!location.default_answer_directory.empty())
			{
				// um monte de coisa
				request.path = location.default_answer_directory;
			}
			else
			{
				response.status_code = 404;
				return;
				// erro (qual?)
			}
		}
	}

	// Verifica se é arquivo e se existe
	checkFileExist(request.path);

	// verifica se tem acesso ao arquivo
	// Erro de falta de acesso
	checkReadPermission(request.path);

	// le arquivo, seta headers e devolve com status 200
	response.body = readFileContent(request.path);
	response.header_fields["Content-Length"] = intToString(response.body.size());

	// fiz Func headers
	response.header_fields["Content-Type"] = getContentType(request.path);
	response.status_code = 200;
}

void deleteResource(const S_Request &request, S_Response &response)
{
	if (isDirectory(request.path))
		throw ForbiddenAccess();

	checkFileExist(request.path);

	if (std::remove(request.path.c_str()) != 0)
	{
		throw InternalDeleteError();
		return;
	}

	response.status_code = 204;
	response.body = "Arquivo deletado com sucesso";
	response.header_fields["Content-Type"] = "text/plain";
	response.header_fields["Content-Length"] = intToString(response.body.size());
}

void postResource(const S_Request &request, S_Response &response, S_Location &location)
{
	std::ofstream newFile;

	std::string toMount = request.path.substr(location.host_directory.length());

	std::vector<std::string> vToMount = split(toMount, '/');
	std::string pathConstruct = location.host_directory;
	for (std::vector<std::string>::iterator it = vToMount.begin(); it != vToMount.end(); it++)
	{
		if (it + 1 == vToMount.end())
			break;
		if ((*it) == "")
			continue;

		pathConstruct = pathConstruct + "/" + (*it);
		mkdir(pathConstruct.c_str(), 0777);
	}

	newFile.open(request.path.c_str(), std::ios::binary);

	if (newFile.fail())
	{
		throw InternalOpenFile();
		return;
	}

	newFile.write(request.body.c_str(), request.body.length());
	newFile.close();

	response.body = "Criado com suceesso";
	response.status_code = 201;
	response.header_fields["Content-Type"] = "text/plain";
}
