读者云，太能罗唆了，一个 Hello World 就折腾了两个大节。OK，从本节开始，我们不再折腾 Hello World 了，我们来折腾 Hello World 的共享库。

本节的任务：

1. 建立一个静态库和动态库，提供 `HelloFunc 函数`供其他程序编程使用，HelloFunc 向终端输出 Hello World 字符串。
2. 安装头文件与共享库。

###### 1. 准备工作

在 `/backup/cmake` 目录建立 `t3 目录`，用于存放本节涉及到的工程。

###### 2. 建立共享库

```bash
cd /backup/cmake/t3
mkdir lib
```

在 t3 目录下建立 `CMakeLists.txt`，内容如下：

```
PROJECT(HELLOLIB)
ADD_SUBDIRECTORY(lib)
```

在 lib 目录下建立两个源文件 `hello.c` 与 `hello.h`

`hello.c` 内容如下：

```c
#include "hello.h"

void HelloFunc() 
{
	printf("Hello World\n");
}
```

`hello.h` 内容如下：

```c
#ifndef HELLO_H
#define HELLO_H
#include <stdio.h>
void HelloFunc();
#endif
```

在 lib 目录下建立 CMakeLists.txt，内容如下：

```
SET(LIBHELLO_SRC hello.c)
ADD_LIBRARY(hello SHARED ${LIBHELLO_SRC})
```

###### 3.编译共享库

仍然采用 out-of-source 编译的方式，按照习惯，我们建立一个 build 目录，在 build 目录中：

```bash
cmake ..
make
```

这时，你就可以在 lib 目录得到一个 `libhello.so`，这就是我们期望的共享库。

?> @Gavin注：在Windows的Cygwin环境下，该共享库为 `cyghello.dll`。

如果你要指定 `libhello.so` 生成的位置，可以通过在主工程文件 CMakeLists.txt 中修改 `ADD_SUBDIRECTORY(lib)` 指令来指定一个编译输出位置或者在 `lib/CMakeLists.txt` 中添加 `SET(LIBRARY_OUTPUT_PATH <路径>)` 来指定一个新的位置。

这两者的区别我们上一节已经提到了，所以，这里不再赘述，下面，我们解释一下一个新的指令 `ADD_LIBRARY` ：

```
ADD_LIBRARY(libname [SHARED|STATIC|MODULE]
 [EXCLUDE_FROM_ALL]
 source1 source2 ... sourceN
)
```

你不需要写全 `libhello.so`，只需要填写 `hello` 即可，cmake 系统会自动为你生成 `libhello.X`。

类型有三种：

1. SHARED，动态库
2. STATIC，静态库
3. MODULE，在使用 dyld 的系统有效，如果不支持 dyld，则被当作 SHARED 对待。

`EXCLUDE_FROM_ALL` 参数的意思是这个库不会被默认构建，除非有其他的组件依赖或者手工构建。

###### 4. 添加静态库

同样使用上面的指令，我们在支持动态库的基础上再为工程添加一个静态库，按照一般的习惯，静态库名字跟动态库名字应该是一致的，只不过后缀是.a 罢了。

下面我们用这个指令再来添加静态库：

`ADD_LIBRARY(hello STATIC ${LIBHELLO_SRC})`

然后再在 build 目录进行外部编译，我们会发现，静态库根本没有被构建，仍然只生成了一个动态库。因为 hello 作为一个 target 是不能重名的，所以，静态库构建指令无效。

如果我们把上面的 `hello` 修改为 `hello_static`：

`ADD_LIBRARY(hello_static STATIC ${LIBHELLO_SRC})`

就可以构建一个 `libhello_static.a` 的静态库了。

这种结果显示不是我们想要的，我们需要的是名字相同的静态库和动态库，因为 target 名称是唯一的，所以，我们肯定不能通过 ADD_LIBRARY 指令来实现了。这时候我们需要用到另外一个指令：

`SET_TARGET_PROPERTIES`，其基本语法是：

