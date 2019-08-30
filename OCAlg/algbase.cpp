#include "algbase.h"

std::map<const std::string, OCAlg::OCShare> OCAlg::g_pID2OCShare;

void OCAlg::OCShare::stepInput(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput)
{
    const GammaAlg::GammaInputInfoReg& currStep = gammaInput;
    GammaAlg::GammaInputInfoReg nextStep{gammaInput.count_ + 1, gammaInput.bandNo_, gammaInput.grayValue_, gammaInput.bindIndex_, gammaInput.algMode_, gammaOutput.regValue_, gammaOutput.volValue_ };
    
    int bindIndex = gammaInput.bindIndex_;
    auto iter = this->bind2Steps_.find(bindIndex);
    if (iter != this->bind2Steps_.end())
    {
        *(iter->second.end() - 1) = currStep;
        iter->second.push_back(nextStep);
    }
    else
    {
        this->bind2Steps_[bindIndex].assign({ currStep, nextStep });
    }
}

int OCAlg::OCShare::stepOutput(int bindIndex, std::vector<GammaAlg::GammaInputInfoReg>& steps) const
{
    auto iter = this->bind2Steps_.find(bindIndex);
    if (iter != this->bind2Steps_.end())
    {
        steps = iter->second;
        return 0;
    }

    return 1;
}

void OCAlg::OCBase::gammaInit(const GammaAlg::GammaInitData& gammaInitData)
{
    this->allBindNum_ = gammaInitData.allBindNum_;
    this->maxRegStep_ = gammaInitData.maxRegStep_;
    this->maxVolStep_ = gammaInitData.maxVolStep_;
    this->bind2Unit_.clear();

    for (int i = 0; i < gammaInitData.allBindNum_; ++i)
    {
        GammaAlg::GammaOneUnitData& unitData = gammaInitData.pGammaAllData_[i];
        this->bind2Unit_[unitData.bindIndex_] = unitData;
    }

    this->bandSwitched_ = true;
    this->bindSwitched_ = true;

    this->resetInit();
}

void OCAlg::OCBase::gammaOneStepCalc(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput)
{
    gammaOutput.regValue_.rRegValue_ = gammaInput.regValue_.rRegValue_;
    gammaOutput.regValue_.gRegValue_ = gammaInput.regValue_.gRegValue_;
    gammaOutput.regValue_.bRegValue_ = gammaInput.regValue_.bRegValue_;
    gammaOutput.volValue_.rVolValue_ = gammaInput.volValue_.rVolValue_;
    gammaOutput.volValue_.gVolValue_ = gammaInput.volValue_.gVolValue_;
    gammaOutput.volValue_.bVolValue_ = gammaInput.volValue_.bVolValue_;

    if (gammaInput.bandNo_ != this->bandNoLast_)
    {
        this->bandSwitched_ = true;
        this->bandNoLast_ = gammaInput.bandNo_;
    }
    else
    {
        this->bandSwitched_ = false;
    }

    if (gammaInput.bindIndex_ != this->bindIndexLast_)
    {
        this->bindSwitched_ = true;
        this->bindIndexLast_ = gammaInput.bindIndex_;
    }
    else
    {
        this->bindSwitched_ = false;
    }
}

void OCAlg::OCBase::restrictRGBReg(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput)
{
    int bindIndex = gammaInput.bindIndex_;

    auto iter = this->bind2Unit_.find(bindIndex);
    if (iter != this->bind2Unit_.end())
    {
        GammaAlg::RegOneUnitConfig& regConfig = iter->second.regConfig_;
        gammaOutput.regValue_.rRegValue_ = (gammaOutput.regValue_.rRegValue_ < regConfig.rRegConfig_.regMinValue_) ? regConfig.rRegConfig_.regMinValue_ : gammaOutput.regValue_.rRegValue_;
        gammaOutput.regValue_.gRegValue_ = (gammaOutput.regValue_.gRegValue_ < regConfig.gRegConfig_.regMinValue_) ? regConfig.gRegConfig_.regMinValue_ : gammaOutput.regValue_.gRegValue_;
        gammaOutput.regValue_.bRegValue_ = (gammaOutput.regValue_.bRegValue_ < regConfig.bRegConfig_.regMinValue_) ? regConfig.bRegConfig_.regMinValue_ : gammaOutput.regValue_.bRegValue_;
        gammaOutput.regValue_.rRegValue_ = (gammaOutput.regValue_.rRegValue_ > regConfig.rRegConfig_.regMaxValue_) ? regConfig.rRegConfig_.regMaxValue_ : gammaOutput.regValue_.rRegValue_;
        gammaOutput.regValue_.gRegValue_ = (gammaOutput.regValue_.gRegValue_ > regConfig.gRegConfig_.regMaxValue_) ? regConfig.gRegConfig_.regMaxValue_ : gammaOutput.regValue_.gRegValue_;
        gammaOutput.regValue_.bRegValue_ = (gammaOutput.regValue_.bRegValue_ > regConfig.bRegConfig_.regMaxValue_) ? regConfig.bRegConfig_.regMaxValue_ : gammaOutput.regValue_.bRegValue_;
    }

    gammaOutput.regValue_.rRegValue_ = (gammaOutput.regValue_.rRegValue_ < gammaInput.regValue_.rRegValue_ - this->maxRegStep_) ? gammaInput.regValue_.rRegValue_ - this->maxRegStep_ : gammaOutput.regValue_.rRegValue_;
    gammaOutput.regValue_.gRegValue_ = (gammaOutput.regValue_.gRegValue_ < gammaInput.regValue_.gRegValue_ - this->maxRegStep_) ? gammaInput.regValue_.gRegValue_ - this->maxRegStep_ : gammaOutput.regValue_.gRegValue_;
    gammaOutput.regValue_.bRegValue_ = (gammaOutput.regValue_.bRegValue_ < gammaInput.regValue_.bRegValue_ - this->maxRegStep_) ? gammaInput.regValue_.bRegValue_ - this->maxRegStep_ : gammaOutput.regValue_.bRegValue_;
    gammaOutput.regValue_.rRegValue_ = (gammaOutput.regValue_.rRegValue_ > gammaInput.regValue_.rRegValue_ + this->maxRegStep_) ? gammaInput.regValue_.rRegValue_ + this->maxRegStep_ : gammaOutput.regValue_.rRegValue_;
    gammaOutput.regValue_.gRegValue_ = (gammaOutput.regValue_.gRegValue_ > gammaInput.regValue_.gRegValue_ + this->maxRegStep_) ? gammaInput.regValue_.gRegValue_ + this->maxRegStep_ : gammaOutput.regValue_.gRegValue_;
    gammaOutput.regValue_.bRegValue_ = (gammaOutput.regValue_.bRegValue_ > gammaInput.regValue_.bRegValue_ + this->maxRegStep_) ? gammaInput.regValue_.bRegValue_ + this->maxRegStep_ : gammaOutput.regValue_.bRegValue_;
}

