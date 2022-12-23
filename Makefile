CC = g++ 
CFLAGS = -I/usr/local/include/opencv4/ -L/usr/local/lib -lopencv_core -fopenmp 
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=)
all: $(OBJS)

.cpp: 
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: clean

clean:
	$(RM) $(OBJS) $(TARGET)

