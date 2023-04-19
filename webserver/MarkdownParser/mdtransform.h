/************************************
 * @file    : mdtransform.h
 * @brief   :
 * @details :
 * @author  : Alliswell
 * @date    : 2020-1-28
 ************************************/
#pragma once
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

#define maxLength 10000

// * 0: null | 开始
// * 1 : <p> | 段落
// * 2 : <a href = " ">...< / a> | 超链接
// * 3 : <ul> | 无序列表
// * 4 : <ol> | 有序列表
// * 5 : <li> | 列表 使用 <ul> 和 <ol> 进行包裹，最后将整个内容使用 <li>
// 进行包装。
// * 6 : <em> | 斜体
// * 7 : <strong> | 加粗
// * 8 : <hr / > | 水平分割线
// * 9 : <br / > | 换行
// * 10 : <img alt = "" src = "" / > | 图片
// * 11 : <blockquote> | 引用
// * 12 : <h1> | h1
// * 13 : <h2> | h2
// * 14 : <h3> | h3
// * 15 : <h4> | h4
// * 16 : <h5> | h5
// * 17 : <h6> | h6
// * 18 : <pre><code> | 代码段
// * 19 : <code> | 行内代码
//词法关键词枚举
enum Token {
    nul = 0,
    paragraph = 1,
    href = 2,
    ul = 3,
    ol = 4,
    li = 5,
    em = 6,
    strong = 7,
    hr = 8,
    br = 9,
    image = 10,
    quote = 11,
    h1 = 12,
    h2 = 13,
    h3 = 14,
    h4 = 15,
    h5 = 16,
    h6 = 17,
    blockcode = 18,
    code = 19,
};

// HTML前置标签
const std::string frontTag[] = {
    "",
    "<p>",
    "",
    "<ul>",
    "<ol>",
    "<li>",
    "<em>",
    "<strong>",
    "<hr color=#CCCCCC size=1 />",
    "<br />",
    "",
    "<blockquote>",
    "<h1 ",
    "<h2 ",
    "<h3 ",
    "<h4 ",
    "<h5 ",
    "<h6 ",  // 右边的尖括号预留给添加其他的标签属性, 如 id
    "<pre><code>",
    "<code>"};
// HTML 后置标签
const std::string backTag[] = {
    "",          "</p>",  "",      "</ul>", "</ol>",         "</li>",  "</em>",
    "</strong>", "",      "",      "",      "</blockquote>", "</h1>",  "</h2>",
    "</h3>",     "</h4>", "</h5>", "</h6>", "</code></pre>", "</code>"};

//保存目录结构
struct Cnode {
    vector<Cnode *> ch;
    string heading;
    string tag;

    Cnode(const string &hd) : heading(hd) {}
};

//保存正文内容
struct node {
    //节点类型
    int type;

    vector<node *> ch;

    //存三个重要属性，elem[0]保存显示内容、elem[1]保存链接、elem[2]保存title
    string elem[3];

    node(int _type) : type(_type) {}
};

class MarkdownTransform {
   private:
    //正文内容
    string contents;
    //目录
    string TOC;

    node *root, *now;
    Cnode *Croot;

    //让目录能够正确的索引到 HTML 的内容位置， cntTag 的进行记录
    int cntTag = 0;
    char s[maxLength];  //读取一行内容buf

   public:
    //************************************
    //类型获取
    //************************************
    // 判断是否为标题
    bool isHeading(node *v) { return (v->type >= h1 && v->type <= h6); }
    // 判断是否为图片
    bool isImage(node *v) { return (v->type == image); }
    //判断为超链接
    bool isHref(node *v) { return (v->type == href); }

    //************************************
    // Method:    start
    // FullName:  MarkdownTransform::start
    // Access:    public
    // Returns:   std::pair<int, char*> 由空格数和有内容处的 char* 指针组成的
    // std::pair Qualifier: Parameter: char * src 源串 details :
    // 解析一行中开始处的空格和Tab
    //************************************
    pair<int, char *> start(char *src);

