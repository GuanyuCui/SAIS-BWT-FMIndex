#ifndef BWT_FM_H
#define BWT_FM_H

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <thread>

class BWT_FM
{
    public:
        BWT_FM(const std::string & str, char eol = '$', double alpha = 1.0 / 128, double beta = 1.0 / 32);
        // 返回经过 bwt 的串
        std::string bwt();
        // 展示子串查询结果
        void displayPatternQuery(const std::string & pattern);
    private:
        // 原字符串(添加结尾符)
        std::string original;
        // 后缀数组 SA
        typedef long satype;
        // 因为是按值等间隔压缩，所以要保留下标
        std::unordered_map<size_t, satype> suffixArray;
        // suffixArray 的保留比
        double alpha;
        // BMT 的字符串
        std::string BWTstr;
        // 字母表
        std::set<char> alphabet;
        // 字母-相对顺序映射
        std::unordered_map<char, size_t> charOrder;
        // 各字符在排序好的后缀中出现的范围
        std::vector<std::pair<size_t, size_t>> FCharRange; 
        // 为了实现O(m)子串查询
        std::vector<std::vector<size_t>> LCheckPoints;
        // LCheckPoints 的保留比
        double beta;

        // 建立后缀数组
        // 建立各字符出现次数的索引
        // 建立BWT处理的子串
        void preprocess();

        // 查询子串，返回所有子串起始点下标
        std::vector<size_t> patternQuery(const std::string & pattern);
        // 打印子串以及附近的情况
        void printSubStr(size_t startPos, size_t subStrLen, size_t backFront = 5);

        // 根据压缩后的LCheckPoints，返回 c 字符在 BWT[index] 位置的位次
        size_t LCheckPointsFind(char c, size_t index);
        // 计算 l 行的 LF-Mapping
        size_t LFMapping(size_t l);
        // 在压缩后的 SA 中计算 BWT[index] 对应的 SA 的位次
        satype suffixArrayFind(size_t index);

        // 不允许复制构造，拷贝构造
        BWT_FM(const BWT_FM & rhs){}
        void operator = (const BWT_FM & rhs){}
};

#endif