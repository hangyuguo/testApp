#include <ctime>
#include <iostream>
#include <windows.h>

using namespace std;

int main()
{
	//���ڵ�ǰϵͳ�ĵ�ǰ����/ʱ��
	time_t now = time(0);
	cout << "1970 ��Ŀǰ��������:" << now << endl;

	cout << "--------------------" << endl;

	// �� now ת��Ϊ�ַ�����ʽ
	char* dt = ctime(&now);
	cout << "ϵͳ���ں�ʱ�䣺" << dt << endl;

	cout << "--------------------" << endl;

	//����ʱ��
	tm* localTm = localtime(&now);
	dt = asctime(localTm);
	cout << "�������ں�ʱ�䣺" << dt << endl;
	// ��� tm �ṹ�ĸ�����ɲ���
	cout << "��: " << 1900 + localTm->tm_year << endl;
	cout << "��: " << 1 + localTm->tm_mon << endl;
	cout << "��: " << localTm->tm_mday << endl;
	cout << "ʱ��: " << localTm->tm_hour << ":";
	cout << localTm->tm_min << ":";
	cout << localTm->tm_sec << endl;

	cout << "--------------------" << endl;

	//UTCʱ��
	tm* gmtm = gmtime(&now);
	dt = asctime(gmtm);
	cout << "UTC ���ں�ʱ�䣺" << dt << endl;

	cout << "--------------------" << endl;
	//mktime
	gmtm->tm_year = 2030-1900;
	gmtm->tm_mon = 1;
	gmtm->tm_mday = 1;

	time_t now2 = mktime(gmtm);
	dt = ctime(&now2);
	cout << "�޸ĺ�����ں�ʱ�䣺" << dt << endl;

	cout << "--------------------" << endl;

	//clock_t
	cout << "CLOCKS_PER_SEC:" << CLOCKS_PER_SEC << endl;

	long i = 10000000L;
	clock_t start, finish;
	double duration;
	/* ����һ���¼�������ʱ��*/
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
	cout << "��ʽ�����ʱ��:" << buffer << endl;

	cout << "--------------------" << endl;


	//difftime
	Sleep(2000);
	time_t now3 = time(0);
	double diff = difftime(now3, now);
	cout << "ʱ���Ϊ:" << diff <<"��"<< endl;

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