```
SET_TARGET_PROPERTIES(target1 target2 ...
 PROPERTIES prop1 value1
 prop2 value2 ...
)
```

这条指令可以用来设置输出的名称，对于动态库，还可以用来指定动态库版本和 API 版本。

在本例中，我们需要作的是向 `lib/CMakeLists.txt` 中添加一条：

`SET_TARGET_PROPERTIES(hello_static PROPERTIES OUTPUT_NAME "hello")`

这样，我们就可以同时得到 `libhello.so/libhello.a` 两个库了。

与它对应的指令是：

`GET_TARGET_PROPERTY(VAR target property)`

具体用法如下例，我们向 `lib/CMakeListst.txt` 中添加：

```
GET_TARGET_PROPERTY(OUTPUT_VALUE hello_static OUTPUT_NAME)
MESSAGE(STATUS “This is the hello_static OUTPUT_NAME:” ${OUTPUT_VALUE})
```

如果没有这个属性定义，则返回 NOTFOUND。

让我们来检查一下最终的构建结果，我们发现，`libhello.a` 已经构建完成，位于 `build/lib` 目录中，但是 `libhello.so` 却消失了。这个问题的原因是：cmake 在构建一个新的 target 时，会尝试清理掉其他使用这个名字的库，因此，在构建 `libhello.a` 时，就会清理掉 `libhello.so`。

为了回避这个问题，比如再次使用 `SET_TARGET_PROPERTIES` 定义 `CLEAN_DIRECT_OUTPUT` 属性。

向 `lib/CMakeLists.txt` 中添加：

```
SET_TARGET_PROPERTIES(hello PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(hello_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
```

这时候，我们再次进行构建，会发现 `build/lib` 目录中同时生成了 `libhello.so` 和 `libhello.a`。

###### 5. 动态库版本号

按照规则，动态库是应该包含一个版本号的，我们可以看一下系统的动态库，一般情况是：

libhello.so.1.2

libhello.so ->libhello.so.1

libhello.so.1->libhello.so.1.2

为了实现动态库版本号，我们仍然需要使用 SET_TARGET_PROPERTIES 指令。

具体使用方法如下：

`SET_TARGET_PROPERTIES(hello PROPERTIES VERSION 1.2 SOVERSION 1)`

VERSION 指代动态库版本，SOVERSION 指代 API 版本。

将上述指令加入 `lib/CMakeLists.txt` 中，重新构建看看结果。

在 build/lib 目录会生成：

libhello.so.1.2

libhello.so.1->libhello.so.1.2

libhello.so ->libhello.so.1

###### 6. 安装共享库和头文件

以上面的例子，我们需要将 `libhello.a`, `libhello.so.x` 以及 `hello.h` 安装到系统目录，才能真正让其他人开发使用，在本例中我们将 hello 的共享库安装到 `/lib` 目录，将 `hello.h` 安装到 `/include/hello` 目录。

利用上一节了解到的 `INSTALL 指令`，我们向 `lib/CMakeLists.txt` 中添加如下指令：

```
INSTALL(TARGETS hello hello_static
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
INSTALL(FILES hello.h DESTINATION include/hello)
```

注意，静态库要使用 ARCHIVE 关键字。

通过：

```bash
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
make install
```

我们就可以将头文件和共享库安装到系统目录 `/usr/lib` 和 `/usr/include/hello` 中了。

?> @Gavin注：在Mac中，请安装到 `/usr/local/lib目录下`。

###### 7. 小结

本小节，我们谈到了：

- 如何通过 `ADD_LIBRARY` 指令构建动态库和静态库。
- 如何通过 `SET_TARGET_PROPERTIES` 同时构建同名的动态库和静态库。
- 如何通过 `SET_TARGET_PROPERTIES` 控制动态库版本。
- 最终使用上一节谈到的 `INSTALL 指令`来安装头文件和动态、静态库。

在下一节，我们需要编写另一个高级一点的 Hello World 来演示怎么使用我们已经构建的构建的共享库 libhello 和外部头文件。