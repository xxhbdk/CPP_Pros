#include <fstream>
#include <sstream>
#include "algbase.h"
#include "utils.h"


IVPAlg::IVPBase::IVPBase(const std::string& id) : id_(id), volRelied_(false), scnUpdate_(true), capacity_(300)
{

}

IVPAlg::IVPBase::~IVPBase()
{

}

void IVPAlg::IVPBase::ivpInit(const std::map<std::string, std::string>& initConfig)
{
    this->filename_ = initConfig.find("filename")->second;
    this->volRelied_ = initConfig.find("volRelied") != initConfig.end() ? std::stoi(initConfig.find("volRelied")->second) : false;
    this->scnUpdate_ = initConfig.find("scnUpdate") != initConfig.end() ? std::stoi(initConfig.find("scnUpdate")->second) : true;
    this->capacity_ = initConfig.find("capacity") != initConfig.end() ? std::stoi(initConfig.find("capacity")->second) : 300;

    if (this->scnQueue_.empty())
    {
        this->ivpLoadCSV(this->filename_);
    }

    if (this->scnUpdate_)
    {
        if (this->scnFullCheck())
        {
            this->scnInsert();
        }
    }
    this->scnClear();

    this->virtualInit(initConfig);
}

void IVPAlg::IVPBase::virtualInit(const std::map<std::string, std::string>& initConfig)
{

}

void IVPAlg::IVPBase::ivpWriteCSV(const std::string& outFilename)
{
    std::deque<std::map<std::array<int, 2>, std::array<double, 3>>> scnQueue(this->scnQueue_);
    if (this->scnUpdate_)
    {
        if (this->currScn_.size() == this->scnQueue_.begin()->size())
        {
            scnQueue.push_back(this->currScn_);
            if (scnQueue.size() > this->capacity_)
            {
                scnQueue.pop_front();
            }
        }
    }
    
    std::string filename = !outFilename.empty() ? outFilename : this->filename_;
    std::ofstream fout(filename);
    std::string&& content = this->buildOutput(scnQueue);
    fout.write(content.c_str(), content.size());
    fout.close();
}

std::string IVPAlg::IVPBase::buildOutput(const std::deque<std::map<std::array<int, 2>, std::array<double, 3>>>& scnQueue)
{
    std::string content;

    size_t scnIdx = 1;
    for (const auto& scn : scnQueue)
    {
        for (const auto& bind : scn)
        {
            std::string&& line = this->buildLine(bind);
            content.append(std::to_string(scnIdx) + "," + line);
        }

        ++scnIdx;
    }

    return content;
}

std::string IVPAlg::IVPBase::buildLine(const std::pair<const std::array<int, 2>, std::array<double, 3>> & bind)
{
    std::string line;
    line.append(std::to_string(bind.first[0]) + "," + std::to_string(bind.first[1]) + ",");

    if (this->volRelied_)
    {
        line.append(std::to_string(bind.second[0]) + "," + std::to_string(bind.second[1]) + "," + std::to_string(bind.second[2]) + "\n");
    }
    else
    {
        line.append(std::to_string(static_cast<int>(bind.second[0])) + "," + std::to_string(static_cast<int>(bind.second[1])) + "," + std::to_string(static_cast<int>(bind.second[2])) + "\n");
    }

    return line;
}

void IVPAlg::IVPBase::scnClear()
{
    this->currScn_.clear();
}

void IVPAlg::IVPBase::scnInsert()
{
    this->scnQueue_.push_back(this->currScn_);
    if (this->scnQueue_.size() > this->capacity_)
    {
        this->scnQueue_.pop_front();
    }
}

bool IVPAlg::IVPBase::scnFullCheck()
{
    if (this->currScn_.size() == this->scnQueue_.begin()->size())
    {
        return true;
    }

    return false;
}

void IVPAlg::IVPBase::ivpLoadCSV(const std::string& inFilename)
{
    std::ifstream fin(inFilename);

    int tmpIdx = -1;
    int scnIdx, bandNo, grayValue;
    double rVal, gVal, bVal;
    std::map<std::array<int, 2>, std::array<double, 3>> bind2rgb;

    std::string line;
    while (std::getline(fin, line))
    {
        utils::commonScanf(line.c_str(), "%d,%d,%d,%lf,%lf,%lf", &scnIdx, &bandNo, &grayValue, &rVal, &gVal, &bVal);
        if (scnIdx != tmpIdx && tmpIdx != -1)
        {
            this->scnQueue_.push_back(bind2rgb);
            bind2rgb.clear();
        }
        bind2rgb[{bandNo, grayValue}] = { rVal, gVal, bVal };
        tmpIdx = scnIdx;
    }
    this->scnQueue_.push_back(bind2rgb);

    if (this->scnQueue_.size() > this->capacity_)
    {
        this->scnQueue_.erase(this->scnQueue_.begin() + this->capacity_, this->scnQueue_.end());
    }

    fin.close();
}

void IVPAlg::IVPBase::ivpPredInitialRGB(GammaAlg::GammaInputInfoReg& IVPData)
{

}

void IVPAlg::IVPBase::ivpFeedOptimalRGB(GammaAlg::GammaInputInfoReg& IVPData)
{
    int bandNo = IVPData.bandNo_;
    int grayValue = IVPData.grayValue_;

    const auto& scnRef = this->scnQueue_[0];
    
    if (scnRef.find({ bandNo, grayValue }) != scnRef.end())
    {
        if (this->volRelied_)
        {
            this->currScn_.insert({ {bandNo, grayValue}, {IVPData.volValue_.rVolValue_, IVPData.volValue_.gVolValue_, IVPData.volValue_.bVolValue_} });
        }
        else
        {
            this->currScn_.insert({ {bandNo, grayValue}, {static_cast<double>(IVPData.regValue_.rRegValue_), static_cast<double>(IVPData.regValue_.gRegValue_), static_cast<double>(IVPData.regValue_.bRegValue_)} });
        }
    }

    this->virtualFeed(IVPData);
}

void IVPAlg::IVPBase::virtualFeed(GammaAlg::GammaInputInfoReg& IVPData)
{

}
