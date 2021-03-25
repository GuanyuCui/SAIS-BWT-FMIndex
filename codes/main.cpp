#include <iostream>
#include <fstream>
#include <ctime>
#include <cctype>
#include <algorithm>
#include "BWT_FM.h"
//#include "BWT_FM.cpp"

int main()
{
    // lambda 打印作者信息
    auto printAuthor = []() -> void
    {
        std::cout << "|----------------------------------------------------------|" << std::endl;
        std::cout << "|                          BWT-FM                          |" << std::endl;
        std::cout << "|                     Written by G.Cui                     |" << std::endl;
        std::cout << "|----------------------------------------------------------|" << std::endl;
    };
    // lambda 打印使用方法
    auto printUsage = []() -> void
    {
        std::cout << "----------------------------------------------------------" << std::endl;
        std::cout << "使用方法: " << std::endl;
        std::cout << "HELP: 查看使用帮助。" << std::endl;
        std::cout << "FILELOAD + 文件名: 加载文件内容并创建索引。" << std::endl;
        std::cout << "STRLOAD + 字符串: 加载字符串内容并创建索引。" << std::endl;
        std::cout << "BWT: 显示加载好的索引的BWT结果。" << std::endl;
        std::cout << "FILEMATCH + 文件名: 从文件加载字符串进行模式匹配。" << std::endl;
        std::cout << "STRMATCH + 模式串: 字符串模式匹配。" << std::endl;
        std::cout << "QUIT: 退出。" << std::endl;
        std::cout << "----------------------------------------------------------" << std::endl;
    };

    // BWT_FM 单例
    BWT_FM * bwtfm = nullptr;
    // 先打印作者和用法
    printAuthor();
    printUsage();
    // prompt 提示符
    std::cout << std::endl << ">>> ";
    // 第一次输入命令
    std::string instr;
    std::cin >> instr;
    // 转换为大写
    std::transform(instr.begin(), instr.end(), instr.begin(),
        [](char c) -> char { return std::toupper(c); });
    while(instr != "QUIT")
    {
        // 帮助
        if(instr == "HELP")
        {
            printUsage();
        }
        // 从文件加载
        else if(instr == "FILELOAD")
        {
            std::string fname;
            // 删除额外空格
            std::cin >> std::ws;
            std::getline(std::cin, fname);
            while(fname.back() == ' ' || fname.back() == '\r'
                || fname.back() == '\n' || fname.back() == '\0')
            {
                fname.pop_back();
            }
            std::cout << "加载文件中..." << std::endl;
            // 打开文件
            std::ifstream file(fname);
            if(!file.is_open())
            {
                std::cout << "无法打开文件！" << std::endl;
                std::cout << std::endl << ">>> ";
                std::cin >> instr;
                std::transform(instr.begin(), instr.end(), instr.begin(),
                    [](char c) -> char { return std::toupper(c); });
                continue;
            }
            // 文件直接构建字符串
            std::string text;
            // 跳过第一行长度
            std::getline(file, text);
            std::getline(file, text);
            // 关闭文件
            file.close();
            // 先删除原来的
            delete bwtfm;
            std::cout << "加载索引中..." << std::endl;
            std::clock_t startTime = std::clock();
            // 创建新的
            bwtfm = new BWT_FM(text);
            std::clock_t endTime = std::clock();
            // 输出用时
            std::cout << "加载索引用时: " << double(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;
        }
        // 从字符串构建
        else if(instr == "STRLOAD")
        {
            std::string text;
            std::cin >> text;
            delete bwtfm;
            std::cout << "加载索引中..." << std::endl;
            std::clock_t startTime = std::clock();
            bwtfm = new BWT_FM(text);
            std::clock_t endTime = std::clock();
            std::cout << "加载索引用时: " << double(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;
        }
        // 输出BWT结果
        else if(instr == "BWT")
        {
            // 如果 bwtfm 不存在
            if(bwtfm == nullptr)
            {
                std::cout << "尚未加载文件！" << std::endl;
                std::cout << std::endl << ">>> ";
                std::cin >> instr;
                std::transform(instr.begin(), instr.end(), instr.begin(),
                    [](char c) -> char { return std::toupper(c); });
                continue;
            }
            std::cout << "BWT 为: " << std::endl;
            std::cout << bwtfm -> bwt() << std::endl;
        }
        // 子串匹配
        else if(instr == "FILEMATCH")
        {
            // 子串
            std::string sub;
            std::string fname;
            // 删除额外空格
            std::cin >> std::ws;
            std::getline(std::cin, fname);
            while(fname.back() == ' ' || fname.back() == '\r'
                || fname.back() == '\n' || fname.back() == '\0')
            {
                fname.pop_back();
            }
            std::cout << "加载文件中..." << std::endl;
            // 打开文件
            std::ifstream file(fname);
            if(!file.is_open())
            {
                std::cout << "无法打开文件！" << std::endl;
                std::cout << std::endl << ">>> ";
                std::cin >> instr;
                std::transform(instr.begin(), instr.end(), instr.begin(),
                    [](char c) -> char { return std::toupper(c); });
                continue;
            }
            // 文件直接构建字符串
            std::string text;
            while(!file.eof())
            {
                std::getline(file, text);
                while(text.back() == '\r' || text.back() == '\n')
                {
                    text.pop_back();
                }
                sub += text;
            }
            // 关闭文件
            file.close();
            if(bwtfm == nullptr)
            {
                std::cout << "尚未加载文件！" << std::endl;
                std::cout << std::endl << ">>> ";
                std::cin >> instr;
                std::transform(instr.begin(), instr.end(), instr.begin(),
                    [](char c) -> char { return std::toupper(c); });
                continue;
            }
            std::cout << "匹配结果:" << std::endl;
            std::clock_t startTime = std::clock();
            bwtfm -> displayPatternQuery(sub);
            std::clock_t endTime = std::clock();
            std::cout << "匹配用时: " << double(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;
        }
        else if(instr == "STRMATCH")
        {
            // 子串
            std::string sub;
            std::cin >> sub;
            if(bwtfm == nullptr)
            {
                std::cout << "尚未加载文件！" << std::endl;
                std::cout << std::endl << ">>> ";
                std::cin >> instr;
                std::transform(instr.begin(), instr.end(), instr.begin(),
                    [](char c) -> char { return std::toupper(c); });
                continue;
            }
            std::cout << "匹配结果:" << std::endl;
            std::clock_t startTime = std::clock();
            bwtfm -> displayPatternQuery(sub);
            std::clock_t endTime = std::clock();
            std::cout << "匹配用时: " << double(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;
        }
        else
        {
            std::cout << "无效指令！" << std::endl;
            // 再次提示用法
            printUsage();
        }
        std::cout << std::endl << ">>> ";
        std::cin >> instr;
        std::transform(instr.begin(), instr.end(), instr.begin(),
            [](char c) -> char { return std::toupper(c); });
    }
    delete bwtfm;
    return 0;
}