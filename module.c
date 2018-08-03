// update scalar (no reflection)
// update container type (vector / string, requires reflection)

// FLATBUFFERS.UPDATE+SCALAR schema key field[.subfield] newValue 
// only support single nested 

#include "redismodule.h"
#include <vector>
#include "flatbuffers/reflection.h"
#include "flatbuffers/util.h"

std::vector<uint8_t> resizingbuf;

/*

enum BaseType {
  None = 0,
  UType = 1,
  Bool = 2,
  Byte = 3,
  UByte = 4,
  Short = 5,
  UShort = 6,
  Int = 7,
  UInt = 8,
  Long = 9,
  ULong = 10,
  Float = 11,
  Double = 12,
  String = 13,
  Vector = 14,
  Obj = 15,
  Union = 16
};
*/

// n t >= reflection::UType && t <= reflection::Double;
// static size_t sizes[] = { 0, 1, 1, 1, 1, 2, 2, 4, 4, 8, 8, 4, 8, 4, 4, 4, 4 };

// bool | byte | ubyte | short | ushort | int | uint | float | long | ulong | double | int8 | uint8 | int16 | uint16 | int32 | uint32| int64 | uint64 | float32 | float64 | string 

//8 bit: byte (int8), ubyte (uint8), bool

//16 bit: short (int16), ushort (uint16)

//32 bit: int (int32), uint (uint32), float (float32)

//64 bit: long (int64), ulong (uint64), double (float64)

static bool switchScalar(flatbuffers::Table *root, const reflection::Field &workingField, const char *newValue) {

   reflection::BaseType type = workingField.type()->base_type();

   switch (type) {

      case reflection::Bool: {

         if (strcmp(newValue, "1") == 0 || strcmp(newValue, "true") == 0) {

            flatbuffers::SetField<bool>(root, workingField, true);
         }
         else if (strcmp(newValue, "0") == 0 || strcmp(newValue, "false") == 0) {

            flatbuffers::SetField<bool>(root, workingField, false);
         }
         
         break;
      }
      case reflection::Byte: {

         break;
      }
      case reflection::UByte: {

  
         break;
      }
      case reflection::Short: {

       
         break;
      }
      case reflection::UShort: {

        
         break;
      }
      case reflection::Int: {

         flatbuffers::SetField<int32_t>(root, workingField, std::stoi(newValue));
         break;
      }
      case reflection::UInt: {

         flatbuffers::SetField<uint32_t>(root, workingField, std::stoi(newValue));
         break;
      }
      case reflection::Long: {

         flatbuffers::SetField<int64_t>(root, workingField, std::stol(newValue));
         break;
      }
      case reflection::ULong: {

         flatbuffers::SetField<uint64_t>(root, workingField, std::stoul(newValue));
         break;
      }
      case reflection::Float: {

         flatbuffers::SetField<float>(root, workingField, std::stof(newValue));
         break;
      }
      case reflection::Double: {

         flatbuffers::SetField<double>(root, workingField, std::stod(newValue));
         break;
      }
      default: {

         return false;
      }
   }

   return true;
}

static int flatbufferxredis_update(RedisModuleCtx *ctx, RedisModuleString **argv, int argc, auto updateLambda)
{
   int result = REDISMODULE_ERR;

   if (RedisModuleKey *schemaKey = (RedisModuleKey *)RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ); schemaKey) {

      if (RedisModuleKey *flatbufferKey = (RedisModuleKey *)RedisModule_OpenKey(ctx, argv[2], REDISMODULE_WRITE); flatbufferKey) {
    
         int result;
         size_t bufferLength;
         char *buffer = RedisModule_StringDMA(flatbufferKey, &bufferLength, REDISMODULE_WRITE);
      
         size_t schemaBufferLength;
         char *schemaBuffer = RedisModule_StringDMA(schemaKey, &schemaBufferLength, REDISMODULE_READ);
         const reflection::Schema *schema = reflection::GetSchema((const void *)schemaBuffer);
         const reflection::Object *root_table = schema->root_table();
         const flatbuffers::Vector<flatbuffers::Offset<reflection::Field>> *fields = root_table->fields();
      
         if (auto possibleField = fields->LookupByKey(RedisModule_StringPtrLen(argv[3], NULL)); possibleField) {

            if (RedisModuleString *newBuffer = updateLambda(schema, (uint8_t *)buffer, bufferLength, *possibleField, result); newBuffer) {

               (void)RedisModule_StringSet(flatbufferKey, newBuffer);
               RedisModule_FreeString(ctx, newBuffer);
            }
         }
         else RedisModule_ReplyWithError(ctx, "ERR No field by that name.");

         RedisModule_CloseKey(flatbufferKey);
      }
      else RedisModule_ReplyWithError(ctx, "ERR No flatbuffer at that key.");
  
      RedisModule_CloseKey(schemaKey);
   }
   else RedisModule_ReplyWithError(ctx, "ERR No schema at that key.");

   return result;
}

