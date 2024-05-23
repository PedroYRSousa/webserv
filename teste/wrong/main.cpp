#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
	// Crie um socket
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1)
	{
		std::cerr << "Erro ao criar o socket." << std::endl;
		return 1;
	}

	// Defina o endereço do servidor e porta
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);					// Porta HTTP
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Endereço IP do servidor

	// Conecte-se ao servidor
	if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		std::cerr << "Erro ao conectar ao servidor." << std::endl;
		close(clientSocket);
		return 1;
	}

	// Envie uma requisição HTTP GET
	std::string request = "GET / adadasd/1.1\r\nHost: localhost\r\nValor: \r\n\r\n";
	if (send(clientSocket, request.c_str(), request.length(), 0) == -1)
	{
		std::cerr << "Erro ao enviar a requisição." << std::endl;
		close(clientSocket);
		return 1;
	}

	// Receba e imprima a resposta do servidor
	char buffer[4096];
	int bytesRead;
	while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0) > 0))
	{
		std::cout.write(buffer, bytesRead);
	}

	// Feche o socket
	close(clientSocket);

	return 0;
}
