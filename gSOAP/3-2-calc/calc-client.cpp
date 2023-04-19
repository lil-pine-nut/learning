#include "soapcalcProxy.h"
#include "calc.nsmap"
#include "iostream"

int main()
{
    calcProxy p;

    double result = 0;
    if (SOAP_OK == p.add(1, 2, &result))
    {
        std::cerr << "1+2 = " << result << std::endl;
    }
    else
    {
        p.soap_stream_fault(std::cerr);
    }

    if (SOAP_OK == p.sub(1000, 7, &result))
    {
        std::cerr << "1000-7 = " << result << std::endl;
    }
    else
    {
        p.soap_stream_fault(std::cerr);
    }

    if (SOAP_OK == p.mul(9, 9, &result))
    {
        std::cerr << "9*9 = " << result << std::endl;
    }
    else
    {
        p.soap_stream_fault(std::cerr);
    }

    if (SOAP_OK == p.div(1000, 7, &result))
    {
        std::cerr << "1000/7 = " << result << std::endl;
    }
    else
    {
        p.soap_stream_fault(std::cerr);
    }

    if (SOAP_OK == p.pow(2, 8, &result))
    {
        std::cerr << "2^8 = " << result << std::endl;
    }
    else
    {
        p.soap_stream_fault(std::cerr);
    }

    std::cerr << "finish" << std::endl;

    return 0;
}