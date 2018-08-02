#include "redismodule.h"
//#include <unordered_map>
//#include <vector>
//#include "flatbuffers/flatbuffers.h"

//std::vector<uint8_t> workingReflectionBuffer;
//std::unordered_map<const char *, std::string> schemaBinaryMap;

// update scalar (no reflection)
// update container type (vector / string, requires reflection)

// FLATBUFFERS.UPDATE+SCALAR schema key field[.subfield] newValue 
// only support single nested 

/*
int UpdateScalar(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) 
{



}
*/

// g++ -fPIC -std=gnu++17 -c -o module.o module.h
// ld -o module.so module.o -shared -Bsymbolic -lc

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

  if (RedisModule_Init(ctx, "flatbuffersxredis", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) return REDISMODULE_ERR;
  
  for (int a = 0; a < argc; a++) {

  		size_t argumentLength;
		const char *argument = RedisModule_StringPtrLen(argv[a], &argumentLength);
		printf("argument = %s\n", argument);
  }

  //workingReflectionBuffer.reserve(2500U);


  //if (RedisModule_CreateCommand(ctx, "nestedTables.getKey", GetKey, "readonly") == REDISMODULE_ERR) return REDISMODULE_ERR;


  //if (RedisModule_CreateCommand(ctx, "FLATBUFFERS.UPDATE+SCALAR", UpdateScalar, "write") == REDISMODULE_ERR) return REDISMODULE_ERR;

  return REDISMODULE_OK;
}