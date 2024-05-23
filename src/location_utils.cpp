#include "lib.hpp"

S_Server &getServer(int serverIndex)
{
	if (serverIndex >= 0 && static_cast<std::vector<S_Server>::size_type>(serverIndex) < servers.size())
		return servers[serverIndex];
	else
		throw ServerNotFound();
}

bool compareLocations(const S_Location &a, const S_Location &b)
{
	return a.location_path.size() > b.location_path.size();
}

S_Location findLocation(int server_number, const std::string &path)
{

	const S_Server &server = getServer(server_number);
	const std::vector<S_Location> &locations = server.locations;

	std::vector<S_Location> sortedLocations = locations;
	std::sort(sortedLocations.begin(), sortedLocations.end(), compareLocations);

	for (std::vector<S_Location>::const_iterator it = sortedLocations.begin(); it != sortedLocations.end(); ++it)
	{
		if (path.find(it->location_path) == 0)
			return *it;
	}

	throw LocationNotFoundException();
}

bool hasRedirection(const S_Location &location)
{
	return !location.redirection.empty();
}

void checkIfMethodIsAllowed(int method, const S_Location &location)
{

	if ((location.accepted_methods & method) <= 0)
		throw MethodNotAllowed();
}

void checkBodySizeLimit(const S_Location &location, std::string body, int method)
{

	if ((method & GET) || (method & DELETE))
		return;
	if (location.body_size_limit < 0)
		return;
	if (location.body_size_limit < body.size())
		throw PayloadTooLarge();
}
