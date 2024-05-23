```curl
curl -X GET http://localhost:8083 -v		// Redirect para o www.google.com
```

```curl
curl -X POST http://localhost:8083			// Vai dar erro 405
```

```curl
curl -X DELETE http://localhost:8083		// Vai dar erro 405
```

```curl
curl -X GET --resolve oitentaetres:8083:127.0.0.1 http://oitentaetres:8083
```
