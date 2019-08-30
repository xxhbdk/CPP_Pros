#include "algnewton.h"
#include "utils.h"
#include <fstream>
#include <sstream>

#define g_encrypted true           // 是否加密依赖
#define g_uValue 3.8               // 加密算法系数
#define g_initValue 0.1            // 加密算法初始值

#define g_volRelied true           // 是否电压依赖

#define g_matUpdate true           // 是否矩阵更新
#define g_epsilon 1.e-6            // 计算精度

std::map<const std::string, OCAlg::OCNewton> OCAlg::g_pID2OCNewton;

GammaAlg::RGBVolValue OCAlg::RegVolConversion::RGBReg2RGBVol(int bindIndex, const GammaAlg::RGBRegValue& regValue, const OCShare& shareSteps, const std::map<int, GammaAlg::GammaOneUnitData>& bind2Unit)
{
    this->fillRGBVol(bindIndex, shareSteps, bind2Unit);
    return this->calcRGBVol(bindIndex, regValue, bind2Unit);
}

GammaAlg::RGBRegValue OCAlg::RegVolConversion::RGBVol2RGBReg(int bindIndex, const GammaAlg::RGBVolValue& volValue, const OCShare& shareSteps, const std::map<int, GammaAlg::GammaOneUnitData>& bind2Unit)
{
    this->fillRGBVol(bindIndex, shareSteps, bind2Unit);
    return this->calcRGBReg(bindIndex, volValue, bind2Unit);
}

void OCAlg::RegVolConversion::fillRGBVol(int bindIndex, const OCShare& shareSteps, const std::map<int, GammaAlg::GammaOneUnitData>& bind2Unit)
{
    const std::map<int, std::vector<GammaAlg::GammaInputInfoReg>>& bind2Steps_ = shareSteps.getAllSteps();
    
    for (auto iter = bind2Steps_.cbegin(); iter != bind2Steps_.cend(); ++iter)
    {
        int currBindIndex = iter->first;
        if (currBindIndex >= bindIndex) break;

        if (this->bind2volValue_.find(currBindIndex) == this->bind2volValue_.end())           // 填充缺失的收敛寄存器电压信息
        {
            const GammaAlg::RGBRegValue& currRegValue = iter->second.back().regValue_;
            GammaAlg::RGBVolValue currVolValue = this->calcRGBVol(currBindIndex, currRegValue, bind2Unit);
            this->bind2volValue_[currBindIndex] = currVolValue;
        }
    }
}

GammaAlg::RGBVolValue OCAlg::RegVolConversion::calcRGBVol(int bindIndex, const GammaAlg::RGBRegValue& regValue, const std::map<int, GammaAlg::GammaOneUnitData>& bind2Unit)
{
    GammaAlg::RGBVolValue volValue;

    const GammaAlg::GammaOneUnitData& unitData = bind2Unit.find(bindIndex)->second;
    int grayValue = unitData.grayValue_;
    const GammaAlg::VolOneUnitConfig& volConfig = unitData.volConfig_;

    if (grayValue == 255)
    {
        volValue.rVolValue_ = volConfig.G_VREF1 + (volConfig.G_VREG1 - volConfig.G_VREF1) / 2047. * (2047. - regValue.rRegValue_);
        volValue.gVolValue_ = volConfig.G_VREF1 + (volConfig.G_VREG1 - volConfig.G_VREF1) / 2047. * (2047. - regValue.gRegValue_);
        volValue.bVolValue_ = volConfig.G_VREF1 + (volConfig.G_VREG1 - volConfig.G_VREF1) / 2047. * (2047. - regValue.bRegValue_);
    }
    else if (grayValue > 7)
    {
        const GammaAlg::RGBVolValue& volValueLast = this->bind2volValue_[bindIndex - 1];    // 要求绑点下标必须连续
        volValue.rVolValue_ = volValueLast.rVolValue_ + (volConfig.G_VTUPP - volValueLast.rVolValue_) / 511. * (511. - regValue.rRegValue_);
        volValue.gVolValue_ = volValueLast.gVolValue_ + (volConfig.G_VTUPP - volValueLast.gVolValue_) / 511. * (511. - regValue.gRegValue_);
        volValue.bVolValue_ = volValueLast.bVolValue_ + (volConfig.G_VTUPP - volValueLast.bVolValue_) / 511. * (511. - regValue.bRegValue_);
    }
    else if (grayValue > 0)
    {
        const GammaAlg::RGBVolValue& volValueLast = this->bind2volValue_[bindIndex - 1];    // 要求绑点下标必须连续
        volValue.rVolValue_ = volValueLast.rVolValue_ + (volConfig.G_VREG1 - volValueLast.rVolValue_) / 511. * (511. - regValue.rRegValue_);
        volValue.gVolValue_ = volValueLast.gVolValue_ + (volConfig.G_VREG1 - volValueLast.gVolValue_) / 511. * (511. - regValue.gRegValue_);
        volValue.bVolValue_ = volValueLast.bVolValue_ + (volConfig.G_VREG1 - volValueLast.bVolValue_) / 511. * (511. - regValue.bRegValue_);
    }
    else
    {
        volValue.rVolValue_ = volConfig.GrayV0;
        volValue.gVolValue_ = volConfig.GrayV0;
        volValue.bVolValue_ = volConfig.GrayV0;
    }

    return volValue;
}

