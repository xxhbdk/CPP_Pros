#pragma once

/*
 说明:
 1. 从全局对象中获取算法实例进行单步调节
 2. 一块屏一个实例对象
 3. 所有状态变量均存储在实例内部
 一根线程 -> 一块屏 -> 一个实例(确保状态变量唯一)
 完整内容: 算法、中间件、脚本
*/


#include <string>
#include <vector>
#include <map>
#include <Eigen/Dense>

#include "gamma_algo_interface.h"



namespace BOE {

    class OCBase {
    public:
        void gammaInit(const GammaAlgo::GammaInitData& initData);                             // 初始化操作
        virtual void gammaSetPara(double bandRatio_, const std::vector<double>& lvBarrier_, const std::vector<int>& lvStepsize_, const std::vector<double>& xBarrier_, const std::vector<int>& xStepsize_, const std::vector<double>& yBarrier_, const std::vector<int>& yStepsize_) {}                          // 额外配置操作
        virtual void gammaSetPara(std::string filename, int fitOrder, double* rgbRatio, std::string tablename) {}
        virtual void gammaProgress(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind);    // 单步调节操作
        static void split_string_to_string(const std::string& s, std::vector<std::string>& sv, const char flag);

    protected:
        int maxbindnum_;
        int maxRegStep_;
        double maxVolStep_;
        std::map<int, GammaAlgo::GammaOneUnitData> gammaAllData_;
	private:
		int lastGrayIndex_ = -1;                                                              // 上一个绑点编号
	private:
		virtual void resetTuningTab() {}
    };

    class OCAlgo1 : public OCBase {
    public:
        double bandRatio_ = 1;
        std::vector<double> lvBarrier_;    // 正常size为5
        std::vector<int> lvStepsize_;      // 正常size为6
        std::vector<double> xBarrier_;
        std::vector<int> xStepsize_;
        std::vector<double> yBarrier_;
        std::vector<int> yStepsize_;
    public:
        OCAlgo1() : tuningTab_(0) {}
        virtual void gammaSetPara(double bandRatio_, const std::vector<double>& lvBarrier_, const std::vector<int>& lvStepsize_, const std::vector<double>& xBarrier_, const std::vector<int>& xStepsize_, const std::vector<double>& yBarrier_, const std::vector<int>& yStepsize_);
        virtual void gammaProgress(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind);

    private:
        int tuningTab_ = 0;
    private:
        void tuningOneStep(const GammaAlgo::GammaInputInfoReg& inputOneBind, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int tuningLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int tuningx(const GammaAlgo::GammaInputInfoReg& inputOneBind, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int tuningy(const GammaAlgo::GammaInputInfoReg& inputOneBind, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int getStepsize(double valLoc, double bandRatio, const std::vector<double>& barrier, const std::vector<int>& stepsize);
		virtual void resetTuningTab() { tuningTab_ = 0; }
	};

    struct MonocParams {
        Eigen::MatrixXd A;
        Eigen::VectorXd D;
        Eigen::MatrixXd E;                // 大小待定的单位矩阵
        Eigen::VectorXd curveParams;
    };

    struct CurveParams {
        int order;
        int bindNum;
        MonocParams rCurveParams;
        MonocParams gCurveParams;
        MonocParams bCurveParams;
    };

    struct RGBMats {
        Eigen::MatrixXd rMat_;
        Eigen::MatrixXd gMat_;
        Eigen::MatrixXd bMat_;
    };

    class OCAlgo2 : public OCBase {
    public:
        OCAlgo2() : tuningTab_(0) {}
        virtual void gammaSetPara(std::string filename, int fitOrder, double* rgbRatio, std::string tablename);
        virtual void gammaProgress(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind);

    private:
        int tuningTab_ = 0;                                                      // 调节状态记录(-1: 迭代初始化; 0: rLv; 1: gLv; 2: bLv)
        std::string filename_ = "./CH1_REG_VOL_gam_tm.csv";                      // 拟合文件名
        int fitOrder_ = 3;                                                       // 多项式拟合阶数
        double rgbRatio_[3] = { 0.7, 0.3, 0 };                                   // {rlv/lv, glv/lv, blv/lv}
        std::map<int, CurveParams> band2curve_;                                  // 拟合曲线参数
        std::string tablename_ = "./20190718CIEtablev0.3_JC.csv";                // 矩阵文件名
        RGBMats ratioMats_;                                                      // rgb比例矩阵(含色坐标信息)

    private:
        void curveFitting();
        void fileParse();
        void dataExtract(std::string& line, int& bandIdx, double& lv, double& rVol, double& gVol, double& bVol);
        void buildMatrix(int bandIdx, std::vector<double>& lvVec, std::vector<double>& rVolVec, std::vector<double>& gVolVec, std::vector<double>& bVolVec);
        void polyfit(double epsilon = 1.e-9);
        void tuningOneStep(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int tuningrLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int tuninggLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int tuningbLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        double newtonIter(const Eigen::VectorXd& curveParams, double targetLv, double epsilon = 1.e-8);
        double polyFunc(const Eigen::VectorXd& curveParams, double targetLv, double x);
        double polySlope(const Eigen::VectorXd& curveParams, double x);
        virtual void resetTuningTab() { tuningTab_ = 0; }
        void tableParse();
        void dataExtract2(std::string& line, std::vector<std::vector<double>>& monocVec);
        void buildMatrix2(std::vector<std::vector<double>>& rRatioVec, std::vector<std::vector<double>>& gRatioVec, std::vector<std::vector<double>>& bRatioVec);
        double getRatio(double x, double y, const Eigen::MatrixXd& monocMat);
        int getRowIndex(double y, const Eigen::MatrixXd& monocMat);
        int getColIndex(double x, const Eigen::MatrixXd& monocMat);
    };

}






