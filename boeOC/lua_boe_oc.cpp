/*
** Boe oc algo
** Copyright JCDZ 2019
**
** BOE OC algo library, implemented by JCDZ.
** This library offers these functions:
**   OC.gammaInit (id, gammaData)
**   OC.gammaUnInit (id)
**   OC.gammaProgress (id, inPut, outPut)
**   OC.gammaSetPara (id, para)
**   OC.getAlgVesion ()
*/

#include "lua.hpp"
#include "lua_boe_oc.h"
#include "boe_oc.h"
#include "jcluascript.h"
#include "gamma_algo_interface.h"

#define OC_VERSION "1.0.0"
#define OC_LIBNAME "boeOC"


using namespace GammaAlgo;

static void parseConfig(std::map<std::string, std::string>& config, GammaAlgo::GammaInitData& initData) {
    for (int i = 0; i < initData.maxbindnum_; ++i) {
        std::string idx = std::to_string(i + 1);
        if (!config["W_Gray_" + idx].empty())
            initData.pGammaAllData_[i].gray_ = std::atoi(config["W_Gray_" + idx].c_str());
        if (!config["Band_Index_" + idx].empty())
            initData.pGammaAllData_[i].bandno_ = std::atoi(config["Band_Index_" + idx].c_str());
        initData.pGammaAllData_[i].grayindex_ = i + 1;
        if (!config["W_x_" + idx].empty())
            initData.pGammaAllData_[i].errorConfig_.targetxylv_.x_ = std::atof(config["W_x_" + idx].c_str());
        if (!config["W_y_" + idx].empty())
            initData.pGammaAllData_[i].errorConfig_.targetxylv_.y_ = std::atof(config["W_y_" + idx].c_str());
        if (!config["W_lv_" + idx].empty())
            initData.pGammaAllData_[i].errorConfig_.targetxylv_.lv_ = std::atof(config["W_lv_" + idx].c_str());
        if (!config["W_xErrorLeft_" + idx].empty())
            initData.pGammaAllData_[i].errorConfig_.xLeftError_ = std::atof(config["W_xErrorLeft_" + idx].c_str());
        if (!config["W_xErrorRight_" + idx].empty())
            initData.pGammaAllData_[i].errorConfig_.xRightError_ = std::atof(config["W_xErrorRight_" + idx].c_str());
        if (!config["W_yErrorLeft_" + idx].empty())
            initData.pGammaAllData_[i].errorConfig_.yLeftError_ = std::atof(config["W_yErrorLeft_" + idx].c_str());
        if (!config["W_yErrorRight_" + idx].empty())
            initData.pGammaAllData_[i].errorConfig_.yRightError_ = std::atof(config["W_yErrorRight_" + idx].c_str());
        if (!config["W_lvErrorLeft_" + idx].empty())
            initData.pGammaAllData_[i].errorConfig_.lvLeftError_ = std::atof(config["W_lvErrorLeft_" + idx].c_str());
        if (!config["W_lvErrorRight_" + idx].empty())
            initData.pGammaAllData_[i].errorConfig_.lvRightError_ = std::atof(config["W_lvErrorRight_" + idx].c_str());
        if (!config["R_FirstValue_" + idx].empty())
            initData.pGammaAllData_[i].regConfig_.rRegConfig_.regValue_ = std::atoi(config["R_FirstValue_" + idx].c_str());
        if (!config["G_FirstValue_" + idx].empty())
            initData.pGammaAllData_[i].regConfig_.gRegConfig_.regValue_ = std::atoi(config["G_FirstValue_" + idx].c_str());
        if (!config["B_FirstValue_" + idx].empty())
            initData.pGammaAllData_[i].regConfig_.bRegConfig_.regValue_ = std::atoi(config["B_FirstValue_" + idx].c_str());
    }
}
/*
** gammaInit.
** @param #1 id.
** @param #2 gammaPara
*/
static int gammaInit(lua_State* L) {
	char id;
	std::map<std::string, std::string> config;
	lua_op_t<char>::lua_to_value(L, 1, id);
	lua_op_t<map<string, string>>::lua_to_value(L, 2, config);

	GammaAlgo::GammaInitData initData;
	//Todo 使用config构造initData

	if (!config["rgbw_count"].empty()) // 插入有效值 0118
        initData.maxbindnum_ = std::atoi(config["rgbw_count"].c_str());

	initData.pGammaAllData_ = new GammaOneUnitData[initData.maxbindnum_];
    parseConfig(config, initData);                                          // 2. 完成初始化

	if (0 == GammaAlgo::gammaInit(&id, &initData))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);

	delete[] initData.pGammaAllData_;
	return 1;
}

static int gammaUnInit (lua_State *L) {
	char id;
	lua_op_t<char>::lua_to_value(L, 1, id);
	
	if (0 == GammaAlgo::gammaUnInit(&id))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);

    return 1;
}

