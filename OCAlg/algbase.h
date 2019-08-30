#pragma once

/*
 说明: OC算法 ~ 基类
*/

#include "alginterface.h"
#include <map>
#include <vector>

namespace OCAlg
{
    class OCShare
    {
    public:
        OCShare(const std::string pID): pID_(pID) {}
        void stepInit() { this->bind2Steps_.clear(); }                                                              // 切屏时调用
        void stepInput(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput);   // 输入迭代信息
        int stepOutput(int bindIndex, std::vector<GammaAlg::GammaInputInfoReg>& steps) const ;                             // 输出迭代信息   0 ~ 输出成功; 1 ~ 输出失败
        const std::map<int, std::vector<GammaAlg::GammaInputInfoReg>>& getAllSteps() const { return this->bind2Steps_; }

    private:
        std::string pID_;
        std::map<int, std::vector<GammaAlg::GammaInputInfoReg>> bind2Steps_;
    };

    class OCBase
    {
    public:
        OCBase(const std::string pID) : pID_(pID), allBindNum_(-1), maxRegStep_(-1), maxVolStep_(-1), bandSwitched_(false), bindSwitched_(false), bandNoLast_(-1), bindIndexLast_(-1) {}
        void gammaInit(const GammaAlg::GammaInitData& gammaInitData);                                                   // 通用初始化 ~ 1次/IC
        virtual void gammaSetPara(const std::string filename) {}                                                        // 个性初始化 ~ 依赖于具体算法 ~ 1次/IC
        virtual void gammaOneStepCalc(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput);   // 必要之预先操作 ~ 1次/Step

    protected:
        void restrictRGBReg(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput);  // 约束RGB寄存器
        void restrictRGBVol(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput);  // 约束RGB电压
    protected:
        std::string pID_;
        int allBindNum_;                                                                                                // 所有绑点数
        int maxRegStep_;
        double maxVolStep_;
        std::map<int, GammaAlg::GammaOneUnitData> bind2Unit_;
        bool bandSwitched_;                                                                                             // band切换标记
        bool bindSwitched_;                                                                                             // bind切换标记

    private:
        virtual void resetInit() {}                                                                                     // 内部对象初始化状态重置
    private:
        int bandNoLast_;                                                                                                // 上一步band号
        int bindIndexLast_;                                                                                             // 上一步bind号
    };

    extern std::map<const std::string, OCShare> g_pID2OCShare;                                                          // 导出OCShare实例
}