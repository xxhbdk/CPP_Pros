#include "boe_oc.h"

namespace BOE {

    void OCSingleStepAlg1::setAllBinds(const GammaAlgo::GammaInitData& initData) {
        this->errorAllBinds_.clear();

        for (int i = 0; i < initData.maxbindnum_; ++i) {
            GammaAlgo::GammaOneUnitData gammaOneUnitData = initData.pGammaAllData_[i];
            std::string bindIdentifier = getBindIdentifier(gammaOneUnitData.bandno_, gammaOneUnitData.gray_);
            this->errorAllBinds_[bindIdentifier] = gammaOneUnitData.errorConfig_;
        }
    }

    void OCSingleStepAlg1::updateStepsize(const StepOneBind& stepOneBind) {
        this->stepAllBinds_ = stepOneBind;
    }

    inline std::string OCSingleStepAlg1::getBindIdentifier(int bandIdx, int grayVal) {
        std::string bindIdentifier = std::to_string(bandIdx) + " " + std::to_string(grayVal);
        return bindIdentifier;
    }

    void OCSingleStepAlg1::singleIter(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        outputOneBind.regValue_.rRegValue_ = inputOneBind.regValue_.rRegValue_;
        outputOneBind.regValue_.gRegValue_ = inputOneBind.regValue_.gRegValue_;
        outputOneBind.regValue_.bRegValue_ = inputOneBind.regValue_.bRegValue_;
        outputOneBind.volValue_.rVolValue_ = inputOneBind.volValue_.rVolValue_;
        outputOneBind.volValue_.gVolValue_ = inputOneBind.volValue_.gVolValue_;
        outputOneBind.volValue_.bVolValue_ = inputOneBind.volValue_.bVolValue_;

        std::string bindIdentifier = getBindIdentifier(inputOneBind.band_, inputOneBind.gray_);
        std::map<std::string, GammaAlgo::GammaErrorConfig>::iterator iter = this->errorAllBinds_.find(bindIdentifier);

        if (iter != this->errorAllBinds_.end()) {
            GammaAlgo::GammaErrorConfig& errorOneBind = iter->second;
            tuningOneStep(inputOneBind, errorOneBind, outputOneBind);
        }

    }

    void OCSingleStepAlg1::tuningOneStep(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaErrorConfig& errorOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        switch (this->tuningTab_) {
        case 0:
            if (tuninglv(inputOneBind, errorOneBind, outputOneBind))
                break;
            else
                ++(this->tuningTab_);
        case 1:
            if (tuningx(inputOneBind, errorOneBind, outputOneBind))
                break;
            else
                ++(this->tuningTab_);
        case 2:
            if (tuningy(inputOneBind, errorOneBind, outputOneBind))
                break;
            else
                this->tuningTab_ = 0;
        default:
            break;
        }
    }

    int OCSingleStepAlg1::tuninglv(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaErrorConfig& errorOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double lvCurr = static_cast<double>(inputOneBind.realLvxy_.lv_);
        double lvTarget = static_cast<double>(errorOneBind.targetxylv_.lv_);

        if (lvCurr <= (lvTarget + errorOneBind.lvRightError_) && lvCurr >= (lvTarget - errorOneBind.lvLeftError_))
            return 0;

        double valLoc = lvCurr / lvTarget;
        double bandRatio = this->stepAllBinds_.bandRatio_;
        std::vector<double>& lvBarrier = this->stepAllBinds_.lvBarrier_;
        std::vector<int>& lvStepsize = this->stepAllBinds_.lvStepsize_;

        int stepsize = getStepsize(valLoc, lvBarrier, bandRatio, lvStepsize);
        outputOneBind.regValue_.gRegValue_ += stepsize;

        return 1;
    }

    int OCSingleStepAlg1::getStepsize(double valLoc, std::vector<double>& barrier, double bandRatio, std::vector<int>& stepsize) {
        size_t idx = 0;

        if (valLoc >= *(--barrier.end()))
            idx = barrier.size();
        else {
            while (barrier[idx] <= valLoc)
                ++idx;
        }
        return static_cast<int>(round(stepsize[idx] * bandRatio));
    }

    int OCSingleStepAlg1::tuningx(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaErrorConfig& errorOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double xCurr = static_cast<double>(inputOneBind.realLvxy_.x_);
        double xTarget = static_cast<double>(errorOneBind.targetxylv_.x_);

        if (xCurr <= (xTarget + errorOneBind.xRightError_) && xCurr >= (xTarget - errorOneBind.xLeftError_))
            return 0;

        double valLoc = xCurr;
        double bandRatio = this->stepAllBinds_.bandRatio_;
        std::vector<double>& xBarrier = this->stepAllBinds_.xBarrier_;
        std::vector<int>& xStepsize = this->stepAllBinds_.xStepsize_;

        int stepsize = getStepsize(valLoc, xBarrier, bandRatio, xStepsize);
        outputOneBind.regValue_.rRegValue_ += stepsize;

        return 1;
    }

    int OCSingleStepAlg1::tuningy(const GammaAlgo::GammaInputInfoReg& inputOneBind, GammaAlgo::GammaErrorConfig& errorOneBind, GammaAlgo::GammaOutputInfoReg& outputOneBind) {
        double yCurr = static_cast<double>(inputOneBind.realLvxy_.y_);
        double yTarget = static_cast<double>(errorOneBind.targetxylv_.y_);

        if (yCurr <= (yTarget + errorOneBind.yRightError_) && yCurr >= (yTarget - errorOneBind.yLeftError_))
            return 0;

        double valLoc = yCurr;
        double bandRatio = this->stepAllBinds_.bandRatio_;
        std::vector<double>& yBarrier = this->stepAllBinds_.yBarrier_;
        std::vector<int>& yStepsize = this->stepAllBinds_.yStepsize_;

        int stepsize = getStepsize(valLoc, yBarrier, bandRatio, yStepsize);
        outputOneBind.regValue_.bRegValue_ += stepsize;

        return 1;
    }

	 void OCSingleStepAlg1::split_string_to_string(const std::string& s, std::vector<std::string>& sv, const char flag)
	{
		
		sv.clear();
		std::istringstream  iss(s);	

		std::string temp;

		while (std::getline(iss, temp, flag))
		{
			sv.push_back(temp.c_str());
		}
		return;
	}
}









