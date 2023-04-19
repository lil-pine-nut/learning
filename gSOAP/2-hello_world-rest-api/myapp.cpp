// myapp.cpp
#include "soapH.h"    // include the generated source code headers
#include "soap.nsmap" // include XML namespaces

// api.cpp
hello::hello(const std::string &text)
{
  name = text;
}
greeting::greeting(const std::string &text)
{
  message = text;
}

int main()
{
  struct soap *soap = soap_new();
  hello request("world");
  greeting response;
  if (soap_POST_send_hello(soap, "http://localhost:8080/cgi-bin/api.cgi", &request) == SOAP_OK && soap_POST_recv_greeting(soap, &response) == SOAP_OK)
    std::cout << response.message << std::endl;
  soap_destroy(soap);
  soap_end(soap);
  soap_free(soap);
}