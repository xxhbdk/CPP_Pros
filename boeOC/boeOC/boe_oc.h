#pragma once

/*
 ˵��:
 1. ��ȫ�ֶ����л�ȡ�㷨ʵ�����е�������
 2. һ����һ��ʵ������
 3. ����״̬�������洢��ʵ���ڲ�
 һ���߳� -> һ���� -> һ��ʵ��(ȷ��״̬����Ψһ)
 ��������: �㷨���м�����ű�
*/


#include <string>
#include <vector>
#include <map>
#include <Eigen/Dense>

#include "gamma_algo_interface.h"



namespace BOE {

    class OCBase {
    public:
        void gammaInit(const GammaAlgo::GammaInitData& initData);                             // ��ʼ������
        virtual void gammaSetPara(const StepOneBind& stepOneBind) {}                          // �������ò���
        virtual void gammaSetPara(std::string filename, int fitOrder, double* rgbRatio) {}
        virtual void gammaProgress(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind) = 0;    // �������ڲ���

    protected:
        int maxbindnum_;
        int maxRegStep_;
        double maxVolStep_;
        std::map<int, GammaAlgo::GammaOneUnitData> gammaAllData_;
    };

    struct StepOneBind {
        double bandRatio_;
        std::vector<double> lvBarrier_;                        // ����sizeΪ5
        std::vector<int> lvStepsize_;                          // ����sizeΪ6
        std::vector<double> xBarrier_;
        std::vector<int> xStepsize_;
        std::vector<double> yBarrier_;
        std::vector<int> yStepsize_;
    };

    class OCAlgo1 : public OCBase {
    public:
        OCAlgo1() : tuningTab_(0) {}
        virtual void gammaSetPara(const StepOneBind& stepOneBind);   // ??????????

    private:
        int tuningTab_ = 0;
    };

    struct MonocParams {
        Eigen::MatrixXd A;
        Eigen::VectorXd D;
        Eigen::MatrixXd E;                // ��С�����ĵ�λ����
        Eigen::VectorXd curveParams;
    };

    struct CurveParams {
        int order;
        int bindNum;
        MonocParams rCurveParams;
        MonocParams gCurveParams;
        MonocParams bCurveParams;
    };

    class OCAlgo2 : public OCBase {
    public:
        OCAlgo2() : tuningTab_(-1) {}
        virtual void gammaSetPara(std::string filename, int fitOrder, double* rgbRatio);
        virtual void gammaProgress(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind);

    private:
        int tuningTab_ = -1;                                                     // ����״̬��¼(-1: ������ʼ��; 0: rLv; 1: gLv; 2: bLv)
        std::string filename_ = "./CH1_REG_VOL_gam_tm.csv";                      // ����ļ���
        int fitOrder_ = 3;                                                       // ����ʽ��Ͻ���
        double rgbRatio_[3] = { 0.7, 0.3, 0 };                                   // {rlv/lv, glv/lv, blv/lv}
        std::map<int, CurveParams> band2curve_;                                  // ������߲���

    private:
        void curveFitting();
        void fileParse();
        void dataExtract(std::string& line, int& bandIdx, double& lv, double& rVol, double& gVol, double& bVol);
        void buildMatrix(int bandIdx, std::vector<double>& lvVec, std::vector<double>& rVolVec, std::vector<double>& gVolVec, std::vector<double>& bVolVec);
        void polyfit(double epsilon = 1.e-9);
        void tuningOneStep(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        void tuningInit(const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int tuningrLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int tuninggLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        int tuningbLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind);
        double newtonIter(const Eigen::VectorXd& curveParams, double targetLv, double epsilon = 1.e-8);
        double polyFunc(const Eigen::VectorXd& curveParams, double targetLv, double x);
        double polySlope(const Eigen::VectorXd& curveParams, double x);
    };

}






