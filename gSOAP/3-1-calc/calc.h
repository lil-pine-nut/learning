// gsoap ns service name: calc
// gsoap ns service style: rpc
// gsoap ns service encoding: encoded
// gsoap ns service namespace: http://127.0.0.1:8089/calc.wsdl
// gsoap ns service location: http://127.0.0.1:8089/cal
// gsoap ns schema  namespace:    urn:calc
int ns__add(double a, double b, double *result);
int ns__sub(double a, double b, double *result);
int ns__mul(double a, double b, double *result);
int ns__div(double a, double b, double *result);
int ns__pow(double a, double b, double *result);