void parseInput(std::map<std::string, std::string>& luaInput, GammaInputInfoReg& input) {
    if (!luaInput["COUNT"].empty())
        input.count_ = std::atoi(luaInput["COUNT"].c_str());
    if (!luaInput["GRAY"].empty())
        input.gray_ = std::atoi(luaInput["GRAY"].c_str());
    if (!luaInput["BANDNO"].empty())
        input.band_ = std::atoi(luaInput["BANDNO"].c_str());
    if (!luaInput["GRAYINDEX"].empty())
        input.grayindex_ = std::atoi(luaInput["GRAYINDEX"].c_str());
    if (!luaInput["ALGMODE"].empty())
        input.algMode_ = std::atoi(luaInput["ALGMODE"].c_str());
    if (!luaInput["REGR"].empty())
        input.regValue_.rRegValue_ = std::atoi(luaInput["REGR"].c_str());
    if (!luaInput["REGG"].empty())
        input.regValue_.gRegValue_ = std::atoi(luaInput["REGG"].c_str());
    if (!luaInput["REGB"].empty())
        input.regValue_.bRegValue_ = std::atoi(luaInput["REGB"].c_str());
    if (!luaInput["x"].empty())
        input.realLvxy_.x_ = std::atof(luaInput["x"].c_str());
    if (!luaInput["y"].empty())
        input.realLvxy_.y_ = std::atof(luaInput["y"].c_str());
    if (!luaInput["Lv"].empty())
        input.realLvxy_.lv_ = std::atof(luaInput["Lv"].c_str());
}
void deparseOutput(GammaOutputInfoReg& output, std::map<std::string, std::string>& outputToLua) {
    outputToLua["REGR"] = std::to_string(output.regValue_.rRegValue_);
    outputToLua["REGG"] = std::to_string(output.regValue_.gRegValue_);
    outputToLua["REGB"] = std::to_string(output.regValue_.bRegValue_);
}
static int gammaProgress(lua_State* L) {
	char id;
	std::map<std::string, std::string> luaInput;
	lua_op_t<char>::lua_to_value(L, 1, id);
	lua_op_t<map<string, string>>::lua_to_value(L, 2, luaInput);

	//Todo  使用luaInput构造input
	//
	GammaInputInfoReg input;
    parseInput(luaInput, input);                      // 解析input
	GammaOutputInfoReg output;


	   	  
	std::map<std::string, std::string> outputToLua;
	if (0 == GammaAlgo::gammaProgress(&id, &input, &output))
	{
		//Todo 使用output构造outputToLua
        deparseOutput(output, outputToLua);           // 反解析output
	}

	lua_op_t< std::map<std::string, std::string>>::push_stack(L, outputToLua);

	return 1;
}

static int gammaSetPara (lua_State *L) {
	char id;
	lua_op_t<char>::lua_to_value(L, 1, id);
	std::map<std::string, std::string> config;
	//lua_op_t<char>::lua_to_value(L, 1, id);
	lua_op_t<map<string, string>>::lua_to_value(L, 2, config);
	//Todo 使用config构造strData
	std::string strData;
	std::map<std::string, std::string>::iterator iterValue = config.begin();

	for (; iterValue != config.end(); iterValue++)
	{
		strData += iterValue->first + "," + iterValue->second + ";";
	}	

	if (0 == GammaAlgo::gammaSetPara(&id, strData.c_str(), strData.length()))
	{
		lua_pushboolean(L, 1);
	}
	else
		lua_pushboolean(L, 0);

	lua_pushboolean(L, 1);
	return 1;
}

static int getAlgVesion (lua_State *L) {
		lua_pushstring(L, OC_VERSION);
        return 1;
}

static const struct luaL_Reg OClib[] = {
        {"gammaInit", gammaInit},
        {"gammaUnInit", gammaUnInit},
        {"gammaProgress", gammaProgress},
        {"gammaSetPara", gammaSetPara},
        {"getAlgVesion", getAlgVesion},
        {NULL, NULL},
};


/*
** Assumes the table is on top of the stack.
*/
static void set_info (lua_State *L) {
  lua_pushliteral(L, "Copyright (C) 2019 BOE&JCDZ");
  lua_setfield(L, -2, "_COPYRIGHT");
  lua_pushliteral(L, "BOE OC algo library, implemented by JCDZ.");
  lua_setfield(L, -2, "_DESCRIPTION");
  lua_pushliteral(L, "boeOC " OC_VERSION);
  lua_setfield(L, -2, "_VERSION");
}

OC_EXPORT int luaopen_boeOC (lua_State *L) {
        luaL_newlib (L, OClib);
        lua_pushvalue(L, -1);
        lua_setglobal(L, OC_LIBNAME);
        set_info (L);
        return 1;
}
