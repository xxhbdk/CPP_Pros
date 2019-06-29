#pragma once

/*
 说明:
 1. 外部开辟动态空间派生实例对象进行单步调节
 2. 一块屏一个实例对象
 3. 所有状态变量均存储在实例内部
 一根线程 -> 一块屏 -> 一个实例(确保状态变量唯一)
 脚本与算法
*/


#include <map>
#include <vector>
#include <string>

namespace BOE {

    struct SpecOneBind {                                        // 替换 - GammaErrorConfig
        double LvSpec;
        double xSpec;
        double ySpec;
        double LvTarget;
        double xTarget;
        double yTarget;
    };

    struct StepOneBind {                                       // 替换 - 暂无
        double bandRatio;
        std::vector<double> LvBarrier_vec;     // 正常size为5
        std::vector<double> LvStepsize_vec;    // 正常size为6
        std::vector<double> xBarrier_vec;
        std::vector<double> xStepsize_vec;
        std::vector<double> yBarrier_vec;
        std::vector<double> yStepsize_vec;
    };

    struct ConfigOneBind {
        SpecOneBind specParams;                // 一个绑点的详细spec信息
        StepOneBind stepParams;                // 一个绑点的详细步进信息
    };

    struct InputMSGOneBind {                                   // 替换 - GammaInputInfoReg
        double rVolValue;
        double gVolValue;
        double bVolValue;
        double Lv;
        double x;
        double y;
    };

    struct OutputMSGOneBind {                                  // 替换 - GammaOutputInfoReg
        double rVolValue;
        double gVolValue;
        double bVolValue;
    };

    struct ConfigOneBind2 {                               // 替换 - 暂无
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

    struct InputMSGOneBind2 {                             // 替换 - 暂无
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
        void pushOneBind(int bandIdx, int grayVal, ConfigOneBind2& configOneBind);                                  // 全局配置
        int singleIter(int bandIdx, int grayVal, InputMSGOneBind2& inputOneBind, OutputMSGOneBind& outputOneBind);  // 收敛 - 1; 未收敛 - 0. 初始状态下, 输入无效, 输出有效
    
    private:
        std::map<std::string, ConfigOneBind2> allBindsConfig_map;                                                   // string = "bandIdx Gray", 定位绑点信息
        int tuningTab = -1;                                                                                         // 调节起点: -1 - config; 0 - rLv; 1 - gLv; 2 - bLv
    private:
        std::string getBindIdentifier(int bandIdx, int grayVal);
        void tuningOneStep(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& OutputOneBind, int& convergedTab);
        void tuningInit(ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind);
        int tuningrLv(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind);
        int tuninggLv(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind);
        int tuningbLv(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind);
        void checkSpec(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, int& convergedTab);
        double newtonIter(double a0, double a1, double a2, double a3, double b, double epsilon=1.e-8);              // 采用牛顿迭代法对三次多项式求根
        double func(double a0, double a1, double a2, double a3, double b, double x);
        double slope(double a0, double a1, double a2, double x);
    };

}









