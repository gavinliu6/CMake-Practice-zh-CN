抱歉，本节仍然继续折腾 Hello World。

上一节我们已经完成了 libhello 动态库的构建以及安装，本节我们的任务很简单：

- 编写一个程序使用我们上一节构建的共享库。

###### 1. 准备工作

请在 `/backup/cmake` 目录建立 `t4 目录`，本节所有资源将存储在 t4 目录。

###### 2. 重复以前的步骤，建立src目录，编写源文件 main.c，内容如下：

```c
#include <hello.h>

int main(void)
{
	HelloFunc();
	
	return 0;
}
```

编写工程主文件 `CMakeLists.txt`：

```
PROJECT(HELLO)
ADD_SUBDIRECTORY(src)
```

编写 `src/CMakeLists.txt`：

```
ADD_EXECUTABLE(main main.c)
```

###### 3. 外部构建

按照习惯，仍然建立 `build 目录`，使用 `cmake ..` 方式构建。

过程：

```bash
cmake ..
make
```

构建失败，如果需要查看细节，可以使用第一节提到的方法：`make VERBOSE=1` 来构建。

错误输出为是：

```
/backup/cmake/t4/src/main.c:1:19: error: hello.h: 没有那个文件或目录
```

###### 4. 引入头文件搜索路径

`hello.h` 位于 `/usr/include/hello` 目录中，并没有位于系统标准的头文件路径，(有人会说了，白痴啊，你就不会 include ，同志，要这么干，我这 一节就没什么可写了，只能选择一个 glib 或者 libX11 来写了，这些代码写出来很多同志是看不懂的)。

为了让我们的工程能够找到 hello.h 头文件，我们需要引入一个新的指令

INCLUDE_DIRECTORIES，其完整语法为：

`INCLUDE_DIRECTORIES([AFTER|BEFORE] [SYSTEM] dir1 dir2 ...)`

这条指令可以用来向工程添加多个特定的头文件搜索路径，路径之间用空格分割，如果路径中包含了空格，可以使用双引号将它括起来，默认的行为是追加到当前的头文件搜索路径的后面，你可以通过两种方式来进行控制搜索路径添加的方式：

1. `CMAKE_INCLUDE_DIRECTORIES_BEFORE`，通过 SET 这个 cmake 变量为 on，可以将添加的头文件搜索路径放在已有路径的前面。
2. 通过 `AFTER` 或者 `BEFORE` 参数，也可以控制是追加还是置前。

现在我们在 `src/CMakeLists.txt` 中添加一个头文件搜索路径，方式很简单，加入：

```
INCLUDE_DIRECTORIES(/usr/include/hello)
```

进入 build 目录，重新进行构建，这时找不到 hello.h 的错误已经消失，但是出现了一个新的错误：

```
main.c:(.text+0x12): undefined reference to `HelloFunc'
```

因为我们并没有 link 到共享库 libhello 上。

###### 5. 为target添加共享库

我们现在需要完成的任务是将目标文件链接到 libhello，这里我们需要引入两个新的指令：`LINK_DIRECTORIES` 和 `TARGET_LINK_LIBRARIES`。

`LINK_DIRECTORIES` 的全部语法是：

`LINK_DIRECTORIES(directory1 directory2 ...)`

这个指令非常简单，添加非标准的共享库搜索路径，比如，在工程内部同时存在共享库和可执行二进制，在编译时就需要指定一下这些共享库的路径。这个例子中我们没有用到这个指令。

`TARGET_LINK_LIBRARIES` 的全部语法是：

```
TARGET_LINK_LIBRARIES(target library1
 <debug | optimized> library2
 ...)
```

这个指令可以用来为 target 添加需要链接的共享库，本例中是一个可执行文件，但是同样可以用于为自己编写的共享库添加共享库链接。

为了解决前面我们遇到的 HelloFunc 未定义错误，我们需要做的是向 `src/CMakeLists.txt` 中添加如下指令：

`TARGET_LINK_LIBRARIES(main hello)`

也可以写成：

`TARGET_LINK_LIBRARIES(main libhello.so)`

这里的 hello 指的是我们上一节构建的共享库 libhello。

进入 build 目录重新进行构建：

```bash
cmake ..
make
```

这时我们就得到了一个链接到 libhello 的可执行程序 main，位于 build/src 目录，运行 main 的结果是输出：

Hello World

让我们来检查一下 main 的链接情况：

```bash
ldd src/main
        linux-gate.so.1 =>  (0xb7ee7000)
        libhello.so.1 => /usr/lib/libhello.so.1 (0xb7ece000)
        libc.so.6 => /lib/libc.so.6 (0xb7d77000)
        /lib/ld-linux.so.2 (0xb7ee8000)
