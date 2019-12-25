#pragma once

#ifdef  JC_GAMMA_DLL_EXPORTS
#define JC_GAMMA_DLL_API  __declspec(dllexport)
#else
#define JC_GAMMA_DLL_API  __declspec(dllimport)
#endif


#include <string>

namespace GammaAlg
{

#pragma region =============================================gamma算法宏定义======================================================

#define GAMMA_ERROR_SUCCESS					        0
#define GAMMA_FAILED							    -1

#define GAMMA_ERROR_BASE							(10000)
#define GAMMA_ERROR(idx)							((GAMMA_ERROR_BASE) + idx)

#define GAMMA_ERROR_INVALID_PARAM					GAMMA_ERROR(1)			// 无效参数 命令格式不正确(如缺少字段,字段值不合法等)
#define GAMMA_ERROR_PARAM_PTR_NULL				    GAMMA_ERROR(2)			// 输入的指针为空
#define GAMMA_ERROR_NOT_IMPL					    GAMMA_ERROR(3)			// 当前的方法没有实现
#define GAMMA_ERROR_PARAM_PATH                      GAMMA_ERROR(4)          // 输入的路径异常
#define GAMMA_ERROR_MEMERY_ALLOC                    GAMMA_ERROR(5)          // 内存申请失败
#define GAMMA_ERROR_PI_NOT_INIT                     GAMMA_ERROR(6)          // 算法未初始化成功
#define GAMMA_ERROR_OPEN_FILE                       GAMMA_ERROR(7)          // 文件打开失败


#define PI_N_DELETE(X)                    if(X)      { delete []X; X = NULL;}
#define PI_DELETE(X)                      if(X)      { delete X;   X = NULL;}
#define PI_FREE(X)                        if(X)      { free(X);    X = NULL;}
#define PI_CLOSE(X)                       if(X)      { fclose(X);  X = NULL;}
#define CHECK_ERROR(state, error_code)    if (state) { LogError("error happend"); return error_code; }
#define CHECK_FORCE_STOP(state)           if (state) { state = !state; return ERROR_PI_FORCE_STOP; }
#define CHECK_ERROR_RETURN(state)         if (state) { return; }
#define CHECK_ERROR_CONTINUE(state)       if (state) { continue; }
#define CHECK_SUCESS(state, error_code)   if (state) { return error_code; }

#define g_CHAN_NUM 3

#pragma endregion


#pragma region =============================================gamma算法数据结构======================================================


    /* 寄存器相关数据 */
    typedef struct _RGBRegValue_
    {
        int rRegValue_;
        int gRegValue_;
        int bRegValue_;
    }RGBRegValue;

    typedef struct _RGBVolValue_
    {
        double rVolValue_;
        double gVolValue_;
        double bVolValue_;
    }RGBVolValue;

    typedef struct _CIElxylvInfo_
    {
        double x_;
        double y_;
        double lv_;
    }CIExylvInfo;

    typedef struct _GammaInputInfoReg_
    {
        int count_;                     // 该绑点第几次调节
        int bandNo_;                    // band号
        int grayValue_;                 // 灰阶值
        int bindIndex_;                 // 绑点序号
        int algMode_;                   // 算法类型    
        RGBRegValue regValue_;
        RGBVolValue volValue_;
        CIExylvInfo realxylv_;          // 真实x, y, lv
    }GammaInputInfoReg;

    typedef struct _GammaOutputInfoReg_
    {
        RGBRegValue regValue_;
        RGBVolValue volValue_;
    }GammaOutputInfoReg;

    typedef struct _RegConfig_
    {
        int regValue_;                  // 寄存器初始值
        int regMaxValue_;               // 寄存器最大值
        int regMinValue_;               // 寄存器最小值
    }RegConfig;

    typedef struct _VolConfig_
    {
        double volValue_;               // 电压初始值
        double volMaxValue_;            // 电压最大值
        double volMinValue_;            // 电压最小值    
    }VolConfig;

    typedef struct _RegOneUnitConfig_
    {
        RegConfig rRegConfig_;
        RegConfig gRegConfig_;
        RegConfig bRegConfig_;
    }RegOneUnitConfig;

    typedef struct _VolOneUnitConfig_
    {
        double GrayV0;
        double G_VREG1;
        double G_VREF1;
        double G_VTUPP;
        VolConfig rVolConfig_;
        VolConfig gVolConfig_;
        VolConfig bVolConfig_;
    }VolOneUnitConfig;

    typedef struct _GammaErrorConfig_
    {
        CIExylvInfo targetxylv_;           // 目标x,y,lv
        double xLeftError_;
        double xRightError_;
        double yLeftError_;
        double yRightError_;
        double lvLeftError_;
        double lvRightError_;
    }GammaErrorConfig;

    typedef struct _GammaOneUnitData_
    {
        int bandNo_;
        int grayValue_;
        int bindIndex_;
        GammaErrorConfig errorConfig_;
        VolOneUnitConfig volConfig_;
        RegOneUnitConfig regConfig_;
    }GammaOneUnitData;

    typedef struct _GammaInitData_
    {
        int allBindNum_;                   // 所有绑点数
        int maxRegStep_;                   // 寄存器最大迭代单步
        double maxVolStep_;                // 电压最大迭代单步
        GammaOneUnitData* pGammaAllData_;
    }GammaInitData;

#pragma endregion

    // 输入: 收敛文件名, 是否电压依赖, 是否更新数据, 最大存储容量
    JC_GAMMA_DLL_API int ivpInit(const char* pID, void* pInitConfig);

    // 更新外部CSV(收敛信息文件)
    JC_GAMMA_DLL_API int ivpWriteCSV(const char* pID, const char* outFilename);

    // 输入: bandNo, grayValue
    // 输出: 预测regValue或volValue
    JC_GAMMA_DLL_API int ivpPredInitialRGB(const char* pID, GammaInputInfoReg* IVPData);

    // 输入: bandNo, grayValue, 收敛regValue或volValue
    JC_GAMMA_DLL_API int ivpFeedOptimalRGB(const char* pID, GammaInputInfoReg* IVPData);

    JC_GAMMA_DLL_API const char* getAlgVesion();
    typedef int (*pGammaPrintMsg)(const char* pID, char* pMsg, ...);
    typedef int (*pCreateLogFile)(const char* pID, const char* fileName, int len);
    JC_GAMMA_DLL_API int gammaRegisterFunGammaPrintMsg(pGammaPrintMsg funPtr);
    JC_GAMMA_DLL_API int gammaRegisterFunCreateLogFile(pCreateLogFile funPtr);

} //namespace GammaAlg end
