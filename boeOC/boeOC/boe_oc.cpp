#include <iostream>
#include <fstream>
#include <sstream>

#include "boe_oc.h"

namespace BOE {

    void OCBase::gammaInit(const GammaAlgo::GammaInitData& initData) {
        this->maxbindnum_ = initData.maxbindnum_;
        this->maxRegStep_ = initData.maxRegStep_;
        this->maxVolStep_ = initData.maxVolStep_;
        this->gammaAllData_.clear();

        for (int i = 0; i < initData.maxbindnum_; ++i) {
            GammaAlgo::GammaOneUnitData& gammaOneUnitData = initData.pGammaAllData_[i];
            this->gammaAllData_[gammaOneUnitData.grayindex_] = gammaOneUnitData;
        }
    }

    void OCAlgo2::gammaSetPara(std::string filename, int fitOrder, double* rgbRatio) {
        this->filename_ = filename;
        this->fitOrder_ = fitOrder;
        this->rgbRatio_[0] = rgbRatio[0];
        this->rgbRatio_[1] = rgbRatio[1];
        this->rgbRatio_[2] = rgbRatio[2];

        this->band2curve_.clear();
        this->curveFitting();
    }

    void OCAlgo2::curveFitting() {
        this->fileParse();
        this->polyfit();
    }

    void OCAlgo2::polyfit(double epsilon) {
        for (auto& iter : this->band2curve_) {
            iter.second.rCurveParams.curveParams = (iter.second.rCurveParams.A * iter.second.rCurveParams.A.transpose() + epsilon * iter.second.rCurveParams.E).inverse() * iter.second.rCurveParams.A * iter.second.rCurveParams.D;
            iter.second.gCurveParams.curveParams = (iter.second.gCurveParams.A * iter.second.gCurveParams.A.transpose() + epsilon * iter.second.gCurveParams.E).inverse() * iter.second.gCurveParams.A * iter.second.gCurveParams.D;
            iter.second.bCurveParams.curveParams = (iter.second.bCurveParams.A * iter.second.bCurveParams.A.transpose() + epsilon * iter.second.bCurveParams.E).inverse() * iter.second.bCurveParams.A * iter.second.bCurveParams.D;
        }
    }

    void OCAlgo2::fileParse() {
        std::ifstream fin(this->filename_);
        std::string line;

        int bandIdx, bandOld = -1;
        double lv;
        double rVol, gVol, bVol;

        std::getline(fin, line);
        int i = 0;
        std::vector<double> lvVec, rVolVec, gVolVec, bVolVec;
        while (std::getline(fin, line)) {
            this->dataExtract(line, bandIdx, lv, rVol, gVol, bVol);
            if (i != 0 && bandIdx != bandOld) {
                // ¢Ù
                this->buildMatrix(bandOld, lvVec, rVolVec, gVolVec, bVolVec);
                lvVec.clear(); rVolVec.clear(); gVolVec.clear(); bVolVec.clear();
            }
            lvVec.push_back(lv); rVolVec.push_back(rVol); gVolVec.push_back(gVol); bVolVec.push_back(bVol);
            bandOld = bandIdx;
            ++i;
        }
        // ¢Ú
        this->buildMatrix(bandIdx, lvVec, rVolVec, gVolVec, bVolVec);

        fin.close();
    }

