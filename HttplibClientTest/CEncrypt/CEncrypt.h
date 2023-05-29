#pragma once
#include <string>
class CEncrypt
{
public:
	//加密管理类 单例
	static CEncrypt& getInstance();

	/*
	@brief : 私钥加密
	@para  : clear_text  -[i] 需要进行加密的明文
			 pri_key     -[i] 私钥
	@return: 加密后的数据
	**/
	std::string RsaPriEncrypt(const std::string& clear_text, const std::string& pri_key);

	/*
	@brief : 公钥解密
	@para  : cipher_text -[i] 加密的密文
			 pub_key     -[i] 公钥
	@return: 解密后的数据
	**/
	std::string RsaPubDecrypt(const std::string& cipher_text, const std::string& pub_key);


private:

	CEncrypt() = default;
	~CEncrypt() = default;

};

