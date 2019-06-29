#pragma once

/*
 ˵��:
 1. �ⲿ���ٶ�̬�ռ�����ʵ��������е�������
 2. һ����һ��ʵ������
 3. ����״̬�������洢��ʵ���ڲ�
 һ���߳� -> һ���� -> һ��ʵ��(ȷ��״̬����Ψһ)
 �ű����㷨
*/


#include <map>
#include <vector>
#include <string>

namespace BOE {

    struct SpecOneBind {                                        // �滻 - GammaErrorConfig
        double LvSpec;
        double xSpec;
        double ySpec;
        double LvTarget;
        double xTarget;
        double yTarget;
    };

    struct StepOneBind {                                       // �滻 - ����
        double bandRatio;
        std::vector<double> LvBarrier_vec;     // ����sizeΪ5
        std::vector<double> LvStepsize_vec;    // ����sizeΪ6
        std::vector<double> xBarrier_vec;
        std::vector<double> xStepsize_vec;
        std::vector<double> yBarrier_vec;
        std::vector<double> yStepsize_vec;
    };

    struct ConfigOneBind {
        SpecOneBind specParams;                // һ��������ϸspec��Ϣ
        StepOneBind stepParams;                // һ��������ϸ������Ϣ
    };

    struct InputMSGOneBind {                                   // �滻 - GammaInputInfoReg
        double rVolValue;
        double gVolValue;
        double bVolValue;
        double Lv;
        double x;
        double y;
    };

    struct OutputMSGOneBind {                                  // �滻 - GammaOutputInfoReg
        double rVolValue;
        double gVolValue;
        double bVolValue;
    };

    struct ConfigOneBind2 {                               // �滻 - ����
        double rLvSpec;
        double gLvSpec;
        double bLvSpec;
        double rLvTarget;
        double gLvTarget;
        double bLvTarget;
        double rCurveParams[4];
        double gCurveParams[4];
        double bCurveParams[4];
    };

    struct InputMSGOneBind2 {                             // �滻 - ����
        double rVolValue;
        double gVolValue;
        double bVolValue;
        double rLv;
        double gLv;
        double bLv;
    };

    class OCSingleStepAlg2 {
    public:
        OCSingleStepAlg2() : tuningTab(-1) {}
        void pushOneBind(int bandIdx, int grayVal, ConfigOneBind2& configOneBind);                                  // ȫ������
        int singleIter(int bandIdx, int grayVal, InputMSGOneBind2& inputOneBind, OutputMSGOneBind& outputOneBind);  // ���� - 1; δ���� - 0. ��ʼ״̬��, ������Ч, �����Ч
    
    private:
        std::map<std::string, ConfigOneBind2> allBindsConfig_map;                                                   // string = "bandIdx Gray", ��λ�����Ϣ
        int tuningTab = -1;                                                                                         // �������: -1 - config; 0 - rLv; 1 - gLv; 2 - bLv
    private:
        std::string getBindIdentifier(int bandIdx, int grayVal);
        void tuningOneStep(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& OutputOneBind, int& convergedTab);
        void tuningInit(ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind);
        int tuningrLv(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind);
        int tuninggLv(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind);
        int tuningbLv(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind);
        void checkSpec(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, int& convergedTab);
        double newtonIter(double a0, double a1, double a2, double a3, double b, double epsilon=1.e-8);              // ����ţ�ٵ����������ζ���ʽ���
        double func(double a0, double a1, double a2, double a3, double b, double x);
        double slope(double a0, double a1, double a2, double x);
    };

}









