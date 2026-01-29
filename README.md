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

### everything in abstractions then call the necessary abstraction

### add multi threading

### how does the choice of protocol affect the way an engineer architects and builds their software? 

###  Which network and software choices are “best practice” for transferring large amounts of data that need to arrive perfectly intact, or for real-time streaming of a multi-person video call, or for sending information from an array of IoT devices? 

### How does a software engineer design for a network connection that may be unreliable?

### Which protocols are useful for software that is running on a device not connected to the internet?


### the concepts and their practical implementation 

##### Relevant network protocols

#####  Network models such as TCP/IP or OSI

##### Operating system APIs such as Berkeley sockets

##### Best-practices for building software across a network, such as remote procedure call (RPC)


### statemagement output file : ./http_server_with_state
clang++ -std=c++17 -pthread -Wall -Wextra \
    -o http_server_with_state \
    main.cpp Socket.cpp HttpRequest.cpp HttpResponse.cpp ChatHandler.cpp HttpServer.cpp StateManager.cpp

### added session management SessionManager.cpp

clang++ -std=c++17 -pthread -Wall -Wextra \
    -o http_server_with_state \
    main.cpp Socket.cpp HttpRequest.cpp HttpResponse.cpp ChatHandler.cpp HttpServer.cpp StateManager.cpp SessionManager.cpp