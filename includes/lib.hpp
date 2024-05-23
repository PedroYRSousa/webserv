#ifndef lib_hpp
#define lib_hpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
#include <cstdlib>
#include <algorithm>
#include <sys/time.h>
#include <sys/wait.h>

#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

#include <sys/types.h>
#include <dirent.h>

#include "Log.hpp"
#include "Error.hpp"
#include "ConfigReader.hpp"
#include "Exceptions.hpp"

#define POST 1
#define GET 2
#define DELETE 4
#define ERROR_FOLDER "error/"

struct S_Location
{
	S_Location()
	{
		accepted_methods = 0;
		body_size_limit = -1;
		directory_listing = false;
		accept_files = false;
		accept_cgi = false;
	};
	std::string location_path;			  // mandatory - OK
	int accepted_methods;				  // mandatory - OK
	double body_size_limit;				  // optional, -1 if empty - OK
	std::string redirection;			  // optional, "" if empty - OK?
	std::string host_directory;			  // madatory
	bool directory_listing;				  // optional, standard is false
	std::string default_answer_directory; // optional, "" if empty
	bool accept_files;					  // optional, standard is false
	std::string file_upload_location;	  // optional, "" if empty
	bool accept_cgi;					  // optional, standard is false
};

struct S_Server
{
	S_Server() : port(-1){};
	int port;
	std::vector<std::string> server_name;
	std::vector<S_Location> locations;
	std::map<std::string, std::string> custom_error;
};

struct S_Request
{
	int server_number;
	int method;
	std::string path;
	std::string pathPure;
	std::string queryString;
	std::map<std::string, std::string> header_fields;
	std::string body; // o que fazer quando recever um binario
};

struct S_Response
{
	int status_code;
	std::string body;
	int server_number;
	std::map<std::string, std::string> header_fields;
};

extern std::vector<S_Server> servers;

// Parse Functions

void parseConfig(std::string configLocation);
void lowerString(std::string &string);
void printConfig();

// Handler server Functions

S_Response run(S_Request request);

// S_Location utils Functions

S_Server &getServer(int serverIndex);
S_Location findLocation(int server_number, const std::string &path);
bool hasRedirection(const S_Location &location);
void checkIfMethodIsAllowed(int method, const S_Location &location);
void checkBodySizeLimit(const S_Location &location, std::string body, int method);

// Resource utils Functions
void checkFileExist(std::string path);
bool isDirectory(std::string path);
void checkReadPermission(std::string path);
void checkWritePermission(std::string path);
std::string readFileContent(const std::string &path);
std::string getFileExtension(std::string path);
std::string getContentType(const std::string &path);
//
bool isCGI(S_Location location, S_Request request);
std::string buildFinalPath(S_Location location, S_Request request);

// Resource Functions
void getResource(S_Request &request, S_Response &response, S_Location &location);
void deleteResource(const S_Request &request, S_Response &response);
void postResource(const S_Request &request, S_Response &response, S_Location &location);

// Response
std::string generateOutMessage(S_Response response);

// Encoder
std::string encode(const std::string &input);
std::string decode(const std::string &input);

#endif
