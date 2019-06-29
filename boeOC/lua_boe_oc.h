/*
** BOE OC algo library, implemented by JCDZ.
** Copyright JCDZ 2019
*/


#ifdef _WIN32
  #define OC_EXPORT __declspec (dllexport)
#else
  #define OC_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

OC_EXPORT  int luaopen_boeOC (lua_State *L);

#ifdef __cplusplus
}
#endif
