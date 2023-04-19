#include <curl/curl.h>

#undef DISABLE_SSH_AGENT

struct FtpFile
{
  const char *filename;
  FILE *stream;
};

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb,
                        void *stream)
{
  struct FtpFile *out = (struct FtpFile *)stream;
  if (!out->stream)
  {
    /* open file for writing */
    out->stream = fopen(out->filename, "wb");
    if (!out->stream)
      return -1; /* failure, can't open file to write */
  }
  return fwrite(buffer, size, nmemb, out->stream);
}

// sftp协议
void get_sftp_File_directory()
{
  CURL *curl;
  CURLcode res;

  const char *filename = "/home/lws/learn-curl/example/sftp/a.txt";    //本地文件名
  const char *remote_url = "sftp://sftpuser11:123456@127.0.0.1:2222/"; //服务器路径

  struct FtpFile ftpfile =
      {
          filename, /* name to store the file as if successful */
          NULL};

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  if (curl)
  {
    /*
     * You better replace the URL with one that works!
     * sftp://user:password@example.com/etc/issue - This specifies the file /etc/issue
     */
    curl_easy_setopt(curl, CURLOPT_URL, remote_url);
    /* Define our callback to get called when there's data to be written */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
    /* Set a pointer to our struct to pass to the callback */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

#ifndef DISABLE_SSH_AGENT
    /* We activate ssh agent. For this to work you need
       to have ssh-agent running (type set | grep SSH_AGENT to check) or
       pageant on Windows (there is an icon in systray if so) */
    curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
#endif

    /* Switch on full protocol/debug output */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);

    if (CURLE_OK != res)
    {
      /* we failed */
      fprintf(stderr, "curl told us %d\n", res);
    }
  }

  if (ftpfile.stream)
    fclose(ftpfile.stream); /* close the local file */

  curl_global_cleanup();
}

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#define REMOTE_URL "sftp://sftpuser11:123456@127.0.0.1:2222/"

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
  curl_off_t nread;
  size_t retcode = fread(ptr, size, nmemb, (FILE *)stream);
  nread = (curl_off_t)retcode;
  fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T " bytes from file\n", nread);
  return retcode;
}

int main()
{
  // if(argc != 3)
  // {
  //     printf("2 paramenters is needed.\n");
  //     return -1;
  // }

  char *local_file = "/home/lws/software/curl-7.45.0.tar.gz";
  char *remote_file = "curl-7.45.0.tar.gz";

  printf("call me for update -:%s,%s\n", local_file, remote_file);

  char remoteurl[1024] = {0};
  strcpy(remoteurl, REMOTE_URL);
  strcat(remoteurl, remote_file);

  CURL *curl;
  CURLcode res;
  FILE *file;
  struct stat file_info;
  curl_off_t fsize;

  if (stat(local_file, &file_info))
  {
    printf("couldnt open '%s': %s\n", local_file, strerror(errno));
    return 1;
  }

  fsize = (curl_off_t)file_info.st_size;
  printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fsize);

  file = fopen(local_file, "rb");
  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();

  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)65526 * 10);
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, (curl_off_t)65536 * 10); //最大也是65536字节，无法再大
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, remoteurl);
    curl_easy_setopt(curl, CURLOPT_READDATA, file);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);
    curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 120);
#ifndef DISABLE_SSH_AGENT
    /* We activate ssh agent. For this to work you need
    to have ssh-agent running (type set | grep SSH_AGENT to check) or
    pageant on Windows (there is an icon in systray if so) */
    curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
#endif
    /* 打开完整的协议/调试输出*/
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    fprintf(stderr, "finished update.");
  }

  fclose(file);
  curl_global_cleanup();
  return 0;
}
