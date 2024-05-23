#include "Schedule.hpp"

// STATIC LOCAL
// STATIC PUBLIC
Error Schedule::start(std::string filePathConfig)
{
	Log::info << "Iniciando o schedule" << Log::eof;

	parseConfig(filePathConfig);
	int index = 0;

	for (std::vector<S_Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		Server *s = new Server((*it), index);
		Error err = s->init();
		if (err.status == ERROR)
			Log::fatal << err.message << Log::eof;
		Schedule::addSocket(s);
		index++;
	}

	return makeSuccess();
}
Error Schedule::loop(void)
{
	Log::info << "Loop do schedule" << Log::eof;

	Error err;
	int result = 0;
	Socket *socket;
	struct pollfd *pollFD;

	while (Schedule::_instance.toContinue)
	{
		Log::info << Schedule::_instance.sockets.size() << " conexões ativas" << Log::eof;

		if ((result = poll(Schedule::_instance.polls.data(), Schedule::_instance.polls.size(), Schedule::_instance.timeout)) == ERROR)
		{
			if (std::string(strerror(errno)).compare("Interrupted system call") != 0)
				return makeError("poll error: " + std::string(strerror(errno)));
			else
				break;
		}

		Log::info << result << " conexõe(s) tem evento(s) ativo(s)" << Log::eof;

		for (size_t i = 0; i < Schedule::_instance.polls.size(); i++)
		{
			pollFD = &Schedule::_instance.polls[i];
			socket = Schedule::_instance.sockets[pollFD->fd];

			if (pollFD->revents != 0 && socket->isServer())
			{
				err = Schedule::handleServer(pollFD, dynamic_cast<Server *>(socket));
				if (err.status == ERROR)
					return err;
			}
			else if (pollFD->revents != 0)
			{
				err = Schedule::handleClient(pollFD, dynamic_cast<Client *>(socket));
				if (err.status == ERROR)
					Log::error << err.message << Log::eof;
			}
		}
	}

	return makeSuccess();
}
Error Schedule::end(void)
{
	Log::info << "Finalizando o schedule" << Log::eof;

	return makeSuccess();
}
void Schedule::stop(void)
{
	Log::info << "Parando schedule" << Log::eof;

	Schedule::_instance.toContinue = false;
}

void Schedule::addSocket(Socket *s)
{
	Schedule::_instance.sockets.insert(std::make_pair(s->getPollFD().fd, s));
	Schedule::_instance.polls.push_back(s->getPollFD());
}
void Schedule::removeSocket(Socket *s)
{
	Schedule::_instance.sockets.erase(s->getPollFD().fd);
	for (size_t i = 0; i < Schedule::_instance.polls.size(); i++)
	{
		if (Schedule::_instance.polls[i].fd == s->getPollFD().fd)
		{
			Schedule::_instance.polls.erase(Schedule::_instance.polls.begin() + i);
			break;
		}
	}

	delete s;
}
// STATIC PROTECTED
// STATIC PRIVATE
Schedule Schedule::_instance = Schedule();
Error Schedule::handleServer(struct pollfd *poll, Server *server)
{
	Log::info << "O servidor " << server->getPort() << " tem uma nova conexão" << Log::eof;

	int newFD = 0;
	Client *c = NULL;

	// Aceita uma conexão de cliente
	if (poll->revents & POLLIN)
	{
		if ((newFD = accept(poll->fd, server->getSockAddress(), server->getSockLenAddress())) < 0)
			return makeError("accept error: " + std::string(strerror(errno)));

		c = new Client(newFD, server);
		c->init();
		Schedule::addSocket(c);

		Log::info << "O servidor " << server->getPort() << " aceitou um novo usuario" << Log::eof;
	}

	return makeSuccess();
}
Error Schedule::handleClient(struct pollfd *poll, Client *client)
{
	Log::info << "Um cliente tem uma requisição" << Log::eof;
	Log::debug << "Eventos: \n"
			   << "\tLeitura (in): " << (poll->revents & POLLIN)
			   << "\n\tEscrita (out): " << (poll->revents & POLLOUT) << Log::eof;

	if (poll->revents & POLLIN) // Entrada (read)
	{
		return Schedule::readClient(poll, client);
	}
	else if (poll->revents & POLLOUT) // Saida (write)
	{
		return Schedule::writeClient(poll, client);
	}
	else
	{
		Schedule::removeSocket(client);
	}

	return makeSuccess();
}
Error Schedule::readClient(struct pollfd *poll, Client *client)
{
	Log::info << "Lendo a requisição" << Log::eof;

	char buffer[1048576] = {0}; // 1 Mega
	int valread = read(poll->fd, buffer, sizeof(buffer));
	if (valread == 0) // FD Fechado pelo cliente
	{
		Log::info << "A requisição foi cancelada pelo cliente" << Log::eof;
		Schedule::removeSocket(client);
	}
	else if (valread <= ERROR) // Lidando com erro de leitura
	{
		Log::info << "Erro ao ler a requisição do cliente" << Log::eof;
		client->clearRequest();
		client->digestRequest();
	}
	else
	{
		client->readRequest(std::string(buffer, valread));
	}

	return makeSuccess();
}

Error Schedule::writeClient(struct pollfd *poll, Client *client)
{
	if (client->getRequest() == NULL && client->getRetry() <= 100)
	{
		client->addRetry();
		usleep(100);
		return makeSuccess();
	}

	if (client->getResponse() == NULL && !client->getIsDigesting())
	{
		Log::info << "Processando a requisição" << Log::eof;

		client->digestRequest();
	}
	else if (client->getResponse() != NULL)
	{
		Log::info << "Enviando a resposta" << Log::eof;

		S_Response *res = client->getResponse();
		std::string resDumped = generateOutMessage(*res);
		Log::debug << resDumped.c_str() << Log::eof;

		int valsend = send(poll->fd, resDumped.c_str(), resDumped.size(), 0);
		if (valsend == 0)
			Log::error << "FD Ja fechado pelo cliente" << Log::eof;
		else if (valsend <= ERROR)
			Log::error << "Erro ao enviar a resposta" << Log::eof;

		Schedule::removeSocket(client);
	}

	return makeSuccess();
}
// PUBLIC
// PROTECTED
// PRIVATE
Schedule::Schedule()
{
	this->timeout = -1;
	this->toContinue = true;
	this->sockets = std::map<int, Socket *>();
	this->polls = std::vector<struct pollfd>();
}
Schedule::~Schedule()
{
	for (std::map<int, Socket *>::iterator it = this->sockets.begin(); it != this->sockets.end(); it++)
	{
		delete it->second;
		it->second = NULL;
	}

	this->sockets.clear();
}
