#pragma once

/*
 说明: IVP算法 ~ 基类
*/

#include <map>
#include <array>
#include <deque>
#include <string>

#include "alginterface.h"


namespace IVPAlg
{
    class IVPBase
    {
    public:
        IVPBase() = delete;
        explicit IVPBase(const std::string& id);
        virtual ~IVPBase();
        void ivpInit(const std::map<std::string, std::string>& initConfig);                        // 补充处理iw之lv判据, 并按bind构造参考基底
        void ivpWriteCSV(const std::string& outFilename);
        virtual void ivpPredInitialRGB(GammaAlg::GammaInputInfoReg& IVPData);
        void ivpFeedOptimalRGB(GammaAlg::GammaInputInfoReg& IVPData);                              // 只有(bandNo, grayValue)与第0块IC匹配时插入 (补充iw之lv判据)

    protected:
        void ivpLoadCSV(const std::string& inFilename);
        virtual void virtualInit(const std::map<std::string, std::string>& initConfig);
        virtual void virtualFeed(GammaAlg::GammaInputInfoReg& IVPData);
    protected:
        std::string id_;                                                                           // 通道号
        std::string filename_;                                                                     // 收敛文件名
        bool volRelied_;                                                                           // 是否电压依赖
        bool scnUpdate_;                                                                           // 是否更新数据
        size_t capacity_;                                                                          // 最大存储容量(IC数量)
        std::deque<std::map<std::array<int, 2>, std::array<double, 3>>> scnQueue_;                 // csv解析之数据(IC队列)
        std::map<std::array<int, 2>, std::array<double, 3>> currScn_;                              // 当前IC的收敛信息

    private:
        bool scnFullCheck();                                                                       // 检查当前IC绑点信息是否完整
        void scnInsert();                                                                          // 根据容量将当前IC插入IC队列
        void scnClear();                                                                           // 清空当前IC的收敛信息
        std::string buildOutput(const std::deque<std::map<std::array<int, 2>, std::array<double, 3>>>& scnQueue);            // 根据IC队列构造输出信息
        std::string buildLine(const std::pair<const std::array<int, 2>, std::array<double, 3>>& bind);
    };
}
