#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char *code1 = "\
#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <string.h>\n\
\n\
#include \"lua.h\"\n\
#include \"lualib.h\"\n\
#include \"lauxlib.h\"\n\
\n\
static const unsigned char luacode[] = {\n";

static const char *code2 = "\
};\n\
\n\
static long luacode_size = ";

static const char *code3 = ";\n\n\
int main(int argc, char *argv[])\n\
{\n\
  lua_State *L;\n\
  int i;\n\
\n\
  lua_executable_dir(argv[0]);\n\
  L = luaL_newstate();\n\
\n\
  if(L == NULL)\n\
  {\n\
    printf(\"error: not enough memory for Lua state\\n\");\n\
    return -1;\n\
  }\n\
\n\
  lua_gc(L, LUA_GCSTOP, 0);\n\
  luaL_openlibs(L);\n\
  lua_gc(L, LUA_GCRESTART, 0);\n\
\n\
  lua_createtable(L, argc-1, 1);\n\
  for(i = 0; i < argc; i++)\n\
  {\n\
    lua_pushstring(L, argv[i]);\n\
    lua_rawseti(L, -2, i);\n\
  }\n\
  lua_setglobal(L, \"arg\");\n\
\n\
  if(luaL_loadbuffer(L, (const char*)luacode, luacode_size, NULL))\n\
  {\n\
    const char *msg = lua_tostring(L, -1);\n\
    printf(\"error: %s\\n\", msg);\n\
    return -1;\n\
  }\n\
\n\
  if(lua_pcall(L, 0, 0, 0))\n\
  {\n\
    const char *msg = lua_tostring(L, -1);\n\
    printf(\"error: %s\\n\", msg);\n\
    return -1;\n\
  }\n\
\n\
  lua_close(L);\n\
  return 0;\n\
}\n\
";

int main(int argc, char *argv[])
{
  FILE *flua;
  FILE *fc;
  char *luacode;
  size_t size;
  size_t i, idx;

  if(argc != 3)
  {
    printf("two arguments expected: %s [lua file] [c file]\n", argv[0]);
    exit(-1);
  }

  if(!(flua = fopen(argv[1], "rb")))
  {
    printf("cannot open file %s for reading\n", argv[1]);
    exit(-1);
  }

  if(!(fc = fopen(argv[2], "wb")))
  {
    printf("cannot open file %s for writing\n", argv[2]);
    exit(-1);
  }

  fwrite(code1, 1, strlen(code1), fc);

  fseek(flua, 0L, SEEK_END);
  size = ftell(flua);
  fseek(flua, 0L, SEEK_SET);
  luacode = malloc(size+1);
  fread(luacode, 1, size, flua);
  luacode[size] = '\0';

  idx = 0;
  for(i = 0; i < size; i++)
  {
    fprintf(fc, "0x%.2x", luacode[i]);
    if(i != size-1)
      fprintf(fc, ",");

    idx++;
    if(idx == 12)
    {
      fprintf(fc, "\n");
      idx = 0;
    }
  }

  fwrite(code2, 1, strlen(code2), fc);
  fprintf(fc, "%ld", size);
  fwrite(code3, 1, strlen(code3), fc);

  free(luacode);
  fclose(flua);
  fclose(fc);

  return 0;
}
