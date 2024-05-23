#include "Error.hpp"

Error makeSuccess()
{
	Error err;
	err.message = "";
	err.status = SUCCESS;

	return err;
}

Error makeError(const std::string &message)
{
	Error err;

	err.message = message;
	err.status = ERROR;

	return err;
}