GammaAlg::RGBRegValue OCAlg::RegVolConversion::calcRGBReg(int bindIndex, const GammaAlg::RGBVolValue& volValue, const std::map<int, GammaAlg::GammaOneUnitData>& bind2Unit)
{
    GammaAlg::RGBRegValue regValue;

    const GammaAlg::GammaOneUnitData& unitData = bind2Unit.find(bindIndex)->second;
    int grayValue = unitData.grayValue_;
    const GammaAlg::VolOneUnitConfig& volConfig = unitData.volConfig_;

    if (grayValue == 255)
    {
        regValue.rRegValue_ = static_cast<int>(round(2047. - (volValue.rVolValue_ - volConfig.G_VREF1) / (volConfig.G_VREG1 - volConfig.G_VREF1) * 2047.));
        regValue.gRegValue_ = static_cast<int>(round(2047. - (volValue.gVolValue_ - volConfig.G_VREF1) / (volConfig.G_VREG1 - volConfig.G_VREF1) * 2047.));
        regValue.bRegValue_ = static_cast<int>(round(2047. - (volValue.bVolValue_ - volConfig.G_VREF1) / (volConfig.G_VREG1 - volConfig.G_VREF1) * 2047.));
    }
    else if (grayValue > 7)
    {
        const GammaAlg::RGBVolValue& volValueLast = this->bind2volValue_[bindIndex - 1];    // 要求绑点下标必须连续
        regValue.rRegValue_ = static_cast<int>(round(511. - (volValue.rVolValue_ - volValueLast.rVolValue_) / (volConfig.G_VTUPP - volValueLast.rVolValue_) * 511.));
        regValue.gRegValue_ = static_cast<int>(round(511. - (volValue.gVolValue_ - volValueLast.gVolValue_) / (volConfig.G_VTUPP - volValueLast.gVolValue_) * 511.));
        regValue.bRegValue_ = static_cast<int>(round(511. - (volValue.bVolValue_ - volValueLast.bVolValue_) / (volConfig.G_VTUPP - volValueLast.bVolValue_) * 511.));
    }
    else if (grayValue > 0)
    {
        const GammaAlg::RGBVolValue& volValueLast = this->bind2volValue_[bindIndex - 1];    // 要求绑点下标必须连续
        regValue.rRegValue_ = static_cast<int>(round(511. - (volValue.rVolValue_ - volValueLast.rVolValue_) / (volConfig.G_VREG1 - volValueLast.rVolValue_) * 511.));
        regValue.gRegValue_ = static_cast<int>(round(511. - (volValue.gVolValue_ - volValueLast.gVolValue_) / (volConfig.G_VREG1 - volValueLast.gVolValue_) * 511.));
        regValue.bRegValue_ = static_cast<int>(round(511. - (volValue.bVolValue_ - volValueLast.bVolValue_) / (volConfig.G_VREG1 - volValueLast.bVolValue_) * 511.));
    }
    else
    {
        const GammaAlg::RGBVolValue& volValueLast = this->bind2volValue_[bindIndex - 1];    // 要求绑点下标必须连续
        regValue.rRegValue_ = static_cast<int>(round(511. - (volValue.rVolValue_ - volValueLast.rVolValue_) / (volConfig.GrayV0 - volValueLast.rVolValue_) * 511.));
        regValue.gRegValue_ = static_cast<int>(round(511. - (volValue.gVolValue_ - volValueLast.gVolValue_) / (volConfig.GrayV0 - volValueLast.gVolValue_) * 511.));
        regValue.bRegValue_ = static_cast<int>(round(511. - (volValue.bVolValue_ - volValueLast.bVolValue_) / (volConfig.GrayV0 - volValueLast.bVolValue_) * 511.));
    }

    return regValue;
}


void OCAlg::OCNewton::gammaSetPara(const std::string filename)
{
    this->filename_ = filename;

    this->bandGray2JMat_.clear();
    this->getJMat();
}

