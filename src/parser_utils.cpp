#include "lib.hpp"

void printList(std::vector<std::string> &list)
{
	for (std::size_t i = 0; i < list.size(); i++)
		Log::debug << list[i] << " " << Log::eof;
}

void printLocation(int serveri, int index)
{
	Log::debug << "location: \n"
			   << "#Path: " << servers[serveri].locations[index].location_path << "\n"
			   << "#Methods: " << servers[serveri].locations[index].accepted_methods << "\n"
			   << "#limit body size: " << servers[serveri].locations[index].body_size_limit << "\n"
			   << "#redirection: " << servers[serveri].locations[index].redirection << "\n"
			   << "#host_directory: " << servers[serveri].locations[index].host_directory << "\n"
			   << "#directory_listing: " << servers[serveri].locations[index].directory_listing << "\n"
			   << "#default_answer_directory: " << servers[serveri].locations[index].default_answer_directory << "\n"
			   << "#execute_cgi: " << servers[serveri].locations[index].accept_cgi << "\n"
			   << "#accept_files: " << servers[serveri].locations[index].accept_files << "\n"
			   << "#file_upload_location: " << servers[serveri].locations[index].file_upload_location << "\n"
			   << Log::eof;
}

void printServer(int index)
{
	Log::debug << "Port: " << servers[index].port << '\n'
			   << "Names: " << Log::eof;
	printList(servers[index].server_name);
	Log::debug << '\n'
			   << Log::eof;
	Log::debug << "Erro Page Numbers: " << servers[index].custom_error.size() << '\n'
			   << Log::eof;
	for (std::size_t i = 0; i < servers[index].locations.size(); i++)
		printLocation(index, i);
	Log::debug << "\n\n"
			   << Log::eof;
}

void printConfig()
{
	for (std::size_t i = 0; i < servers.size(); i++)
		printServer(i);
}
