#include "alginterface.h"
#include "algiw.h"         // 载入惯性加权之map

GammaAlg::pGammaPrintMsg g_GammaPrintMsg;
GammaAlg::pCreateLogFile g_CreateLogFile;

JC_GAMMA_DLL_API int GammaAlg::ivpInit(const char* pID, void* pInitConfig)
{
    auto pTra = static_cast<std::map<std::string, std::string>*>(pInitConfig);
    
    if (IVPAlg::g_id2IVPIW.find(pID) == IVPAlg::g_id2IVPIW.end())
    {
        IVPAlg::g_id2IVPIW.insert({ pID, IVPAlg::IVPIW(pID) });
    }

    IVPAlg::g_id2IVPIW.find(pID)->second.ivpInit(*pTra);

    return 0;
}

JC_GAMMA_DLL_API int GammaAlg::ivpWriteCSV(const char* pID, const char* outFilename)
{
    if (IVPAlg::g_id2IVPIW.find(pID) == IVPAlg::g_id2IVPIW.end())
    {
        return 1;                         // 实例不存在
    }

    IVPAlg::g_id2IVPIW.find(pID)->second.ivpWriteCSV(outFilename);

    return 0;
}

JC_GAMMA_DLL_API int GammaAlg::ivpPredInitialRGB(const char* pID, GammaInputInfoReg* IVPData)
{
    if (IVPAlg::g_id2IVPIW.find(pID) == IVPAlg::g_id2IVPIW.end())
    {
        return 1;                         // 实例不存在
    }

    IVPAlg::g_id2IVPIW.find(pID)->second.ivpPredInitialRGB(*IVPData);

    return 0;
}

JC_GAMMA_DLL_API int GammaAlg::ivpFeedOptimalRGB(const char* pID, GammaInputInfoReg* IVPData)
{
    if (IVPAlg::g_id2IVPIW.find(pID) == IVPAlg::g_id2IVPIW.end())
    {
        return 1;                         // 实例不存在
    }

    IVPAlg::g_id2IVPIW.find(pID)->second.ivpFeedOptimalRGB(*IVPData);

    return 0;
}

JC_GAMMA_DLL_API const char* GammaAlg::getAlgVesion()
{
    return "1.0.0.0";
}

JC_GAMMA_DLL_API int GammaAlg::gammaRegisterFunGammaPrintMsg(pGammaPrintMsg funPtr)
{
    g_GammaPrintMsg = funPtr;

    return 0;
}

JC_GAMMA_DLL_API int GammaAlg::gammaRegisterFunCreateLogFile(pCreateLogFile funPtr)
{
    g_CreateLogFile = funPtr;

    return 0;
}
