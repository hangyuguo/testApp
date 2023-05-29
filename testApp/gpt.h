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

    // ���ص�ǰϵͳ��¼�Ļ�û�����
    int getActiveUserCount();

    // ��ȡ��ǰ�ڴ�ʹ�����
    void getMemoryUsage();

    // ��ȡ��ǰϵͳ��CPUʹ����
    double getCPUUsage();

    // ���ݴ������ƽ�ĳ�������ö�
    void setWindowTop(std::wstring windowName);

    // ���������
    int reboot();

    // base64 ת��
    void base64Test();

    // ��ȡ�豸����
    void getDeviceName();
};
