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
#include <sstream>

#include "gamma_algo_interface.h"
namespace BOE {


    struct StepOneBind {
        double bandRatio_ = 1;
        std::vector<double> lvBarrier_ = { 0.7, 0.9, 1, 1.1, 1.3 };                        // 正常size为5
        std::vector<int> lvStepsize_ = { 50, 10, 1, -1, -10, -50 };                          // 正常size为6
        std::vector<double> xBarrier_ = { 0.21, 0.29, 0.31, 0.34, 0.41 };
        std::vector<int> xStepsize_ = { 50, 10, 1, -1, -10, -50 };
        std::vector<double> yBarrier_ = { 0.21, 0.29, 0.31, 0.34, 0.41 };
        std::vector<int> yStepsize_ = { 50, 10, 1, -1, -10, -50 };
    };




    class OCSingleStepAlg1 {
    public:
        OCSingleStepAlg1() : tuningTab_(0) {}
        void setAllBinds(const GammaAlgo::GammaInitData& initData);
        void updateStepsize(const StepOneBind& stepOneBind);
        void singleIter(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind);
		void split_string_to_string(const std::string& s, std::vector<std::string>& sv, const char flag);
		StepOneBind stepAllBinds_;

    private:
        std::map<std::string, GammaAlgo::GammaErrorConfig> errorAllBinds_;                                                                                           // string = "bandIdx Gray", 定位绑点信息
        
        int tuningTab_ = 0;                                                                                                                                          // 调节起点: 0 - Lv; 1 - x; 2 - y
    private:
        std::string getBindIdentifier(int bandIdx, int grayVal);
        void tuningOneStep(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaErrorConfig& errorOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int tuninglv(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaErrorConfig& errorOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind);     // 调 - 1; 未调 - 0
        int getStepsize(double valLoc, std::vector<double>& barrier, double bandRatio, std::vector<int>& stepsize);
        int tuningx(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaErrorConfig& errorOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind);      // 调 - 1; 未调 - 0
        int tuningy(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaErrorConfig& errorOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind);      // 调 - 1; 未调 - 0
    };

}









