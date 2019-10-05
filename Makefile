EXE 		= x80806_64_test
CXX 		= g++
CXXFLAGS 	= -Wall -std=c++11 -O0 -g 
LDFLAGS 	= 

BIN			= bin
SOURCES		= $(wildcard *.cpp)
OBJECTS		= $(SOURCES:%.cpp=$(BIN)/%.o)

.PHONY: all clean

all: $(BIN) $(BIN)/$(EXE)

clean:
	rm -rf bin

$(BIN):
	mkdir -p $@

$(BIN)/$(EXE): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)
	@echo "Build successful"

$(OBJECTS): $(BIN)/%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)