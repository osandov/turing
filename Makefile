CPP := g++
CXXFLAGS := -Wall -g -std=c++11

SRCS := StateRegister.cpp \
    	StateParser.cpp \
    	Tape.cpp \
	TuringCurses.cpp \
	TuringMachine.cpp \
    	main.cpp

OBJS := $(SRCS:.cpp=.o)

turing : $(OBJS)
	$(CPP) $(CXXFLAGS) -o turing $(OBJS) -lcurses

%.o : %.cpp
	$(CPP) $(CXXFLAGS) -o $*.o -c $*.cpp

clean:
	rm -f turing $(OBJS)
