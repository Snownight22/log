CC = gcc
DEBUG = -g
INC = -I./include/ 
LIBS = -lpthread 
CFLAGS = -std=c11 -Wall -Werror
#-D__FILENAME__='"$(subst $(dir $<), ,$<)"'
SHARED = -fpic -shared


SRC_DIR = .        \
		  config   \
		  core

SRC = $(foreach dir,$(SRC_DIR), $(wildcard $(dir)/*.c))

BIN = logmain

SHAREDBIN = liblog.so

all:$(SRC)
	$(CC) $(SRC) $(CFLAGS) $(DEBUG) $(INC) $(LIBS) -o $(BIN)

shared:$(SRC)
	$(CC) $(SRC) $(CFLAGS) $(INC) $(LIBS) $(SHARED) -o $(SHAREDBIN)


.PHONY:

clean:
	rm -rf $(BIN) $(SHAREDBIN)
