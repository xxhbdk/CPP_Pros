#pragma once

/*
 说明: OC算法 ~ 牛顿迭代法(基于雅可比矩阵)
*/

#include "algbase.h"
#include <array>
#include <string>
#include <Eigen/Dense>

namespace OCAlg
{
    struct CIEuvlvInfo
    {
        double u_;
        double v_;
        double lv_;
    };

    // 电压转换算法
    class RegVolConversion
    {
    public:
        RegVolConversion() = default;
        void conversionInit() { this->bind2volValue_.clear(); }
        GammaAlg::RGBVolValue RGBReg2RGBVol(int bindIndex, const GammaAlg::RGBRegValue& regValue, const OCShare& shareSteps, const std::map<int, GammaAlg::GammaOneUnitData>& bind2Unit);
        GammaAlg::RGBRegValue RGBVol2RGBReg(int bindIndex, const GammaAlg::RGBVolValue& volValue, const OCShare& shareSteps, const std::map<int, GammaAlg::GammaOneUnitData>& bind2Unit);

    private:
        void fillRGBVol(int bindIndex, const OCShare& shareSteps, const std::map<int, GammaAlg::GammaOneUnitData>& bind2Unit);
        GammaAlg::RGBVolValue calcRGBVol(int bindIndex, const GammaAlg::RGBRegValue& regValue, const std::map<int, GammaAlg::GammaOneUnitData>& bind2Unit);
        GammaAlg::RGBRegValue calcRGBReg(int bindIndex, const GammaAlg::RGBVolValue& volValue, const std::map<int, GammaAlg::GammaOneUnitData>& bind2Unit);
    private:
        std::map<int, GammaAlg::RGBVolValue> bind2volValue_;                                 // 已知收敛绑点的电压 -> 必须严格与共享对象中的所有收敛寄存器对映
    };

    // OC算法(牛顿迭代法)
    class OCNewton : public OCBase
    {
    public:
        OCNewton(const std::string pID) : OCBase(pID) {};
        virtual void gammaSetPara(const std::string filename);
        virtual void gammaOneStepCalc(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput);

    private:
        void getJMat();                                                                      // 获取Jacobian矩阵 ~ 依赖解密宏定义
        void parseFile(std::string& content);
        void buildJMat(const std::string& content);
        virtual void resetInit() { this->volConversion_.conversionInit(); }
        void xylv2uvlv(const GammaAlg::CIExylvInfo& xylv, CIEuvlvInfo& uvlv);                // xylv转uvlv
        void updateJMat(const GammaAlg::GammaInputInfoReg& gammaInput);                      // 更新Jacobian矩阵 ~ 依赖矩阵更新宏定义
        void buildDeltaMatOnReg(const GammaAlg::GammaInputInfoReg& gammaInput, Eigen::MatrixXd& regOrVolDeltaMat, Eigen::MatrixXd& uvlvDeltaMat);
        void buildDeltaMatOnVol(const GammaAlg::GammaInputInfoReg& gammaInput, Eigen::MatrixXd& regOrVolDeltaMat, Eigen::MatrixXd& uvlvDeltaMat);
        void updateRank1(Eigen::Matrix3d& XT_X, Eigen::Matrix3d& YT_X, Eigen::Matrix3d& JMat);   // 秩1更新
        void updateRank2(Eigen::Matrix3d& XT_X, Eigen::Matrix3d& YT_X, Eigen::Matrix3d& JMat);   // 秩2更新
        void updateRank3(Eigen::Matrix3d& XT_X, Eigen::Matrix3d& YT_X, Eigen::Matrix3d& JMat);   // 秩3更新
    private:
        std::string filename_;
        std::map<std::array<int, 2>, Eigen::Matrix3d> bandGray2JMat_;
        RegVolConversion volConversion_;                                                     // 电压转换 ~ 依赖电压计算宏定义
    };

    extern std::map<const std::string, OCNewton> g_pID2OCNewton;                             // 导出OCNewton实例
}