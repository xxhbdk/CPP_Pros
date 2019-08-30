#pragma once

/*
 ˵��: OC�㷨 ~ ����
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
        void stepInit() { this->bind2Steps_.clear(); }                                                              // ����ʱ����
        void stepInput(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput);   // ���������Ϣ
        int stepOutput(int bindIndex, std::vector<GammaAlg::GammaInputInfoReg>& steps) const ;                             // ���������Ϣ   0 ~ ����ɹ�; 1 ~ ���ʧ��
        const std::map<int, std::vector<GammaAlg::GammaInputInfoReg>>& getAllSteps() const { return this->bind2Steps_; }

    private:
        std::string pID_;
        std::map<int, std::vector<GammaAlg::GammaInputInfoReg>> bind2Steps_;
    };

    class OCBase
    {
    public:
        OCBase(const std::string pID) : pID_(pID), allBindNum_(-1), maxRegStep_(-1), maxVolStep_(-1), bandSwitched_(false), bindSwitched_(false), bandNoLast_(-1), bindIndexLast_(-1) {}
        void gammaInit(const GammaAlg::GammaInitData& gammaInitData);                                                   // ͨ�ó�ʼ�� ~ 1��/IC
        virtual void gammaSetPara(const std::string filename) {}                                                        // ���Գ�ʼ�� ~ �����ھ����㷨 ~ 1��/IC
        virtual void gammaOneStepCalc(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput);   // ��Ҫ֮Ԥ�Ȳ��� ~ 1��/Step

    protected:
        void restrictRGBReg(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput);  // Լ��RGB�Ĵ���
        void restrictRGBVol(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput);  // Լ��RGB��ѹ
    protected:
        std::string pID_;
        int allBindNum_;                                                                                                // ���а����
        int maxRegStep_;
        double maxVolStep_;
        std::map<int, GammaAlg::GammaOneUnitData> bind2Unit_;
        bool bandSwitched_;                                                                                             // band�л����
        bool bindSwitched_;                                                                                             // bind�л����

    private:
        virtual void resetInit() {}                                                                                     // �ڲ������ʼ��״̬����
    private:
        int bandNoLast_;                                                                                                // ��һ��band��
        int bindIndexLast_;                                                                                             // ��һ��bind��
    };

    extern std::map<const std::string, OCShare> g_pID2OCShare;                                                          // ����OCShareʵ��
}