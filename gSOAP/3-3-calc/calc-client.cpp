#include "soapH.h"
#include "calc.nsmap"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Input fomat: Execute-Bin <Port>" << std::endl;
        return 0;
    }

    struct soap CalcSoap;
    soap_init(&CalcSoap);
    std::string server_addr = std::string("http://127.0.0.1:") + argv[1];
    std::cerr << "server_addr = " << server_addr << std::endl;

    double result = 0;
    if (SOAP_OK == soap_call_ns__add(&CalcSoap, server_addr.c_str(), "", 1, 2, &result))
    {
        std::cerr << "1+2 = " << result << std::endl;
    }
    else
    {
        std::cerr << "Error while calling the soap_call_ns__add: " << std::endl;
        soap_stream_fault(&CalcSoap, std::cerr);
    }

    if (SOAP_OK == soap_call_ns__sub(&CalcSoap, server_addr.c_str(), "", 1000, 7, &result))
    {
        std::cerr << "1000-7 = " << result << std::endl;
    }
    else
    {
        std::cerr << "Error while calling the soap_call_ns__sub: " << std::endl;
        soap_stream_fault(&CalcSoap, std::cerr);
    }

    if (SOAP_OK == soap_call_ns__mul(&CalcSoap, server_addr.c_str(), "", 9, 9, &result))
    {
        std::cerr << "9*9 = " << result << std::endl;
    }
    else
    {
        std::cerr << "Error while calling the soap_call_ns__mul: " << std::endl;
        soap_stream_fault(&CalcSoap, std::cerr);
    }

    if (SOAP_OK == soap_call_ns__div(&CalcSoap, server_addr.c_str(), "", 9, 9, &result))
    {
        std::cerr << "1000/7 = " << result << std::endl;
    }
    else
    {
        std::cerr << "Error while calling the soap_call_ns__mul: " << std::endl;
        soap_stream_fault(&CalcSoap, std::cerr);
    }

    if (SOAP_OK == soap_call_ns__pow(&CalcSoap, server_addr.c_str(), "", 2, 10, &result))
    {
        std::cerr << "2^10 = " << result << std::endl;
    }
    else
    {
        std::cerr << "Error while calling the soap_call_ns__pow: " << std::endl;
        soap_stream_fault(&CalcSoap, std::cerr);
    }

    soap_done(&CalcSoap); //分离运行时的环境
    return 0;
}