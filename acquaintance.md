?> CMake 不再使你在构建项目时郁闷地想自杀了。    --一位 KDE 开发者

###### 1. 背景知识

cmake 是 kitware 公司以及一些开源开发者在开发几个工具套件(VTK)的过程中的衍生品，最终形成体系，成为一个独立的开放源代码的项目。项目的诞生时间是 2001 年。其官方网站是 www.cmake.org，可以通过访问官方网站获得更多关于 cmake 的信息。cmake 的流行其实要归功于 KDE4 的开发(似乎跟当年的 svn 一样，KDE 将代码仓库从 CVS 迁移到SVN，同时证明了 SVN 管理大型项目的可用性)，在 KDE 开发者使用了近 10 年 autotools 之后，他们终于决定为 KDE4 选择一个新的工程构建工具，其根本原因用 KDE 开发者的话来说就是：只有少数几个“编译专家”能够掌握 KDE 现在的构建体系(admin/Makefile.common)，在经历了 unsermake， scons 以及 cmake 的选型和尝试之后，KDE4 决定使用 cmake 作为自己的构建系统。在迁移过程中，进展异常的顺利，并获得了 cmake 开发者的支持。所以，目前的 KDE4 开发版本已经完全使用 cmake 来进行构建。像 kdesvn，rosegarden 等项目也开始使用cmake，这也注定了 cmake 必然会成为一个主流的构建体系。

###### 2. 特点

cmake 的特点主要有：

1. 开放源代码，使用类 BSD 许可发布。http://cmake.org/HTML/Copyright.html
2. 跨平台，并可生成 native 编译配置文件，在 Linux/Unix 平台，生成 makefile，在苹果平台，可以生成 xcode，在 Windows 平台，可以生成 MSVC 的工程文件。
3. 能够管理大型项目，KDE4 就是最好的证明。
4. 简化编译构建过程和编译过程。Cmake 的工具链非常简单：cmake+make。
5. 高效虑，按照 KDE 官方说法，CMake 构建 KDE4 的 kdelibs 要比使用 autotools 来构建 KDE3.5.6 的 kdelibs 快 40%，主要是因为 Cmake 在工具链中没有 libtool。
6. 可扩展，可以为 cmake 编写特定功能的模块，扩充 cmake 功能。

###### 3. 问题，难道就没有问题？

1. cmake 很简单，但绝对没有听起来或者想象中那么简单。
2. cmake 编写的过程实际上是编程的过程，跟以前使用 autotools 一样，不过你需要编写的是 `CMakeLists.txt`(每个目录一个)，使用的是”cmake 语言和语法”。
3. cmake 跟已有体系的配合并不是特别理想，比如 pkgconfig，您在实际使用中会有所体会，虽然有一些扩展可以使用，但并不理想。

###### 4. 个人的建议

1. 如果你没有实际的项目需求，那么看到这里就可以停下来了，因为 cmake 的学习过程就是实践过程，没有实践，读的再多几天后也会忘记。
2. 如果你的工程只有几个文件，直接编写 Makefile 是最好的选择。
3. 如果使用的是 C/C++/Java 之外的语言，请不要使用 cmake(至少目前是这样)。
4. 如果你使用的语言有非常完备的构建体系，比如 java 的 ant，也不需要学习 cmake，虽然有成功的例子，比如 QT4.3 的 csharp 绑定 qyoto。
5. 如果项目已经采用了非常完备的工程管理工具，并且不存在维护问题，没有必要迁移到 cmake 。
6. 如果仅仅使用 qt 编程，没有必要使用 cmake，因为 qmake 管理 Qt 工程的专业性和自动化程度比 cmake 要高很多。