?> 没有最好，只有更好

从本小节开始，后面所有的构建我们都将采用 out-of-source 外部构建，约定的构建目录是工程目录下的 build 自录。

本小节的任务是让前面的 Hello World 更像一个工程，我们需要做的是：

1. 为工程添加一个`子目录 src`，用来放置工程源代码
2. 添加一个`子目录 doc`，用来放置这个工程的文档 `hello.txt`
3. 在工程目录添加文本文件 `COPYRIGHT`，`README`
4. 在工程目录添加一个 `runhello.sh` 脚本，用来调用 hello 二进制
5. 将构建后的目标文件放入构建目录的 `bin 子目录`
6. 最终安装这些文件：将 `hello` 二进制与 `runhello.sh` 安装至`/usr/bin`，将`doc 目录`的内容以及 `COPYRIGHT/README` 安装到`/usr/share/doc/cmake/t2`

###### 1. 准备工作

在`/backup/cmake/`目录下建立 `t2 目录`。

将 t1 工程的 `main.c` 和 `CMakeLists.txt` 拷贝到 t2 目录中。

###### 2. 添加子目录src

```bash
mkdir src
mv main.c src
```

现在的工程看起来是这个样子：

一个子目录 src，一个 CMakeLists.txt。

上一节我们提到，需要为任何子目录建立一个 CMakeLists.txt，进入子目录 src，编写 CMakeLists.txt 如下：

```
ADD_EXECUTABLE(hello main.c)
```

将 t2 工程的 CMakeLists.txt 修改为：

```
PROJECT(HELLO C)
ADD_SUBDIRECTORY(src bin)
```

然后建立 build 目录，进入 build 目录进行外部编译：

```bash
cmake ..
make
```

构建完成后，你会发现生成的目标文件 hello 位于 build/bin 目录中。

**语法解释：**

ADD_SUBDIRECTORY 指令

`ADD_SUBDIRECTORY(source_dir [binary_dir] [EXCLUDE_FROM_ALL])`

这个指令用于向当前工程添加存放源文件的子目录，并可以指定中间二进制和目标二进制存放的位置。`EXCLUDE_FROM_ALL` 参数的含义是将这个目录从编译过程中排除，比如，工程的 example，可能就需要工程构建完成后，再进入 example 目录单独进行构建(当然，你也可以通过定义依赖来解决此类问题)。

上面的例子定义了将 `src 子目录`加入工程，并指定编译输出(包含编译中间结果)路径为 `bin 目录`。如果不进行 `bin 目录`的指定，那么编译结果(包括中间结果)都将存放在 `build/src 目录`(这个目录跟原有的 src 目录对应)，指定 `bin 目录`后，相当于在编译时将 src 重命名为 bin，所有的中间结果和目标二进制都将存放在 `bin 目录`。

这里需要提一下的是 `SUBDIRS 指令`，使用方法是：

`SUBDIRS(dir1 dir2...)`，但是这个指令已经不推荐使用。它可以一次添加多个子目录，并且，即使外部编译，子目录体系仍然会被保存。

如果我们在上面的例子中将 `ADD_SUBDIRECTORY (src bin)` 修改为 `SUBDIRS(src)`，那么在 build 目录中将出现一个 src 目录，生成的目标代码 hello 将存放在 src 目录中。

###### 3. 换个地方保存目标二进制

不论是 SUBDIRS 还是 ADD_SUBDIRECTORY 指令(不论是否指定编译输出目录)，我们都可以通过 SET 指令重新定义 `EXECUTABLE_OUTPUT_PATH` 和 `LIBRARY_OUTPUT_PATH` 变量来指定最终的目标二进制的位置（指最终生成的 hello 或者最终的共享库，不包含编译生成的中间文件）。

```
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
```

在第一节我们提到了 `<projectname>_BINARY_DIR` 和 `PROJECT_BINARY_DIR` 变量，它们指的是编译发生的当前目录，如果是内部编译，就相当于 `PROJECT_SOURCE_DIR` 也就是工程代码所在目录，如果是外部编译，指的是外部编译所在目录，也就是本例中的 `build 目录`。

