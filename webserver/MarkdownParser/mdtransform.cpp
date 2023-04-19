#include "mdtransform.h"

#include <algorithm>

pair<int, char *> MarkdownTransform::start(char *src) {
    //如果行空，则返回
    if ((int)strlen(src) == 0) {
        return make_pair(0, nullptr);
    }

    //行前有空格和Tab
    //统计空格数和Tab数
    int cntspace = 0;
    int cnttab = 0;
    //从该行的第一个字符读其, 统计空格键和 Tab 键,
    // 当遇到不是空格和 Tab 时，立即停止
    for (int i = 0; src[i] != '\0'; i++) {
        if (src[i] == ' ') {
            cntspace++;
        } else if (src[i] == '\t') {
            cnttab++;
        }
        // 如果内容前有空格和 Tab，那么将其统一按 Tab 的个数处理,
        // 其中, 一个 Tab = 四个空格
        return make_pair(cnttab + cnttab / 4, src + i);
    }
    //行前无空格和Tab
    return make_pair(0, nullptr);
}

bool FollowedIsEmpty(char *ptr)
{
    while(*ptr != '\0')
    {
        if(*ptr++ != ' ')
            return false;
    }
    return true;
}

pair<int, char *> MarkdownTransform::JudgeType(char *src) {
    if(src == nullptr)
        return make_pair(paragraph, src);
    char *ptr = src;
    //跳过'#'
    while (*ptr == '#') {
        ptr++;
    }

    //接着出现空格，则是'<h>'标签
    if (ptr - src > 0 && *ptr == ' ') {
        return make_pair(ptr - src + h1 - 1, ptr + 1);  //累加判断几级标题
    }

    //重置分析位置
    ptr = src;

    //出现 ```则是代码块
    if (strncmp(ptr, "```", 3) == 0 ){
        return make_pair(blockcode, ptr + 3);
    }

    //如果出现 (* +) -, 并且他们的下一个字符为空格，则说明是无序列表
    if (strncmp(ptr, "- ", 2) == 0) {
        return make_pair(ul, ptr + 1);
    }

    //如果出现 > 且下一个字符为空格，则说明是引用
    if (*ptr == '>' && (ptr[1] == ' ')) {
        return make_pair(quote, ptr + 1);
    }

    // 如果出现的是数字, 且下一个字符是 '.',下下个空格，则说明是是有序列表
    char *ptr1 = ptr;
    while (*ptr1 && (isdigit((unsigned char)*ptr1))) {
        ptr1++;
    }
    if (ptr1 != ptr && *ptr1 == '.' && ptr1[1] == ' ') {
        return make_pair(ol, ptr1 + 1);
    }
    //否则就是普通段落
    return make_pair(paragraph, ptr);
}

node * MarkdownTransform::findnode(int depth) {
    node *ptr = root;
    while (!ptr->ch.empty() && depth != 0) {
        ptr = ptr->ch.back();  //最后一个元素
        if (ptr->type == li) {
            depth--;
        }
    }
    return ptr;
}

void MarkdownTransform::Cins(Cnode *v, int x, const string &hd, int tag) {
    int n = (int)v->ch.size();
    if (x == 1) {
        v->ch.push_back(new Cnode(hd));
        v->ch.back()->tag = "tag" + to_string(tag);
        return;
    }
    if (!n || v->ch.back()->heading.empty()) {
        v->ch.push_back(new Cnode(""));
    }
    Cins(v->ch.back(), x - 1, hd, tag);
}

