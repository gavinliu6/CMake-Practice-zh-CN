?> _Hello world_，世界 你好

本节选择了一个最简单的例子 Helloworld 来演练一下 cmake 的完整构建过程，本节并不会深入的探讨 cmake，仅仅展示一个简单的例子，并加以粗略的解释。我们选择了Everest Linux 作为基本开发平台，因为这个只有一张 CD 的发行版本，包含了 gcc4.2/gtk/qt3/qt4等完整的开发环境，同时，系统默认集成了 cmake 最新版本 2.4.6。

###### 1. 准备工作

首先，在 `/backup` 目录建立一个 `cmake` 目录，用来放置我们学习过程中的所有练习。

```bash
mkdir -p /backup/cmake
```

以后我们所有的 cmake 练习都会放在/backup/cmake 的子目录下(你也可以自行安排目录，这个并不是限制，仅仅是为了叙述的方便)。

然后在 cmake 建立第一个练习目录 `t1`：

```bash
cd /backup/cmake
mkdir t1
cd t1
```

在 t1 目录建立 `main.c` 和 `CMakeLists.txt`(注意文件名大小写)：

`main.c` 文件内容：

```c
// main.c
#include <stdio.h>
int main()
{
    printf("Hello World from t1 Main!\n");
    return 0;
}
```

`CmakeLists.txt` 文件内容：

```
PROJECT(HELLO)
SET(SRC_LIST main.c)
MESSAGE(STATUS "This is BINARY dir " ${HELLO_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir " ${HELLO_SOURCE_DIR})
ADD_EXECUTABLE(hello ${SRC_LIST})
```

###### 2. 开始构建

所有的文件创建完成后，`t1` 目录中应该存在 `main.c` 和 `CMakeLists.txt` 两个文件。接下来我们来构建这个工程，在这个目录运行：`cmake .` (注意命令后面的点号，代表本目录)。

输出大概是这个样子：

```bash
-- Check for working C compiler: /usr/bin/gcc
-- Check for working C compiler: /usr/bin/gcc -- works
-- Check size of void*
-- Check size of void* - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- This is BINARY dir /backup/cmake/t1
-- This is SOURCE dir /backup/cmake/t1
-- Configuring done
-- Generating done
-- Build files have been written to: /backup/cmake/t1
```

再让我们看一下目录中的内容，你会发现，系统自动生成了：

`CMakeFiles`，`CMakeCache.txt`，`cmake_install.cmake` 等文件，并且生成了 `Makefile`。

然后进行工程的**实际构建**，在这个目录输入 `make` 命令，大概会得到如下的彩色输出：

```bash
Scanning dependencies of target hello
[100%] Building C object CMakeFiles/hello.dir/main.o
Linking C executable hello
[100%] Built target hello
```

如果你需要看到 `make` 构建的详细过程，可以使用 `make VERBOSE=1` 或者 `VERBOSE=1 make` 命令来进行构建。

这时候，我们需要的目标文件 hello 已经构建完成，位于当前目录，尝试运行一下：

```bash
./hello
```

得到输出：

```bash
Hello World from t1 Main!
```

恭喜您，到这里为止您已经完全掌握了 cmake 的使用方法。

###### 3. 简单的解释

我们来重新看一下 `CMakeLists.txt`，这个文件是 cmake 的构建定义文件，文件名是大小写相关的，如果工程存在多个目录，需要确保每个要管理的目录都存在一个 `CMakeLists.txt`。(关于多目录构建，后面我们会提到，这里不作过多解释)。

上面例子中的 CMakeLists.txt 文件内容如下：

```
PROJECT (HELLO)
SET(SRC_LIST main.c)
MESSAGE(STATUS "This is BINARY dir " ${HELLO_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir "${HELLO_SOURCE_DIR})
ADD_EXECUTABLE(hello ${SRC_LIST})
```

_PROJECT 指令的语法是：_

`PROJECT(projectname [CXX] [C] [Java])`

你可以用这个指令定义工程名称，并可指定工程支持的语言，支持的语言列表是可以忽略的，默认情况表示支持所有语言。这个指令隐式的定义了两个 cmake 变量：

`<projectname>_BINARY_DIR` 以及 `<projectname>_SOURCE_DIR`，这里就是 `HELLO_BINARY_DIR` 和 `HELLO_SOURCE_DIR` (所以 `CMakeLists.txt` 中两个 `MESSAGE` 指令可以直接使用了这两个变量)，因为采用的是内部编译，两个变量目前指的都是工程所在路径`/backup/cmake/t1`，后面我们会讲到外部编译，两者所指代的内容会有所不同。