所以，上面两个指令分别定义了：

可执行二进制的输出路径为 `build/bin` 和库的输出路径为 `build/lib`。

本节我们没有提到共享库和静态库的构建，所以，你可以不考虑第二条指令。

问题是，我应该把这两条指令写在`工程的 CMakeLists.txt` 还是 `src 目录下的CMakeLists.txt`，把握一个简单的原则，在哪里 `ADD_EXECUTABLE` 或 `ADD_LIBRARY`，如果需要改变目标存放路径，就在哪里加入上述的定义。

在这个例子里，当然就是指 `src 下的 CMakeLists.txt` 了。

###### 4. 如何安装

安装的需要有两种，一种是从代码编译后直接 `make install` 安装，一种是打包时的指定目录安装。

所以，即使最简单的手工编写的 Makefile，看起来也是这个样子的：

```
DESTDIR=
install:
    mkdir -p $(DESTDIR)/usr/bin
    install -m 755 hello $(DESTDIR)/usr/bin
```

你可以通过：`make install` 将 hello 直接安装到 `/usr/bin` 目录，也可以通过 `make install DESTDIR=/tmp/test` 将它安装在 `/tmp/test/usr/bin` 目录，打包时这个方式经常被使用。

稍微复杂一点的是还需要定义 PREFIX，一般 autotools 工程，会运行这样的指令：

`./configure --prefix=/usr` 或者 `./configure --prefix=/usr/local` 来指定 PREFIX

比如上面的 Makefile 就可以改写成：

```
DESTDIR=
PREFIX=/usr
install:
    mkdir -p $(DESTDIR)/$(PREFIX)/bin
    install -m 755 hello $(DESTDIR)/$(PREFIX)/bin
```

那么我们的 HelloWorld 应该怎么进行安装呢？

这里需要引入一个新的 cmake 指令 `INSTALL` 和一个非常有用的变量 `CMAKE_INSTALL_PREFIX`。

`CMAKE_INSTALL_PREFIX` 变量类似于 configure 脚本的 `--prefix`，常见的使用方法看起来是这个样子：`cmake -DCMAKE_INSTALL_PREFIX=/usr`。

INSTALL 指令用于定义安装规则，安装的内容可以包括目标二进制、动态库、静态库以及文件、目录、脚本等。

INSTALL 指令包含了各种安装类型，我们需要一个个分开解释：

**目标文件的安装：**

```
INSTALL(TARGETS targets...
 [
 	[ARCHIVE|LIBRARY|RUNTIME]
 	[DESTINATION <dir>]
 	[PERMISSIONS permissions...]
 	[
 		CONFIGURATIONS [Debug|Release|...]
 	]
 	[COMPONENT <component>]
 	[OPTIONAL]
 ] [...])
```

参数中的 `TARGETS` 后面跟的就是我们通过 ADD_EXECUTABLE 或者 ADD_LIBRARY 定义的目标文件，可能是可执行二进制、动态库、静态库。

目标类型也就相对应的有三种，`ARCHIVE` 特指静态库，`LIBRARY` 特指动态库，`RUNTIME` 特指可执行目标二进制。

`DESTINATION` 定义了安装的路径，如果路径以/开头，那么指的是绝对路径，这时候 `CMAKE_INSTALL_PREFIX` 其实就无效了。如果你希望使用 `CMAKE_INSTALL_PREFIX` 来定义安装路径，就要写成相对路径，即不要以 `/`开头，那么安装后的路径就是 `${CMAKE_INSTALL_PREFIX}/<DESTINATION 定义的路径>`。

举个简单的例子：

```
INSTALL(TARGETS myrun mylib mystaticlib
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION libstatic
)
```

上面的例子会将：

- 可执行二进制 myrun 安装到 `${CMAKE_INSTALL_PREFIX}/bin` 目录
- 动态库 mylib 安装到 `${CMAKE_INSTALL_PREFIX}/lib` 目录
- 静态库 mystaticlib 安装到 `${CMAKE_INSTALL_PREFIX}/libstatic` 目录

特别注意的是你不需要关心 TARGETS 具体生成的路径，只需要写上 TARGETS 名称就可以了。

**普通文件的安装：**

```
INSTALL(FILES files... DESTINATION <dir>
 [PERMISSIONS permissions...]
 [CONFIGURATIONS [Debug|Release|...]]
 [COMPONENT <component>]
 [RENAME <name>] [OPTIONAL]
)
```

可用于安装一般文件，并可以指定访问权限，文件名是此指令所在路径下的相对路径。如果默认不定义权限 PERMISSIONS，安装后的权限为：

OWNER_WRITE, OWNER_READ, GROUP_READ,和 WORLD_READ，即 644 权限。

**非目标文件的可执行程序安装(比如脚本之类)：**

```
INSTALL(PROGRAMS files... DESTINATION <dir>
 [PERMISSIONS permissions...]
 [CONFIGURATIONS [Debug|Release|...]]
 [COMPONENT <component>]
 [RENAME <name>] [OPTIONAL]
)
```

跟上面的 FILES 指令使用方法一样，唯一的不同是安装后权限为：

OWNER_EXECUTE, GROUP_EXECUTE, 和 WORLD_EXECUTE，即 755 权限。

**目录的安装：**

```
INSTALL(DIRECTORY dirs... DESTINATION <dir>
 [FILE_PERMISSIONS permissions...]
 [DIRECTORY_PERMISSIONS permissions...]
 [USE_SOURCE_PERMISSIONS]
 [CONFIGURATIONS [Debug|Release|...]]
 [COMPONENT <component>]
 [[PATTERN <pattern> | REGEX <regex>]
 [EXCLUDE] [PERMISSIONS permissions...]] [...]
)
```

这里主要介绍其中的 DIRECTORY、PATTERN 以及 PERMISSIONS 参数。

`DIRECTORY` 后面连接的是所在 Source 目录的相对路径，但务必注意：abc 和 abc/有很大的区别。

如果目录名不以 / 结尾，那么这个目录将被安装为目标路径下的 abc，如果目录名以 / 结尾，代表将这个目录中的内容安装到目标路径，但不包括这个目录本身。

`PATTERN` 用于使用正则表达式进行过滤，`PERMISSIONS` 用于指定 PATTERN 过滤后的文件权限。

我们来看一个例子：

```
INSTALL(DIRECTORY icons scripts/ DESTINATION share/myproj
 PATTERN "CVS" EXCLUDE
 PATTERN "scripts/*"
 PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
 GROUP_EXECUTE GROUP_READ
)
```

这条指令的执行结果是：

将 icons 目录安装到 /share/myproj，将 scripts/中的内容安装到 /share/myproj

不包含目录名为 CVS 的目录，对于 scripts/* 文件指定权限为 OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ。

**安装时 CMAKE 脚本的执行：**

`INSTALL([[SCRIPT ] [CODE ]] [...])`

`SCRIPT` 参数用于在安装时调用 cmake 脚本文件（也就是 `.cmake` 文件）

`CODE` 参数用于执行 CMAKE 指令，必须以双引号括起来。比如：

```bash
INSTALL(CODE "MESSAGE(\"Sample install message.\")")
```

安装还有几个被标记为过时的指令，比如 INSTALL_FILES 等，这些指令已经不再推荐使用，所以，这里就不再赘述了。

下面，我们就来改写我们的工程文件，让它来支持各种文件的安装，并且，我们要使用 `CMAKE_INSTALL_PREFIX`指令。

##### 5. 修改 Helloworld 支持安装

在本节开头我们定义了本节的任务如下：

1. 为工程添加一个`子目录 src`，用来放置工程源代码
2. 添加一个`子目录 doc`，用来放置这个工程的文档 `hello.txt`
3. 在工程目录添加文本文件 `COPYRIGHT`，`README`
4. 在工程目录添加一个 `runhello.sh` 脚本，用来调用 hello 二进制
5. 将构建后的目标文件放入构建目录的 `bin 子目录`
6. 最终安装这些文件：将 `hello` 二进制与 `runhello.sh` 安装至`/<prefix>/bin`，将`doc 目录`的内容以及 `COPYRIGHT/README` 安装到`/<prefix>/share/doc/cmake/t2`

首先我们先补上未添加的文件：

:white_check_mark: 添加 `doc 目录`及文件

```bash
cd /backup/cmake/t2
mkdir doc
vi doc/hello.txt随便填写一些内容并保存。
```

:white_check_mark: 在工程目录添加 `runhello.sh` 脚本，内容为：

```bash
./hello
```

:white_check_mark: ​添加工程目录中的 `COPYRIGHT` 和 `README`：

```bash
touch COPYRIGHT
touch README
```

下面改写各目录的 CMakeLists.txt：

1. 安装 COPYRIGHT/README，直接修改主工程文件 CMakelists.txt，加入以下指令：

   `INSTALL(FILES COPYRIGHT README DESTINATION share/doc/cmake/t2)`

2. 安装 runhello.sh，直接修改主工程文件 CMakeLists.txt，加入如下指令：

   `INSTALL(PROGRAMS runhello.sh DESTINATION bin)`

3. 安装 doc 中的 hello.txt，这里有两种方式：一是通过在 doc 目录建立 CMakeLists.txt 并将 doc 目录通过 ADD_SUBDIRECTORY 加入工程来完成。另一种方法是直接在工程目录通过 `INSTALL(DIRECTORY )来完成，前者比较简单，各位可以根据兴趣自己完成，我们来尝试后者，顺便演示以下 DIRECTORY 的安装。

   因为 hello.txt 要安装到 `/prefix/share/doc/cmake/t2`，所以我们不能直接安装整个 doc 目录，这里采用的方式是安装 doc 目录中的内容，也就是使用 `doc/`。

   在工程文件中添加：

   `INSTALL(DIRECTORY doc/ DESTINATION share/doc/cmake/t2)`

4. @Gavin注：在 `src`目录的 `CMakeLists.txt`文件中添加如下内容，以安装 hello 到 `/<prefix>/bin`中：

   `INSTALL(TARGETS hello RUNTIME DESTINATION bin)`

###### 6. 尝试我们修改的结果

现在进入 build 目录进行外部编译，注意使用 `CMAKE_INSTALL_PREFIX` 参数，这里我们将它安装到了`/tmp/t2`目录：

   ```bash
   cmake -DCMAKE_INSTALL_PREFIX=/tmp/t2/usr ..
   ```

   然后运行：

   ```bash
   make
   make install
   ```


让我们进入/tmp/t2 目录看一下安装结果：

```
./usr
./usr/share
./usr/share/doc
./usr/share/doc/cmake
./usr/share/doc/cmake/t2
./usr/share/doc/cmake/t2/hello.txt
./usr/share/doc/cmake/t2/README
./usr/share/doc/cmake/t2/COPYRIGHT
./usr/bin
./usr/bin/hello
./usr/bin/runhello.sh
```

如果你要直接安装到系统，可以使用如下指令：

`cmake -DCMAKE_INSTALL_PREFIX=/usr ..`

###### 7. 一个疑问

如果我没有定义 `CMAKE_INSTALL_PREFIX` 会安装到什么地方？

你可以尝试以下，`cmake ..;make;make install`，你会发现 `CMAKE_INSTALL_PREFIX` 的默认定义是 `/usr/local`。

###### 8. 小结

本小节主要描述了如何在工程中使用多目录、各种安装指令以及 CMAKE_INSTALL_PREFIX 变量(你真够牛的，这么点东西居然罗唆了这么多文字)。

在下一小节，我们将探讨如何在 cmake 中构建动态库和静态库，以及如何使用外部头文件和外部共享库，毕竟，这是程序编写中最长使用的（对了，你知道用怎样的 gcc 参数可以直接构建静态库和动态库吗？）