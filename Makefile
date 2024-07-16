# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++17 -I D:/boost_1_85_0

# Linker flags
LDFLAGS = 

# Boost libraries
BOOST_LIBS =

# Source files
SOURCES = main.cpp ConfigurationSpace.cpp Lattice.cpp ModuleManager.cpp MoveManager.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
EXECUTABLE = main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@ $(BOOST_LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
