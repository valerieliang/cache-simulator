CXX = g++
CXXFLAGS = -g -Wall -Wextra -pedantic -std=c++17

CXX_SRCS = cache_simulator.cpp main.cpp
CXX_OBJS = $(CXX_SRCS:.cpp=.o)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $*.o

csim : $(CXX_OBJS)
	$(CXX) -o $@ $(CXX_OBJS) 

clean :
	rm -f csim *.o depend.mak

# Generate header file dependencies
depend :
	$(CXX) $(CXXFLAGS) -M $(CXX_SRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak
