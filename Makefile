CC = g++
CFLAGS =  -O3 -I/usr/local/include/opencv4/ -L/usr/local/lib -lopencv_core -fopenmp -mavx2 
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=)
all: $(OBJS)

.cpp: 
	$(CC) -DSIZE=512 -DITER=100 $^ -o $@ $(CFLAGS)
	$(CC) -DSIZE=512 -DITER=100 $^ -S -g $(CFLAGS)

.PHONY: clean

clean:
	$(RM) $(OBJS) *.s

