# Centos7.3 安装bashdb和vim bash-support插件

## 一. 环境准备

### 1.1. 系统版本和内核信息

```
[root]# cat /etc/redhat-release
CentOS Linux release .1611 (Core)
[root]# uname -a
Linux localhost.localdomain -el786_64 #1 SMP Tue Nov 22 16:42:41 UTC 2016 x86_64 x86_64 x86_64 GNU/Linux
```

### 1.2. 安装基础软件

```
[root] groupinstall "Development Tools"
[root] install ncurses-devel  readline-devel texinfo
[root] install 
```

## 二. 文件准备

### 2.1. 查看bash版本

```
[root]# rpm -q bash
bash--el7_286_64
```

### 2.2. 下载bashdb-4.2-0.7

通过查看文档和自己的测试,bashdb-4.2-0.7 和centos7的bash-4.2.46版本匹配,并且编译顺利.

bashdb-4.2-0.7 下载地址 <http://bashdb.sourceforge.net/> 
备用下载地址 <http://download.csdn.net/detail/shile/9740693> 
解压bashdb 
`[root]#tar xf bashdb-4.2-0.7.tar.gz -C /usr/src`

## 三. 编译安装bashdb

### 3.1. 编译安装bashdb

有两种方法,区别可以看bashdb的帮助信息.

#### 第一种编译方法

不结合bash源代码编译和安装bashdb

```
[root]#cd /usr/src
[root]#cd bashdb-4.2-0.7
[root]#touch OME/missing
[root]#touch libtoolT
[root]#./configure --prefix=OME 
[root]#make
[root]#make install
```

#### 第二种编译方法

结合bash源代码编译和安装bashdb

需要准备bash源代码

a).下载bash-4.2.46-20的src.rpm(需要与bash版本号完全一样)

bash-4.2.46-20.el7_2.src.rpm <http://vault.centos.org/7.3.1611/os/Source/SPackages/>

b). 提取bash-4.2.46-20源代码

```
[root]#rpm -ivh bash-4.2.46-20.el7_2.src.rpm
[root]#cd /root/rpmbuild/SPECS
[root]#rpmbuild -bp bash.spec
```

此时，源代码已经安装到了/root/rpmbuild/BUILD目录

c).移动bash的源代码到/usr/src

```
[root]#cd /root/rpmbuild/BUILD
[root]#mv bash-4.2 bash-4.2.46-20
[root]#mv bash-4.2.46-20 /usr/src
```

d). 准备bash的源码环境

```
 [root]#cd /usr/src
 [root]#cd bash-4.2.46-20
 [root]#./configure  && make
```

e). 结合bash编译安装bashdb

```
[root] /usr/src
[root] bashdb--
[root]#touch OME/missing
[root]#touch libtoolT
[root]#./configure --prefix=OME --with-bash-src=/usr/src/bash--
[root]#make
[root]#make install
```

### 3.2. 配置bashdb

`[root]#vi ~/.bash_profile` 
在最后添加

```
MANPATH=$HOME/share/man:$MANPATH
export MANPATH
INFOPATH=$HOME/share/info
export INFOPATH
```

然后,生效新的设置.

```
[root]#. ~/.bash_profile
```

设置inc文件路径.[重要]

```
[root]#mkdir /usr/share/bashdb/
[root]#ln -s ~/share/bashdb/bashdb-main.inc /usr/share/bashdb/
```

### 3.3. 使用bashdb

```
[root]#bash --debugger yourscript   
```

一些常用的调试方法 
run 运行 
R 重启 
l m n 列出m到n行之间的代码 
b n 在第n行设断点 
L 列出所有断点 
d n 删除第n行的断点 
D 删除所有断点 
W var 观察var 
n 单步执行 
s 单步执行，进入函数 
print $var显示var，观察用，用得最多的命令 
T 观察栈 
skip 跳过要执行的命令 
ret 执行完到函数返回 
finish 执行到程序结束 
a n cmd 在第n行执行cmd命令行为 
A 取消所有添加的行为 
q 退出调试

## 四. 配置vim的bash-support插件

### 4.1. 下载bash-support 插件

从vim.org下载插件：

```
[root]/usr/src
[root]#wget -O bash-support.zip http://www.vim.org/scripts/download_script.php?
src_id=
```

或者 从github下载插件： <https://github.com/vim-scripts/bash-support.vim>

### 4.2. 安装bash-support Vim 插件

```
[root]#mkdir ~/.vim #  the directory does not exist already
[root]#cd ~/.vim
[root]#unzip /usr/src/bash-support.zip
```

### 4.3. 在~/.vimrc中启动这个插件

Add the following line to the ~/.vimrc to enable the plugin for Vim editor. 
把下列行添加到~/.vimrc文件来启动这个Vim编辑器的插件。

```
[root]#vim ~/.vimrc
filetype plugin 
 This is the minimal content of the file '$HOME/.vimrc'. Create one if there
    is none or use the files in $HOME/.vim/bash-support/rc as a starting point.
```

### 4.4. 使用vim编辑shell脚本

就可以使用bash-support的多种功能，详情可以查看自带帮助,好多快捷键. 
在vim里面运行 :help bash-support,可以在vim里面打开手册.

## 五. vim配合bashdb调试

远程连接centos7,vim里面按F9调试,是在打开的xterm里面打开bashdb调试,需要 
`[root]#yum install xterm xorg-x11-xauth` 
安装完毕后，断开ssh重新连接ssh生效。同时，需要开启x转发，以及xserver。



![这里写图片描述](https://img-blog.csdn.net/20170118170104562?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvc2hpbGU=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



如果只在shell直接运行bash –debugger或者bashdb命令行调试，可以不考虑上面的X相关的步骤。

在本地终端的vim里面按F9，仍然是在终端里面运行调试，也不用考虑上面的X相关的步骤。