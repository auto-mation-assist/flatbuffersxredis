SHOBJ_CFLAGS ?= -fno-common -g -ggdb
SHOBJ_LDFLAGS ?= -shared -Bsymbolic
CXXFLAGS = -Wall -g -fPIC -lc -lm -Og -std=gnu17
CXX=g++

all: flatbuffersxredis.so

flatbuffersxredis.so: flatbuffersxredis.o
    $(LD) -o $@ flatbuffersxredis.o $(SHOBJ_LDFLAGS) $(LIBS) -lc 

clean:
    rm -rf *.xo *.so *.o


#Compile it: gcc -fPIC -std=gnu99 -c -o example.o example.c

#Link it: ld -o example.so example.o -shared -Bsymbolic -lc

#Load it: ../redis/src/redis-server --loadmodule ./example.so

#Test it: ../redis/src/redis-cli example.echo tango