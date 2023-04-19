#include <pthread.h>
#include <openssl/crypto.h>
#include <unistd.h>
#include <stdlib.h>

int THREAD_setup(void);
int THREAD_cleanup(void);

#if defined(WIN32)
	#define MUTEX_TYPE			HANDLE
	#define MUTEX_SETUP(x) 		(x) = CreateMutex(NULL, FALSE, NULL)
	#define MUTEX_CLEANUP(x)	CloseHandle(x)
	#define MUTEX_LOCK(x)		WaitForSingleObject( (x), INFINITE)
	#define MUTEX_UNLOCK(x)		ReleaseMutex(x)
	#define THREAD_ID			GetCurrentThreadId()
#elif _POSIX_THREADS
	/* unistd.h pthreads */
	#define MUTEX_TYPE			pthread_mutex_t
	#define MUTEX_SETUP(x)		pthread_mutex_init(&(x), NULL)
	#define MUTEX_CLEANUP(x)	pthread_mutex_destroy(&(x))
	#define MUTEX_LOCK(x)		pthread_mutex_lock(&(x))
	#define MUTEX_UNLOCK(x)		pthread_mutex_unlock(&(x))
	#define THREAD_ID			pthread_self()
#else
	#error Need mutex operations for platform
#endif

static void locking_function (int mode, int n, const char* file, int line);
static unsigned long int id_function(void);
int THREAD_setup(void);
int THREAD_cleanup(void);

// dynamic locking extensions
struct CRYPTO_dynlock_value
{
	MUTEX_TYPE mutex;
};


static struct CRYPTO_dynlock_value* dyn_create_function(const char* file, int line);
static void dyn_lock_function(int mode, struct CRYPTO_dynlock_value* l, const char* file, int line);
static void dyn_destroy_function(struct CRYPTO_dynlock_value* l, const char* file, int line);

