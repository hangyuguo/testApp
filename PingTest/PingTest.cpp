// PingTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
//#include "Ping.h"
//#include "network_base.h"
#include <string>
#include <codecvt>
#include <vector>

#include "rebootcmd.h"

using std::wstring;

//std::string Unicode2Utf8(std::wstring& widestring)
//{
//    using namespace std;
//    int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
//    if (utf8size == 0)
//    {
//        throw std::exception("Error in conversion.");
//    }
//    std::vector<char> resultstring(utf8size);
//    int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);
//    if (convresult != utf8size)
//    {
//        throw std::exception("La falla!");
//    }
//    return std::string(&resultstring[0]);
//}

//std::string unicodeToString(const std::string& str) {
//    std::string u8str;
//    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
//    for (size_t i = 0; i < str.length(); ) {
//        char32_t uhan = strtol(str.substr(i, 4).c_str(), nullptr, 16);
//        u8str += converter.to_bytes(uhan);
//
//        i += 4;
//    }
//
//    return u8str;
//}
//
//std::string WideToUTF8(const std::wstring& source)
//{
//    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
//    return conv.to_bytes(source);
//}

int main()
{
    char ch;
    printf("******************************************\n*\n");
    printf("* 1. Power_Off\n*\n");
    printf("* 2. Log_Off\n*\n");
    printf("* 3. ReBoot\n*\n");
    printf("******************************************\n");
    ch = getchar();
    while (1) {
        switch (ch) {
        case '1':
            if (!powerOffProc()) {
                printf("Process Error!\n");
                continue;
            }
            return 0;
        case '2':
            if (!logOffProc()) {
                printf("Process Error!\n");
                continue;
            }
            return 0;
        case '3':
            if (!reBootProc()) {
                printf("Process Error!\n");
                continue;
            }
            return 0;
        default:
            printf("Error!\n");
        }
    }
    system("pause");
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
