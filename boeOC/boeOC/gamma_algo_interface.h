#pragma once


namespace GammaAlgo {

    typedef struct _RGBRegValue_
    {
        int rRegValue_;
        int gRegValue_;
        int bRegValue_;
    }RGBRegValue;

    typedef struct _RGBVolValue_
    {
        double rVolValue_;				//寄存器电压值
        double gVolValue_;			    //寄存器电压值最大值
        double bVolValue_;			    //寄存器电压值最小值	
    }RGBVolValue;

    typedef struct _CielxylvInfo_
    {
        double x_;
        double y_;
        double lv_;
    }CIExylvInfo;

    typedef struct _GammaInputInfoReg_
    {
        int count_;                     // 该绑点第几次调节
        int gray_;
        int band_;
        int grayindex_;                 // 灰阶序号
        int algMode_;                   // 算法类型	
        RGBRegValue regValue_;
        RGBVolValue volValue_;
        CIExylvInfo realLvxy_;          // 真实Lv、x、y
    }GammaInputInfoReg;

    typedef struct _GammaOutputInfoReg_
    {
        RGBRegValue regValue_;
        RGBVolValue volValue_;
    }GammaOutputInfoReg;

    typedef struct _RegConfig_
    {
        int regValue_;
        int regMaxValue_;			    //寄存器最大值
        int regMinValue_;			    //寄存器最小值
    }RegConfig;

    typedef struct _VolConfig_
    {
        double volValue_;
        double volMaxValue_;
        double volMinValue_;
    }VolConfig;

    typedef struct _RegOneUnitConfig_
    {
        RegConfig rRegConfig_;
        RegConfig gRegConfig_;
        RegConfig bRegConfig_;

    }RegOneUnitConfig;

    typedef struct _VolOneUnitConfig_
    {
        VolConfig rVolConfig_;
        VolConfig gVolConfig_;
        VolConfig bVolConfig_;
    }VolOneUnitConfig;

    typedef struct _GammaErrorConfig_
    {
        CIExylvInfo targetxylv_;       // 目标Lv、x、y
        double xLeftError_;
        double xRightError_;
        double yLeftError_;
        double yRightError_;
        double lvLeftError_;
        double lvRightError_;
    }GammaErrorConfig;

    typedef struct _GammaOneUnitData_
    {
        int gray_;
        int bandno_;
        int grayindex_;
        GammaErrorConfig errorConfig_;
        VolOneUnitConfig volConfig_;
        RegOneUnitConfig regConfig_;

    }GammaOneUnitData;

    typedef struct _GammaInitData_
    {
        int maxbindnum_;
        int maxRegStep_;
        double maxVolStep_;
        GammaOneUnitData* pGammaAllData_;
    }GammaInitData;

}