    void OCAlgo2::buildMatrix(int bandIdx, std::vector<double>& lvVec, std::vector<double>& rVolVec, std::vector<double>& gVolVec, std::vector<double>& bVolVec) {
        CurveParams curveParams;
        curveParams.order = this->fitOrder_;
        curveParams.bindNum = lvVec.size();

        Eigen::MatrixXd tmpA_r(curveParams.order + 1, curveParams.bindNum);
        Eigen::VectorXd tmpD_r(curveParams.bindNum);
        Eigen::MatrixXd tmpA_g(curveParams.order + 1, curveParams.bindNum);
        Eigen::VectorXd tmpD_g(curveParams.bindNum);
        Eigen::MatrixXd tmpA_b(curveParams.order + 1, curveParams.bindNum);
        Eigen::VectorXd tmpD_b(curveParams.bindNum);
        Eigen::MatrixXd tmpE = Eigen::MatrixXd::Identity(curveParams.order + 1, curveParams.order + 1);

        for (int i = 0; i < curveParams.order + 1; ++i) {
            int currOrder = curveParams.order - i;
            // ¢Ù
            for (int j = 0; j < curveParams.bindNum; ++j) {
                tmpA_r(i, j) = pow(rVolVec[j], currOrder);
                tmpA_g(i, j) = pow(gVolVec[j], currOrder);
                tmpA_b(i, j) = pow(bVolVec[j], currOrder);
            }
        }
        // ¢Ú
        for (int i = 0; i < curveParams.bindNum; ++i) {
            tmpD_r(i) = lvVec[i] * this->rgbRatio_[0];
            tmpD_g(i) = lvVec[i] * this->rgbRatio_[1];
            tmpD_b(i) = lvVec[i] * this->rgbRatio_[2];
        }

        curveParams.rCurveParams.A = tmpA_r;
        curveParams.gCurveParams.A = tmpA_g;
        curveParams.bCurveParams.A = tmpA_b;
        curveParams.rCurveParams.D = tmpD_r;
        curveParams.gCurveParams.D = tmpD_g;
        curveParams.bCurveParams.D = tmpD_b;
        curveParams.rCurveParams.E = tmpE;
        curveParams.gCurveParams.E = tmpE;
        curveParams.bCurveParams.E = tmpE;

        this->band2curve_[bandIdx] = curveParams;
    }

    void OCAlgo2::dataExtract(std::string& line, int& bandIdx, double& lv, double& rVol, double& gVol, double& bVol) {
        int i = 0;
        std::istringstream sin(line);
        std::string ele;

        while (std::getline(sin, ele, ',')) {
            if (i == 0)
                bandIdx = std::stoi(ele);
            else if (i == 2)
                lv = std::stod(ele);
            else if (i == 6)
                rVol = std::stod(ele);
            else if (i == 7)
                gVol = std::stod(ele);
            else if (i == 8)
                bVol = std::stod(ele);
            ++i;
        }
    }

    void OCAlgo2::gammaProgress(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        outputOneBind.volValue_.rVolValue_ = inputOneBind.volValue_.rVolValue_;
        outputOneBind.volValue_.gVolValue_ = inputOneBind.volValue_.gVolValue_;
        outputOneBind.volValue_.bVolValue_ = inputOneBind.volValue_.bVolValue_;
        
        std::map<int, CurveParams>::iterator iter = this->band2curve_.find(inputOneBind.band_);
        std::map<int, GammaAlgo::GammaOneUnitData>::iterator iter2 = this->gammaAllData_.find(inputOneBind.grayindex_);

        if (iter != this->band2curve_.end() && iter2 != this->gammaAllData_.end()) {
            CurveParams& currCurve = iter->second;
            GammaAlgo::GammaOneUnitData& currConfig = iter2->second;
            
            this->tuningOneStep(inputOneBind, currCurve, currConfig, outputOneBind);
        }
    }

    void OCAlgo2::tuningOneStep(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        switch (this->tuningTab_) {
        case -1:
            this->tuningInit(currCurve, currConfig, outputOneBind);
            ++this->tuningTab_;
            break;
        case 0:
            if (this->tuningrLv(inputOneBind, currCurve, currConfig, outputOneBind))
                break;
            else
                ++this->tuningTab_;
        case 1:
            if (this->tuninggLv(inputOneBind, currCurve, currConfig, outputOneBind))
                break;
            else
                ++this->tuningTab_;
        case 2:
            if (this->tuningbLv(inputOneBind, currCurve, currConfig, outputOneBind))
                break;
            else
                this->tuningTab_ = 0;
        }
    }

    int OCAlgo2::tuningrLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double rLvCurr = inputOneBind.realLvxy_.lv_ * this->rgbRatio_[0];
        double rLvTarget = currConfig.errorConfig_.targetxylv_.lv_ * this->rgbRatio_[0];
        double rLvLeftError = currConfig.errorConfig_.lvLeftError_ * this->rgbRatio_[0];
        double rLvRightError = currConfig.errorConfig_.lvRightError_ * this->rgbRatio_[0];

        if (rLvCurr <= (rLvTarget + rLvRightError) && rLvCurr >= (rLvTarget - rLvLeftError))
            return 0;

        double rVolTmp = newtonIter(currCurve.rCurveParams.curveParams, rLvCurr);
        double rVolDelta = fabs(rVolTmp - inputOneBind.volValue_.rVolValue_);

        if (rLvCurr < rLvTarget)
            outputOneBind.volValue_.rVolValue_ -= rVolDelta;
        else
            outputOneBind.volValue_.rVolValue_ += rVolDelta;

