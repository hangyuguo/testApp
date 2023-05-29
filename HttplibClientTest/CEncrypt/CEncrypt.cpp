#include "CEncrypt.h"
#include "openssl/evp.h"
#include "openssl/ssl.h"
#include "openssl/sha.h"
#include "openssl/aes.h"
#include "openssl/x509.h"
#include "openssl/pkcs12.h"
#include "openssl/pem.h"
#include "openssl/engine.h"
#include "openssl/err.h"
#include "openssl/bio.h"
#include "openssl/rsa.h"

#define RSA_KEY_LENGTH 1024
#define RSA_PADDING RSA_PKCS1_PADDING

CEncrypt& CEncrypt::getInstance()
{
	//静态变量 线程安全
	static CEncrypt CEncrypter;
	return CEncrypter;
}

std::string CEncrypt::RsaPriEncrypt(const std::string& clear_text, const std::string& pri_key)
{
	std::string encrypt_text;
	BIO* keybio = BIO_new_mem_buf((unsigned char*)pri_key.c_str(), -1);
	RSA* rsa = RSA_new();
	rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
	if (!rsa)
	{
		BIO_free_all(keybio);
		return std::string("");
	}

	// 获取RSA单次可以处理的数据块的最大长度
	int key_len = RSA_size(rsa);
	int block_len = key_len - 11;    // 因为填充方式为RSA_PKCS1_PADDING, 所以要在key_len基础上减去11

	// 申请内存：存贮加密后的密文数据
	char* sub_text = new char[key_len + 1];
	memset(sub_text, 0, key_len + 1);
	int ret = 0;
	int pos = 0;
	std::string sub_str;
	// 对数据进行分段加密（返回值是加密后数据的长度）
	while (pos < clear_text.length()) {
		sub_str = clear_text.substr(pos, block_len);
		memset(sub_text, 0, key_len + 1);
		ret = RSA_private_encrypt(sub_str.length(), (const unsigned char*)sub_str.c_str(), (unsigned char*)sub_text, rsa, RSA_PKCS1_PADDING);
		if (ret >= 0) {
			encrypt_text.append(std::string(sub_text, ret));
		}
		pos += block_len;
	}

	// 释放内存  
	delete sub_text;
	BIO_free_all(keybio);
	RSA_free(rsa);

	return encrypt_text;

}

std::string CEncrypt::RsaPubDecrypt(const std::string& cipher_text, const std::string& pub_key)
{
	std::string decrypt_text;
	BIO* keybio = BIO_new_mem_buf((unsigned char*)pub_key.c_str(), -1);
	RSA* rsa = RSA_new();

	// 注意-------使用第1种格式的公钥进行解密
	//rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);
	// 注意-------使用第2种格式的公钥进行解密（我们使用这种格式作为示例）
	rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
	if (!rsa)
	{
		unsigned long err = ERR_get_error(); //获取错误号
		char err_msg[1024] = { 0 };
		//ERR_error_string(ulErr, szErrMsg); // 格式：error:errId:库:函数:原因
		printf("err msg: err:%ld, msg:%s\n", err, err_msg);
		BIO_free_all(keybio);

		return decrypt_text;
	}

	// 获取RSA单次处理的最大长度
	int len = RSA_size(rsa);
	char* sub_text = new char[len + 1];
	memset(sub_text, 0, len + 1);
	int ret = 0;
	std::string sub_str;
	int pos = 0;
	// 对密文进行分段解密
	while (pos < cipher_text.length()) {
		sub_str = cipher_text.substr(pos, len);
		memset(sub_text, 0, len + 1);
		ret = RSA_public_decrypt(sub_str.length(), (const unsigned char*)sub_str.c_str(), (unsigned char*)sub_text, rsa, RSA_PKCS1_PADDING);
		if (ret >= 0) {
			decrypt_text.append(std::string(sub_text, ret));
			printf("pos:%d, sub: %s\n", pos, sub_text);
			pos += len;
		}
	}

	// 释放内存  
	delete sub_text;
	BIO_free_all(keybio);
	RSA_free(rsa);

	return decrypt_text;
}
