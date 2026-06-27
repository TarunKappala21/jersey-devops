CXX      = g++
CXXFLAGS = -std=c++17 -I src -Wall -Wextra
LDFLAGS  = -lpthread

SHARED_SRCS = src/inventory/Inventory.cpp \
              src/customer/Fan.cpp \
              src/order/Order.cpp

# CLI target
cli: $(SHARED_SRCS) src/main.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp $(SHARED_SRCS) -o jersey-store $(LDFLAGS)

# REST API server target
server: $(SHARED_SRCS) src/server.cpp
	$(CXX) $(CXXFLAGS) src/server.cpp $(SHARED_SRCS) -o jersey-server $(LDFLAGS)

# Build both
all: cli server

clean:
	rm -f jersey-store jersey-server

run-cli: cli
	./jersey-store

run-server: server
	./jersey-server

.PHONY: all cli server clean run-cli run-server
