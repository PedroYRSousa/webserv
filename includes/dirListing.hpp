#ifndef dirListing_hpp
#define dirListing_hpp

#include "lib.hpp"
#include <list>

class DirListing
{
public:
	DirListing();
	DirListing(S_Request request, std::string root_directory);
	~DirListing();

	void addFile(std::string file_name, std::string path);
	void addFiles(std::list<dirent *> files, S_Request request);
	void makePage(S_Request request, std::string root_directory);
	std::string getPageString();

	void setHost(std::string host);

private:
	std::string _host;
	std::string _path;
	std::stringstream _page;
};

#endif
