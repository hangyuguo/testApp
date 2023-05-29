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
	//��̬���� �̰߳�ȫ
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

	// ��ȡRSA���ο��Դ�������ݿ����󳤶�
	int key_len = RSA_size(rsa);
	int block_len = key_len - 11;    // ��Ϊ��䷽ʽΪRSA_PKCS1_PADDING, ����Ҫ��key_len�����ϼ�ȥ11

	// �����ڴ棺�������ܺ����������
	char* sub_text = new char[key_len + 1];
	memset(sub_text, 0, key_len + 1);
	int ret = 0;
	int pos = 0;
	std::string sub_str;
	// �����ݽ��зֶμ��ܣ�����ֵ�Ǽ��ܺ����ݵĳ��ȣ�
	while (pos < clear_text.length()) {
		sub_str = clear_text.substr(pos, block_len);
		memset(sub_text, 0, key_len + 1);
		ret = RSA_private_encrypt(sub_str.length(), (const unsigned char*)sub_str.c_str(), (unsigned char*)sub_text, rsa, RSA_PKCS1_PADDING);
		if (ret >= 0) {
			encrypt_text.append(std::string(sub_text, ret));
		}
		pos += block_len;
	}

	// �ͷ��ڴ�  
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

	// ע��-------ʹ�õ�1�ָ�ʽ�Ĺ�Կ���н���
	//rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);
	// ע��-------ʹ�õ�2�ָ�ʽ�Ĺ�Կ���н��ܣ�����ʹ�����ָ�ʽ��Ϊʾ����
	rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
	if (!rsa)
	{
		unsigned long err = ERR_get_error(); //��ȡ�����
		char err_msg[1024] = { 0 };
		//ERR_error_string(ulErr, szErrMsg); // ��ʽ��error:errId:��:����:ԭ��
		printf("err msg: err:%ld, msg:%s\n", err, err_msg);
		BIO_free_all(keybio);

		return decrypt_text;
	}

	// ��ȡRSA���δ������󳤶�
	int len = RSA_size(rsa);
	char* sub_text = new char[len + 1];
	memset(sub_text, 0, len + 1);
	int ret = 0;
	std::string sub_str;
	int pos = 0;
	// �����Ľ��зֶν���
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

	// �ͷ��ڴ�  
	delete sub_text;
	BIO_free_all(keybio);
	RSA_free(rsa);

	return decrypt_text;
}
