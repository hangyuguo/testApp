#include <ctime>
#include <iostream>
#include <windows.h>

using namespace std;

int main()
{
	//基于当前系统的当前日期/时间
	time_t now = time(0);
	cout << "1970 到目前经过秒数:" << now << endl;

	cout << "--------------------" << endl;

	// 把 now 转换为字符串形式
	char* dt = ctime(&now);
	cout << "系统日期和时间：" << dt << endl;

	cout << "--------------------" << endl;

	//本地时间
	tm* localTm = localtime(&now);
	dt = asctime(localTm);
	cout << "本地日期和时间：" << dt << endl;
	// 输出 tm 结构的各个组成部分
	cout << "年: " << 1900 + localTm->tm_year << endl;
	cout << "月: " << 1 + localTm->tm_mon << endl;
	cout << "日: " << localTm->tm_mday << endl;
	cout << "时间: " << localTm->tm_hour << ":";
	cout << localTm->tm_min << ":";
	cout << localTm->tm_sec << endl;

	cout << "--------------------" << endl;

	//UTC时间
	tm* gmtm = gmtime(&now);
	dt = asctime(gmtm);
	cout << "UTC 日期和时间：" << dt << endl;

	cout << "--------------------" << endl;
	//mktime
	gmtm->tm_year = 2030-1900;
	gmtm->tm_mon = 1;
	gmtm->tm_mday = 1;

	time_t now2 = mktime(gmtm);
	dt = ctime(&now2);
	cout << "修改后的日期和时间：" << dt << endl;

	cout << "--------------------" << endl;

	//clock_t
	cout << "CLOCKS_PER_SEC:" << CLOCKS_PER_SEC << endl;

	long i = 10000000L;
	clock_t start, finish;
	double duration;
	/* 测量一个事件持续的时间*/
	printf("Time to do %ld empty loops is ", i);
	start = clock();
	while (i--);
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << duration << endl;

	cout << "--------------------" << endl;

	//strftime
	char buffer[80];
	strftime(buffer, 80, "Now it's %I:%M%p.", localTm);
	cout << "格式化后的时间:" << buffer << endl;

	cout << "--------------------" << endl;


	//difftime
	Sleep(2000);
	time_t now3 = time(0);
	double diff = difftime(now3, now);
	cout << "时间差为:" << diff <<"秒"<< endl;

	system("pause");
	return 0;
}

void stringParseTest() {
	string cert_details = "";
}


//Not After : Nov 10 01:53:38 2023 GMT

void getCSTTimeFormat(char* pStdTimeFormat)
{
	time_t nTimeStamp;
	time(&nTimeStamp);
	char pTmpString[256] = { 0 };
	tm* pTmStruct = localtime(&nTimeStamp);
	sprintf(pTmpString, "%04d-%02d-%02d %02d:%02d:%02d", pTmStruct->tm_year + 1900, pTmStruct->tm_mon + 1, pTmStruct->tm_mday, \
		pTmStruct->tm_hour, pTmStruct->tm_min, pTmStruct->tm_sec);

	strcpy(pStdTimeFormat, pTmpString);
	return;
}
