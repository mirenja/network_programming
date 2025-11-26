CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
TARGET = http_server

SOURCES = main.cpp Socket.cpp HttpRequest.cpp HttpResponse.cpp ChatHandler.cpp HttpServer.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
