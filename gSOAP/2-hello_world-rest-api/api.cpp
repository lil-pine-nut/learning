#include "soapH.h"           // include the generated source code headers
#include "soap.nsmap"        // include XML namespaces
#include "plugin/httppost.h" // HTTP POST plugin for gSOAP servers

// api.cpp
hello::hello(const std::string &text)
{
  name = text;
}
greeting::greeting(const std::string &text)
{
  message = text;
}

int soap_serve(struct soap *soap)
{
  soap->keep_alive = soap->max_keep_alive + 1;
  do
  {
    if (soap->keep_alive > 0 && soap->max_keep_alive > 0)
      soap->keep_alive--;
    if (soap_begin_serve(soap))
    {
      if (soap->error >= SOAP_STOP)
        continue;
      return soap->error;
    }
    soap_closesock(soap);
  } while (soap->keep_alive);
  return SOAP_OK;
}

int xml_handler(struct soap *);
struct http_post_handlers handlers[] =
    {
        {"text/xml", xml_handler}, // handle POST text/xml messages (you can add other MIME types and handlers here)
        {NULL}};

// main
int main()
{
  struct soap *soap = soap_new();                      // new context
  soap_register_plugin_arg(soap, http_post, handlers); // register HTTP POST plugin
  soap_serve(soap);                                    // run server
  soap_destroy(soap);                                  // clean up
  soap_end(soap);                                      // clean up
  soap_free(soap);                                     // free context
}
// soap_PUT_greeting 函数貌似有问题 根据 https://www.genivia.com/dev.html#how-rest
int xml_handler(struct soap *soap) // REST API function
{
  hello request;
  if (soap_get_hello(soap, &request, "hello", NULL)) // try getting <hello>
    return soap->error;                              // no <hello> (you can try to get other XML here)
  greeting response("Hello " + request.name);        // create <greeting><message>Hello name</message></greeting>
  soap->http_content = "text/xml";                   // HTTP content: text/xml
  if (soap_response(soap, SOAP_FILE)                 // HTTP 200 OK
      || response.soap_put(soap, "greeting", NULL)   // HTTP body with <greeting>
      || soap_end_send(soap))
    return soap->error;
  return SOAP_OK;
}