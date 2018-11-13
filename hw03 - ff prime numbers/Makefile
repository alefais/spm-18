CXX      = g++
CXXFLAGS = -Wall -std=c++11
INCLUDES = -I /usr/local/fastflow/
LDFLAGS  = -pthread
OPTFLAGS = -O3

DEPS = modules.h utils.h
OBJS = main.o modules.o utils.o

.PHONY: main clean cleanall

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) $(OPTFLAGS) -o $@ -c $<

main: $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) $(OPTFLAGS) -o $@ $^

clean:
	rm -f $(OBJS)

cleanall: clean
	rm -f *.o *~