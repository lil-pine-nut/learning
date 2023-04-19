#include "common.h"
#include "sslthread.h"

void do_server_loop(BIO *conn)
{
	// int done, err, nread;
	// char buf[80];
	// //err=0;
	// do
	// {
	// 	for (nread=0; nread<sizeof(buf); nread+=err)
	// 	{
	// 		err=BIO_read(conn, buf+nread, sizeof(buf)-nread);
	// 		if (err<=0)
	// 			break;
	// 	}
	// 	fwrite(buf, 1, nread, stdout);
	// }
	// while(err>0);
	int 	err, nwritten;
	char 	buf[80];
	memcpy(buf, "hello", 6);
	for (;;)
	{
		// if (!fgets(buf, sizeof(buf), stdin))
		// 	break;
		for (nwritten = 0; nwritten < sizeof(buf); nwritten += err)
		{
			err = BIO_write(conn, buf + nwritten, strlen(buf) - nwritten);
			if (err <=0)
				return;
		}
	}
}

void THREAD_CC server_thread(void* arg)
{
	BIO* client =(BIO *)arg;

#ifndef WIN32
	pthread_detach(pthread_self());
#endif
	fprintf(stderr, "Connection opened.\n");
	do_server_loop(client);
	fprintf(stderr, "connection closed.\n");

	BIO_free(client);
	ERR_remove_state(0);
#ifdef WIN32
	_endthread();
#endif
}


int main (int argc, char *argv[])
{
	BIO *acc, *client;
	THREAD_TYPE tid;

	init_OpenSSL();

	acc = BIO_new_accept(PORT);
	if(!acc)
		int_error("Error creating server socket");
	if (BIO_do_accept(acc) <=0)
		int_error("Error binding server socket");

	for (;;)
	{
		if (BIO_do_accept(acc) <=0)
			int_error("Error accepting connection");

		client = BIO_pop(acc);
		THREAD_CREATE(tid, server_thread, client);
	}

	BIO_free(acc);
	return 0;
}

