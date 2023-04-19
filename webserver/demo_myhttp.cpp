#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "myhttpserver.h"

int main(int argc, char **argv)
{
	Myhttpserver httpserver;
	if (!httpserver.inithttp())
	{
		printf(httpserver.geterrmsg());
		return 1;
	}

	if (!httpserver.start(5000))
	{
		printf(httpserver.geterrmsg());
		return 1;
	}
	httpserver.set_gencb(HttpGenericCallback);
	httpserver.dispatch();
	httpserver.free();

	return 0;
}
