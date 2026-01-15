HttpServer* server_ptr = nullptr;
HttpServer* → this declares a pointer to an object of type HttpServer.

server_ptr → this is the name of the pointer.

= nullptr → it is initialized to point to nothing (no memory yet).


## nopw later 
HttpServer server(PORT);
server_ptr = &server; // adress of server saved in sever_ptr
server is a real object of type HttpServer created on the stack.

&server → takes the memory address of that object.

server_ptr = &server; → now the pointer points to the memory where the HttpServer object lives.


#
main.cpp


#### call stack?
* we create the http server object :  HttpServer server(PORT)
* server_socket_ os created as part of the class as a private member , an instance of class Socket.
* server.run()
* server_socket_.bind(port_)
we instantiate socket:
 Socket server_socket_; since socket has no arguments c++ will call  Socket::Socket()
 fd_ is iniitialised to -1 when the socket() is called. the value is changed within the instace of the class socket , in this case server-socket, hence the check here:
   if (!server_socket_.bind(port_))
because if it was -1  this block: if (fd_ < 0) return false;
would return false thus this :
 if (!server_socket_.bind(port_)) {
        std::cerr << "Failed to bind to port " << port_ << std::endl;
        return;
    }
    
will exit on the error: this prechecks happen before each step.

* then we listen : server_socket_.listen()
the backlog is the backlog defines the maximum number of pending connections the kernel should queue.

* set runnint to true indicate the server is running... not obvious at all..
* then now while the server remains running:
the accept() method returns  a socket, that socket is allocated to the variable client_socket.
we are calling the accept method against the server_socket whose file decsriptor value was set a value > 0 since it was succesful .
server_socket_.accept(); is used only for accepting connections, not for exchanging data.By default, socket operations are synchronous, or blocking, and accept will block until a connection is present on the queue.
accept takes the fd_ of the socket that was set for accepting connections with listen, and the address of the client.
then the second Socket () is called that aceepts the fd which is of the client socket and uts address
* in the background on the listening backlog, the kernel checks for clients that have completed the TCP handshake then allocates a new scoket and file descriptor and copies the address in the sockaddr_in.
* in the code after the connection is made we increase the connection count and call the handle client fucntion with the client_socket
* before proceeding note:When the TCP handshake completed, the kernel created an internal connection record identified by:(client IP, client port, server IP, server port).This 4-tuple uniquely identifies one TCP connection.then accept gives me a file descriptor (FD) that refers to one specific kernel TCP connection.The client socket is just a handle (identifier) to a specific TCP connection managed by the kernel.

* on this tcp connection , we call the receive method.. inside habdle client , first step is tor receive.it defines buffer_size which is how many bytes you are willing to read from the TCP receive queue in one recv() call.

* we define a buffer which is an array to hold the bytes we are receiving of size ,40964Kb,
* we declare an unsigned int type ssize_t n to hold the value recv returns, since its unsigned it can be negative for errors.
* recv() is a system call provided by the OS, as part of posix,t’s used to read data from a connected TCP socket. we pass the fd- the buffer and the buffer size, so we know how far along to read,then the flag, 0 to set a normal read no specail behaviour.

* recv can return:> 0:Number of bytes actually read from the socket. Could be less than buffer_size.or 0:The peer has closed the connection. No more data will arrive.or -1: An error occurred. Check errno for details.

* then we store that in a string called raw_request.

* substr(start, length) returns a substring of raw_request.is should slice the string but if the string is less tha 200, defined in the fxn call then returns the whole thing.and prints it to the std output.

* Then we create the http request,an instance of it. its parse method returns a boolean.

* within the request instance createdw we call the parse method, we pass the address of the stringraw_request

* std::istringstream stream(raw_request);  std::string line;Converts the string into a stream so you can read it line by line.

* read the line, if its empty its false, if the last character is "r" remove it.
* getline, reads chars from the string until it hits \n, then stores them in line,
* then we create anothe robject of type isstream called requestline that only has that particular line.

* >> This is the extraction operator for streams. It reads the next token (sequence of characters until a space) from the stream and stores it in the variable on the right.

* request_line >> method_str >> path_ >> version_;, we split the line into 3 and each is allovated into the variables : GET /chat HTTP/1.1

* The method is parsed through parseMethod function

* parseMethod: this just returns the method like Method::GET .

* Then we loop through while there are still lines and the line doesn't have just \r, we check if the line is empty, break also if line is not empty and is ending with r we remove the r

*  now for each line we find the colon then separate the header key and value, then check if the colon position is npos means no position then it skips the loop.

* Then find the first value thats not a tab or a space to remove leading spaces from the splitting of the line

* after we create the body:the stream is position on the first line of content since previously we said if line is empty break,

* The std::string(iterator_begin, iterator_end) constructor Copies everything between the two iterators into the string. so
std::string(
    std::istreambuf_iterator<char>(stream),
    std::istreambuf_iterator<char>()
);
“Read characters from stream until EOF and put them into a string”, since std::istreambuf_iterator<char>() This is a default-constructed iterator, which means:  end-of-stream marker

* then now the response: creating the Http resposne object that we send back by  passing the parsed request object:

* within routeRequest(request), from HttpResponse HttpServer::routeRequest(const HttpRequest& request), The function takes a reference to an HttpRequest which is a constant.then from the rquest we get the path, like /, or /chat the path on the server requested,

* with auto keyword the it variable type is allocated by the compiler at compile time.

* routes_ is a dictionary or map. the key is a string and the value is a fucntion that takes a http req and returns a response,  std::map<std::string, RouteHandler> routes_;
python equivalent bitte: 
    routes_ = {
        "/": RouteHandler,
    }

*  std::function is used to wrap a particular function or any other callable object such as a lambda, or function object. It is generally used to write generic code where we need to pass functions as arguments in another function (callbacks).defines as :
std::function< rtype (atype...)> name();
        where: 
            name: Name of the wrapper.
            atype: Types of the arguments that function takes.
            rtype: Return type of the function you want to store.

* using in C++ creates a type alias.RouteHandler = new name for std::function<HttpResponse(const HttpRequest&)>

Everywhere you see RouteHandler, it means a callable that takes a const HttpRequest& and returns an HttpResponse.

* recall A lambda in C++ is basically an anonymous function.so a fucntion without a name.

* this, gives the lambda acesss to the current object its called in for this insatnce the http server and its memebers

* fro each response, we have a sapearte moduld efor handling the chat.