CXX      = g++-8
CXXFLAGS = -Wall -Wextra -std=c++11 \
           # -Og -fsanitize=thread -fstack-check -fstack-protector-strong \
           # -fno-omit-frame-pointer
           # -g

LDFLAGS  = -pthread
OPTFLAGS = -O3

DEPS = scan.h
OBJS = main.o

.PHONY: main clean cleanall

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OPTFLAGS) -c -o $@ $<

main: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OPTFLAGS) -o $@ $^

clean:
	rm -f $(OBJS)

cleanall: clean
	\rm -f *.o *~
