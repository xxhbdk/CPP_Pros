#pragma once

/*
 说明: IVP算法 ~ 惯性加权(基于二次规划)
*/

#include <Eigen/Dense>
#include "algbase.h"


namespace IVPAlg
{
    class IVPIW : public IVPBase
    {
    public:
        IVPIW() = delete;
        explicit IVPIW(const std::string& id);
        virtual void ivpPredInitialRGB(GammaAlg::GammaInputInfoReg& IVPData);             // 根据lv判据, 完成rgb之预测

    protected:
        virtual void virtualInit(const std::map<std::string, std::string>& initConfig);
        virtual void virtualFeed(GammaAlg::GammaInputInfoReg& IVPData);                   // 根据收敛数据D, 完成W之更新

    private:
        void scnLvClear();
        void buildAMat();
        Eigen::VectorXd getWold(GammaAlg::GammaInputInfoReg& IVPData);
        std::array<int, 2> getLastBind(double currBindLv);
        void refreshPred(GammaAlg::GammaInputInfoReg& IVPData, Eigen::VectorXd& D);                           // 根据电压依赖与否及D, 刷新输出
        Eigen::VectorXd getD(GammaAlg::GammaInputInfoReg& IVPData);
        Eigen::VectorXd calcW(Eigen::VectorXd& Wold, Eigen::MatrixXd& A, Eigen::VectorXd& D);
    private:
        std::map<double, std::array<int, 2>> lv2bind_;
        std::map<std::array<int, 2>, Eigen::MatrixXd> bind2AMat_;
        std::map<std::array<int, 2>, Eigen::VectorXd> bind2WVec_;
    };

    extern std::map<const std::string, IVPIW> g_id2IVPIW;
}
