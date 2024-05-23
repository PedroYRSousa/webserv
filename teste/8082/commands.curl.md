```curl
curl -X GET http://localhost:8082		// Retorna um site estatico que esta definifido como padrão quando buscar diretorios
```

```curl
curl -X POST http://localhost:8082		// Vai dar erro 405
```

```curl
curl -X DELETE http://localhost:8082	// Vai dar erro 405
```

```curl
curl -X GET --resolve oitentaedois:8082:127.0.0.1 http://oitentaedois:8082
```
