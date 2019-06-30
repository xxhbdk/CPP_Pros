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
        double rVolValue_;				//�Ĵ�����ѹֵ
        double gVolValue_;			    //�Ĵ�����ѹֵ���ֵ
        double bVolValue_;			    //�Ĵ�����ѹֵ��Сֵ	
    }RGBVolValue;

    typedef struct _CielxylvInfo_
    {
        double x_;
        double y_;
        double lv_;
    }CIExylvInfo;

    typedef struct _GammaInputInfoReg_
    {
        int count_;                     // �ð��ڼ��ε���
        int gray_;
        int band_;
        int grayindex_;                 // �ҽ����
        int algMode_;                   // �㷨����	
        RGBRegValue regValue_;
        RGBVolValue volValue_;
        CIExylvInfo realLvxy_;          // ��ʵLv��x��y
    }GammaInputInfoReg;

    typedef struct _GammaOutputInfoReg_
    {
        RGBRegValue regValue_;
        RGBVolValue volValue_;
    }GammaOutputInfoReg;

    typedef struct _RegConfig_
    {
        int regValue_;
        int regMaxValue_;			    //�Ĵ������ֵ
        int regMinValue_;			    //�Ĵ�����Сֵ
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
        CIExylvInfo targetxylv_;       // Ŀ��Lv��x��y
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