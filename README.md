# HttpServer

A multithreaded HTTP/1.1 server built from scratch in C++ using POSIX sockets. No external libraries — just raw system calls.

## Build & Run

```bash
make        # compile
make run    # compile + run
make clean  # remove build artifacts
```

Then visit `http://localhost:8080` in your browser.

## Project Structure

```
HttpServer/
├── src/           # C++ source files
│   └── main.cpp   # entry point
├── include/       # header files
├── static/        # files served by the server (HTML, CSS, JS, images)
│   └── index.html
├── Makefile
└── README.md
```

## Roadmap

- [ ] TCP socket setup (bind, listen, accept)
- [ ] HTTP request parser (method, path, headers)
- [ ] Static file serving with MIME type detection
- [ ] Response builder (status line, headers, body)
- [ ] GET and POST handling
- [ ] Thread pool for concurrent connections
- [ ] Keep-alive support
- [ ] Graceful shutdown (SIGINT/SIGTERM)
- [ ] Simple route matching
- [ ] Benchmark with `wrk`