    //************************************
    // Method:    JudgeType
    // FullName:  MarkdownTransform::JudgeType
    // Access:    public
    // Returns:   std::pair<int, char*>
    // 当前行的类型和除去行标志性关键字的正是内容的 char* 指针组成的 std::pair
    // Qualifier:
    // Parameter: char * src 源串
    // details :  判断当前行类型，MD语法关键字位于行首
    //************************************
    pair<int, char *> JudgeType(char *src);

    //************************************
    // 树操作
    //************************************
    //************************************//************************************//************************************
    //************************************
    // Method:    findnode
    // FullName:  MarkdownTransform::findnode
    // Access:    public
    // Returns:   node* 找到的节点指针
    // Qualifier:
    // Parameter: int depth 深度
    // details :  给定树的深度寻找节点
    //************************************
    node *findnode(int depth);

    //************************************//************************************//************************************

    //************************************
    // Method:    Cins
    // FullName:  MarkdownTransform::Cins
    // Access:    public
    // Returns:   void
    // Qualifier:
    // Parameter: Cnode * v
    // Parameter: int x 目录级别序号
    // Parameter: const string & hd
    // Parameter: int tag  tag序号 标签来标记这个目录所指向的内容
    // details :  递归Cnode节点插入
    //************************************
    void Cins(Cnode *v, int x, const string &hd, int tag);

    //************************************
    // Method:    insert
    // FullName:  MarkdownTransform::insert
    // Access:    public
    // Returns:   void
    // Qualifier:
    // Parameter: node * v 指定节点
    // Parameter: const string & src
    // details :  向指定的node节点中插入要处理的串
    //************************************
    void insert(node *v, const string &src);

    //************************************
    // Method:    isCutline
    // FullName:  MarkdownTransform::isCutline
    // Access:    public
    // Returns:   bool
    // Qualifier:
    // Parameter: char * src
    // details :  判断是否换行,Markdown 支持使用 ---进行人为分割
    //************************************
    bool isCutline(char *src);

    //************************************
    // Method:    mkpara
    // FullName:  MarkdownTransform::mkpara
    // Access:    public
    // Returns:   void
    // Qualifier:
    // Parameter: node * v
    // details :  拿到一个段落文本
    //************************************
    void mkpara(node *v);

    //遍历与生成
    //语法树的遍历是需要深度优先的，而对目录的深度遍历和正文内容的深度遍历逻辑并不一样

    //************************************
    // Method:    dfs
    // FullName:  MarkdownTransform::dfs
    // Access:    public
    // Returns:   void
    // Qualifier:
    // Parameter: node * v
    // details :  深度优先遍历正文
    //************************************
    void dfs(node *v);

    //目录的遍历和正文内容的遍历差别在于，目录的展示方式是需要使用无序列表的形式展示在
    // HTML 中
    //************************************
    // Method:    Cdfs
    // FullName:  MarkdownTransform::Cdfs
    // Access:    public
    // Returns:   void
    // Qualifier:
    // Parameter: Cnode * v
    // Parameter: string index
    // details :  遍历目录
    //************************************
    void Cdfs(Cnode *v, string index);

    //************************************
    // Method:    MarkdownTransform
    // FullName:  MarkdownTransform::MarkdownTransform
    // Access:    public
    // Returns:
    // Qualifier:
    // Parameter: const std::string & filename
    // details :  构造函数对文档树结构处理，生成内容与目录
    //************************************
    MarkdownTransform(const std::string &filename);

    //递归销毁节点
    template <typename T>
    void destroy(T *v) {
        for (int i = 0; i < (int)v->ch.size(); i++) {
            destroy(v->ch[i]);
        }
        delete v;
    }

    // 获得 Markdown 目录
    std::string getTableOfContents() { return TOC; }

    // 获得 Markdown 内容
    std::string getContents() { return contents; }

    //析构函数
    ~MarkdownTransform();

};
