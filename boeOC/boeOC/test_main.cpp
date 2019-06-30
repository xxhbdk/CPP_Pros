#include "boe_oc.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, GammaAlgo::GammaInputInfoReg& inputOneBind) {
    out << "(" << inputOneBind.volValue_.rVolValue_ << ", " << inputOneBind.volValue_.gVolValue_ << ", " << inputOneBind.volValue_.bVolValue_ << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
    out << "(" << outputOneBind.volValue_.rVolValue_ << ", " << outputOneBind.volValue_.gVolValue_ << ", " << outputOneBind.volValue_.bVolValue_ << ")";
    return out;
}

int main() {
    BOE::OCAlgo2 algo2;

    GammaAlgo::GammaInitData initData;
    GammaAlgo::GammaOneUnitData gammaAllData[1];
    gammaAllData[0].bandno_ = 1;
    gammaAllData[0].gray_ = 255;
    gammaAllData[0].grayindex_ = 1;
    gammaAllData[0].errorConfig_.lvLeftError_ = 0.3;
    gammaAllData[0].errorConfig_.lvRightError_ = 0.3;
    gammaAllData[0].errorConfig_.targetxylv_.lv_ = 1202;
    initData.maxbindnum_ = 1;
    initData.pGammaAllData_ = gammaAllData;
    algo2.gammaInit(initData);

    std::string filename = "./CH1_REG_VOL_gam_tm.csv";                      // 拟合文件名
    int fitOrder = 3;                                                       // 多项式拟合阶数
    double rgbRatio[3] = { 0.7, 0.3, 0.1 };                                   // {rlv/lv, glv/lv, blv/lv}
    algo2.gammaSetPara(filename, fitOrder, rgbRatio);

    GammaAlgo::GammaInputInfoReg inputOneBind;
    inputOneBind.algMode_ = 0;
    inputOneBind.band_ = 1;
    inputOneBind.gray_ = 255;
    inputOneBind.grayindex_ = 1;
    inputOneBind.realLvxy_.lv_ = 1302;
    inputOneBind.volValue_.rVolValue_ = 1.8;
    inputOneBind.volValue_.gVolValue_ = 2.8;
    inputOneBind.volValue_.bVolValue_ = 1.5;
    GammaAlgo::GammaOutputInfoReg outputOneBind;
    algo2.gammaProgress(inputOneBind, outputOneBind);
    std::cout << inputOneBind << std::endl << outputOneBind << std::endl << std::endl;

    inputOneBind.realLvxy_.lv_ = 2302;
    inputOneBind.volValue_.rVolValue_ = 1.89763;
    inputOneBind.volValue_.gVolValue_ = 2.8126;
    inputOneBind.volValue_.bVolValue_ = 1.5875;
    algo2.gammaProgress(inputOneBind, outputOneBind);
    std::cout << inputOneBind << std::endl << outputOneBind;
}