void MarkdownTransform::insert(node *v, const string &src) {
    int n = (int)src.size();
    bool incode = false, inem = false, instrong = false, inautolink = false;
    v->ch.push_back(new node(nul));

    for (int i = 0; i < n; i++) {
        char ch = src[i];
        if (ch == '\\') {
            ch = src[++i];
            v->ch.back()->elem[0] += string(1, ch);  //保存内容
            continue;
        }

        //处理行内代码 `code`
        if (ch == '`' && !inautolink) {
            incode ? v->ch.push_back(new node(nul))
                    : v->ch.push_back(new node(code));
            incode = !incode;
            continue;
        }

        //处理加粗 **加粗**
        if (ch == '*' && (i < n - 1 && (src[i + 1] == '*')) && !incode &&
            !inautolink) {
            ++i;
            instrong ? v->ch.push_back(new node(nul))
                        : v->ch.push_back(new node(strong));
            instrong = !instrong;
            continue;
        }

        //处理斜体 _斜体_
        if (ch == '_' && !incode && !inautolink && !instrong) {
            inem ? v->ch.push_back(new node(nul))
                    : v->ch.push_back(new node(em));
            inem = !inem;
            continue;
        }

        //处理图片 ![image](https://web/image.png)
        //![image](https://web/image.png "title")
        if (ch == '!' && (i < n - 1 && src[i + 1] == '[') && !incode &&
            !instrong && !inem && !inautolink) {
            //获取图片显示内容
            v->ch.push_back(new node(image));
            for (i += 2; i < n - 1 && src[i] != ']'; i++) {
                v->ch.back()->elem[0] += string(1, src[i]);
            }
            i++;
            //获取图片链接
            for (i++; i < n - 1 && src[i] != ' ' && src[i] != ')'; i++) {
                v->ch.back()->elem[1] += string(1, src[i]);
            }
            //获取title
            if (src[i] != ')') {
                for (i++; i < n - 1 && src[i] != ')'; i++) {
                    if (src[i] != '"') {
                        v->ch.back()->elem[2] += string(1, src[i]);
                    }
                }
            }
            v->ch.push_back(new node(nul));
            continue;
        }

        //处理超链接  [github](https://github.com/)
        //[github](https://github.com/ "title")
        if (ch == '[' && !incode && !instrong && !inem && !inautolink) {
            //获取链接显示内容
            v->ch.push_back(new node(href));
            for (i++; i < n - 1 && src[i] != ']'; i++) {
                v->ch.back()->elem[0] += string(1, src[i]);
            }
            i++;
            //获取链接
            for (i++; i < n - 1 && src[i] != ' ' && src[i] != ')'; i++) {
                v->ch.back()->elem[1] += string(1, src[i]);
            }
            //获取title
            if (src[i] != ')') {
                for (i++; i < n - 1 && src[i] != ')'; i++) {
                    if (src[i] != '"') {
                        v->ch.back()->elem[2] += string(1, src[i]);
                    }
                }
            }
            v->ch.push_back(new node(nul));
            continue;
        }

        //普通文本
        v->ch.back()->elem[0] += string(1, ch);
        if (!inautolink) {
            v->ch.back()->elem[1] += string(1, ch);
        }
    }

    //结尾两个空格，换行
    if (src.size() >= 2) {
        if (src.at(src.size() - 1) == ' ' &&
            src.at(src.size() - 2) == ' ') {
            v->ch.push_back(new node(br));
        }
    }
}

bool MarkdownTransform::isCutline(char *src) {
    int cnt = 0;
    char *ptr = src;
    while (*ptr) {
        // 如果不是 空格、tab、- 或 *，那么则不需要换行
        if (*ptr != ' ' && *ptr != '\t' && *ptr != '-') {
            return false;
        }
        if (*ptr == '-') {
            cnt++;
        }
        ptr++;
    }
    // 如果出现 --- 则需要增加一个分割线, 这时需要换行
    return (cnt >= 3);
}

void MarkdownTransform::mkpara(node *v) {
    if (v->ch.size() == 1u &&
        v->ch.back()->type == paragraph) {  // 1u 1 unsigned int
        return;
    }
    if (v->type == paragraph) {
        return;
    }
    if (v->type == nul) {
        v->type = paragraph;
        return;
    }
    node *x = new node(paragraph);
    x->ch = v->ch;
    v->ch.clear();
    v->ch.push_back(x);
}

void MarkdownTransform::dfs(node *v) {
    if (v->type == paragraph && v->elem[0].empty() && v->ch.empty()) {
        return;
    }

    contents += frontTag[v->type];
    bool flag = true;

    //处理标题，支持目录进行跳转
    if (isHeading(v)) {
        contents += "id=\"" + v->elem[0] + "\">\n";
        flag = false;
    }

    //处理超链接
    if (isHref(v)) {
        contents += "<a href=\"" + v->elem[1] + "\" title=\"" + v->elem[2] +
                    "\">" + v->elem[0] + "\n</a>\n";
        flag = false;
    }

    //处理图片
    if (isImage(v)) {
        contents += "<img alt=\"" + v->elem[0] + "\" src=\"" + v->elem[1] +
                    "\" title=\"" + v->elem[2] + "\" />\n";
        flag = false;
    }

    // 如果上面三者都不是, 则直接添加内容
    if (flag) {
        contents += v->elem[0];
        flag = false;
    }

    // 递归遍历所有
    for (int i = 0; i < (int)v->ch.size(); i++) {
        dfs(v->ch[i]);
    }

    // 拼接结束标签
    contents += backTag[v->type];
}

void MarkdownTransform::Cdfs(Cnode *v, string index) {
    TOC += "<li>\n";
    TOC += "<a href=\"#" + v->tag + "\">" + index + " " + v->heading +
            "</a>\n";
    int n = (int)v->ch.size();
    if (n) {
        TOC += "<ul>\n";
        for (int i = 0; i < n; i++) {
            Cdfs(v->ch[i], index + to_string(i + 1) + ".");
        }
        TOC += "</ul>\n";
    }
    TOC += "</li>\n";
}

/**
 * @brief 处理尖括号
 *
 * @param src
 * @return string
 */
string HandleAngleBrackets(char * src)
{
    char* pos = src;
    string str;
    while(*pos != '\0') {
        if(*pos == '<')
            str += "&lt;";
        else if(*pos == '>')
            str += "&gt;";
        else
            str += *pos;
        pos++;
    }
    return str;
}

