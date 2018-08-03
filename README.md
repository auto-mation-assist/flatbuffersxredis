# flatbuffersxredis
Redis module that allows you to update flatbuffers stored at redis keys

must compile flatbuffers as shared library and make it run-time visible to the linker! add this command when compiling flatbuffers:

    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DFLATBUFFERS_BUILD_SHAREDLIB=yes

command to compile the module:

    g++ -lflatbuffers -fPIC -fconcepts -std=gnu++17 module.c -shared -o module.so
    
module can be loaded into redis like such:

    redis-server --loadmodule ./module.so 
    
produce a binary version of your flatbuffer schema like so:

    flatc -b yourSchema.fbs --schema
    
then store the binary file in redis at some key.
    
schema = key at which binary schema is stored
key = key at which flatbuffer you want to update is stored
field = the name of the field you want to update
newValue = the updated value you want to overwrite the field with

command to update scalar field:

    FB.UPDATE+SCALAR schema key field newValue
    
command to update string field:

    FB.UPDATE+STRING schema key field newValue
    

