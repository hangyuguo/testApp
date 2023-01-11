#pragma once

void computerType();

void computerType2();

void getSysVersion();

double GetZoom();

void getScreenScale();

void getSysInfo();

//获取网卡状态
void netDriverStatus();

//---------------------------------------------------------------------------
//功能:ChangeNetState ZTN网卡的启用与禁用
//参数:Enabled   true = 启用   false = 禁用
//返回值
//---------------------------------------------------------------------------
void ChangeNetState(bool bEnable);