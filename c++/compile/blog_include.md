# 关于C语言include尖括号和双引号的对话

### [source]https://my.oschina.net/2bit/blog/389556 

A: #include "..." 和 #include <...> 有什么区别?</br>

B: 他喵的, 这么简单的问题. 尖括号是先在系统目录下找, 双引号是首先在当前目录下找, 找不到就找系统目录.</br>

A: 说得很含糊. 那么我问你, 当前目录有个叫hdr.h的文件, </br>
    当前目录下还有一个子目录sub, sub里面有两个文件hdr.h和1.c,</br>
    1.c中#include "hdr.h",</br>
    那么在当前目录编译sub/1.c, 它应该包含哪一个hdr.h?</br>
```shell
   bash-3.2$ tree
   |-- sub
   |   |-- 1.c
   |   `-- hdr.h
   `-- hdr.h
   bash-3.2$ cat sub/1.c
   #include "hdr.h"
   bash-3.2$ gcc -c sub/1.c
```

B: 应该包含外面的hdr.h吧, 因为它在gcc工作的当前目录, 而子目录的hdr.h不在当前目录, 应该搜不到.</br>

A: 错, 包含的是sub/hdr.h !</br>

B: 啊!... 喔... 双引号是首先在使用include的源文件所在目录找(也就是1.c所在的目录sub找), 而不是编译器当前工作目录找, 我原来理解错了.</br>

A: 是的, gcc和msvc的include双引号都是先在使用include的源文件所在目录找.</br>

B: C99标准是怎么规定的? 是源文件当前目录还是编译器工作目录?</br>

A: 其实C99标准没有规定#include "..." 先找哪个目录, 只说取决于实现, 你可以设计一个C编译器include双引号时按你设计的方式找(如果找不到退化为#include <...>). 但事实上的工业标准是"双引号优先从使用include的源文件所在目录".</br>

B: 明白了.</br>

A: 那么我再问你, #include <...> 所谓的"从系统目录找", 那么"系统目录"到底有哪些, 怎么看呢?</br>

B: 编译时-I指定的目录呗.</br>

A: 如果没指定-I的参数时, 是不是系统目录列表就是空的?</br>

B: 当然不是, 有默认的吧.</br>

A: 怎么看默认的?</br>

B: 这个不知道. 怎么看?</br>

A: 以gcc为例, 在命令行运行 echo | gcc -E -v -</br>

```shell
   bash-3.2$ echo | gcc -E -v -
   * * *
   #include "..." search starts here:
   #include <...> search starts here:
   /usr/lib/gcc/i686-pc-linux-gnu/4.8.2/include
   /usr/local/include
   /usr/lib/gcc/i686-pc-linux-gnu/4.8.2/include-fixed
   /usr/include
   End of search list.
```

B: 那么我指定gcc -I. 时是把 . 作为搜索路径还是把当前目录转成绝对路径加入到列表中呢?</br>

A: 这个问题问得好! 其实我也不知道, 要不我们试一下.</br>

```shell
   bash-3.2$ echo | gcc -E -v -I. -
   * * *
   #include "..." search starts here:
   #include <...> search starts here:
   .
   /usr/lib/gcc/i686-pc-linux-gnu/4.8.2/include
   /usr/local/include
   /usr/lib/gcc/i686-pc-linux-gnu/4.8.2/include-fixed
   /usr/include
   End of search list.
```

B: 并没有转成绝对路径, 就是一个点.</br>

A: 恩, 那么问题来了, 下面这种情形下( 注意: 1.c的内容修改为 #include <hdr.h> ), 会包含那个hdr.h ?</br>

```shell
   bash-3.2$ tree
   |-- sub
   |   |-- 1.c
   |   `-- hdr.h
   `-- hdr.h
   bash-3.2$ cat sub/1.c
   #include <hdr.h>
   bash-3.2$ gcc -c -I. sub/1.c
```

B: 我猜会用子目录的hdr.h.</br>

A: 我猜会用外层的hdr.h,</br>
    在hdr.h写个1,</br>
    在sub/hdr.h写个2,</br>
    然后 gcc -E -I. sub/1.c.</br>
    你看结果是1, 我猜对了!</br>

B: 嗯, 看来搜索路径列表是基于编译器工作路径的.</br>
