#include <cmath>
#include <string>

#include "boe_oc.h"

namespace BOE {

    void OCSingleStepAlg2::pushOneBind(int bandIdx, int grayVal, ConfigOneBind2& configOneBind) {
        std::string bindIdentifier = getBindIdentifier(bandIdx, grayVal);
        this->allBindsConfig_map[bindIdentifier] = configOneBind;
    }

    std::string OCSingleStepAlg2::getBindIdentifier(int bandIdx, int grayVal) {
        std::string bindIdentifier = std::to_string(bandIdx) + " " + std::to_string(grayVal);
        return bindIdentifier;
    }

    int OCSingleStepAlg2::singleIter(int bandIdx, int grayVal, InputMSGOneBind2& inputOneBind, OutputMSGOneBind& outputOneBind) {
        outputOneBind.rVolValue = inputOneBind.rVolValue;
        outputOneBind.gVolValue = inputOneBind.gVolValue;
        outputOneBind.bVolValue = inputOneBind.bVolValue;

        int convergedTab = 0;                                                             // 收敛标记: 0 - 未收敛; 1 - 收敛
        std::string bindIdentifier = getBindIdentifier(bandIdx, grayVal);
        std::map<std::string, ConfigOneBind2>::iterator iter = this->allBindsConfig_map.find(bindIdentifier);

        if (iter != this->allBindsConfig_map.end()) {
            ConfigOneBind2& currConfig = iter->second;
            tuningOneStep(inputOneBind, currConfig, outputOneBind, convergedTab);
        }

        return convergedTab;
    }

    void OCSingleStepAlg2::tuningOneStep(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind, int& convergedTab) {
        switch (this->tuningTab) {
        case -1:
            tuningInit(currConfig, outputOneBind);
            ++tuningTab;
            break;
        case 0:
            if (tuningrLv(inputOneBind, currConfig, outputOneBind))
                break;
            else
                ++(this->tuningTab);
        case 1:
            if (tuninggLv(inputOneBind, currConfig, outputOneBind))
                break;
            else
                ++(this->tuningTab);
        case 2:
            if (tuningbLv(inputOneBind, currConfig, outputOneBind))
                break;
            else
                this->tuningTab = 0;
        default:
            checkSpec(inputOneBind, currConfig, convergedTab);                // 检查spec, 并修改convergedTab状态
        }
    }

    void OCSingleStepAlg2::tuningInit(ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind) {
        outputOneBind.rVolValue = newtonIter(currConfig.rCurveParams[0], currConfig.rCurveParams[1], currConfig.rCurveParams[2], currConfig.rCurveParams[3], currConfig.rLvTarget);
        outputOneBind.gVolValue = newtonIter(currConfig.gCurveParams[0], currConfig.gCurveParams[1], currConfig.gCurveParams[2], currConfig.gCurveParams[3], currConfig.gLvTarget);
        outputOneBind.bVolValue = newtonIter(currConfig.bCurveParams[0], currConfig.bCurveParams[1], currConfig.bCurveParams[2], currConfig.bCurveParams[3], currConfig.bLvTarget);
    }

    int OCSingleStepAlg2::tuningrLv(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind) {
        double& rLvCurr = inputOneBind.rLv;
        double& rLvTarget = currConfig.rLvTarget;
        double& rLvSpec = currConfig.rLvSpec;

        if (fabs((rLvCurr - rLvTarget) / rLvTarget) <= rLvSpec)
            return 0;

        double rVolTmp = newtonIter(currConfig.rCurveParams[0], currConfig.rCurveParams[1], currConfig.rCurveParams[2], currConfig.rCurveParams[3], rLvCurr);
        double rVol_delta = fabs(rVolTmp - inputOneBind.rVolValue);

        if (rLvCurr < rLvTarget)
            outputOneBind.rVolValue -= rVol_delta;
        else
            outputOneBind.rVolValue += rVol_delta;

        return 1;
    }

