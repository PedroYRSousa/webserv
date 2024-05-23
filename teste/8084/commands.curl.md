```curl
curl -X GET http://localhost:8084						// Erro de 404, o arquivo .py é necessario
curl -X GET http://localhost:8084/teste.php				// Erro de 404, o arquivo .py é necessario
curl -X GET http://localhost:8084/cgi/main.py			// Executa o cgi
curl -X GET http://localhost:8084/cgi/mainError.py		// Executa o cgi. Retorna erro 500. Tem um erro no .py
curl -X GET http://localhost:8084/cgi/mainLoop.py -v	// Executa o cgi. Retorna erro 408. Tem um erro no .py
curl -X GET http://localhost:8084/cgi/mainpost.py		// Executa o cgi. Mas ele executa melhor no POST.
```

```curl
curl -X POST http://localhost:8084									// Erro de 403
curl -X POST http://localhost:8084/teste.php						// Erro de 403
curl -X POST http://localhost:8084/cgi/main.py						// Executa o cgi
curl -X POST http://localhost:8084/cgi/mainError.py					// Executa o cgi. Retorna erro 500. Tem um erro no .py
curl -X POST http://localhost:8084/cgi/mainLoop.py -v				// Executa o cgi. Retorna erro 408. Tem um erro no .py
curl -X POST http://localhost:8084/cgi/mainpost.py -d "BODY"		// Executa o cgi. E mostra o body que recebeu.
curl -X POST --data-binary "@/home/yago/42Web/src/main.cpp" http://localhost:8084/cgi/mainpost.py	// Executa o cgi. E mostra o body que recebeu.
curl -X POST --data-binary "@/home/yago/42Web/webserv" http://localhost:8084/cgi/mainpost.py		// Executa o cgi. Porem o body é grande d+.
```

```curl
curl -X DELETE http://localhost:8084		// Vai dar erro 405
```
