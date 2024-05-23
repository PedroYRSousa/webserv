```curl
curl -X GET http://localhost:8085
```

```curl
curl -X POST http://localhost:8085 															// Vai dar erro 500
curl -X POST http://localhost:8085/teste.txt												// OK arquivo vazio
curl -X POST http://localhost:8085/teste2.txt -d "BODY"										// OK
curl -X POST http://localhost:8085/teste2.txt -d "1234567890A"								// Erro 413
curl -X POST -H "expect:" --data-binary "@/home/yago/42Web/src/main.cpp" http://localhost:8085/main.cpp	// Erro 413
```

```curl
curl -X DELETE http://localhost:8085	// Vai dar erro 405
```
