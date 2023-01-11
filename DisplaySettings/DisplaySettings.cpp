// DisplaySettings.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <set>

using std::cout;
using std::endl;
using std::set;

struct LocalResolution {
    DWORD res_width;
    DWORD res_height;

    //比较运算符重载,按位置排序，必须const
    bool operator <(const LocalResolution& a) const
    {
        return res_width < a.res_width;
    }
};


std::set<LocalResolution> enumDisplayInfo();


int main()
{
    std::cout << "Hello World!\n";
    enumDisplayInfo();
}

std::set<LocalResolution> enumDisplayInfo() {

    std::set<LocalResolution> setRes;

    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(dm);
    for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum++) {
        //cout << "Mode #" << iModeNum << " = " << dm.dmPelsWidth << "x" << dm.dmPelsHeight << endl;
        LocalResolution re;
        re.res_width = dm.dmPelsWidth;
        re.res_height = dm.dmPelsHeight;
        setRes.insert(re);
    }

    for (auto re : setRes) {
        std::cout<< re.res_width << "x" << re.res_height << std::endl;
    }
    return setRes;
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
