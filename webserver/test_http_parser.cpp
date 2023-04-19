

#include "http_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

struct line
{
  char *field;
  size_t field_len;
  char *value;
  size_t value_len;
};

#define CURRENT_LINE (&header[nlines - 1])
#define MAX_HEADER_LINES 2000

static struct line header[MAX_HEADER_LINES];
static int nlines = 0;
static int last_was_value = 0;

int on_header_field(http_parser *_, const char *at, size_t len)
{
  if (last_was_value)
  {
    nlines++;

    if (nlines == MAX_HEADER_LINES)
      ; // error!

    CURRENT_LINE->value = NULL;
    CURRENT_LINE->value_len = 0;

    CURRENT_LINE->field_len = len;
    CURRENT_LINE->field = (char *)malloc(len + 1);
    strncpy(CURRENT_LINE->field, at, len);
  }
  else
  {
    assert(CURRENT_LINE->value == NULL);
    assert(CURRENT_LINE->value_len == 0);

    CURRENT_LINE->field_len += len;
    CURRENT_LINE->field = (char *)realloc(CURRENT_LINE->field,
                                          CURRENT_LINE->field_len + 1);
    strncat(CURRENT_LINE->field, at, len);
  }

  CURRENT_LINE->field[CURRENT_LINE->field_len] = '\0';
  last_was_value = 0;
}

int on_header_value(http_parser *_, const char *at, size_t len)
{
  if (!last_was_value)
  {
    CURRENT_LINE->value_len = len;
    CURRENT_LINE->value = (char *)malloc(len + 1);
    strncpy(CURRENT_LINE->value, at, len);
  }
  else
  {
    CURRENT_LINE->value_len += len;
    CURRENT_LINE->value = (char *)realloc(CURRENT_LINE->value,
                                          CURRENT_LINE->value_len + 1);
    strncat(CURRENT_LINE->value, at, len);
  }

  CURRENT_LINE->value[CURRENT_LINE->value_len] = '\0';
  last_was_value = 1;
}

static http_parser *parser;

int on_message_begin(http_parser *_)
{
  (void)_;
  printf("\n***MESSAGE BEGIN***\n\n");
  return 0;
}

int on_headers_complete(http_parser *_)
{
  (void)_;
  printf("\n***HEADERS COMPLETE***\n\n");
  return 0;
}

int on_message_complete(http_parser *_)
{
  (void)_;
  printf("\n***MESSAGE COMPLETE***\n\n");
  return 0;
}

int on_url(http_parser *_, const char *at, size_t length)
{
  (void)_;
  printf("Url: %.*s\n", (int)length, at);

  return 0;
}

int on_body(http_parser *_, const char *at, size_t length)
{
  (void)_;
  printf("Body: %.*s\n", (int)length, at);
  return 0;
}

int main()
{
  http_parser_settings parser_set;

  // http_parser的回调函数，需要获取HEADER后者BODY信息，可以在这里面处理。
  parser_set.on_message_begin = on_message_begin;
  parser_set.on_header_field = on_header_field;
  parser_set.on_header_value = on_header_value;
  parser_set.on_url = on_url;
  parser_set.on_body = on_body;
  parser_set.on_headers_complete = on_headers_complete;
  parser_set.on_message_complete = on_message_complete;

  char buf[1024] = "GET / HTTP/1.1\n"
                   "Host: 127.0.0.1:9090\n"
                   "Connection: keep-alive\n"
                   "Cache-Control: max-age=0\n"
                   "Upgrade-Insecure-Requests: 1\n"
                   "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.51 Safari/537.36\n"
                   "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\n"
                   "Accept-Encoding: gzip, deflate\n"
                   "Accept-Language: zh-CN,zh;q=0.9\n";

  size_t parsed;
  parser = (http_parser *)malloc(sizeof(http_parser)); // 分配一个http_parser

  for (int i = 0; i < 1; i++)
  {
    http_parser_init(parser, HTTP_REQUEST);                              // 初始化parser为Request类型
    parsed = http_parser_execute(parser, &parser_set, buf, strlen(buf)); // 执行解析过程
  }

  free(parser);
  parser = NULL;
}
