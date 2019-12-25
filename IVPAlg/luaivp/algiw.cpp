#include "algiw.h"


std::map<const std::string, IVPAlg::IVPIW> IVPAlg::g_id2IVPIW;

IVPAlg::IVPIW::IVPIW(const std::string& id) : IVPAlg::IVPBase(id)
{

}

void IVPAlg::IVPIW::virtualInit(const std::map<std::string, std::string>& initConfig)
{
    this->scnLvClear();

    if (this->bind2AMat_.empty() || this->scnUpdate_)
    {
        this->buildAMat();
    }
}

void IVPAlg::IVPIW::scnLvClear()
{
    this->lv2bind_.clear();
    this->bind2WVec_.clear();
}

void IVPAlg::IVPIW::buildAMat()
{
    this->bind2AMat_.clear();

    Eigen::MatrixXd tmpA(4, this->scnQueue_.size());

    for (size_t scnIdx = 0; scnIdx < this->scnQueue_.size(); ++scnIdx)
    {
        for (const auto& bind : this->scnQueue_[scnIdx])
        {
            if (this->bind2AMat_.find(bind.first) == this->bind2AMat_.end())
            {
                this->bind2AMat_.insert({ bind.first, tmpA });
            }
            this->bind2AMat_[bind.first](0, scnIdx) = bind.second[0];
            this->bind2AMat_[bind.first](1, scnIdx) = bind.second[1];
            this->bind2AMat_[bind.first](2, scnIdx) = bind.second[2];
            this->bind2AMat_[bind.first](3, scnIdx) = 1;
        }
    }
}

void IVPAlg::IVPIW::ivpPredInitialRGB(GammaAlg::GammaInputInfoReg& IVPData)
{
    std::array<int, 2> currBind{ IVPData.bandNo_, IVPData.grayValue_ };

    Eigen::MatrixXd& A = this->bind2AMat_[currBind];
    Eigen::VectorXd&& Wold = this->getWold(IVPData);
    Eigen::VectorXd&& D = A * Wold;
    this->refreshPred(IVPData, D);

    this->lv2bind_[IVPData.realxylv_.lv_] = currBind;
    this->bind2WVec_[currBind] = Wold;
}

void IVPAlg::IVPIW::refreshPred(GammaAlg::GammaInputInfoReg& IVPData, Eigen::VectorXd& D)
{
    if (this->volRelied_)
    {
        IVPData.volValue_.rVolValue_ = D(0);
        IVPData.volValue_.gVolValue_ = D(1);
        IVPData.volValue_.bVolValue_ = D(2);
    }
    else
    {
        IVPData.regValue_.rRegValue_ = static_cast<int>(round(D(0)));
        IVPData.regValue_.gRegValue_ = static_cast<int>(round(D(1)));
        IVPData.regValue_.bRegValue_ = static_cast<int>(round(D(2)));
    }
}

Eigen::VectorXd IVPAlg::IVPIW::getWold(GammaAlg::GammaInputInfoReg& IVPData)
{
    if (this->lv2bind_.empty())
    {
        return Eigen::VectorXd::Ones(this->scnQueue_.size()) / this->scnQueue_.size();
    }

    double currBindLv = IVPData.realxylv_.lv_;
    std::array<int, 2> && lastBind = this->getLastBind(currBindLv);

    return this->bind2WVec_[lastBind];
}

std::array<int, 2> IVPAlg::IVPIW::getLastBind(double currBindLv)
{
    auto iter = this->lv2bind_.begin();
    double minDeltaLv = std::abs(currBindLv - iter->first);
    std::array<int, 2> lastBind = iter->second;

    ++iter;
    double currDeltaLv;
    while (iter != this->lv2bind_.end())
    {
        currDeltaLv = std::abs(currBindLv - iter->first);
        if (currDeltaLv < minDeltaLv)
        {
            minDeltaLv = currDeltaLv;
            lastBind = iter->second;
        }

        ++iter;
    }

    return lastBind;
}

void IVPAlg::IVPIW::virtualFeed(GammaAlg::GammaInputInfoReg& IVPData)
{
    std::array<int, 2> currBind{ IVPData.bandNo_, IVPData.grayValue_ };

    Eigen::MatrixXd& A = this->bind2AMat_[currBind];
    Eigen::VectorXd& Wold = this->bind2WVec_[currBind];
    Eigen::VectorXd&& D = this->getD(IVPData);

    Eigen::VectorXd&& W = this->calcW(Wold, A, D);
    this->bind2WVec_[currBind] = W;
}

Eigen::VectorXd IVPAlg::IVPIW::calcW(Eigen::VectorXd& Wold, Eigen::MatrixXd& A, Eigen::VectorXd& D)
{
    constexpr double epsilon = 1.e-9;
    Eigen::MatrixXd&& I = Eigen::MatrixXd::Identity(4, 4);

    Eigen::VectorXd&& beta = (epsilon * I + A * A.transpose()).inverse() * (A * Wold - D);
    Eigen::VectorXd&& W = Wold - A.transpose() * beta;

    return W;
}

Eigen::VectorXd IVPAlg::IVPIW::getD(GammaAlg::GammaInputInfoReg& IVPData)
{
    Eigen::VectorXd D(4);

    if (this->volRelied_)
    {
        D << IVPData.volValue_.rVolValue_, IVPData.volValue_.gVolValue_, IVPData.volValue_.bVolValue_, 1;
    }
    else
    {
        D << IVPData.regValue_.rRegValue_, IVPData.regValue_.gRegValue_, IVPData.regValue_.bRegValue_, 1;
    }

    return D;
}