```

可以清楚的看到 main 确实链接了共享库 libhello，而且链接的是动态库 libhello.so.1。

那如何链接到静态库呢？

方法很简单：

将 `TARGET_LINK_LIBRRARIES` 指令修改为：

`TARGET_LINK_LIBRARIES(main libhello.a)`

重新构建后再来看一下 main 的链接情况：

```bash
ldd src/main
        linux-gate.so.1 =>  (0xb7fa8000)
        libc.so.6 => /lib/libc.so.6 (0xb7e3a000)
        /lib/ld-linux.so.2 (0xb7fa9000)
```

说明，main 确实链接到了静态库 libhello.a。

###### 6. 特殊的环境变量CMAKE_INCLUDE_PATH和CMAKE_LIBRARY_PATH

务必注意，这两个是环境变量而不是 cmake 变量。

使用方法是要在 bash 中用 export 或者在 csh 中使用 set 命令设置或者 `CMAKE_INCLUDE_PATH=/home/include cmake ..`等方式。

这两个变量主要是用来解决以前 autotools 工程中 `--extra-include-dir` 等参数的支持的。

也就是，如果头文件没有存放在常规路径(/usr/include, /usr/local/include 等)，则可以通过这些变量就行弥补。

我们以本例中的 `hello.h` 为例，它存放在 `/usr/include/hello` 目录，所以直接查找肯定是找不到的。

前面我们直接使用了绝对路径 `INCLUDE_DIRECTORIES(/usr/include/hello)` 告诉工程这个头文件目录。

为了将程序更智能一点，我们可以使用 `CMAKE_INCLUDE_PATH` 来进行，使用 bash 的方法如下：

```bash
export CMAKE_INCLUDE_PATH=/usr/include/hello
```

然后在头文件中将 `INCLUDE_DIRECTORIES(/usr/include/hello)` 替换为：

```c
FIND_PATH(myHeader hello.h)
IF(myHeader)
INCLUDE_DIRECTORY(${myHeader})
ENDIF(myHeader)
```

上述的一些指令我们在后面会介绍。

 这里简单说明一下，FIND_PATH 用来在指定路径中搜索文件名，比如: 

```
FIND_PATH(myHeader NAMES hello.h PATHS /usr/include /usr/include/hello)
```

这里我们没有指定路径，但是，cmake 仍然可以帮我们找到 hello.h 存放的路径，就是因为我们设置了环境变量 CMAKE_INCLUDE_PATH。 

如果你不使用 `FIND_PATH`，`CMAKE_INCLUDE_PATH` 变量的设置是没有作用的，你不能指望它会直接为编译器命令添加参数 `-I<CMAKE_INCLUDE_PATH>`。 

以此为例，`CMAKE_LIBRARY_PATH` 可以用在 FIND_LIBRARY 中。 

同样，因为这些变量直接为 FIND 指令所使用，所以所有使用 `FIND_` 指令的 cmake 模块都会受益。 

###### 7. 小结

本节我们探讨了：

- 如何通过 INCLUDE_DIRECTORIES 指令加入非标准的头文件搜索路径。
- 如何通过 LINK_DIRECTORIES 指令加入非标准的库文件搜索路径。
- 如果通过 TARGET_LINK_LIBRARIES 为库或可执行二进制加入库链接。
- 并解释了如何链接到静态库。

到这里为止，您应该基本可以使用 cmake 工作了，但是还有很多高级的话题没有探讨，比如编译条件检查、编译器定义、平台判断、如何跟 pkgconfig 配合使用等等。 

到这里，或许你可以理解前面讲到的“cmake 的使用过程其实就是学习 cmake 语言并编写 cmake 程序的过程”，既然是“cmake 语言”，自然涉及到变量、语法等。

下一节，我们将抛开程序的话题，看看常用的 CMAKE 变量以及一些基本的控制语法规则。 