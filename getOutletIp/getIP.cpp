#include <string.h>
#include <iostream>
#include <regex>

using namespace std;



int main()
{
	/*string strSrc = "ipCallback({ip:\"183.136.182.138\"})";
	string strIp = "183.136.182.138";
	regex ipPattern("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
	regex domainPattern("([0-9A-Za-z\\-_\\.]+)\\.([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");

	bool bMatch = false;

	if (regex_match(strIp, ipPattern)) {
		bMatch = true;
	}
	*/

	std::string s("ipCallback({ip:\"183.136.182.138\"})");
	std::smatch m;
	std::regex e("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
	regex ipPattern("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");

	/*while (std::regex_search(s, m, e)) {
		for (auto x : m) std::cout << x << " ";
		std::cout << std::endl;
		s = m.suffix().str();
	}*/

	bool bMatch = false;
	string strIP;
	while (std::regex_search(s, m, e)) {
		for (auto x : m) {
			if (regex_match((string)x, ipPattern)) {
				bMatch = true;
				strIP = (string)x;
				break;
			}
		}
		if (bMatch) {
			break;
		}
		s = m.suffix().str();
	}

	cout << strIP << endl;

	return 0;
}
