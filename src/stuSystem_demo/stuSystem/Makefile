CURDIR=$(shell pwd)
INCDIR:=$(CURDIR)/inc
SRCDIR:=$(CURDIR)/src

CPPFLAGS = -I$(INCDIR)

CC = gcc
TARGET = main

SRCS = $(wildcard *.c)
SRCS += $(foreach dir,$(SRCDIR),$(wildcard $(dir)/*.c))
OBJS = $(patsubst %.c,%.o, $(SRCS))


all: $(TARGET) 
$(TARGET):$(OBJS)
	$(CC)  $(OBJS) -o $@ 

%.o:%.c
	$(CC) -c $< -o $@ $(CPPFLAGS)

clean:
	rm -rf ./*.o  
	rm ./main
	make clean -C src 
