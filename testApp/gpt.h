#pragma once
#include <string>
#include <vector>
//#include <experimental/filesystem>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

namespace gptTest {

    void deleteOldVersions(std::string version);

    // 返回当前系统登录的活动用户数量
    int getActiveUserCount();

    // 获取当前内存使用情况
    void getMemoryUsage();

    // 获取当前系统的CPU使用率
    double getCPUUsage();

    // 根据窗口名称将某个窗口置顶
    void setWindowTop(std::wstring windowName);

    // 重启计算机
    int reboot();

    // base64 转码
    void base64Test();

    // 获取设备名称
    void getDeviceName();
};
