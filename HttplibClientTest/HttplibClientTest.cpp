// HttplibServerTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "httplib.h"
#include "CEncrypt/CEncrypt.h"
#include "base64/base64.h"
#include "CWinFirewall.h"

#define RSA_PRIVATE_KEY "-----BEGIN RSA PRIVATE KEY-----\n\
MIICXQIBAAKBgQDO/Ossc9MFTUonGLDJiJ4p+Xp30XO61D/Gd8OPXju1pBkQ7Gk+\n\
0RWOm3hHuD1UyNmoPS7r+LhhpDOqZYuR6ilPs14faSqZbe1OXcXiR0ijW+jRLv+p\n\
32niq4pYfCpLZAW94Mg7UM6Jd6Ess8vJclpobxA+XPgijUqtijZ7AT1B7QIDAQAB\n\
AoGBAKBhYGeFJ1DIXliEzRabANJNOBN2+1Eb/H5vddu0jFdl2vY9+B5wK1/gHwaT\n\
PxJevieZoK2+fj9J166EF+zrny15dpGwIE47KXf9bs+e4aWeZk4qp14N24el3whr\n\
ax7EcNcFYVMWBX/bJOr1ECbSxmo6TGc5WFleycKb84ntXAKRAkEA52ZeeJG89j7V\n\
p0Sv8XC8XxOCEwGUpfwmYWk7WFN80M93A9Upk29+g57AVwS0DlwGHjiDutFVCT9d\n\
mpP9HSQq5wJBAOT+K6lsXUlxmK4+Ln7QKAAuW035j/Hk2FUldUAiLJvN8ZKEZiz2\n\
t7UCQFn6qtJfebloRF/j4Ve+wdywRoMiBgsCQExsvfTaRWua0xu8jtOSJVTNb59x\n\
XTP49IAlrkj8wfOBIdCtPV8JauJzsxjKX+GCOS8QAUnZS1mTiQm9qNJQGK8CQHqb\n\
pKEYRnKWght5a8e2iFRgfb6oSuxHjZU5cygdpCjgAeJjRZn5l68xP2duEZyGRlSB\n\
gWqeRv+GQaJ6N6bqxrUCQQDFaaU/Ip+OvvpkBx21f7l8YcpRyKt1ZaBvuRXVw1vZ\n\
8eyhl0BnkCCzoe1/oPswpg8N0tudYCUR+8Cgr/JEr3kd\n\
-----END RSA PRIVATE KEY-----"


int main(int argc, char* argv[])
{
    //std::wstring program_name, program_path;
    //program_name = L"NeitStart.exe";
    //program_path = L"C:\\Program Files (x86)\\ITtools\\AdmittanceLite\\222\\NeitStart.exe";
    ////program_path = L"C:\222";

    //CWinFirewall::AddFirewall(program_path, program_name);

    //system("pause");

    /*std::wstring program_name, program_path;
    wchar_t* path = new wchar_t[1024];
    GetModuleFileName(NULL, path, 1024);

    std::wstring rulename = L"NeitStartRule";
    std::string program = "C:\\Program Files(x86)\\ITtools\\AdmittanceLite\\444\\NeitStart.exe";

    wchar_t* path = new wchar_t[1024];
    GetModuleFileName(NULL, path, 1024);
    
    std::wstring cmd = L"netsh advfirewall firewall add rule name=\"" + rulename + L"\" dir=in action=allow program=\"" + std::wstring(path) + L"\" enable=yes";
    system(cmd.c_str());

    delete path;*/
    //return 0;



    httplib::Client client("127.0.0.1", 9001);

    httplib::Params params;
    //std::string hostname = "HIH-D-23176";
    std::string hostname = "HIH-L-3296Z";
    std::string hostnameEnc, hostnameEncBase64;
    hostnameEnc = CEncrypt::getInstance().RsaPriEncrypt(hostname, RSA_PRIVATE_KEY);
    hostnameEncBase64 = encode64(hostnameEnc);

    params.emplace("hostname", hostnameEncBase64);

    auto res = client.Post("/neitstart/reboot", params);
    if (res->status == 200)
    {
        std::cout << "OK:" << res->body << std::endl;

    }else
    {
        auto err = res.error();
        std::cout << "err:" << err << std::endl;
    }
    system("pause");
    return 0;
}
