#include <cstring>
#include <csignal>
#include <iostream>
#include <cstdlib>

#include "Log.hpp"
#include "Error.hpp"
#include "Schedule.hpp"

static void showHelp(void)
{
	std::cerr << "Webserv :\
\n	Programa criado para como atividade da 42 São Paulo com base no ngix.\
\nInputs:\
\n	Nao ha necessidade de flag para passar o arquivo de configuracao.\
\nUso:\
\n	./webserv [arquivo de configuracao] (flags)\
\n	[] => obrigatorio	() => opcional\
\nFlags :\
\n	--debug -d: Mostra todos os logs.\
\n	--info -i: Mostra todos os logs do tipo info, warning, erro e fatal.\
\n	--help -h: Mostra estas informações.\
\nAutores :\
\n	Pedro Yago Rabelo de Sousa (pyago-ra)\
\n	Vinicius Varussa Seneda (vvarussa)\
\n	Pedro Augusto Pinheiro De Souza Dias (paugusto)"
			  << std::endl;
	exit(0);
}

static void handleSignal(int a, siginfo_t *b, void *c)
{
	(void)b;
	(void)c;

	if (a == SIGINT || a == SIGKILL)
	{
		Log::info << "Recebido sinal de parada" << Log::eof;
		Log::debug << "Sinal: " << a << Log::eof;
		Schedule::stop();
	}
}

static Error parseFlags(size_t argc, const char **argv, std::string *filePath)
{
	for (size_t i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0)
			Log::setLevelLog(DEBUG_LEVEL);
		else if (strcmp(argv[i], "--info") == 0 || strcmp(argv[i], "-i") == 0)
			Log::setLevelLog(INFO_LEVEL);
		else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			showHelp();
		else
		{
			if (!(*filePath).empty())
				return makeError("Excesso de argumentos");

			(*filePath) = argv[i];
		}
	}

	Log::info << "parseFlags" << Log::eof;

	if ((*filePath).empty())
		return makeError("O arquivo de configuracao e obrigatorio");

	Log::debug << "Local do arquivo de configuração: " << (*filePath) << Log::eof;

	return makeSuccess();
}

#ifndef TEST_MODE // Normal
int main(int argc, const char **argv)
{
	struct sigaction listenSignal;
	listenSignal.sa_sigaction = handleSignal;
	listenSignal.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &listenSignal, NULL);
	sigaction(SIGKILL, &listenSignal, NULL);

	std::string filePath = "";
	Error err = parseFlags(argc, argv, &filePath);
	if (err.status == ERROR)
	{
		Log::error << "Erro ao ler os argumentos: " << err.message << Log::eof;
		showHelp();
	}

	err = Schedule::start(filePath);
	if (err.status == ERROR)
		Log::fatal << "Erro ao configurar tudo: " << err.message << Log::eof;

	err = Schedule::loop();
	if (err.status == ERROR)
		Log::fatal << "Erro critico no loop: " << err.message << Log::eof;

	err = Schedule::end();
	if (err.status == ERROR)
		Log::fatal << "Erro ao limpar tudo: " << err.message << Log::eof;

	return (0);
}
#else // Testes
#include <gtest/gtest.h>
int main(int argc, char **argv)
{
	// Inicialize o Google Test
	::testing::InitGoogleTest(&argc, argv);

	// Execute os testes
	return RUN_ALL_TESTS();
}
#endif // TEST_MODE
