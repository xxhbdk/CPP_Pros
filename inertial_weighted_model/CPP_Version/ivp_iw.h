#pragma once

/*
 ��ʼֵԤ��: ���Լ�Ȩ�㷨
 ���ò���:
 �� csv�ļ���������
 �� iw�㷨����������
 ��������:
 1. ����csv�ļ�����(��ѡ)
 2. ����csv�ļ�
 3. ����iw�㷨������(��ѡ)
 4. ��ȡiw�㷨���ų�����(��ѡ)
 5. Ԥ���ʼֵ
 6. ι������ֵ
*/


#include <map>
#include <string>
#include <vector>
#include <Eigen/Dense>

#include "gamma_algo_interface.h"


namespace IVP {


    class IVPBase {
    public:
        void csvSetPara(int maxSize = -1, bool refreshable = true) { this->maxSize_ = maxSize; this->refreshable_ = refreshable; }   // csv�ļ���������
        void loadCSV(std::string filename = "./CH1_InitReg.csv");                              // ����maxSize
        void refreshScreen(const std::map<int, std::vector<double>>& screen);

    protected:
        std::vector<std::map<int, std::vector<double>>> screenList_;
        std::map<int, int> bind2gray;
        bool refreshable_ = true;                                                              // 0 - ˢ��; 1 - ��ˢ��

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
        Eigen::MatrixXd E_;                                 // ��С�����ĵ�λ����
    };

    struct CalcW {
        Eigen::VectorXd W255_;
        Eigen::VectorXd Wold_;
    };

    class IWAlgo : public IVPBase {
    public:
        IWAlgo() : initLamda_(0) {}
        void iwSetPara(double lamda = 0);
        void iwAutoPara(const std::vector<double> lamdaRange = {});                          // iw�㷨����������, ͨ��leave one outѡ�����ų�����
        void predInitialRGB(GammaAlgo::IVPType& IVPData);
        void feedOptimalRGB(const GammaAlgo::IVPType& IVPData);

    private:
        CalcW W_;
        double initLamda_ = 0;
        std::vector<double> lamdaList_;
        std::vector<double> lamdaRange_ = { -1, 10, 0.1 };                                // Ĭ��lamda��������
        std::map<int, CalcMat> bind2Mat_;
        std::map<int, std::vector<double>> screen_;                                     // ��¼һ�����ϵ�������������
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