void OCAlg::OCBase::restrictRGBVol(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput)
{
    int bindIndex = gammaInput.bindIndex_;

    auto iter = this->bind2Unit_.find(bindIndex);
    if (iter != this->bind2Unit_.end())
    {
        GammaAlg::VolOneUnitConfig& volConfig = iter->second.volConfig_;
        gammaOutput.volValue_.rVolValue_ = (gammaOutput.volValue_.rVolValue_ < volConfig.rVolConfig_.volMinValue_) ? volConfig.rVolConfig_.volMinValue_ : gammaOutput.volValue_.rVolValue_;
        gammaOutput.volValue_.gVolValue_ = (gammaOutput.volValue_.gVolValue_ < volConfig.gVolConfig_.volMinValue_) ? volConfig.gVolConfig_.volMinValue_ : gammaOutput.volValue_.gVolValue_;
        gammaOutput.volValue_.bVolValue_ = (gammaOutput.volValue_.bVolValue_ < volConfig.bVolConfig_.volMinValue_) ? volConfig.bVolConfig_.volMinValue_ : gammaOutput.volValue_.bVolValue_;
        gammaOutput.volValue_.rVolValue_ = (gammaOutput.volValue_.rVolValue_ > volConfig.rVolConfig_.volMaxValue_) ? volConfig.rVolConfig_.volMaxValue_ : gammaOutput.volValue_.rVolValue_;
        gammaOutput.volValue_.gVolValue_ = (gammaOutput.volValue_.gVolValue_ > volConfig.gVolConfig_.volMaxValue_) ? volConfig.gVolConfig_.volMaxValue_ : gammaOutput.volValue_.gVolValue_;
        gammaOutput.volValue_.bVolValue_ = (gammaOutput.volValue_.bVolValue_ > volConfig.bVolConfig_.volMaxValue_) ? volConfig.bVolConfig_.volMaxValue_ : gammaOutput.volValue_.bVolValue_;
    }

    gammaOutput.volValue_.rVolValue_ = (gammaOutput.volValue_.rVolValue_ < gammaInput.volValue_.rVolValue_ - this->maxVolStep_) ? gammaInput.volValue_.rVolValue_ - this->maxVolStep_ : gammaOutput.volValue_.rVolValue_;
    gammaOutput.volValue_.gVolValue_ = (gammaOutput.volValue_.gVolValue_ < gammaInput.volValue_.gVolValue_ - this->maxVolStep_) ? gammaInput.volValue_.gVolValue_ - this->maxVolStep_ : gammaOutput.volValue_.gVolValue_;
    gammaOutput.volValue_.bVolValue_ = (gammaOutput.volValue_.bVolValue_ < gammaInput.volValue_.bVolValue_ - this->maxVolStep_) ? gammaInput.volValue_.bVolValue_ - this->maxVolStep_ : gammaOutput.volValue_.bVolValue_;
    gammaOutput.volValue_.rVolValue_ = (gammaOutput.volValue_.rVolValue_ > gammaInput.volValue_.rVolValue_ + this->maxVolStep_) ? gammaInput.volValue_.rVolValue_ + this->maxVolStep_ : gammaOutput.volValue_.rVolValue_;
    gammaOutput.volValue_.gVolValue_ = (gammaOutput.volValue_.gVolValue_ > gammaInput.volValue_.gVolValue_ + this->maxVolStep_) ? gammaInput.volValue_.gVolValue_ + this->maxVolStep_ : gammaOutput.volValue_.gVolValue_;
    gammaOutput.volValue_.bVolValue_ = (gammaOutput.volValue_.bVolValue_ > gammaInput.volValue_.bVolValue_ + this->maxVolStep_) ? gammaInput.volValue_.bVolValue_ + this->maxVolStep_ : gammaOutput.volValue_.bVolValue_;
}