    int OCSingleStepAlg2::tuninggLv(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind) {
        double& gLvCurr = inputOneBind.gLv;
        double& gLvTarget = currConfig.gLvTarget;
        double& gLvSpec = currConfig.gLvSpec;

        if (fabs((gLvCurr - gLvTarget) / gLvTarget) <= gLvSpec)
            return 0;

        double gVolTmp = newtonIter(currConfig.gCurveParams[0], currConfig.gCurveParams[1], currConfig.gCurveParams[2], currConfig.gCurveParams[3], gLvCurr);
        double gVol_delta = fabs(gVolTmp - inputOneBind.gVolValue);

        if (gLvCurr < gLvTarget)
            outputOneBind.gVolValue -= gVol_delta;
        else
            outputOneBind.gVolValue += gVol_delta;

        return 1;
    }

    int OCSingleStepAlg2::tuningbLv(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, OutputMSGOneBind& outputOneBind) {
        double& bLvCurr = inputOneBind.bLv;
        double& bLvTarget = currConfig.bLvTarget;
        double& bLvSpec = currConfig.bLvSpec;

        if (fabs((bLvCurr - bLvTarget) / bLvTarget) <= bLvSpec)
            return 0;

        double bVolTmp = newtonIter(currConfig.bCurveParams[0], currConfig.bCurveParams[1], currConfig.bCurveParams[2], currConfig.bCurveParams[3], bLvCurr);
        double bVol_delta = fabs(bVolTmp - inputOneBind.bVolValue);

        if (bLvCurr < bLvTarget)
            outputOneBind.bVolValue -= bVol_delta;
        else
            outputOneBind.bVolValue += bVol_delta;

        return 1;
    }

    void OCSingleStepAlg2::checkSpec(InputMSGOneBind2& inputOneBind, ConfigOneBind2& currConfig, int& convergedTab) {
        int rLvInSpec = 0, gLvInSpec = 0, bLvInSpec = 0;

        double& rLvCurr = inputOneBind.rLv;
        double& rLvTarget = currConfig.rLvTarget;
        double& rLvSpec = currConfig.rLvSpec;

        if (fabs((rLvCurr - rLvTarget) / rLvTarget) <= rLvSpec)
            rLvInSpec = 1;

        double& gLvCurr = inputOneBind.gLv;
        double& gLvTarget = currConfig.gLvTarget;
        double& gLvSpec = currConfig.gLvSpec;

        if (fabs((gLvCurr - gLvTarget) / gLvTarget) <= gLvSpec)
            gLvInSpec = 1;

        double& bLvCurr = inputOneBind.bLv;
        double& bLvTarget = currConfig.bLvTarget;
        double& bLvSpec = currConfig.bLvSpec;

        if (fabs((bLvCurr - bLvTarget) / bLvTarget) <= bLvSpec)
            bLvInSpec = 1;

        if (rLvInSpec && gLvInSpec && bLvInSpec)
            convergedTab = 1;
    }

    double OCSingleStepAlg2::newtonIter(double a0, double a1, double a2, double a3, double b, double epsilon) {
        double soluOld = 0;
        double soluNew = -func(a0, a1, a2, a3, b, soluOld) / slope(a0, a1, a2, soluOld) + soluOld;

        while (fabs(soluNew - soluOld) >= epsilon) {
            soluOld = soluNew;
            soluNew = -func(a0, a1, a2, a3, b, soluOld) / slope(a0, a1, a2, soluOld) + soluOld;
        }

        return soluNew;
    }
    
    inline double OCSingleStepAlg2::func(double a0, double a1, double a2, double a3, double b, double x) {
        return a0 * pow(x, 3) + a1 * pow(x, 2) + a2 * x + a3 - b;
    }

    inline double OCSingleStepAlg2::slope(double a0, double a1, double a2, double x) {
        return 3 * a0 * pow(x, 2) + 2 * a1 * x + a2;
    }
}