#pragma once

/*
 ˵��:
 1. �ⲿ���ٶ�̬�ռ�����ʵ��������е�������
 2. һ����һ��ʵ������
 3. ����״̬�������洢��ʵ���ڲ�
 һ���߳� -> һ���� -> һ��ʵ��(ȷ��״̬����Ψһ)
 �ű����㷨
*/

/*
 BOE�㷨����
 ���߼̳��㷨��
*/

#include <string>
#include <vector>
#include <map>

#include "gamma_algo_interface.h"

namespace BOE {

    class OCAlgoBase {
    public:
        OCAlgoBase();
    };

    class OCSingleStepAlg2 {
    public:
        OCSingleStepAlg2();
        void initAllBinds(const GammaAlgo::GammaInitData& gammaInitData);
        void updateParams(int fitOrder);
        void updateParams(double* rgbRatio);
        void updateParams(int fitOrder, double* rgbRatio);

    private:
        int maxbindnum_;
        int maxRegStep_;
        double maxVolStep_;
        std::map<int, GammaAlgo::GammaOneUnitData> gammaAllData_;
        std::string filename_;
        int fitOrder_;
        double rgbRatio_[3];
    private:
        void 
    };


}











