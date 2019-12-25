#pragma once

/*
 说明: 功能函数及其跨平台实现
*/

#include <string>
#include <vector>

namespace utils
{
    // 函数区域
    void StringSplit(const std::string& str, std::vector<std::string>& vecStr, const char flag);
    
    // 类区域
    class JCLogistic
    {
    public:
        JCLogistic(double uValue, double initValue);
        bool ReadDEcodeFile(const std::string& filename, std::string& content);
        bool WriteDEcodeFile(const std::string& filename, const std::string& content);
        void ResetInitValue(double initValue);

    private:
        char ChaosKeyCreate();
        void DEcodeCharArry(char* pInput, int len);
    private:
        double uValue_;
        double initValue_;
    };

    // 跨平台区域
    extern int (*commonScanf)(const char* buffer, const char* format, ...);
}
