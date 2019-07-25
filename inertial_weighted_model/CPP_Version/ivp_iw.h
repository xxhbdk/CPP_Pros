#pragma once

/*
 初始值预测: 惯性加权算法
 配置参数:
 ① csv文件参数配置
 ② iw算法超参数配置
 调用流程:
 1. 配置csv文件参数(可选)
 2. 加载csv文件
 3. 配置iw算法超参数(可选)
 4. 获取iw算法最优超参数(可选)
 5. 预测初始值
 6. 喂入最优值
*/


#include <map>
#include <string>
#include <vector>
#include <Eigen/Dense>

#include "gamma_algo_interface.h"


namespace IVP {


    class IVPBase {
    public:
        void csvSetPara(int maxSize = -1, bool refreshable = true) { this->maxSize_ = maxSize; this->refreshable_ = refreshable; }   // csv文件参数配置
        void loadCSV(std::string filename = "./CH1_InitReg.csv");                              // 修正maxSize
        void refreshScreen(const std::map<int, std::vector<double>>& screen);

    protected:
        std::vector<std::map<int, std::vector<double>>> screenList_;
        std::map<int, int> bind2gray;
        bool refreshable_ = true;                                                              // 0 - 刷新; 1 - 不刷新

    private:
        int maxSize_ = -1;
        int initSize_ = 20;
        int refreshIdx_ = 19;
    private:
        virtual void buildMatrix() {}
    };


    struct CalcMat {
        Eigen::MatrixXd A_;
        Eigen::VectorXd D_;
        Eigen::MatrixXd E_;                                 // 大小待定的单位矩阵
    };

    struct CalcW {
        Eigen::VectorXd W255_;
        Eigen::VectorXd Wold_;
    };

    class IWAlgo : public IVPBase {
    public:
        IWAlgo() : initLamda_(0) {}
        void iwSetPara(double lamda = 0);
        void iwAutoPara(const std::vector<double> lamdaRange = {});                          // iw算法超参数配置, 通过leave one out选择最优超参数
        void predInitialRGB(GammaAlgo::IVPType& IVPData);
        void feedOptimalRGB(const GammaAlgo::IVPType& IVPData);

    private:
        CalcW W_;
        double initLamda_ = 0;
        std::vector<double> lamdaList_;
        std::vector<double> lamdaRange_ = { -1, 10, 0.1 };                                // 默认lamda搜索网格
        std::map<int, CalcMat> bind2Mat_;
        std::map<int, std::vector<double>> screen_;                                     // 记录一块屏上的所有收敛数据
    private:
        void initLamdaList();
        void initInnerW(std::vector<CalcW>& screen2W);
        void initW(CalcW& W, int size);
        void initInnerMat(std::vector<CalcMat>& Mat_List, int bindIdx);
        void updateInnerW(std::vector<CalcW>& W_List, std::vector<CalcMat>& Mat_List, int bindIdx, double lamda);
        Eigen::VectorXd calcW(CalcW& W_old, CalcMat& Mat, int bindIdx, double lamda);
        void updateW(CalcW& W_old, Eigen::VectorXd& W, int bindIdx);
        double calcTotalLoss(std::vector<CalcW>& W_List, std::vector<CalcMat>& Mat_List, int bindIdx);
        double calcLoss(CalcW& W_old, CalcMat& Mat, int bindIdx);
        double findLamdaByLoss(std::map<double, double>& lamda2loss);
        virtual void buildMatrix();
    };







}














