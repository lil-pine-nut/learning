#include <string>
using namespace std;

void GetckPlayerHtml(const string &host_file, string &out_html)
{
    out_html =
        // ckplayer-x2
        // "<!DOCTYPE html>\n"
        // "<html>\n"
        // "\n"
        // "	<head>\n"
        // "<link rel=\"icon\" href=\"data:;base64,=\">  <!-- 不希望产生 /favicon.ico 的请求 -->\n"
        // "		<meta charset=\"UTF-8\">\n"
        // "		<title>ckplayer</title>\n"
        // "\n"
        // "		<style type=\"text/css\">\n"
        // "			body {\n"
        // "				margin: 0;\n"
        // "				padding: 0px;\n"
        // "				font-family: \"Microsoft YaHei\", YaHei, \"微软雅黑\", SimHei, \"黑体\";\n"
        // "				font-size: 18px;\n"
        // "			}\n"
        // "			p{\n"
        // "				padding-left: 2em;\n"
        // "			}\n"
        // "		</style>\n"
        // "\n"
        // "	</head>\n"
        // "\n"
        // "	<body>\n"
        // "		<center style=\"padding:60px;\">\n"
        // "		<div id=\"video\" style=\"width: 100%; height: 400px;max-width: 600px; \">\n"
        // "		</div>\n"
        // "		</center>\n"
        // "		<script type=\"text/javascript\" src=\"ckplayer-x2/ckplayer/ckplayer.js\" charset=\"UTF-8\"></script>\n"
        // "		<script type=\"text/javascript\">\n"
        // "			var videoObject = {\n"
        // "				container: '#video', //容器的ID或className\n"
        // "				variable: 'player',//播放函数名称\n"
        // "				autoplay:false,\n"
        // "				video: '"+host_file+"'\n"
        // "			};\n"
        // "			var player = new ckplayer(videoObject);\n"
        // "		</script>\n"
        // "	</body>\n"
        // "\n"
        // "</html>\n";

        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<link rel=\"icon\" href=\"data:;base64,=\">  <!-- 不希望产生 /favicon.ico 的请求 -->\n"
        "  <link href=\"https://vjs.zencdn.net/7.18.1/video-js.css\" rel=\"stylesheet\" />\n"
        "\n"
        "  <!-- If you'd like to support IE8 (for Video.js versions prior to v7) -->\n"
        "  <!-- <script src=\"https://vjs.zencdn.net/ie8/1.1.2/videojs-ie8.min.js\"></script> -->\n"
        "</head>\n"
        "\n"
        "<body>\n"
        "  <video\n"
        "    id=\"my-video\"\n"
        "    class=\"video-js\"\n"
        "    controls\n"
        "    preload=\"auto\"\n"
        "    width=\"640\"\n"
        "    height=\"480\"\n"
        "    poster=\"MY_VIDEO_POSTER.jpg\"\n"
        "    data-setup=\"{}\" >\n"
        "    <source src=\"" +
        host_file + "\" type=\"video/mp4\" />\n"
                    "    <p class=\"vjs-no-js\">\n"
                    "      To view this video please enable JavaScript, and consider upgrading to a\n"
                    "      web browser that\n"
                    "      <a href=\"https://videojs.com/html5-video-support/\" target=\"_blank\"\n"
                    "        >supports HTML5 video</a\n"
                    "      >\n"
                    "    </p>\n"
                    "  </video>\n"
                    "  <script src=\"https://vjs.zencdn.net/7.18.1/video.min.js\"></script>\n"
                    "</body>\n"
                    "</html>\n";
}