同时 cmake 系统也帮助我们预定义了 `PROJECT_BINARY_DIR` 和 `PROJECT_SOURCE_DIR` 变量，他们的值分别跟 `HELLO_BINARY_DIR` 与 `HELLO_SOURCE_DIR` 一致。

为了统一起见，建议以后直接使用`PROJECT_BINARY_DIR`和`PROJECT_SOURCE_DIR`，即使修改了工程名称，也不会影响这两个变量。如果使用了`<projectname_SOURCE_DIR>`，修改工程名称后，需要同时修改这些变量。

_SET指令的语法是：_

`SET(VAR [VALUE] [CACHE TYPE DOCSTRING [FORCE]])`

现阶段，你只需要了解 SET 指令可以用来显式的定义变量即可。比如我们用到的是`SET(SRC_LIST main.c)`，如果有多个源文件，也可以定义成：`SET(SRC_LIST main.c t1.c t2.c)`。

_MESSAGE语法的指令是：_

`MESSAGE([SEND_ERROR | STATUS | FATAL_ERROR] "message to display" ...) `

这个指令用于向终端输出用户定义的信息，包含了三种类型：

1. `SEND_ERROR`，产生错误，生成过程被跳过
2. `STATUS`，输出前缀为`--`的信息
3. `FATAL_ERROR`，立即终止所有cmake过程

我们在这里使用的是 STATUS 信息输出，演示了由 PROJECT 指令定义的两个隐式变量 HELLO_BINARY_DIR 和 HELLO_SOURCE_DIR。 

**`ADD_EXECUTABLE(hello ${SRC_LIST})`**

定义了这个工程会生成一个文件名为 hello 的可执行文件，相关的源文件是 SRC_LIST 中定义的源文件列表， 本例中你也可以直接写成`ADD_EXECUTABLE(hello main.c)`。 

在本例我们使用了`${}`来引用变量，这是 cmake 的变量应用方式，但是，有一些例外，比如在 `IF 控制语句`，变量是直接使用变量名引用，而不需要`${}`。如果使用了`${}`去应用变量，其实 IF 会去判断名为`${}`所代表的值的变量，那当然是不存在的了。

将本例改写成一个最简化的`CMakeLists.txt`：

```
PROJECT(HELLO)
ADD_EXECUTABLE(hello main.c)
```

###### 4. 基本语法规则

前面提到过，cmake 其实仍然要使用”cmake 语言和语法”去构建，上面的内容就是所谓的 ”cmake 语言和语法”，最简单的语法规则是：

1. 变量使用`${}`方式取值，但是在 IF 控制语句中是直接使用变量名。

2. 指令(参数1 参数2...)

   参数使用括弧括起，参数之间使用空格或分号分开。

   以上面的 `ADD_EXECUTABLE` 指令为例，如果存在另外一个 func.c 源文件，就要写成：

   ADD_EXECUTABLE(hello main.c func.c)或者

   ADD_EXECUTABLE(hello main.c;func.c)

3. 指令是大小写无关的，参数和变量是大小写相关的。但，推荐你全部使用大写指令。

   上面的 MESSAGE 指令已经用到了这条规则：

   MESSAGE(STATUS “This is BINARY dir” ${HELLO_BINARY_DIR})

   也可以写成：

   MESSAGE(STATUS “This is BINARY dir ${HELLO_BINARY_DIR}”)

!> 这里需要特别解释的是作为工程名 HELLO 和生成的可执行文件 hello 是没有任何关系的。hello 定义的可执行文件的文件名，你完全可以写成：`ADD_EXTCUTABLE(t1 main.c)`，编译后会生成一个 t1 可执行文件。

###### 5. 关于语法的疑惑

cmake 的语法还是比较灵活而且考虑到各种情况，比如 `SET(SRC_LIST main.c)` 也可以写成 `SET(SRC_LIST "main.c") ` 是没有区别的，但是假设一个源文件的文件名是 `fu nc.c`(文件名中间包含了空格)。 这时候就必须使用双引号，如果写成了SET(SRC_LIST fu nc.c)，就会出现错误，提示你找不到 `fu 文件`和 `nc.c 文件`。这种情况，就必须写成：`SET(SRC_LIST "fu nc.c")`。

此外，你可以可以忽略掉 source 列表中的源文件后缀，比如可以写成 `ADD_EXECUTABLE(t1 main)`，cmake 会自动的在本目录查找 `main.c` 或者 `main.cpp` 等，当然，最好不要偷这个懒，以免这个目录确实存在一个 `main.c` 一个 `main`。

同时参数也可以使用分号来进行分割。

下面的例子也是合法的：

ADD_EXECUTABLE(t1 main.c t1.c) 可以写成 ADD_EXECUTABLE(t1 main.c;t1.c)。

