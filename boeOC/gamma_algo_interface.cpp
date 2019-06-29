
#pragma once

#include "gamma_algo_interface.h"
#include "boe_oc.h"
#include <string>

using namespace GammaAlgo;

pGammaPrintMsg g_GammaPrintMsg;

std::map<char, BOE::OCSingleStepAlg1> g_mapID2Alg;
JC_GAMMA_DLL_API int GammaAlgo::gammaInit(const char* pID, const GammaInitData* pGammaData)
{
    //BOE::OCSingleStepAlg1* pOCAlg = NULL;
    std::map<char, BOE::OCSingleStepAlg1>::iterator iter = g_mapID2Alg.find(pID[0]);
    if (iter == g_mapID2Alg.end()) {
        //pOCAlg = new BOE::OCSingleStepAlg1();
        //g_mapID2Alg[pID[0]] = *pOCAlg;                 // 拷贝 --> 析构 ***
        //pOCAlg = &g_mapID2Alg[pID[0]];

        g_mapID2Alg[pID[0]] = BOE::OCSingleStepAlg1();
    }
    //else {
    //    pOCAlg = &(iter->second);
    //}
    //pOCAlg->setAllBinds(*pGammaData);                  // Ìí¼ÓÅäÖÃ²ÎÊý
    g_mapID2Alg[pID[0]].setAllBinds(*pGammaData);
	return 0;
}

JC_GAMMA_DLL_API int GammaAlgo::gammaUnInit(const char* pID)
{
	return 0;
}

JC_GAMMA_DLL_API int GammaAlgo::gammaProgress(const char* pID, const GammaInputInfoReg* pInput, GammaOutputInfoReg* pOutput)
{
    std::map<char, BOE::OCSingleStepAlg1>::iterator iter = g_mapID2Alg.find(pID[0]);
	
    if (iter == g_mapID2Alg.end())
        return -1;                                     // ¸ÃÍ¨µÀÏÂÎÞÊµÀý¶ÔÏó
    BOE::OCSingleStepAlg1& OCAlg = iter->second;
    OCAlg.singleIter(*pInput, *pOutput);
	return 0;
}

JC_GAMMA_DLL_API int GammaAlgo::gammaSetPara(const char* pID, const char* pData, int nLen)
{
    std::map<char, BOE::OCSingleStepAlg1>::iterator iter = g_mapID2Alg.find(pID[0]);
    if (iter == g_mapID2Alg.end())
        return -1;                                          // ¸ÃÍ¨µÀÏÂÎÞÊµÀý¶ÔÏó
    BOE::OCSingleStepAlg1& OCAlg = iter->second;


	std::string strData;
	char* pTemp = const_cast<char *>(pData);
	for (int i = 0; i < nLen; ++i, ++pTemp)
	{
		strData.push_back(*pTemp);
	}

	std::map<std::string, std::string> mapInput;
	std::vector<std::string> vecStr;

	OCAlg.split_string_to_string(strData, vecStr, ';');

	std::vector<std::string>::iterator itVec = vecStr.begin();
	for (; itVec != vecStr.end(); ++itVec)
	{
		std::vector<std::string> vecTemp;
		OCAlg.split_string_to_string(*itVec, vecTemp, ',');
		if (2 == vecTemp.size())
		{
			mapInput.insert(std::map<std::string, std::string>::value_type(vecTemp[0], vecTemp[1]));
		}
	}
	
	std::map<std::string, std::string>::iterator iterValue = mapInput.begin();

	for (; iterValue != mapInput.end(); iterValue++)
	{
		if (iterValue->first == "lvBarrier")
		{
			std::vector<std::string> vecStrValue;
			OCAlg.split_string_to_string(iterValue->second, vecStrValue, '_');
			OCAlg.stepAllBinds_.lvBarrier_.clear();

			for (auto& ele : vecStrValue)
			{
				OCAlg.stepAllBinds_.lvBarrier_.push_back(atof(ele.c_str()));
			}		

		}
		else if (iterValue->first == "xBarrier")
		{
			std::vector<std::string> vecStrValue;
			OCAlg.split_string_to_string(iterValue->second, vecStrValue, '_');
			OCAlg.stepAllBinds_.xBarrier_.clear();

			for (auto& ele : vecStrValue)
			{
				OCAlg.stepAllBinds_.xBarrier_.push_back(atof(ele.c_str()));
			}
		}
		else if (iterValue->first == "yBarrier")
		{
			std::vector<std::string> vecStrValue;
			OCAlg.split_string_to_string(iterValue->second, vecStrValue, '_');
			OCAlg.stepAllBinds_.yBarrier_.clear();

			for (auto& ele : vecStrValue)
			{
				OCAlg.stepAllBinds_.yBarrier_.push_back(atof(ele.c_str()));
			}
		}
		else if (iterValue->first == "lvStepsize")
		{
			std::vector<std::string> vecStrValue;
			OCAlg.split_string_to_string(iterValue->second, vecStrValue, '_');
			OCAlg.stepAllBinds_.lvStepsize_.clear();

			for (auto& ele : vecStrValue)
			{
				OCAlg.stepAllBinds_.lvStepsize_.push_back(atof(ele.c_str()));
			}
		}
		else if (iterValue->first == "xStepsize")
		{
			std::vector<std::string> vecStrValue;
			OCAlg.split_string_to_string(iterValue->second, vecStrValue, '_');
			OCAlg.stepAllBinds_.xStepsize_.clear();

			for (auto& ele : vecStrValue)
			{
				OCAlg.stepAllBinds_.xStepsize_.push_back(atof(ele.c_str()));
			}
		}
		else if (iterValue->first == "yStepsize")
		{
			std::vector<std::string> vecStrValue;
			OCAlg.split_string_to_string(iterValue->second, vecStrValue, '_');
			OCAlg.stepAllBinds_.yStepsize_.clear();

			for (auto& ele : vecStrValue)
			{
				OCAlg.stepAllBinds_.yStepsize_.push_back(atof(ele.c_str()));
			}
		}
	}

	return 0;
}

JC_GAMMA_DLL_API char* GammaAlgo::getAlgVesion()
{
	return "1.0.0.0";
}

JC_GAMMA_DLL_API int GammaAlgo::gammaRegisterFunGammaPrintMsg(pGammaPrintMsg funPtr)
{
	g_GammaPrintMsg = funPtr;

	return 0;
}