void OCAlg::OCNewton::getJMat()
{
    std::string content;
    this->parseFile(content);
    this->buildJMat(content);
}

void OCAlg::OCNewton::buildJMat(const std::string& content)
{
    std::istringstream in(content);
    std::string line;
    
    while (std::getline(in, line))
    {
        int bandNo, grayValue;
        Eigen::Matrix3d JMat;
        utils::commonScanf(line.c_str(), "%d%d%lf%lf%lf%lf%lf%lf%lf%lf%lf", &bandNo, &grayValue,
            &JMat(0, 0), &JMat(0, 1), &JMat(0, 2),
            &JMat(1, 0), &JMat(1, 1), &JMat(1, 2),
            &JMat(2, 0), &JMat(2, 1), &JMat(2, 2));
        this->bandGray2JMat_[{bandNo, grayValue}] = JMat;
    }
}

void OCAlg::OCNewton::parseFile(std::string& content)
{
    if (g_encrypted)
    {
        utils::JCLogistic jcObj(g_uValue, g_initValue);
        jcObj.ReadDEcodeFile(this->filename_, content);
    }
    else
    {
        std::ifstream fin(this->filename_);
        std::ostringstream sout;
        sout << fin.rdbuf();
        content = sout.str();
        fin.close();
    }
}

void OCAlg::OCNewton::xylv2uvlv(const GammaAlg::CIExylvInfo& xylv, CIEuvlvInfo& uvlv)
{
    uvlv.u_ = 4 * xylv.x_ / (-2 * xylv.x_ + 12 * xylv.y_ + 3);
    uvlv.v_ = 9 * xylv.y_ / (-2 * xylv.x_ + 12 * xylv.y_ + 3);
    uvlv.lv_ = xylv.lv_;
}

void OCAlg::OCNewton::gammaOneStepCalc(const GammaAlg::GammaInputInfoReg& gammaInput, GammaAlg::GammaOutputInfoReg& gammaOutput)
{
    OCBase::gammaOneStepCalc(gammaInput, gammaOutput);

    if (g_matUpdate)
    {
        this->updateJMat(gammaInput);          /////
    }
    //int bindIndex = gammaInput.bindIndex_;
    //const GammaAlg::RGBRegValue& regValue = gammaInput.regValue_;
    //const GammaAlg::CIExylvInfo& xylv = gammaInput.realxylv_;
    //const GammaAlg::GammaErrorConfig& errorConfig = this->bind2Unit_[bindIndex].errorConfig_;
    //const GammaAlg::CIExylvInfo& targetxylv = errorConfig.targetxylv_;
    //Eigen::Matrix3d& JMat = this->bandGray2JMat_[{gammaInput.bandNo_, gammaInput.bindIndex_}];
}

void OCAlg::OCNewton::updateJMat(const GammaAlg::GammaInputInfoReg& gammaInput)
{
    if (gammaInput.count_ > 1)       // 第1次调节不更新
    {
        const OCShare& shareSteps = g_pID2OCShare.find(this->pID_)->second;
        int bindIndex = gammaInput.bindIndex_;
        std::vector<GammaAlg::GammaInputInfoReg> steps;
        shareSteps.stepOutput(bindIndex, steps);

        Eigen::MatrixXd regOrVolDeltaMat(steps.size(), 3), uvlvDeltaMat(steps.size(), 3);
        if (g_volRelied)
        {
            this->buildDeltaMatOnVol(gammaInput, regOrVolDeltaMat, uvlvDeltaMat);
        }
        else
        {
            this->buildDeltaMatOnReg(gammaInput, regOrVolDeltaMat, uvlvDeltaMat);
        }

        Eigen::Matrix3d XT_X = regOrVolDeltaMat.transpose() * regOrVolDeltaMat;
        Eigen::Matrix3d YT_X = uvlvDeltaMat.transpose() * regOrVolDeltaMat;
        Eigen::FullPivLU<Eigen::Matrix3d> luRank(XT_X);
        int rank = luRank.rank();
        Eigen::Matrix3d& JMat = this->bandGray2JMat_.find({ gammaInput.bandNo_, gammaInput.bindIndex_ })->second;

        switch (rank)
        {
        case 1:
            this->updateRank1(XT_X, YT_X, JMat);    /////
            break;
        case 2:
            this->updateRank2(XT_X, YT_X, JMat);
            break;
        case 3:
            this->updateRank3(XT_X, YT_X, JMat);
            break;
        }
    }
}

void OCAlg::OCNewton::updateRank1(Eigen::Matrix3d& XT_X, Eigen::Matrix3d& YT_X, Eigen::Matrix3d& JMat)
{

}

