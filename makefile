EXECS=scheduler_os

# Use g++ for compiling C++ programs
CC=g++

# Replace with the name of your C++ source code file.
MYFILE=project.cpp

all: $(EXECS)

$(EXECS): $(MYFILE)
	$(CC) -o $(EXECS) $(MYFILE) -lpthread -lcurl -std=c++11

clean:
	rm -f $(EXECS)