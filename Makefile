CC = g++ 
INCLUDE = -I/usr/local/include/opencv4/ -L/usr/local/lib -lopencv_core -fopenmp -mavx2
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=)

all: $(OBJS)

mm: mm.cpp
	$(CC) $^ -o $@ $(INCLUDE)

.PHONY: clean

clean:
	$(RM) $(OBJS) $(TARGET)