void OCAlg::OCNewton::updateRank2(Eigen::Matrix3d& XT_X, Eigen::Matrix3d& YT_X, Eigen::Matrix3d& JMat)
{

}

void OCAlg::OCNewton::updateRank3(Eigen::Matrix3d& XT_X, Eigen::Matrix3d& YT_X, Eigen::Matrix3d& JMat)
{

}

void OCAlg::OCNewton::buildDeltaMatOnReg(const GammaAlg::GammaInputInfoReg& gammaInput, Eigen::MatrixXd& regOrVolDeltaMat, Eigen::MatrixXd& uvlvDeltaMat)
{
    const OCShare& shareSteps = g_pID2OCShare.find(this->pID_)->second;

    int bindIndex = gammaInput.bindIndex_;
    std::vector<GammaAlg::GammaInputInfoReg> steps;
    shareSteps.stepOutput(bindIndex, steps);
    steps.push_back(gammaInput);

    for (int i = 0; i < static_cast<int>(steps.size()) - 1; ++i)
    {
        const GammaAlg::RGBRegValue& regValueCurr = steps[i].regValue_;
        const GammaAlg::RGBRegValue& regValueNext = steps[i + 1].regValue_;
        regOrVolDeltaMat(i, 0) = static_cast<double>(regValueNext.rRegValue_ - regValueCurr.rRegValue_);
        regOrVolDeltaMat(i, 1) = static_cast<double>(regValueNext.gRegValue_ - regValueCurr.gRegValue_);
        regOrVolDeltaMat(i, 2) = static_cast<double>(regValueNext.bRegValue_ - regValueCurr.bRegValue_);

        const GammaAlg::CIExylvInfo& xylvCurr = steps[i].realxylv_;
        CIEuvlvInfo uvlvCurr;
        this->xylv2uvlv(xylvCurr, uvlvCurr);
        const GammaAlg::CIExylvInfo& xylvNext = steps[i + 1].realxylv_;
        CIEuvlvInfo uvlvNext;
        this->xylv2uvlv(xylvNext, uvlvNext);
        uvlvDeltaMat(i, 0) = uvlvNext.lv_ - uvlvCurr.lv_;
        uvlvDeltaMat(i, 1) = uvlvNext.u_ - uvlvCurr.u_;
        uvlvDeltaMat(i, 2) = uvlvNext.v_ - uvlvCurr.v_;
    }
}

void OCAlg::OCNewton::buildDeltaMatOnVol(const GammaAlg::GammaInputInfoReg& gammaInput, Eigen::MatrixXd& regOrVolDeltaMat, Eigen::MatrixXd& uvlvDeltaMat)
{
    const OCShare& shareSteps = g_pID2OCShare.find(this->pID_)->second;

    int bindIndex = gammaInput.bindIndex_;
    std::vector<GammaAlg::GammaInputInfoReg> steps;
    shareSteps.stepOutput(bindIndex, steps);
    steps.push_back(gammaInput);

    for (int i = 0; i < static_cast<int>(steps.size()) - 1; ++i)
    {
        const GammaAlg::RGBRegValue& regValueCurr = steps[i].regValue_;
        const GammaAlg::RGBVolValue volValueCurr = this->volConversion_.RGBReg2RGBVol(bindIndex, regValueCurr, shareSteps, this->bind2Unit_);
        const GammaAlg::RGBRegValue& regValueNext = steps[i + 1].regValue_;
        const GammaAlg::RGBVolValue volValueNext = this->volConversion_.RGBReg2RGBVol(bindIndex, regValueNext, shareSteps, this->bind2Unit_);
        regOrVolDeltaMat(i, 0) = volValueNext.rVolValue_ - volValueCurr.rVolValue_;
        regOrVolDeltaMat(i, 1) = volValueNext.gVolValue_ - volValueCurr.gVolValue_;
        regOrVolDeltaMat(i, 2) = volValueNext.bVolValue_ - volValueCurr.bVolValue_;

        const GammaAlg::CIExylvInfo& xylvCurr = steps[i].realxylv_;
        CIEuvlvInfo uvlvCurr;
        this->xylv2uvlv(xylvCurr, uvlvCurr);
        const GammaAlg::CIExylvInfo& xylvNext = steps[i + 1].realxylv_;
        CIEuvlvInfo uvlvNext;
        this->xylv2uvlv(xylvNext, uvlvNext);
        uvlvDeltaMat(i, 0) = uvlvNext.lv_ - uvlvCurr.lv_;
        uvlvDeltaMat(i, 1) = uvlvNext.u_ - uvlvCurr.u_;
        uvlvDeltaMat(i, 2) = uvlvNext.v_ - uvlvCurr.v_;
    }
}