MarkdownTransform::MarkdownTransform(const std::string &filename) {
    // 首先为文档的树结构进行初始化，并将当前指针 now 指向根节点
    Croot = new Cnode("");
    root = new node(nul);
    now = root;

    // //从文件流中读取文件
    std::ifstream fin(filename);

    //默认不是新段落，默认不在代码块内
    bool newpara = false;
    bool inblock = false;

    //读取到eof为止
    while (!fin.eof()) {
        //从文件读取一行
        fin.getline(s, maxLength);

        //处理不在代码块且需要换行
        if (!inblock && isCutline(s)) {
            now = root;
            now->ch.push_back(new node(hr));
            newpara = false;
            continue;
        }

        // std::pair 实质上是一个结构体, 可以将两个数据组合成一个数据
        // 计算一行中开始的空格和 Tab 数
        // 由空格数和有内容处的 char* 指针组成的
        auto ps = start(s);

        // 如果没有位于代码块中, 且没有统计到空格和 Tab, 则直接读取下一行
        if (!inblock && ps.second == nullptr) {
            now = root;
            newpara = true;
            continue;
        }

        // 分析该行文本的类型
        // 当前行的类型和除去行标志性关键字的正文内容的 char* 的pair
        auto tj = JudgeType(ps.second);
        // 如果是代码块类型
        if (tj.first == blockcode) {
            // 如果已经位于代码块中, 则 push 一个空类型的节点，否则push
            // 新代码块节点
            inblock ? now->ch.push_back(new node(nul))
                    : now->ch.push_back(new node(blockcode));
            inblock = !inblock;
            continue;
        }
        //如果已经在代码块中，内容拼接到当前节点
        if (inblock) {
            now->ch.back()->elem[0] += HandleAngleBrackets(s) + '\n';
            continue;
        }

        //普通段落？
        if (tj.first == paragraph) {
            if (now == root) {  //?
                now = findnode(ps.first);
                now->ch.push_back(new node(paragraph));
                now = now->ch.back();
            }
            bool flag = false;
            if (newpara && !now->ch.empty()) {
                node *ptr = nullptr;
                for (auto i : now->ch) {
                    if (i->type == nul) {
                        ptr = i;
                    }
                }
                if (ptr != nullptr) {
                    mkpara(ptr);
                }
                flag = true;
            }
            if (flag) {
                now->ch.push_back(new node(paragraph));
                now = now->ch.back();
            }
            now->ch.push_back(new node(nul));
            insert(now->ch.back(), string(tj.second));
            newpara = false;
            continue;
        }

        now = findnode(ps.first);

        //标题行，标签中插入属性tag
        if (tj.first >= h1 && tj.first <= h6) {
            now->ch.push_back(new node(tj.first));
            now->ch.back()->elem[0] = "tag" + to_string(++cntTag);
            insert(now->ch.back(), string(tj.second));
            Cins(Croot, tj.first - h1 + 1, string(tj.second), cntTag);
        }

        //如果是无序列表
        if (tj.first == ul) {
            if (now->ch.empty() || now->ch.back()->type != ul) {
                now->ch.push_back(new node(ul));
            }
            now = now->ch.back();
            bool flag = false;
            if (newpara && !now->ch.empty()) {
                node *ptr = nullptr;
                for (auto i : now->ch) {
                    if (i->type == li) {
                        ptr = i;
                    }
                }
                if (ptr != nullptr) {
                    mkpara(ptr);
                }
                flag = true;
            }
            now->ch.push_back(new node(li));
            now = now->ch.back();
            if (flag) {
                now->ch.push_back(new node(paragraph));
                now = now->ch.back();
            }
            insert(now, string(tj.second));
        }

        //如果是有序列表
        if (tj.first == ol) {
            if (now->ch.empty() || now->ch.back()->type != ol) {
                now->ch.push_back(new node(ol));
            }
            now = now->ch.back();
            bool flag = false;
            if (newpara && !now->ch.empty()) {
                node *ptr = nullptr;
                for (auto i : now->ch) {
                    if (i->type == li) {
                        ptr = i;
                    }
                }
                if (ptr != nullptr) {
                    mkpara(ptr);
                }
                flag = true;
            }
            now->ch.push_back(new node(li));
            now = now->ch.back();
            if (flag) {
                now->ch.push_back(new node(paragraph));
                now = now->ch.back();
            }
            insert(now, string(tj.second));
        }

        //如果是引用
        if (tj.first == quote) {
            if (now->ch.empty() || now->ch.back()->type != quote) {
                now->ch.push_back(new node(quote));
            }
            now = now->ch.back();
            if (newpara || now->ch.empty()) {
                now->ch.push_back(new node(paragraph));
            }
            insert(now->ch.back(), string(tj.second));
        }

        newpara = false;
    }

    //文件读取分析完毕
    fin.close();

    //深度遍历语法树
    dfs(root);

    //构造目录
    TOC += "<ul>";
    for (int i = 0; i < (int)Croot->ch.size(); i++) {
        Cdfs(Croot->ch[i], to_string(i + 1) + ".");
    }
    TOC += "</ul>";
}

//析构函数
MarkdownTransform::~MarkdownTransform() {
    destroy<node>(root);
    destroy<Cnode>(Croot);
}
