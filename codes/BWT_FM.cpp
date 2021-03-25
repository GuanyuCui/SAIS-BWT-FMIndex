#include "BWT_FM.h"
#include "./sais-lite-2.4.1/sais.hxx"

BWT_FM::BWT_FM(const std::string & str, char eol, double alpha, double beta)
: original(str), alpha(alpha), beta(beta)
{
    // 去掉之前的结尾符
    original.pop_back();
    original.push_back(eol);
    // 认为是较短的串的长度
    const size_t shortStrLen = 1000;
    // 认为是较长的串的长度
    const size_t longStrLen = 100000;
    // 长度太短，不压缩
    if(original.length() <= shortStrLen)
    {
        this -> alpha = this -> beta = 1.00;
    }
    // 中间按线性变化
    else if(original.length() <= shortStrLen + longStrLen)
    {
        this -> alpha = (alpha - 1.00) / longStrLen * (original.length() - shortStrLen) + 1.00;
        this -> beta = (beta - 1.00) / longStrLen * (original.length() - shortStrLen) + 1.00;
    }
    // 够长，开始压缩
    else
    {
        this -> alpha = alpha;
        this -> beta = beta;
    }
    // 建索引等
    preprocess();
}

std::string BWT_FM::bwt()
{
    return BWTstr;
}

void BWT_FM::displayPatternQuery(const std::string & pattern)
{
    // 做子串查询
    auto ret = patternQuery(pattern);
    std::cout << "成功匹配子串数: " << ret.size() << std::endl;
    // 依次打印结果
    for(size_t i = 0; i < ret.size(); i++)
    {
        printSubStr(ret[i], pattern.length(), 5);
        std::cout << std::endl;
    }
}

void BWT_FM::preprocess()
{
    // 调用 sais 库, 可在 O(n) 时间内建立后缀数组
    satype * pSA = new satype[original.length()];
    saisxx<const char *, satype *, satype>(original.c_str(), pSA, original.length());

    // 后缀数组的值间隔
    satype SAValueGap = satype(1 / beta);

    // 构建后缀数组同时构建字母表
    // 循环 n 次, 每次 O(1), 共计 O(n)
    for(size_t i = 0; i < original.length(); i++)
    {
        // 按值等间隔保存
        if(pSA[i] % SAValueGap == 0)
        {
            suffixArray.insert(std::make_pair(i, pSA[i]));
        }
        // find 时间复杂度为 O(log k), k 为已有字符数
        // 因为是常数字母表, 所以 k <= \Sigma = O(1)
        if(alphabet.find(original[i]) == alphabet.end())
        {
            alphabet.insert(original[i]);
        }
    }

    // 计算字母表的相对顺序
    // 因为是常数字母表, 所以时间复杂度 O(1)
    charOrder.reserve(alphabet.size());
    {
        size_t cOrder = 0;
        for(auto i = alphabet.begin(); i != alphabet.end(); i++, cOrder++)
        {
            charOrder[(*i)] = cOrder;
        }
    }

    // 初始化
    FCharRange.resize(alphabet.size(), std::make_pair<size_t, size_t>(0, 0));
    BWTstr.resize(original.length(), 0);
    LCheckPoints.resize(alphabet.size(), std::vector<size_t>(size_t(std::ceil(original.length() * alpha)), 0));

    // LCheckPoints 的间隔
    size_t LCheckGap = size_t(1 / alpha);
    {
        char prevChar = original[ pSA[0] ];
        FCharRange[charOrder[prevChar]].first = 0;

        // 临时的 LCheckPoints 的一行，压缩用
        std::vector<size_t> tmpLCheckPointsRow(alphabet.size(), 0);
        for(size_t i = 0; i < original.length(); i++)
        {
            // 计算F列各字母出现范围
            // L 数组中 c 出现的下标范围 [begin, end)
            // 从头扫到尾, 时间复杂度 O(n)
            char nowChar = original[ pSA[i] ];
            if(nowChar != prevChar)
            {
                FCharRange[charOrder[ prevChar ]].second = i;
                FCharRange[charOrder[ nowChar ]].first = i;
                prevChar = nowChar;
            }

            // 计算 BWTstr
            // 利用后缀数组建立BWTstr
            // 显然也是 O(n)
            BWTstr[i] = ( ( pSA[i] > 0 ) ? original[pSA[i] - 1] : '$');

            // 计算L列的Checkpoints
            // n 次循环, 每次 O(Sigma), 共计 O(Sigma n)=O(n)
            // Sigma次循环

            for(auto j = alphabet.begin(); j != alphabet.end(); j++)
            {
                // 根据之前的行，变更CheckPoint的值
                tmpLCheckPointsRow[ charOrder[*j] ] = (i == 0 ? 0 : tmpLCheckPointsRow[ charOrder[*j] ]) + (BWTstr[i] == *j);
            }
            // 保留一部分
            if(i % LCheckGap == 0)
            {
                for(auto j = alphabet.begin(); j != alphabet.end(); j++)
                {
                    LCheckPoints[ charOrder[*j] ][i / LCheckGap] = (tmpLCheckPointsRow[ charOrder[*j] ]);
                }
            }
        }
        delete[] pSA;
        FCharRange[charOrder[ prevChar ]].second = original.length();
    }
}

