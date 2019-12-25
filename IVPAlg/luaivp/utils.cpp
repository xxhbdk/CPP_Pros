#include <sstream>
#include <fstream>
#include <stdio.h>
#include "utils.h"

#define READBUF_SIZE 512
#define SET_BIT(x,n) ((x)|=(0x01<<(n))) 


void utils::StringSplit(const std::string& str, std::vector<std::string>& vecStr, const char flag)
{
    vecStr.clear();
    vecStr.reserve(10);

    std::istringstream sIn(str);
    std::string tmpStr;
    while (std::getline(sIn, tmpStr, flag))
    {
        vecStr.push_back(tmpStr);
    }
}

utils::JCLogistic::JCLogistic(double uValue, double initValue) : uValue_(uValue), initValue_(initValue)
{

}

bool utils::JCLogistic::ReadDEcodeFile(const std::string& filename, std::string& content)
{
    std::ifstream fin(filename.c_str(), std::ios::in | std::ios::binary);
    
    char tmpBuf[READBUF_SIZE];
    int getCount = 0;
    while (!fin.eof())
    {
        memset(tmpBuf, 0, READBUF_SIZE);
        fin.read(tmpBuf, READBUF_SIZE);
        getCount = static_cast<int>(fin.gcount());
        this->DEcodeCharArry(tmpBuf, getCount);
        content += std::string(tmpBuf, getCount);
    }
    fin.close();

    return true;
}

bool utils::JCLogistic::WriteDEcodeFile(const std::string& filename, const std::string& content)
{
    std::string tmpStr = content;
    std::ofstream fout;
    fout.open(filename.c_str(), std::ios::out | std::ios::binary);
    
    char* pbuf = new char[tmpStr.length()];
    memset(pbuf, 0, tmpStr.length());
    memcpy(pbuf, tmpStr.c_str(), tmpStr.length());

    this->DEcodeCharArry(pbuf, static_cast<int>(tmpStr.length()));
    fout.write(pbuf, tmpStr.length());
    fout.close();

    delete[] pbuf;
    pbuf = NULL;

    return true;
}

void utils::JCLogistic::ResetInitValue(double initValue)
{
    this->initValue_ = initValue;
}

void utils::JCLogistic::DEcodeCharArry(char* pInput, int len)
{
    for (int i = 0; i < len; ++i)
    {
        *(pInput + i) = *(pInput + i) ^ this->ChaosKeyCreate();
    }
}

char utils::JCLogistic::ChaosKeyCreate()
{
    char key = 0;
    double uValue = this->uValue_;
    double initVaule = this->initValue_;

    int bitValue = 0;
    double result = 0;
    for (int i = 0; i < 8; ++i)
    {
        initVaule = uValue * initVaule * (1 - initVaule);
        if (initVaule >= 0.5)
        {
            SET_BIT(key, i);
        }
    }
    this->initValue_ = initVaule;
    return key;
}

#ifdef WIN32
    int (*utils::commonScanf)(const char*, const char*, ...) = sscanf_s;
#else
    int (*utils::commonScanf)(const char*, const char*, ...) = sscanf;
#endif
