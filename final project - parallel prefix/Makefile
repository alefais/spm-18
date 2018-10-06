#CXX      = g++
#CXXFLAGS = -std=c++11 -Wall
CXX      = g++-8
CXXFLAGS = -Wall
INCLUDES =
LDFLAGS  = -pthread
OPTFLAGS = -O3

DEPS = scan.h test.h utils.h
OBJS = main.o utils.o

.PHONY: all clean cleanall

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) $(OPTFLAGS) -c -o $@ $<

scan : $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) $(OPTFLAGS) -o $@ $^

clean :
	rm -f $(OBJS)

cleanall : clean
	\rm -f *.o *~