std::vector<size_t> BWT_FM::patternQuery(const std::string & pattern)
{
    auto i = pattern.rbegin();
    // 略过空白
    while(i != pattern.rend() && *i == '\0')
    {
        i++;
    }
    if(alphabet.find(*i) == alphabet.end())
    {
        return {};
    }

    // 出事范围
    std::pair<size_t, size_t> range = FCharRange[charOrder[*i]];
    i++;
    for(; i != pattern.rend(); i++)
    {
        char charToFind = *i;
        // 找不到
        if(alphabet.find(*i) == alphabet.end())
        {
            return {};
        }
        // 生成新范围
        range = std::make_pair(
            FCharRange[charOrder[charToFind]].first + LCheckPointsFind(charToFind, range.first - 1), 
            FCharRange[charOrder[charToFind]].first + LCheckPointsFind(charToFind, range.second - 1) - 1 + 1
        );
        // 如果是空范围
        if(range.second <= range.first)
        {
            return {};
        }
    }
    // 排序搜索到的子串位置
    std::vector<size_t> ret;
    for(size_t k = range.first; k < range.second; k++)
    {
        ret.push_back(suffixArrayFind(k));
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}

void BWT_FM::printSubStr(size_t startPos, size_t subStrLen, size_t backFront)
{
    std::cout << "位于 " << startPos << ": ";
    // 打印前半部分
    if(startPos <= backFront)
    {
        std::cout << original.substr(0, startPos);
    }
    else
    {
        std::cout << "..." << original.substr(startPos - backFront, backFront);
    }
    // 当查找子串过长，省略中间部分
    const int N = 30;
    assert(N % 2 == 0);
    if(subStrLen <= N)
    {
        std::cout << "^" << original.substr(startPos, subStrLen) << "^";
    }
    else
    {
        std::cout << "^" << original.substr(startPos, N / 2) << "...(省略" 
            << subStrLen - N << "字符)..." 
            << original.substr(startPos + subStrLen - N / 2, N / 2) << "^";
    }
    // 打印后半部分
    if(startPos + subStrLen - 1 + backFront >= original.length())
    {
        std::cout << original.substr(startPos + subStrLen);
    }
    else
    {
        std::cout << original.substr(startPos + subStrLen, backFront) << "...";
    }
}

// 在压缩过的 LCheckPoints 里定位
size_t BWT_FM::LCheckPointsFind(char c, size_t index)
{
    size_t LCheckGap = (1 / alpha);
    if ((index % LCheckGap) == 0)
    {
        return LCheckPoints[charOrder[c]][index / LCheckGap];
    } 
    // 根据余数（即组间位置）确定最近的CheckPoint在高下标还是低下标
    // 余数超过间隔的一半，最近的CheckPoint在高下标
    else if ((index % LCheckGap) > (LCheckGap / 2) && ((index / LCheckGap) + 1 < LCheckPoints.size())) 
    {
        size_t nearestCP = ((index / LCheckGap) + 1) * LCheckGap;
        // 从高下标往回扫
        // 原基准
        size_t base = LCheckPoints[charOrder[c]][(index / LCheckGap + 1)];
        // 沿路遇到的该字符数目
        size_t charCount = 0;
        // 注意：此处 size_t 必定不会小于 LCheckGap / 2，所以不会溢出
        for(size_t i = nearestCP; i > index; i--)
        {
            charCount += (BWTstr[i] == c);
        }
        return base - charCount;
    }
    // 余数没有超过间隔的一半，最近的CheckPoint在高下标
    else
    {
        size_t nearestCP = (index / LCheckGap) * LCheckGap;
        // 从低下标往后扫
        // 原基准
        size_t base = LCheckPoints[charOrder[c]][(index / LCheckGap)];
        // 沿路遇到的该字符数目
        size_t charCount = 0;
        for(size_t i = nearestCP + 1; i <= index; i++)
        {
            charCount += (BWTstr[i] == c);
        }
        return base + charCount;
    }
}

size_t BWT_FM::LFMapping(size_t l)
{
    return FCharRange[charOrder[BWTstr[l]]].first + LCheckPointsFind(BWTstr[l], l) - 1;
}

BWT_FM::satype BWT_FM::suffixArrayFind(size_t index)
{
    // 如果是保留的部分
    if(suffixArray.find(index) != suffixArray.end())
    {
        return suffixArray[index];
    }
    // 跳跃次数
    size_t jumpCount = 0;
    size_t nowIndex = index;
    // 循环跳
    while (suffixArray.find(nowIndex) == suffixArray.end())
    {
        jumpCount++;
        nowIndex = LFMapping(nowIndex);
    }
    return suffixArray[nowIndex] + jumpCount;
}