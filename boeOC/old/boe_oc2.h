#pragma once

/*
 说明:
 1. 外部开辟动态空间派生实例对象进行单步调节
 2. 一块屏一个实例对象
 3. 所有状态变量均存储在实例内部
 一根线程 -> 一块屏 -> 一个实例(确保状态变量唯一)
 脚本与算法
*/

/*
 BOE算法基类
 两者继承算法类
*/

#include <string>
#include <vector>
#include <map>

#include "gamma_algo_interface.h"

namespace BOE {

    class OCAlgoBase {
    public:
        OCAlgoBase();
    };

    class OCSingleStepAlg2 {
    public:
        OCSingleStepAlg2();
        void initAllBinds(const GammaAlgo::GammaInitData& gammaInitData);
        void updateParams(int fitOrder);
        void updateParams(double* rgbRatio);
        void updateParams(int fitOrder, double* rgbRatio);

    private:
        int maxbindnum_;
        int maxRegStep_;
        double maxVolStep_;
        std::map<int, GammaAlgo::GammaOneUnitData> gammaAllData_;
        std::string filename_;
        int fitOrder_;
        double rgbRatio_[3];
    private:
        void 
    };


}