        return 1;
    }

    int OCAlgo2::tuninggLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double gLvCurr = inputOneBind.realLvxy_.lv_ * this->rgbRatio_[1];
        double gLvTarget = currConfig.errorConfig_.targetxylv_.lv_ * this->rgbRatio_[1];
        double gLvLeftError = currConfig.errorConfig_.lvLeftError_ * this->rgbRatio_[1];
        double gLvRightError = currConfig.errorConfig_.lvRightError_ * this->rgbRatio_[1];

        if (gLvCurr <= (gLvTarget + gLvRightError) && gLvCurr >= (gLvTarget - gLvLeftError))
            return 0;

        double gVolTmp = newtonIter(currCurve.gCurveParams.curveParams, gLvCurr);
        double gVolDelta = fabs(gVolTmp - inputOneBind.volValue_.gVolValue_);

        if (gLvCurr < gLvTarget)
            outputOneBind.volValue_.gVolValue_ -= gVolDelta;
        else
            outputOneBind.volValue_.gVolValue_ += gVolDelta;

        return 1;
    }

    int OCAlgo2::tuningbLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double bLvCurr = inputOneBind.realLvxy_.lv_ * this->rgbRatio_[2];
        double bLvTarget = currConfig.errorConfig_.targetxylv_.lv_ * this->rgbRatio_[2];
        double bLvLeftError = currConfig.errorConfig_.lvLeftError_ * this->rgbRatio_[2];
        double bLvRightError = currConfig.errorConfig_.lvRightError_ * this->rgbRatio_[2];

        if (bLvCurr <= (bLvTarget + bLvRightError) && bLvCurr >= (bLvTarget - bLvLeftError))
            return 0;

        double bVolTmp = newtonIter(currCurve.bCurveParams.curveParams, bLvCurr);
        double bVolDelta = fabs(bVolTmp - inputOneBind.volValue_.bVolValue_);

        if (bLvCurr < bLvTarget)
            outputOneBind.volValue_.bVolValue_ -= bVolDelta;
        else
            outputOneBind.volValue_.bVolValue_ += bVolDelta;

        return 1;
    }

    double OCAlgo2::newtonIter(const Eigen::VectorXd& curveParams, double targetLv, double epsilon) {
        double soluOld = 0;
        if (fabs(this->polySlope(curveParams, soluOld)) < epsilon)
            return soluOld;
        double soluNew = -this->polyFunc(curveParams, targetLv, soluOld) / this->polySlope(curveParams, soluOld) + soluOld;

        while (fabs(soluNew - soluOld) >= epsilon) {
            soluOld = soluNew;
            if (fabs(this->polySlope(curveParams, soluOld)) < epsilon)
                return soluOld;
            soluNew = -this->polyFunc(curveParams, targetLv, soluOld) / this->polySlope(curveParams, soluOld) + soluOld;
        }
        
        return soluNew;
    }

    double OCAlgo2::polyFunc(const Eigen::VectorXd& curveParams, double targetLv, double x) {
        double funcVal = 0;
        for (int i = 0; i <= this->fitOrder_; ++i) {
            int currOrder = this->fitOrder_ - i;
            funcVal += curveParams(i) * pow(x, currOrder);
        }
        funcVal -= targetLv;

        return funcVal;
    }

    double OCAlgo2::polySlope(const Eigen::VectorXd& curveParams, double x) {
        double slope = 0;
        for (int i = 0; i < this->fitOrder_ - 1; ++i) {
            int currOrder = this->fitOrder_ - i;
            slope += curveParams(i) * currOrder * pow(x, currOrder - 1);
        }
        slope += curveParams(this->fitOrder_ - 1);

        return slope;
    }

    void OCAlgo2::tuningInit(const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        outputOneBind.volValue_.rVolValue_ = this->newtonIter(currCurve.rCurveParams.curveParams, currConfig.errorConfig_.targetxylv_.lv_ * this->rgbRatio_[0]);
        outputOneBind.volValue_.gVolValue_ = this->newtonIter(currCurve.gCurveParams.curveParams, currConfig.errorConfig_.targetxylv_.lv_ * this->rgbRatio_[1]);
        outputOneBind.volValue_.bVolValue_ = this->newtonIter(currCurve.bCurveParams.curveParams, currConfig.errorConfig_.targetxylv_.lv_ * this->rgbRatio_[2]);
    }



}