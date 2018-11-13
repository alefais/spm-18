CXX      = g++
CXXFLAGS = -Wall -std=c++17
INCLUDES = -I /usr/local/fastflow/
LDFLAGS  = -pthread
OPTFLAGS = -O3

DEPS = scan.h
OBJS = main.o

.PHONY: main clean cleanall

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) $(OPTFLAGS) -o $@ -c $<

main: $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) $(OPTFLAGS) -o $@ $^

clean:
	rm -f $(OBJS)

cleanall: clean
	\rm -f *.o *~