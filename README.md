# network_programming

## RELATED RFC
RFC 7230 to RFC 7235 to implement basic workings of HTTP.

HTTP/2 → RFC 7540 and RFC 7541

FTP → RFC959

###  Questions for fabio
- reusing the address variable 
- low level sockets interaction need to be with characters? - move them to strings

## PSA
clang++ -std=c++17 -o my_program my_file.cpp
./my_program  # Run the compiled program
clang++ -std=c++17 -o  server2 main1.cpp

clang++ -std=c++17 -pthread -Wall -Wextra \
    -o http_server \
    main.cpp Socket.cpp HttpRequest.cpp HttpResponse.cpp ChatHandler.cpp HttpServer.cpp


## next steps
- maintain state
- abstract those into classes.

### pseudocode
- Receive client connection request.
- Read client request data.
- Analyze the client request data, determine the request method (GET, POST, etc.), the requested URI and other request header information.
- Process the request and generate response data.
- Send the response data to the client.

but everything in abstractions then call the necessary abstraction: add multi threading.