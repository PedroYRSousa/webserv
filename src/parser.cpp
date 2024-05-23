#include "lib.hpp"

std::vector<S_Server> servers;

void parseError(std::string message, ConfigReader &config)
{
	std::cerr << message << " (line: " << config.getLineNumber() << ")" << std::endl;
	exit(1);
}

void parseError(std::string message)
{
	std::cerr << message << std::endl;
	exit(1);
}

int parsePortNumber(ConfigReader &configFile, S_Server &server)
{
	if (configFile.hasLine() && configFile.getLineVector()[0] == "listen")
	{
		int port;

		if (configFile.getNumberOfWords() != 2)
			throw "Port requires one number as argument";
		port = std::atoi(configFile.getLineVector()[1].c_str());
		// write check for port number limits
		server.port = port;
		configFile.readLine();
		return (1);
	}
	return (0);
}

int parseCustomError(ConfigReader &configFile, S_Server &server)
{
	if (configFile.hasLine() && configFile.getLineVector()[0] == "error_page")
	{
		std::string error_code;

		if (configFile.getNumberOfWords() == 1)
			parseError("There must be at least one server name", configFile);
		error_code = configFile.getLineVector()[1];
		server.custom_error[error_code] = configFile.getLineVector()[2];
		configFile.readLine();
		return (1);
	}
	return (0);
}

int parseServerName(ConfigReader &configFile, S_Server &server)
{
	if (configFile.hasLine() && configFile.getLineVector()[0] == "server_name")
	{
		if (configFile.getNumberOfWords() == 1)
			parseError("There must be at least one server name", configFile);
		for (int i = 1; i < configFile.getNumberOfWords(); i++)
			server.server_name.push_back(configFile.getLineVector()[i]);
		configFile.readLine();
		return (1);
	}
	return (0);
}

void parsePath(ConfigReader &configFile, S_Location &location)
{
	if (configFile.hasLine() && configFile.getLineVector()[0] == "path")
	{
		if (configFile.getNumberOfWords() != 2)
			parseError("There must be a Path especified", configFile);
		location.location_path = configFile.getLineVector()[1];
		configFile.readLine();
	}
}

void parseBodySizeLimit(ConfigReader &configFile, S_Location &location)
{
	if (configFile.hasLine() && configFile.getLineVector()[0] == "limit_body_size")
	{
		if (configFile.getNumberOfWords() != 2)
			parseError("There must be a value", configFile);
		location.body_size_limit = std::atoi(configFile.getLineVector()[1].c_str());
		configFile.readLine();
	}
}

void parseRedirection(ConfigReader &configFile, S_Location &location)
{
	if (configFile.hasLine() && configFile.getLineVector()[0] == "redirection")
	{
		if (configFile.getNumberOfWords() != 2)
			parseError("There must be a value", configFile);
		location.redirection = configFile.getLineVector()[1];
		configFile.readLine();
	}
}

void parseHostDirectory(ConfigReader &configFile, S_Location &location)
{
	if (configFile.hasLine() && configFile.getLineVector()[0] == "host_directory")
	{
		if (configFile.getNumberOfWords() != 2)
			parseError("There must be a value", configFile);
		location.host_directory = configFile.getLineVector()[1];
		configFile.readLine();
	}
}

void parseDefaultAwnswerDirectory(ConfigReader &configFile, S_Location &location)
{
	if (configFile.hasLine() && configFile.getLineVector()[0] == "default_answer_directory")
	{
		if (configFile.getNumberOfWords() != 2)
			parseError("There must be a value", configFile);
		location.default_answer_directory = configFile.getLineVector()[1];
		configFile.readLine();
	}
}

void parseCgi(ConfigReader &configFile, S_Location &location)
{
	if (configFile.hasLine() && configFile.getLineVector()[0] == "execute_cgi")
	{
		if (configFile.getNumberOfWords() != 2)
			parseError("There must be a value", configFile);
		location.accept_cgi = true;
		configFile.readLine();
	}
}

void parseFileUploadLocation(ConfigReader &configFile, S_Location &location)
{
	if (configFile.hasLine() && configFile.getLineVector()[0] == "accept_files")
	{
		if (configFile.getNumberOfWords() != 2)
			parseError("There must be a path to save files", configFile);
		location.accept_files = true;
		location.file_upload_location = configFile.getLineVector()[1];
		configFile.readLine();
	}
}

