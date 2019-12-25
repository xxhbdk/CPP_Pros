#pragma once

/*
** IVP algo library, implemented by JCDZ.
** Copyright JCDZ 2019
*/


#ifdef _WIN32
  #define IVP_EXPORT __declspec (dllexport)
#else
  #define IVP_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

IVP_EXPORT int luaopen_IVPAlg(lua_State *L);

#ifdef __cplusplus
}
#endif