我们只需要在编写 `CMakeLists.txt` 时注意形成统一的风格即可。 

###### 6. 清理工程

跟经典的 autotools 系列工具一样，运行：

```bash
make clean
```

即可对构建结果进行清理。

###### 7. 问题？问题！

“我尝试运行了 `make distclean`，这个指令一般用来清理构建过程中产生的中间文件的，如果要发布代码，必然要清理掉所有的中间文件，但是为什么在 cmake 工程中这个命令是无效的?”

是的，cmake 并不支持 `make distclean`，关于这一点，官方是有明确解释的：

?> 因为 `CMakeLists.txt` 可以执行脚本并通过脚本生成一些临时文件，但是却没有办法来跟踪这些临时文件到底是哪些。因此，没有办法提供一个可靠的 make distclean 方案。 

?> Some build trees created with GNU autotools have a "make distclean" target that cleans the build and also removes Makefiles and other parts of the generated build system. CMake does not generate a "make distclean" target because `CMakeLists.txt` files can run scripts and arbitrary commands; CMake has no way of tracking exactly which files are generated as part of running CMake. Providing a distclean target would give users the false impression that it would work as expected. (CMake does generate a "make clean" target to remove files generated by the compiler and linker.)

?> A "make distclean" target is only necessary if the user performs an in-source build. CMake supports in-source builds, but we strongly encourage users to adopt the notion of an out-of-source build. Using a build tree that is separate from the source tree will prevent CMake from generating any files in the source tree. Because CMake does not change the source tree, there is no need for a distclean target. One can start a fresh build by deleting the build tree or creating a separate build tree.

同时，还有另外一个非常重要的提示，就是：我们刚才进行的是内部构建(in-source build)，而 cmake 强烈推荐的是外部构建(out-of-source build)。

###### 8. 内部构建与外部构建

上面的例子展示的是“内部构建”，相信看到生成的临时文件比您的代码文件还要多的时候，估计这辈子你都不希望再使用内部构建 :-D

举个简单的例子来说明外部构建，以编译 wxGTK 动态库和静态库为例，在 Everest 中打包方式是这样的：

解开 wxGTK 后，

在其中建立 static 和 shared 目录，

进入 static 目录，运行 `../configure –enable-static;make` 会在 static 目录生成 wxGTK 的静态库，

进入 shared 目录，运行 `../configure –enable-shared;make` 就会在 shared 目录生成动态库。

这就是外部编译的一个简单例子。

对于 cmake ，内部编译上面已经演示过了，它生成了一些无法自动删除的中间文件，所以，引出了我们对外部编译的探讨，外部编译的过程如下：

1. 首先请清除 `t1 目录`中除 `main.c`、`CMakeLists.txt`之外的所有中间文件，最关键的是`CMakeCache.txt`。
2. 在 t1 目录中建立 `build 目录`，当然你也可以在任何地方建立 build 目录，不一定必须在工程目录中。
3. 进入 `build 目录` ，运行`cmake ..`（注意，`..`代表父目录，因为父目录存在我们需要的 `CMakeLists.txt`，如果你在其他地方建立了 build 目录，需要运行 `cmake <工程的全路径>`），查看一下 build 目录，就会发现生成了编译需要的Makefile以及其他的中间文件。
4. 运行 `make` 构建工程，就会在当前目录（build 目录）中获得目标文件 hello。

上述过程就是所谓的 out-of-source 外部编译，一个最大的好处是，对于原有的工程没有任何影响，所有动作全部发生在编译目录。通过这一点，也足以说服我们全部采用外部编译方式构建工程。

这里需要特别注意的是：

通过外部编译进行工程构建，`HELLO_SOURCE_DIR`仍然指代工程目录，即`/backup/cmake/t1`，而`HELLO_BINARY_DIR`则指代编译路径，即`/backup/cmake/t1/build`。

###### 9. 小结

本小结描述了使用 cmake 构建 Hello World 程序全部过程，并介绍了三个简单的指令：PROJECT/MESSAGE/ADD_EXECUTABLE 以及变量调用的方法，同时提及了两个隐式变量 `<projectname>_SOURCE_DIR` 及`<projectname>_BINARY_DIR`，演示了变量调用的方法，从这个过程来看，有些开发者可能会想，这实在比我写Makefile要复杂多了，甚至我都可以不编写Makefile，直接使用`gcc main.c`即可生成需要的目标文件。是的，正如第一节提到的，如果工程只有几个文件，还是直接编写Makefile最简单。但是，kdelibs 压缩包达到 50M，您认为使用什么方案更容易一点呢？

下一节，我们的任务是让 Hello World 看起来更像一个工程。