void lowerString(std::string &string)
{
	for (std::string::iterator it = string.begin(); it != string.end(); it++)
		*it = std::tolower(*it);
}

void parseDirectoryListing(ConfigReader &configFile, S_Location &location)
{
	std::string value;

	if (configFile.hasLine() && configFile.getLineVector()[0] == "directory_listing")
	{
		if (configFile.getNumberOfWords() != 2)
			parseError("There must be a value", configFile);

		value = configFile.getLineVector()[1];
		lowerString(value);
		if (value != "true" && value != "false")
			parseError("Only true or false", configFile);
		if (value == "true")
			location.directory_listing = true;
		configFile.readLine();
	}
}

void parseMethods(ConfigReader &configFile, S_Location &location)
{
	std::string method;

	if (configFile.hasLine() && configFile.getLineVector()[0] == "accepted_methods")
	{
		if (configFile.getNumberOfWords() == 1)
			parseError("There must be at least one method", configFile);
		for (int i = 1; i < configFile.getNumberOfWords(); i++)
		{
			method = configFile.getLineVector()[i];
			lowerString(method);
			if (method != "post" && method != "get" && method != "delete")
				parseError("Only POST, PUT or DELETE are available", configFile);
			if (method == "post")
				location.accepted_methods = location.accepted_methods | POST;
			else if (method == "get")
				location.accepted_methods = location.accepted_methods | GET;
			else if (method == "delete")
				location.accepted_methods = location.accepted_methods | DELETE;
		}
		configFile.readLine();
	}
}

void verifyLocation(S_Location &location)
{
	if (location.location_path == "")
		parseError("there must be a path in each location");
	if (location.accepted_methods == 0)
		parseError("there must be at least an accepted method in each location");
	if (location.host_directory == "")
		parseError("there must be a directory in each location");
}

int parseLocation(ConfigReader &configFile, S_Server &server)
{
	S_Location location;
	int n;

	if (configFile.hasLine() && configFile.getLineVector()[0] == "location:")
	{
		configFile.readLine();
		while (configFile.getIfstream() && configFile.getLineLevel() == 2)
		{
			n = configFile.getLineNumber();
			parsePath(configFile, location);
			parseMethods(configFile, location);
			parseBodySizeLimit(configFile, location);
			parseRedirection(configFile, location);
			parseHostDirectory(configFile, location);
			parseDirectoryListing(configFile, location);
			parseDefaultAwnswerDirectory(configFile, location);
			parseCgi(configFile, location);
			parseFileUploadLocation(configFile, location);
			// make error for unknown lines
			if (configFile.getLineNumber() == n)
				parseError("impossible config", configFile);
		}
		verifyLocation(location);
		server.locations.push_back(location);
		return (1);
	}
	return (0);
}

void verifyServer(S_Server &server)
{
	if (server.locations.size() < 1)
		parseError("There must be at least one location");
	// if (server.server_name.size() < 1)
	//	parseError("There must be at least one server name");
	if (server.port < 0)
		parseError("there must be a designated port");
}

void parseServer(ConfigReader &config)
{
	S_Server server;
	int n;

	if (config.getNumberOfWords() > 0 && config.getLineVector()[0] == "server:")
	{
		config.readLine();
		while (config.getIfstream() && config.getLineLevel() == 1)
		{
			n = config.getLineNumber();
			parsePortNumber(config, server);
			parseCustomError(config, server);
			parseServerName(config, server);
			parseLocation(config, server);
			if (config.getLineNumber() == n)
				parseError("impossible config", config);
		}
		verifyServer(server);
		// if(config.getNumberOfWords() == 0)
		// 	parseError("There must be no empty lines", config);
		servers.push_back(server);
	}
	else
		config.readLine();
}

void parseConfig(std::string configLocation)
{
	Log::info << "parseConfig" << Log::eof;

	ConfigReader config(configLocation);

	while (config.getIfstream())
	{
		try
		{
			parseServer(config);
		}
		catch (char const *s)
		{
			Log::error << s << Log::eof;
			exit(1);
		}
	}
}
