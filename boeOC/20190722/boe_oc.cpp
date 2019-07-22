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
        
        this->resetTuningTab();
    }

    void OCBase::gammaProgress(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        outputOneBind.regValue_.rRegValue_ = inputOneBind.regValue_.rRegValue_;
        outputOneBind.regValue_.gRegValue_ = inputOneBind.regValue_.gRegValue_;
        outputOneBind.regValue_.bRegValue_ = inputOneBind.regValue_.bRegValue_;
        outputOneBind.volValue_.rVolValue_ = inputOneBind.volValue_.rVolValue_;
        outputOneBind.volValue_.gVolValue_ = inputOneBind.volValue_.gVolValue_;
        outputOneBind.volValue_.bVolValue_ = inputOneBind.volValue_.bVolValue_;

        if (inputOneBind.grayindex_ != this->lastGrayIndex_) {
            this->resetTuningTab();
            this->lastGrayIndex_ = inputOneBind.grayindex_;
        }
    }

    void OCBase::split_string_to_string(const std::string& s, std::vector<std::string>& sv, const char flag) {
        sv.clear();
        std::istringstream  iss(s);

        std::string temp;

        while (std::getline(iss, temp, flag))
        {
            sv.push_back(temp.c_str());
        }
    }

    void OCAlgo1::gammaSetPara(double bandRatio_, const std::vector<double>& lvBarrier_, const std::vector<int>& lvStepsize_, const std::vector<double>& xBarrier_, const std::vector<int>& xStepsize_, const std::vector<double>& yBarrier_, const std::vector<int>& yStepsize_) {
        this->bandRatio_ = bandRatio_;
        this->lvBarrier_ = lvBarrier_;
        this->lvStepsize_ = lvStepsize_;
        this->xBarrier_ = xBarrier_;
        this->xStepsize_ = xStepsize_;
        this->yBarrier_ = yBarrier_;
        this->yStepsize_ = yStepsize_;
    }

    void OCAlgo1::gammaProgress(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        OCBase::gammaProgress(inputOneBind, outputOneBind);

        std::map<int, GammaAlgo::GammaOneUnitData>::iterator iter = this->gammaAllData_.find(inputOneBind.grayindex_);

        if (iter != this->gammaAllData_.end()) {
            GammaAlgo::GammaOneUnitData& currConfig = iter->second;
            this->tuningOneStep(inputOneBind, currConfig, outputOneBind);
        }
    }

    void OCAlgo1::tuningOneStep(const GammaAlgo::GammaInputInfoReg& inputOneBind, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        switch (this->tuningTab_) {
        case 0:
            if (this->tuningLv(inputOneBind, currConfig, outputOneBind))
                break;
            else
                ++(this->tuningTab_);
        case 1:
            if (this->tuningx(inputOneBind, currConfig, outputOneBind))
                break;
            else
                ++(this->tuningTab_);
        case 2:
            if (this->tuningy(inputOneBind, currConfig, outputOneBind))
                break;
            else
                this->tuningTab_ = 0;
        }
    }

    int OCAlgo1::tuningLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double lvCurr = inputOneBind.realLvxy_.lv_;
        double lvTarget = currConfig.errorConfig_.targetxylv_.lv_;
        double lvLeftError = currConfig.errorConfig_.lvLeftError_;
        double lvRightError = currConfig.errorConfig_.lvRightError_;

        if (lvCurr <= (lvTarget + lvRightError) && lvCurr >= (lvTarget - lvLeftError))
            return 0;

        double valLoc = lvCurr / lvTarget;
        int stepsize = this->getStepsize(valLoc, this->bandRatio_, this->lvBarrier_, this->lvStepsize_);
        outputOneBind.regValue_.gRegValue_ += stepsize;

        return 1;
    }

    int OCAlgo1::tuningx(const GammaAlgo::GammaInputInfoReg& inputOneBind, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double xCurr = inputOneBind.realLvxy_.x_;
        double xTarget = currConfig.errorConfig_.targetxylv_.x_;
        double xLeftError = currConfig.errorConfig_.xLeftError_;
        double xRightError = currConfig.errorConfig_.xRightError_;

        if (xCurr <= (xTarget + xRightError) && xCurr >= (xTarget - xLeftError))
            return 0;

        double valLoc = xCurr;
        int stepsize = this->getStepsize(valLoc, this->bandRatio_, this->xBarrier_, this->xStepsize_);
        outputOneBind.regValue_.rRegValue_ += stepsize;

        return 1;
    }

    int OCAlgo1::tuningy(const GammaAlgo::GammaInputInfoReg& inputOneBind, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double yCurr = inputOneBind.realLvxy_.y_;
        double yTarget = currConfig.errorConfig_.targetxylv_.y_;
        double yLeftError = currConfig.errorConfig_.yLeftError_;
        double yRightError = currConfig.errorConfig_.yRightError_;

        if (yCurr <= (yTarget + yRightError) && yCurr >= (yTarget - yLeftError))
            return 0;

        double valLoc = yCurr;
        int stepsize = this->getStepsize(valLoc, this->bandRatio_, this->yBarrier_, this->yStepsize_);
        outputOneBind.regValue_.bRegValue_ += stepsize;

        return 1;
    }

    int OCAlgo1::getStepsize(double valLoc, double bandRatio, const std::vector<double>& barrier, const std::vector<int>& stepsize) {
        size_t idx = 0;

        if (valLoc >= *(--barrier.end()))
            idx = barrier.size();
        else {
            while (barrier[idx] <= valLoc)
                ++idx;
        }

        return static_cast<int>(round(stepsize[idx] * bandRatio));
    }

    void OCAlgo2::gammaSetPara(std::string filename, int fitOrder, double* rgbRatio, std::string tablename) {
        this->filename_ = filename;
        this->fitOrder_ = fitOrder;
        this->rgbRatio_[0] = rgbRatio[0];
        this->rgbRatio_[1] = rgbRatio[1];
        this->rgbRatio_[2] = rgbRatio[2];
        this->tablename_ = tablename;

        this->band2curve_.clear();
        this->curveFitting();
        this->tableParse();
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

        int bandIdx = -1, bandOld = -1;
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
            for (int j = 0; j < curveParams.bindNum; ++j) {      //matrix from Tabpoint 255~1 (Voltage)
                tmpA_r(i, j) = pow(rVolVec[j], currOrder);
                tmpA_g(i, j) = pow(gVolVec[j], currOrder);
                tmpA_b(i, j) = pow(bVolVec[j], currOrder);
            }
        }
        // ¢Ú
        for (int i = 0; i < curveParams.bindNum; ++i) {          //matrix Lv(Tabpoint) x RGB ratio 
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
        curveParams.rCurveParams.E = tmpE;                       // unit dig. matrix 
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
        OCBase::gammaProgress(inputOneBind, outputOneBind);

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
        double xCurr = inputOneBind.realLvxy_.x_;
        double yCurr = inputOneBind.realLvxy_.y_;
        double rRatio = this->getRatio(xCurr, yCurr, this->ratioMats_.rMat_);

        double rLvCurr = inputOneBind.realLvxy_.lv_ * rRatio;
        double rLvTarget = currConfig.errorConfig_.targetxylv_.lv_ * rRatio;
        double rLvLeftError = currConfig.errorConfig_.lvLeftError_ * rRatio;
        double rLvRightError = currConfig.errorConfig_.lvRightError_ * rRatio;

        if (rLvCurr <= (rLvTarget + rLvRightError) && rLvCurr >= (rLvTarget - rLvLeftError))
            return 0;

        double rVolTmp = newtonIter(currCurve.rCurveParams.curveParams, rLvCurr);
        outputOneBind.volValue_.rVolValue_ = rVolTmp;

        return 1;
    }

    int OCAlgo2::tuninggLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double xCurr = inputOneBind.realLvxy_.x_;
        double yCurr = inputOneBind.realLvxy_.y_;
        double gRatio = this->getRatio(xCurr, yCurr, this->ratioMats_.gMat_);

        double gLvCurr = inputOneBind.realLvxy_.lv_ * gRatio;
        double gLvTarget = currConfig.errorConfig_.targetxylv_.lv_ * gRatio;
        double gLvLeftError = currConfig.errorConfig_.lvLeftError_ * gRatio;
        double gLvRightError = currConfig.errorConfig_.lvRightError_ * gRatio;

        if (gLvCurr <= (gLvTarget + gLvRightError) && gLvCurr >= (gLvTarget - gLvLeftError))
            return 0;

        double gVolTmp = newtonIter(currCurve.gCurveParams.curveParams, gLvCurr);
        outputOneBind.volValue_.gVolValue_ = gVolTmp;

        return 1;
    }

    int OCAlgo2::tuningbLv(const GammaAlgo::GammaInputInfoReg& inputOneBind, const CurveParams& currCurve, const GammaAlgo::GammaOneUnitData& currConfig, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double xCurr = inputOneBind.realLvxy_.x_;
        double yCurr = inputOneBind.realLvxy_.y_;
        double bRatio = this->getRatio(xCurr, yCurr, this->ratioMats_.bMat_);

        double bLvCurr = inputOneBind.realLvxy_.lv_ * bRatio;
        double bLvTarget = currConfig.errorConfig_.targetxylv_.lv_ * bRatio;
        double bLvLeftError = currConfig.errorConfig_.lvLeftError_ * bRatio;
        double bLvRightError = currConfig.errorConfig_.lvRightError_ * bRatio;

        if (bLvCurr <= (bLvTarget + bLvRightError) && bLvCurr >= (bLvTarget - bLvLeftError))
            return 0;

        double bVolTmp = newtonIter(currCurve.bCurveParams.curveParams, bLvCurr);
        outputOneBind.volValue_.bVolValue_ = bVolTmp;

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

    void OCAlgo2::tableParse() {
        std::ifstream fin(this->tablename_);
        std::string line;

        int rows = 0;
        int cols = 0;
        std::vector<std::vector<double>> rRatioVec;
        std::vector<std::vector<double>> gRatioVec;
        std::vector<std::vector<double>> bRatioVec;
        char tab = '\0';
        while (std::getline(fin, line)) {
            if (line[0] == 'R') {
                tab = 'R';
                continue;
            }
            else if (line[0] == 'G') {
                tab = 'G';
                continue;
            }
            else if (line[0] == 'B') {
                tab = 'B';
                continue;
            }

            if (tab == 'R')
                this->dataExtract2(line, rRatioVec);
            else if (tab == 'G')
                this->dataExtract2(line, gRatioVec);
            else if (tab == 'B')
                this->dataExtract2(line, bRatioVec);

        }

        fin.close();

        this->buildMatrix2(rRatioVec, gRatioVec, bRatioVec);
    }

    void OCAlgo2::dataExtract2(std::string& line, std::vector<std::vector<double>>& monocVec) {
        std::string ele;
        std::vector<double> eleVec;
        std::istringstream sin(line);

        while (std::getline(sin, ele, ',')) {
            if (!ele.empty())
                eleVec.push_back(std::stod(ele));
        }

        monocVec.push_back(eleVec);
    }

    void OCAlgo2::buildMatrix2(std::vector<std::vector<double>>& rRatioVec, std::vector<std::vector<double>>& gRatioVec, std::vector<std::vector<double>>& bRatioVec) {
        Eigen::MatrixXd tmpRMat(rRatioVec.size(), rRatioVec[0].size());
        Eigen::MatrixXd tmpGMat(gRatioVec.size(), gRatioVec[0].size());
        Eigen::MatrixXd tmpBMat(bRatioVec.size(), bRatioVec[0].size());

        for (int row = 0; row < tmpRMat.rows(); ++row)
            for (int col = 0; col < tmpRMat.cols(); ++col)
                tmpRMat(row, col) = rRatioVec[row][col];

        for (int row = 0; row < tmpGMat.rows(); ++row)
            for (int col = 0; col < tmpGMat.cols(); ++col)
                tmpGMat(row, col) = gRatioVec[row][col];

        for (int row = 0; row < tmpBMat.rows(); ++row)
            for (int col = 0; col < tmpBMat.cols(); ++col)
                tmpBMat(row, col) = bRatioVec[row][col];

        this->ratioMats_.rMat_ = tmpRMat;
        this->ratioMats_.gMat_ = tmpGMat;
        this->ratioMats_.bMat_ = tmpBMat;
    }

    double OCAlgo2::getRatio(double x, double y, const Eigen::MatrixXd& monocMat) {
        int row = this->getRowIndex(y, monocMat);
        int col = this->getColIndex(x, monocMat);

        return monocMat(row, col);
    }

    int OCAlgo2::getRowIndex(double y, const Eigen::MatrixXd& monocMat) {
        int row = 0;

        while (row < monocMat.rows()) {
            if (y >= monocMat(row, 0))
                break;
            ++row;
        }

        if (row > 0) {
            if (y > (monocMat(row - 1, 0) + monocMat(row, 0)) / 2)
                row -= 1;
        }

        return row;
    }

    int OCAlgo2::getColIndex(double x, const Eigen::MatrixXd& monocMat) {
        int col = 0;

        while (col < monocMat.cols()) {
            if (x <= monocMat(monocMat.rows() - 1, col))
                break;
            ++col;
        }

        if (col > 0) {
            if (x < (monocMat(monocMat.rows() - 1, col - 1) + monocMat(monocMat.rows() - 1, col)) / 2)
                col -= 1;
        }

        return col;
    }

}