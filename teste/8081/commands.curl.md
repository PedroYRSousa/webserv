```curl
curl -X GET http://localhost:8081
```

```curl
curl -X POST http://localhost:8081 															// Vai dar erro 500
curl -X POST http://localhost:8081/teste.txt												// OK arquivo vazio
curl -X POST http://localhost:8081/teste2.txt -d "BODY"										// OK
curl -X POST -H "expect:" --data-binary "@/home/yago/42Web/src/main.cpp" http://localhost:8081/main.cpp	// OK
```

```curl
curl -X DELETE http://localhost:8081 															// Vai dar erro 403
curl -X DELETE http://localhost:8081/teste.txt 													// OK, se post for executado antes
```

```curl
curl -X GET --resolve oitentaeum:8081:127.0.0.1 http://oitentaeum:8081
```
