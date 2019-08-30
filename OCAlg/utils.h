#pragma once

/*
 ˵��: ���ܺ��������ƽ̨ʵ��
*/

#include <string>
#include <vector>

namespace utils
{
    // ��������
    void stringSplit(const std::string& str, std::vector<std::string>& vecStr, const char flag);
    
    // ������
    class JCLogistic
    {
    public:
        JCLogistic(double uValue, double initValue) : uValue_(uValue), initValue_(initValue) {}
        bool ReadDEcodeFile(const std::string& filename, std::string& content);
        bool WriteDEcodeFile(const std::string& filename, const std::string& content);
        void ResetInitValue(double initValue) { this->initValue_ = initValue; }

    private:
        char ChaosKeyCreate();
        void DEcodeCharArry(char* pInput, int len);
    private:
        double uValue_;
        double initValue_;
    };

    // ��ƽ̨����
    extern int (*commonScanf)(const char*, ...);
}