#include "HardwareInfo.h"
#include <Windows.h>

int main(int argc, char** argv)
{
	//computerType();
	//computerType2();
	//getSysVersion();
	//getScreenScale();
	//GetZoom();
	//getSysInfo();
	//NetCardStateChange(true, "ZTN Adapter V1 #2");
	//adapterTest2();
	//ChangeNetState();
	//driverRestart();
	std::vector<CAPITAL_HARDWARE> moniter_arr;
	DetectMoniter(moniter_arr);

	system("pause");
	return 0;

}