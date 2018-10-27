你现在还会觉得 cmake 简单吗？

本章我们将着重介绍系统预定义的 Find 模块的使用以及自己编写 Find 模块，系统中提供了其他各种模块，一般情况需要使用 `INCLUDE` 指令显式的调用，`FIND_PACKAGE` 指令是一个特例，可以直接调用预定义的模块。

其实使用纯粹依靠 cmake 本身提供的基本指令来管理工程是一件非常复杂的事情，所以，cmake 设计成了可扩展的架构，可以通过编写一些通用的模块来扩展 cmake。

在本章，我们准备首先介绍一下 cmake 提供的 FindCURL 模块的使用。然后，基于我们前面的 libhello 共享库，编写一个 FindHello.cmake 模块。

##### 一、使用 FindCURL 模块

在 `/backup/cmake` 目录建立 `t5 目录`，用于存放我们的 CURL 的例子。

建立 `src 目录`，并建立 `src/main.c`，内容如下：

```c
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE *fp;

int write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = fwrite(ptr, size, nmemb, (FILE *)fp);
    return written;
}

int main()
{
    const char * path = "/tmp/curl-test";
    const char * mode = "w";
    fp = fopen(path, mode);
    curl_global_init(CURL_GLOBAL_ALL);
    CURLcode res;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.linux-ren.org");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}
```

这段代码的作用是通过 curl 取回 www.linux-ren.org 的首页并写入 `/tmp/curl-test` 文件中。

?> @Gavin注：原书中的 www.linux-ren.org 会产生301重定向，建议换成其他，比如：https://www.baidu.com

建立主工程文件 `CMakeLists.txt`：

```
PROJECT(CURLTEST) 
ADD_SUBDIRECTORY(src)
```

建立 `src/CMakeLists.txt`：

```
ADD_EXECUTABLE(curltest main.c)
```

现在自然是没办法编译的，我们需要添加 curl 的头文件路径和库文件。

**方法 1：**

直接通过 `INCLUDE_DIRECTORIES` 和 `TARGET_LINK_LIBRARIES` 指令添加

我们可以直接在 `src/CMakeLists.txt` 中添加：

```
INCLUDE_DIRECTORIES(/usr/include)
TARGET_LINK_LIBRARIES(curltest curl)
```

然后建立 `build 目录`进行外部构建即可。

现在我们要探讨的是使用 cmake 提供的 FindCURL 模块。

**方法2：**

向 `src/CMakeLists.txt` 中添加：

```
FIND_PACKAGE(CURL)
IF(CURL_FOUND)
   INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
   TARGET_LINK_LIBRARIES(curltest ${CURL_LIBRARY})
ELSE(CURL_FOUND)
     MESSAGE(FATAL_ERROR ”CURL library not found”)
ENDIF(CURL_FOUND)
```

对于系统预定义的 `Find<name>.cmake` 模块，使用方法一般如上例所示：

每一个模块都会定义以下几个变量：

-  `<name>_FOUND `

- `<name>_INCLUDE_DIR` or `<name>_INCLUDES`

- `<name>_LIBRARY` or `<name>_LIBRARIES`

你可以通过`<name>_FOUND`来判断模块是否被找到，如果没有找到，按照工程的需要关闭某些特性、给出提醒或者中止编译，上面的例子就是报出致命错误并终止构建。 

如果 `<name>_FOUND` 为真，则将 `<name>_INCLUDE_DIR` 加入 `INCLUDE_DIRECTORIES`， 将 `<name>_LIBRARY` 加入 `TARGET_LINK_LIBRARIES` 中。 

我们再来看一个复杂的例子，通过 `<name>_FOUND` 来控制工程特性：

```
SET(mySources viewer.c)
SET(optionalSources)
SET(optionalLibs)
FIND_PACKAGE(JPEG)
IF(JPEG_FOUND)
   SET(optionalSources ${optionalSources} jpegview.c)
   INCLUDE_DIRECTORIES( ${JPEG_INCLUDE_DIR} )
   SET(optionalLibs ${optionalLibs} ${JPEG_LIBRARIES} )
   ADD_DEFINITIONS(-DENABLE_JPEG_SUPPORT)
ENDIF(JPEG_FOUND)
IF(PNG_FOUND)
   SET(optionalSources ${optionalSources} pngview.c)
   INCLUDE_DIRECTORIES( ${PNG_INCLUDE_DIR} )
   SET(optionalLibs ${optionalLibs} ${PNG_LIBRARIES} )
   ADD_DEFINITIONS(-DENABLE_PNG_SUPPORT)
ENDIF(PNG_FOUND)
ADD_EXECUTABLE(viewer ${mySources} ${optionalSources} ) TARGET_LINK_LIBRARIES(viewer ${optionalLibs}
```

通过判断系统是否提供了 JPEG 库来决定程序是否支持 JPEG 功能。

##### 二、编写属于自己的 FindHello 模块

我们在此前的 t3 实例中，演示了构建动态库、静态库的过程并进行了安装。

接下来，我们在 t6 示例中演示如何自定义 FindHELLO 模块并使用这个模块构建工程：

请在 `/backup/cmake/` 中建立 `t6 目录`，并在其中建立 `cmake 目录`用于存放我们自己定义的 `FindHELLO.cmake` 模块，同时建立 `src 目录`，用于存放我们的源文件。

1. 定义 `cmake/FindHELLO.cmake` 模块

```
FIND_PATH(HELLO_INCLUDE_DIR hello.h /usr/include/hello /usr/local/include/hello)
FIND_LIBRARY(HELLO_LIBRARY NAMES hello PATH /usr/lib /usr/local/lib)
IF(HELLO_INCLUDE_DIR AND HELLO_LIBRARY)
   SET(HELLO_FOUND TRUE)
ENDIF(HELLO_INCLUDE_DIR AND HELLO_LIBRARY)
IF(HELLO_FOUND)
   IF(NOT HELLO_FIND_QUIETLY)
       MESSAGE(STATUS "Found Hello: ${HELLO_LIBRARY}")
   ENDIF(NOT HELLO_FIND_QUIETLY)
ELSE(HELLO_FOUND)
   IF(HELLO_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find hello library")
   ENDIF(HELLO_FIND_REQUIRED)
ENDIF(HELLO_FOUND)
```

针对上面的模块让我们再来回顾一下 `FIND_PACKAGE` 指令：

```
FIND_PACKAGE(<name> [major.minor] [QUIET] [NO_MODULE]
				[[REQUIRED|COMPONENTS] [componets...]])
```

前面的 CURL 例子中我们使用了最简单的 FIND_PACKAGE 指令，其实他可以使用多种参数，QUIET参数，对应与我们编写的FindHELLO中的 HELLO_FIND_QUIETLY，如果不指定 这个参数，就会执行：

`MESSAGE(STATUS "Found Hello: ${HELLO_LIBRARY}")`

REQUIRED 参数，其含义是指这个共享库是否是工程必须的，如果使用了这个参数，说明这个链接库是必备库，如果找不到这个链接库，则工程不能编译。对应于`FindHELLO.cmake模块`中的 `HELLO_FIND_REQUIRED` 变量。

同样，我们在上面的模块中定义了 HELLO_FOUND, HELLO_INCLUDE_DIR,HELLO_LIBRARY 变量供开发者在FIND_PACKAGE 指令中使用。 

OK，下面建立 `src/main.c`，内容为：

```c
#include <hello.h>
int main()
{
     HelloFunc();
	return 0; 
}
```

建立 `src/CMakeLists.txt` 文件，内容如下：

```
FIND_PACKAGE(HELLO)
IF(HELLO_FOUND)
    ADD_EXECUTABLE(hello main.c)
    INCLUDE_DIRECTORIES(${HELLO_INCLUDE_DIR})
    TARGET_LINK_LIBRARIES(hello ${HELLO_LIBRARY})
ENDIF(HELLO_FOUND)
```

为了能够让工程找到 FindHELLO.cmake 模块(存放在工程中的 cmake 目录)我们在主工程文件 CMakeLists.txt 中加入：

`SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)`

##### 三、使用自定义的 FindHELLO 模块构建工程

仍然采用外部编译的方式，建立 `build 目录`，进入目录运行：

```bash
cmake ..
```

我们可以从输出中看到：

```
Found Hello: /usr/lib/libhello.so
```

如果我们把上面的`FIND_PACKAGE(HELLO)`修改为`FIND_PACKAGE(HELLO QUIET)`，则不会看到上面的输出。 

接下来就可以使用 make 命令构建工程，运行：` ./src/hello` 可以得到输出： Hello World。

说明工程成功构建。 

##### 四、如果没有找到 hello library 呢?

我们可以尝试将 `/usr/lib/libhello.x` 移动到 `/tmp 目录`，这样，按照 FindHELLO 模块的定义，就找不到 hello library 了，我们再来看一下构建结果：

```bash
cmake ..
```

仍然可以成功进行构建，但是这时候是没有办法编译的。

修改`FIND_PACKAGE(HELLO)为FIND_PACKAGE(HELLO REQUIRED)`，将 `hello library` 定义为工程必须的共享库。

这时候再次运行`cmake ..`

我们得到如下输出：

CMake Error: Could not find hello library

因为找不到 libhello.x，所以，整个 Makefile 生成过程被出错中止。

**小结**

 在本节中，我们学习了如何使用系统提供的 `Find<NAME>` 模块并学习了自己编写 `Find<NAME>` 模块以及如何在工程中使用这些模块。 

后面的章节，我们会逐渐学习更多的 cmake 模块使用方法以及用 cmake 来管理 GTK 和 QT4 工程。 