// FB.UPDATE+SCALAR schema key field newValue
static int flatbufferxredis_updateScalar(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) 
{
   return flatbufferxredis_update(ctx, argv, argc, [&] (const reflection::Schema *schema, uint8_t *buffer, size_t bufferLength, const reflection::Field &workingField, int& result) -> RedisModuleString * {

      if (switchScalar(flatbuffers::GetAnyRoot((uint8_t *)buffer), workingField, RedisModule_StringPtrLen(argv[4], NULL))) {

         RedisModule_ReplyWithLongLong(ctx, 1);
         result = REDISMODULE_OK;

         return RedisModule_CreateString(ctx, (const char *)buffer, bufferLength);
      }
      else {
         RedisModule_ReplyWithError(ctx, "ERR Provided field is not a scalar type.");
         return NULL;
      }
   });
}

// FB.UPDATE+STRING schema key field newValue
static int flatbufferxredis_updateString(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) 
{
   return flatbufferxredis_update(ctx, argv, argc, [&] (const reflection::Schema *schema, uint8_t *buffer, size_t bufferLength, const reflection::Field &workingField, int& result) -> RedisModuleString * {

         if (workingField.type()->base_type() == reflection::String) {

            if (resizingbuf.size() < bufferLength) resizingbuf.reserve(bufferLength);

            (void)memset(resizingbuf.data(), 0, resizingbuf.size());
            (void)memcpy(resizingbuf.data(), buffer, bufferLength);

            auto rrootObj = flatbuffers::piv(flatbuffers::GetAnyRoot(flatbuffers::vector_data(resizingbuf)), resizingbuf);
            flatbuffers::pointer_inside_vector<flatbuffers::Table, uint8_t> *rroot = &rrootObj;

            SetString(*schema, RedisModule_StringPtrLen(argv[4], NULL), GetFieldS(**(*rroot), workingField), &resizingbuf);

            RedisModule_ReplyWithLongLong(ctx, 1);
            result = REDISMODULE_OK;

            return RedisModule_CreateString(ctx, (const char *)resizingbuf.data(), resizingbuf.size());
         }
         else {
            RedisModule_ReplyWithError(ctx, "ERR Provided field is not of string type.");
            return NULL;
         }
      });
}

//  g++ -lflatbuffers -fPIC -fconcepts -std=gnu++17 module.c -shared -o module.so
//	export LD_LIBRARY_PATH=/usr/local/lib64 && /root/redis/src/redis-server --loadmodule ./module.so /root/nametag.server.v1x0x1/flatbuffersxredis/test/EveryScalarTest.bfbs

//  export LD_LIBRARY_PATH=/usr/local/lib64 && /root/redis/src/redis-server --loadmodule ./module.so /root/nametag.server.v1x0x1/base/nametag.flatDatas.v0.bfbs

//  /root/redis/src/redis-server --loadmodule ./module.so /root/nametag.server.v1x0x1/base/nametag.flatDatas.v0.bfbs
//  /root/redis/src/redis-server --loadmodule ./module.so /path/to/schema1.bfbs /path/to/schema2.bfbs /path/to/schema3.bfbs

extern "C" {
   
int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {

  if (RedisModule_Init(ctx, "flatbuffersxredis", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) return REDISMODULE_ERR;
  
  for (int a = 0; a < argc; a++) {

      printf("argument = %s\n", RedisModule_StringPtrLen(argv[a], NULL));
  }

  resizingbuf.reserve(2500U);

  //if (RedisModule_CreateCommand(ctx, "nestedTables.getKey", GetKey, "readonly") == REDISMODULE_ERR) return REDISMODULE_ERR;

  if (RedisModule_CreateCommand(ctx, "FB.UPDATE+SCALAR", flatbufferxredis_updateScalar, "write", 0, 0, 0) == REDISMODULE_ERR) return REDISMODULE_ERR;
  if (RedisModule_CreateCommand(ctx, "FB.UPDATE+STRING", flatbufferxredis_updateString, "write", 0, 0, 0) == REDISMODULE_ERR) return REDISMODULE_ERR;

  return REDISMODULE_OK;
}
}