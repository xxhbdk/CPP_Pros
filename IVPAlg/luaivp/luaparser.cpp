/*
** IVPAlg algo
** Copyright JCDZ 2019
**
** IVP algo library, implemented by JCDZ.
** This library offers these functions:
**   IVP.ivpInit (id, gammaData)
**   IVP.ivpWriteCSV (id)
**   IVP.ivpPredInitialRGB (id, inPut, outPut)
**   IVP.ivpFeedOptimalRGB (id, para)
**   IVP.getAlgVesion ()
*/

#include "lua.hpp"
#include "luaparser.h"
#include "jcluascript.h"
#include "alginterface.h"

#define IVP_VERSION "1.0.0"
#define IVP_LIBNAME "IVPAlg"

static void parseInput(std::map<std::string, std::string>& luaInput, GammaAlg::GammaInputInfoReg& input)
{
    input.count_ = (luaInput.find("COUNT") != luaInput.end()) ? std::stoi(luaInput["COUNT"]) : -1;
    input.bandNo_ = (luaInput.find("BANDNO") != luaInput.end()) ? std::stoi(luaInput["BANDNO"]) : -1;
    input.grayValue_ = (luaInput.find("GRAY") != luaInput.end()) ? std::stoi(luaInput["GRAY"]) : -1;
    input.bindIndex_ = (luaInput.find("GRAYINDEX") != luaInput.end()) ? std::stoi(luaInput["GRAYINDEX"]) : -1;
    input.algMode_ = (luaInput.find("ALGMODE") != luaInput.end()) ? std::stoi(luaInput["ALGMODE"]) : -1;
    input.regValue_.rRegValue_ = (luaInput.find("REGR") != luaInput.end()) ? std::stoi(luaInput["REGR"]) : -1;
    input.regValue_.gRegValue_ = (luaInput.find("REGG") != luaInput.end()) ? std::stoi(luaInput["REGG"]) : -1;
    input.regValue_.bRegValue_ = (luaInput.find("REGB") != luaInput.end()) ? std::stoi(luaInput["REGB"]) : -1;
    input.volValue_.rVolValue_ = (luaInput.find("VOLR") != luaInput.end()) ? std::stod(luaInput["VOLR"]) : -1;
    input.volValue_.gVolValue_ = (luaInput.find("VOLG") != luaInput.end()) ? std::stod(luaInput["VOLG"]) : -1;
    input.volValue_.bVolValue_ = (luaInput.find("VOLB") != luaInput.end()) ? std::stod(luaInput["VOLB"]) : -1;
    input.realxylv_.x_ = (luaInput.find("x") != luaInput.end()) ? std::stod(luaInput["x"]) : -1;
    input.realxylv_.y_ = (luaInput.find("y") != luaInput.end()) ? std::stod(luaInput["y"]) : -1;
    input.realxylv_.lv_ = (luaInput.find("Lv") != luaInput.end()) ? std::stod(luaInput["Lv"]) : -1;
}


static void deparseOutput(GammaAlg::GammaInputInfoReg& output, std::map<std::string, std::string>& outputToLua)
{
    outputToLua["REGR"] = std::to_string(output.regValue_.rRegValue_);
    outputToLua["REGG"] = std::to_string(output.regValue_.gRegValue_);
    outputToLua["REGB"] = std::to_string(output.regValue_.bRegValue_);
    outputToLua["VOLR"] = std::to_string(output.volValue_.rVolValue_);
    outputToLua["VOLG"] = std::to_string(output.volValue_.gVolValue_);
    outputToLua["VOLB"] = std::to_string(output.volValue_.bVolValue_);
}


static int ivpInit(lua_State* L)
{
    char id;
    std::map<std::string, std::string> luaInput;
    lua_op_t<char>::lua_to_value(L, 1, id);
    lua_op_t<map<string, string>>::lua_to_value(L, 2, luaInput);

    char tmpID[2]{ id, '\0' };
    if (0 == GammaAlg::ivpInit(tmpID, &luaInput))
    {
        lua_pushboolean(L, 1);
    }
    else
    {
        lua_pushboolean(L, 0);
    }

    return 0;
}


static int ivpWriteCSV(lua_State* L)
{
    char id;
    std::map<std::string, std::string> luaInput;
    lua_op_t<char>::lua_to_value(L, 1, id);
    lua_op_t<map<string, string>>::lua_to_value(L, 2, luaInput);

    char tmpID[2]{ id, '\0' };
    if (0 == GammaAlg::ivpWriteCSV(tmpID, luaInput["filename"].c_str()))
    {
        lua_pushboolean(L, 1);
    }
    else
    {
        lua_pushboolean(L, 0);
    }

    return 0;
}


static int ivpPredInitialRGB(lua_State* L)
{
    char id;
    std::map<std::string, std::string> luaInput;
    lua_op_t<char>::lua_to_value(L, 1, id);
    lua_op_t<map<string, string>>::lua_to_value(L, 2, luaInput);

    GammaAlg::GammaInputInfoReg luaInputTra;
    parseInput(luaInput, luaInputTra);
    std::map<std::string, std::string> outputToLua;

    char tmpID[2]{ id, '\0' };
    if (0 == GammaAlg::ivpPredInitialRGB(tmpID, &luaInputTra))
    {
        deparseOutput(luaInputTra, outputToLua);
    }

    lua_op_t<std::map<std::string, std::string>>::push_stack(L, outputToLua);

    return 1;
}


static int ivpFeedOptimalRGB(lua_State* L)
{
    char id;
    std::map<std::string, std::string> luaInput;
    lua_op_t<char>::lua_to_value(L, 1, id);
    lua_op_t<map<string, string>>::lua_to_value(L, 2, luaInput);

    GammaAlg::GammaInputInfoReg luaInputTra;
    parseInput(luaInput, luaInputTra);

    char tmpID[2]{ id, '\0' };
    if (0 == GammaAlg::ivpFeedOptimalRGB(tmpID, &luaInputTra))
    {
        lua_pushboolean(L, 1);
    }
    else
    {
        lua_pushboolean(L, 0);
    }

    return 0;
}


static int getAlgVesion(lua_State* L)
{
    lua_pushstring(L, IVP_VERSION);
    return 0;
}


static const struct luaL_Reg IVPlib[] = {
    {"ivpInit", ivpInit},
    {"ivpWriteCSV", ivpWriteCSV},
    {"ivpPredInitialRGB", ivpPredInitialRGB},
    {"ivpFeedOptimalRGB", ivpFeedOptimalRGB},
    {"getAlgVesion", getAlgVesion},
    {NULL, NULL},
};


/*
** Assumes the table is on top of the stack.
*/
static void set_info(lua_State* L)
{
    lua_pushliteral(L, "Copyright (C) 2019 JCDZ");
    lua_setfield(L, -2, "_COPYRIGHT");
    lua_pushliteral(L, "IVP algorithm library, implemented by JCDZ.");
    lua_setfield(L, -2, "_DESCRIPTION");
    lua_pushliteral(L, "IVPAlg " IVP_VERSION);
    lua_setfield(L, -2, "_VERSION");
}


IVP_EXPORT int luaopen_IVPAlg(lua_State* L)
{
    luaL_newlib(L, IVPlib);
    lua_pushvalue(L, -1);
    lua_setglobal(L, IVP_LIBNAME);
    set_info(L);

    GammaAlg::gammaRegisterFunGammaPrintMsg(luaPrintGammaMessage);
    GammaAlg::gammaRegisterFunCreateLogFile(luaCreateLogFile);
    return 1;
}
