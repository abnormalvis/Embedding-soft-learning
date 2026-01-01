
CC=gcc

TARGET=dS_demo

SRCS=$(wildcard *.c)     # main.c dStruct.c 
OBJS=$(patsubst %.c,%.o,$(SRCS)) 	# main.o  dStruct.o  

all:$(TARGET) clean

$(TARGET):$(OBJS)
	$(CC)  $(OBJS) -o $(TARGET) 
	@./$(TARGET)

$(OBJS):%.o:%.c
	$(CC) -c $^ -o $@ 
        
clean: 
	rm *.o $(TARGET) 




