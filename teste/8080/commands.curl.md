```curl
curl -X GET http://localhost:8080
```

```curl
curl -X POST http://localhost:8080		// Vai dar erro 405
```

```curl
curl -X DELETE http://localhost:8080	// Vai dar erro 405
```

```curl
curl -X GET --resolve oitenta:8080:127.0.0.1 http://oitenta:8080
```
