#pragma once
#include <string>
class CEncrypt
{
public:
	//���ܹ����� ����
	static CEncrypt& getInstance();

	/*
	@brief : ˽Կ����
	@para  : clear_text  -[i] ��Ҫ���м��ܵ�����
			 pri_key     -[i] ˽Կ
	@return: ���ܺ������
	**/
	std::string RsaPriEncrypt(const std::string& clear_text, const std::string& pri_key);

	/*
	@brief : ��Կ����
	@para  : cipher_text -[i] ���ܵ�����
			 pub_key     -[i] ��Կ
	@return: ���ܺ������
	**/
	std::string RsaPubDecrypt(const std::string& cipher_text, const std::string& pub_key);


private:

	CEncrypt() = default;
	~CEncrypt() = default;

};

