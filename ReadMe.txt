关于friso中文分词组建：

欢迎报告你在使用该软件中遇到的各种问题到下面的电子邮件.

作者：陈鑫
电子邮件：chenxin619315@gmail.com
更多关于：http://code.google.com/p/friso
Java版本：http://code.google.com/p/jcseg
PHP版本：http://code.google.com/p/robbe


* How to Install

cd src
make
sudo make install

* How to Run

friso -lex {friso lex path}

#for example:
#the configuration file is in the $(HOME) dictionary name friso.ini
#try:
friso -init $(HOME)/friso.ini


* How to Uninstall

sudo rm /usr/local/bin/friso




一。friso中文分词器
friso是使用c语言开发的一个中文分词器，使用流行的mmseg算法实现。完全基于模块化设计和实现，可以很方便的植入到其他程序中，例如：MySQL，PHP等。

1。目前最高版本：friso 0.1，只支持UTF-8编码。【源码无需修改就能在各种平台下编译使用，加载完20万的词条，内存占用稳定为14M。】。

2。mmseg四种过滤算法，分词准确率达到了98.41%。

3。支持自定义词库。在dict文件夹下，可以随便添加/删除/更改词库和词库词条，并且对词库进行了分类。

4。词库使用了friso的Java版本jcseg的简化词库。

5。支持中英混合词的识别。例如：c语言，IC卡。

7。很好的英文支持，电子邮件，网址，小数，分数，百分数。

8。支持阿拉伯数字基本单字单位的识别，例如2012年，5吨，120斤。

9。自动英文圆角/半角，大写/小写转换。

附件目录结构说明
core - friso核心目录

└——dict ----- friso词库配置文件和所有词库文件。

└——src ----- friso c源程序。

└——friso.ini ----- friso配置文件【如何配置friso.ini】。

└——Install ----- 安装说明文件。

└——Run ----- 测试程序运行说明文件。

└——Uninstall ----- 卸载说明文件。

lib - friso二进制目录

└——win32 ----- win32的dll和lib文件。

└——linux ----- linux下的so和a文件。

ReadMe.txt ----- friso说明文件

二。分词速度
测试环境：2.8GHZ/2G/Ubuntu

简单模式：3.7M/秒

复杂模式：1.8M/秒

三。分词结果：
1.文本1：

歧义去除:研究生命起源，friso是使用c语言开发的高性能中文分词组件，混合词: 做B超检查身体，本质是X射线，单位和全角: 2009年８月６日开始大学之旅，英文数字: bug report chenxin619315@gmail.com or visit http://code.google.com/p/friso, 15% of the day's time i will be there.

friso分词结果：

歧义 去除 研究 生命 起源 friso 是 使用 c语言 开发 的 高性能 中文分词 组件 混合 词 做 b超 检查 身体 本质 是 x射线 单位 和 全角 2009年 8月 6日 开始 大学 之旅 英文 数字 bug report chenxin619315@gmail.com or visit http://code.google.com/p/friso 15% of the day's time i will be there

2.文本2：

叔叔亲了我妈妈也亲了我

friso分词结果：

叔叔 亲了 我 妈妈 也 亲了 我

四。使用方法
1.分词测试：

请按照附件中的Install说明(linux)安装friso：

运行如下命令来启动friso测试程序：

//run friso test program.

friso -init friso.ini文件地址

//例如我的friso.ini在/c/friso/文件夹中：
//请在friso.ini中正确的填写friso.lex_dir（词库配置文件friso.lex.ini的目录地址，必须以“/”结尾）

friso -init /c/friso/friso.ini
2.二次开发使用：

具体请参考tst-friso.c文件：

friso_t friso;
friso_task_t task;

//1.实例化一个friso分词实例。
//__path__为friso.ini的地址。
friso = friso_new_from_ifile(__path__);

//2.创建一个分词任务：
task = friso_new_task();

//3.设置分词任务的分词文本：
friso_set_text( task, "要被分词的文本");

//4.分词主程序：

while ( ( friso_next( friso, friso->mode, task ) ) != NULL ) {
    //printf("%s[%d,%d]/ ", task->hits->word, task->hits->type, task->hits->offset );
    printf("%s/ ", task->hits->word );
    if ( task->hits->type == __FRISO_NEW_WORDS__ ) {
        FRISO_FREE( task->hits->word );//释放组合词的内存。
    }
}


//5.释放friso和分词任务：
friso_free_task( task );
friso_free( friso );
五。friso动态：
friso的下一个版本：

1.加入中文数字识别以及其自动转换为阿拉伯数字的功能。

2.加入同义词功能。

3.加入中文人名识别。
