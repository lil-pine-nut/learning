#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "HMAC_SHA1.h"
#include "ZBase64.h"
#include <sstream>
#include <iostream>
#include <assert.h>

using namespace std;

// C++实现对URL的编码和解码（支持ansi和utf8格式） https://blog.csdn.net/qq_37781464/article/details/113977888
unsigned char ToHex(unsigned char x)
{
	return x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z')
		y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z')
		y = x - 'a' + 10;
	else if (x >= '0' && x <= '9')
		y = x - '0';
	return y;
}

std::string UrlEncode(const std::string &str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

std::string UrlDecode(const std::string &str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+')
			strTemp += ' ';
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else
			strTemp += str[i];
	}
	return strTemp;
}

int main()
{
	string key = "axnHhlFmJvxLM8ZCPFzmM5antrxOUuu/L5W9YzmXUm4=";
	string res = "products/ux9jchZp8s/devices/mqtt_test_2022_06_d1";
	string et = "2547098723";
	string method = "sha1";
	string version = "2018-10-31";
	ostringstream StringForSignature;
	StringForSignature << et << "\n"
					   << method << "\n"
					   << res << "\n"
					   << version;
	cerr << StringForSignature << endl;

	ZBase64 the_ZBase64;
	int base64_decode_outbyte;
	string base64_decode_str = the_ZBase64.Decode(key.c_str(), key.size(), base64_decode_outbyte);
	cerr << "base64_decode_outbyte:" << base64_decode_outbyte << endl;
	cerr << "base64_decode_str.size():" << base64_decode_str.size() << endl;
	cerr << "key.size():" << key.size() << endl;

	// 各种算法得到的摘要的长度
	// 算法	摘要长度（字节）
	// MD2	16
	// MD5	16
	// SHA	20
	// SHA1	20
	// SHA224	28
	// SHA256	32
	// SHA384	48
	// SHA512	64
	char HMAC_buff[20];
	int HMAC_len = 0;
	CHMAC_SHA1 the_CHMAC_SHA1;
	the_CHMAC_SHA1.HMAC_SHA1((BYTE *)StringForSignature.str().c_str(), StringForSignature.str().size(),
							 (BYTE *)base64_decode_str.c_str(), base64_decode_str.size(), (BYTE *)HMAC_buff);

	string base64_encode_str = the_ZBase64.Encode((unsigned char *)HMAC_buff, sizeof(HMAC_buff));
	cerr << "base64_encode_str.size():" << base64_encode_str.size() << endl;

	ostringstream Token;
	Token << "version=" << UrlEncode(version) << "&res=" << UrlEncode(res) << "&et="
		  << UrlEncode(et) << "&method=" << UrlEncode(method) << "&sign=" << UrlEncode(base64_encode_str);
	cerr << Token.str() << endl;
	cerr << UrlEncode(base64_encode_str) << endl;

	return 0;
}