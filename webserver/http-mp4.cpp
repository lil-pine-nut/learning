#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>

// 请求报文
class HttpRequest
{
public:
	// 请求行
	std::string method;
	std::string url;
	std::string http_version;
	// 请求首部
	std::unordered_map<std::string, std::string> headers;
	// GET方法无请求体
};

// 响应报文
class HttpResponse
{
public:
	// 状态行
	std::string http_version;
	int code;
	std::string status;
	// 响应首部
	std::unordered_map<std::string, std::string> headers;
	// 响应体
	char *body;
};

void error_handler(const char *msg)
{
	perror(msg);
	exit(1);
}

void do_client(int client_sock, const char *file_path)
{
	// 解析请求报文
	HttpRequest req;
	char buf[4096];
	// 客户端主动断开链接
	if (recv(client_sock, buf, sizeof(buf), MSG_PEEK | MSG_DONTWAIT) == 0)
	{
		std::cout << "client off" << std::endl;
		close(client_sock);
		return;
	}

	FILE *fp_read = fdopen(client_sock, "r");
	fgets(buf, sizeof(buf), fp_read);
	// 解析请求行
	std::string request_line(buf);
	// 找到两个空格
	int pos1 = request_line.find(" ");
	int pos2 = request_line.rfind(" ");
	req.method = request_line.substr(0, pos1);
	req.url = request_line.substr(pos1 + 1, pos2 - (pos1 + 1));
	// 尾部有"\r\n"
	req.http_version = request_line.substr(pos2 + 1, request_line.size() - 2 - (pos2 + 1));
	// 解析请求首部
	while (1)
	{
		fgets(buf, sizeof(buf), fp_read);
		std::string head(buf);
		if (head == "\r\n")
		{
			break;
		}
		else
		{
			int pos = head.find(":");
			auto key = head.substr(0, pos);
			// ": "有空格，尾部有"\r\n"
			auto value = head.substr(pos + 2, head.size() - 2 - (pos + 2));
			req.headers.insert({key, value});
		}
	}

	// 打印接收的请求首部
	for (const auto &c : req.headers)
	{
		std::cout << c.first << ": " << c.second << "\r\n";
	}

	// 生成响应报文
	HttpResponse resp;
	int video_fd = open(file_path, O_RDONLY);
	int video_length = lseek(video_fd, 0, SEEK_END) - lseek(video_fd, 0, SEEK_SET);

	resp.headers["Server"] = "ArchLinux";
	resp.headers["Connection"] = "keep-alive";
	resp.headers["Accept-Ranges"] = "bytes";
	resp.headers["Content-Type"] = "video/mp4";
	resp.headers["Content-Length"] = std::to_string(video_length);

	/*
	 * 如果没有Range, 返回全部文件字节并设置状态码为200,
	 * 如果有，添加Content-Range首部并按需返回对应范围的字节
	 */
	if (req.headers.find("Range") == req.headers.end())
	{
		std::ostringstream out;
		resp.http_version = req.http_version;
		resp.code = 200;
		resp.status = "OK";

		out << resp.http_version << " " << resp.code << " " << resp.status << "\r\n";
		for (const auto &c : resp.headers)
		{
			out << c.first << ": " << c.second << "\r\n";
		}
		out << "\r\n";
		write(client_sock, out.str().c_str(), out.str().size());

		int read_len = 0;
		while ((read_len = read(video_fd, buf, sizeof(buf))) != 0)
		{
			write(client_sock, buf, read_len);
		}
	}
	else
	{
		resp.http_version = req.http_version;
		resp.code = 206;
		resp.status = "Partial Content";

		// bytes有三种格式
		// bytes=x-x
		// bytes=x-
		// bytes=-x
		std::string range_value = req.headers["Range"].substr(6);
		int pos = range_value.find("-");
		std::string beg = range_value.substr(0, pos);
		std::string end = range_value.substr(pos + 1);
		int beg_num = 0, end_num = 0;
		if (beg != "" && end != "")
		{
			beg_num = stoi(beg);
			end_num = stoi(end);
		}
		else if (beg != "" && end == "")
		{
			beg_num = stoi(beg);
			if (beg_num + 65535 >= video_length)
				end_num = video_length - 1;
			else
				end_num = beg_num + 65535;
		}
		else if (beg == "" && end != "")
		{
			beg_num = video_length - stoi(end);
			if (beg_num + 65535 >= video_length)
				end_num = video_length - 1;
			else
				end_num = beg_num + 65535;
		}
		// 需要读need_len个字节
		unsigned need_len = end_num - beg_num + 1;

		std::ostringstream os_range;
		os_range << "bytes " << beg_num << "-" << end_num << "/" << video_length;
		resp.headers["Content-Range"] = os_range.str();
		resp.headers["Content-Length"] = std::to_string(need_len);

		std::ostringstream out;
		out << resp.http_version << " " << resp.code << " " << resp.status << "\r\n";
		for (const auto &c : resp.headers)
		{
			out << c.first << ": " << c.second << "\r\n";
		}
		out << "\r\n";
		std::cerr << "respon:[" << out.str() << "]" << std::endl;
		write(client_sock, out.str().c_str(), out.str().size());
		std::cerr << "lseek beg_num:" << beg_num << ", end_num:" << end_num << std::endl;
		if (lseek(video_fd, beg_num, SEEK_SET) == -1)
		{
			printf("lseek error\n");
		}

		int read_len = 0;
		while (need_len >= sizeof(buf))
		{
			read_len = read(video_fd, buf, sizeof(buf));
			// std::cerr << "read read_len:" << read_len << std::endl;
			write(client_sock, buf, read_len);
			need_len -= read_len;
		}
		read(video_fd, buf, need_len);
		write(client_sock, buf, need_len);
	}

	close(video_fd);
	fclose(fp_read);
	return;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <filepath>" << std::endl;
		exit(1);
	}

	int server_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (server_sock == -1)
	{
		error_handler("socket");
	}

	struct sockaddr_in server_addr, client_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		error_handler("bind");
	}

	if (listen(server_sock, 10) == -1)
	{
		error_handler("listen");
	}

	int constexpr epoll_size = 64;
	int epfd = epoll_create(epoll_size);
	struct epoll_event event;
	struct epoll_event *ep_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * epoll_size);
	event.events = EPOLLIN;
	event.data.fd = server_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, server_sock, &event);
	while (1)
	{
		int ep_cnt = epoll_wait(epfd, ep_events, epoll_size, 5000);
		if (ep_cnt == -1)
		{
			error_handler("epoll_wait");
		}
		if (ep_cnt == 0)
		{
			std::cout << "wait......" << std::endl;
			continue;
		}
		for (int i = 0; i < ep_cnt; ++i)
		{
			if (ep_events[i].data.fd == server_sock)
			{
				socklen_t client_addr_len = sizeof(client_addr);
				int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
				if (client_sock == -1)
				{
					perror("accept");
					continue;
				}
				event.events = EPOLLIN;
				event.data.fd = client_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &event);
			}
			else
			{
				do_client(ep_events[i].data.fd, argv[2]);
				epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
			}
		}
	}
	close(epfd);
	close(server_sock);
	return 0;
}
