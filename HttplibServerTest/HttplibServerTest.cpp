// HttplibServerTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "httplib.h"
using namespace std;

void doGetHi(const httplib::Request& req, httplib::Response& res)
{
    res.set_content("hi", "text/plain");
}

void onReboot(const httplib::Request& req, httplib::Response& res)
{
    cout << "req.params" << req.body << endl;

    string name, note;
    if (req.has_param("name")) {
        name = req.get_param_value("name");
        cout << "value of name is:" << name << endl;
    }
    if (req.has_param("note")) {
        note = req.get_param_value("note");
        cout << "value of note is:" << note << endl;
    }
    string strRet = "get name:" + name + "; get note:" + note;
    res.set_content(strRet, "text/plain");
}

int main(int argc, char* argv[])
{
    httplib::Server server;
    server.Get("/hi", doGetHi);
    server.Post("/reboot", onReboot);

    auto ret = server.listen("0.0.0.0", 8901);

    return 0;
}
