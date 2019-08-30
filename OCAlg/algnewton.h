#pragma once

/*
 ˵��: OC�㷨 ~ ţ�ٵ�����(�����ſɱȾ���)
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

    // ��ѹת���㷨
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
        std::map<int, GammaAlg::RGBVolValue> bind2volValue_;                                 // ��֪�������ĵ�ѹ -> �����ϸ��빲������е����������Ĵ�����ӳ
    };

    // OC�㷨(ţ�ٵ�����)
    class OCNewton : public OCBase
    {
    public:
        OCNewton(const std::string pID) : OCBase(pID) {};
        virtual void gammaSetPara(const std::string filename);
        virtual void gammaOneStepCalc(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput);

    private:
        void getJMat();                                                                      // ��ȡJacobian���� ~ �������ܺ궨��
        void parseFile(std::string& content);
        void buildJMat(const std::string& content);
        virtual void resetInit() { this->volConversion_.conversionInit(); }
        void xylv2uvlv(const GammaAlg::CIExylvInfo& xylv, CIEuvlvInfo& uvlv);                // xylvתuvlv
        void updateJMat(const GammaAlg::GammaInputInfoReg& gammaInput);                      // ����Jacobian���� ~ ����������º궨��
        void buildDeltaMatOnReg(const GammaAlg::GammaInputInfoReg& gammaInput, Eigen::MatrixXd& regOrVolDeltaMat, Eigen::MatrixXd& uvlvDeltaMat);
        void buildDeltaMatOnVol(const GammaAlg::GammaInputInfoReg& gammaInput, Eigen::MatrixXd& regOrVolDeltaMat, Eigen::MatrixXd& uvlvDeltaMat);
        void updateRank1(Eigen::Matrix3d& XT_X, Eigen::Matrix3d& YT_X, Eigen::Matrix3d& JMat);   // ��1����
        void updateRank2(Eigen::Matrix3d& XT_X, Eigen::Matrix3d& YT_X, Eigen::Matrix3d& JMat);   // ��2����
        void updateRank3(Eigen::Matrix3d& XT_X, Eigen::Matrix3d& YT_X, Eigen::Matrix3d& JMat);   // ��3����
    private:
        std::string filename_;
        std::map<std::array<int, 2>, Eigen::Matrix3d> bandGray2JMat_;
        RegVolConversion volConversion_;                                                     // ��ѹת�� ~ ������ѹ����궨��
    };

    extern std::map<const std::string, OCNewton> g_pID2OCNewton;                             // ����OCNewtonʵ��
}