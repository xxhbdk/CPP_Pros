
#luaivpLib
set(luaIVP_srcs
luaparser.cpp
alginterface.cpp
utils.cpp
algbase.cpp
algiw.cpp
)

set(luaIVP_incs
luaparser.h
alginterface.h
utils.h
algbase.h
algiw.h
)

add_library(IVPAlg SHARED  ${luaIVP_srcs} ${luaIVP_incs})

#set (CMAKE_VS_PLATFORM_TOOLSET )

target_compile_definitions(IVPAlg PUBLIC "JC_GAMMA_DLL_EXPORTS")

target_link_libraries(IVPAlg JCLuaScript)
if (WIN32)
    target_compile_options(IVPAlg PUBLIC "/utf-8")
endif()

target_link_directories(IVPAlg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../JCluascript/lib)

target_include_directories(IVPAlg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../JCluascript/inc)
target_include_directories(IVPAlg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../eigen3)

