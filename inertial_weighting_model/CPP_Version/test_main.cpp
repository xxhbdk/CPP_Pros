#include "ivp_iw.h"
#include <iostream>

int main() {
    IVP::IWAlgo obj;
    obj.loadCSV();
    obj.iwAutoPara();

    GammaAlgo::IVPType IVPData;
    IVPData.bindIdx_ = 1;

    obj.predInitialRGB(IVPData);
    std::cout << IVPData.regValue_.rRegValue_;
    std::cout << IVPData.regValue_.gRegValue_;
    std::cout << IVPData.regValue_.bRegValue_;
}