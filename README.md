# flatbuffersxredis
Redis module that allows you to update flatbuffers stored at redis keys

must compile flatbuffers as shared library and make it run-time visible to the linker! add this command when compiling flatbuffers -> cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DFLATBUFFERS_BUILD_SHAREDLIB=yes

command to compile the module:
g++ -lflatbuffers -fPIC -fconcepts -std=gnu++17 module.c -shared -o module.so
