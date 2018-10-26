cmake 已经开发了 5，6 年的时间，如果没有 KDE4，也许不会有人或者 Linux 发行版本重视 cmake，因为除了 Kitware 似乎没有人使用它。通过 KDE4 的选型和开发，cmake 逐渐进入了人们的视线，在实际的使用过程中，cmake 的优势也逐渐的被大家所认识，至少 KDE 的开发者们给予了 cmake 极高的评价，同时庞大的 KDE 项目使用 cmake 来作为构建工具也证明了 cmake 的可用性和大项目管理能力。

所以，cmake 应该感谢 KDE，也正因为如此，cmake 的开发者投入了 KDE 从 autotools 到 cmake 的迁移过程中，并相当快速和顺利的完成了迁移，现在整个 KDE4 开发版本全部使用 cmake 构建。

这也是促使我们学习 cmake 的原因，首先 cmake 被接受并成功应用，其次，cmake 的优势在实际使用中不断的体现出来。

我们为什么不来认识一下这款优秀的工程构建工具呢？

在 2006 年 KDE 大会，听 cmake 开发者当面介绍了 cmake 之后，我就开始关注 cmake，并将 cmake 纳入了 Everest 发行版，作为系统默认组件。最近 QT-4.3 也正式进入了 Everest 系统，为 KDE4 构建完成了准备工作。

但是，在学习 cmake 的过程中，发现官方的文档非常的少，而且错误也较多，比如：在介绍 Find 模块编写的文档中，模块名称为 FOO，但是后面却出现了 Foo_FIND_QUIETLY 的定义，这显然是错误的，这样的定义永远不可能有效，正确的定义是 FOO_FIND_QUIETLY。种种原因，促使我开始写一份“面向使用和实用”的 cmake 文档，也就是本教程《cmake 实践》(Cmake Practice)。

本文档是边学习边编写的成果，更像是一个学习笔记和 Tutorial，因此难免有失误或者理解不够透彻的地方，比如，我仍然不能理解为什么绝大部分使用变量的情况要通过`${}`引用，而在 `IF 语句`中却必须直接使用变量名。也希望能够有 cmake 的高手来指点迷津。

补：从 cmake 的 maillist,我找到了一些答案，原文是：

?> The `IF(var) `or `IF(NOT var)` command expects var to be the name of a variable. This is stated in CMake's manual. So, for your situation `IF(${libX})` is the same as `IF(/usr/lib/xorg)` and then CMake will check the value of the variable named `/usr/lib/xorg`.

也就是说 IF 需要的是变量名而不是变量值。

这个文档是开放的，开放的目的是为了让更多的人能够读到并且能够修改，任何人都可以对它作出修改和补充，但是，为了大家都能够获得你关于 cmake 的经验和积累，如果你发现错误或者添加了新内容后，请务必 CC 给我一份，让我们共同把 cmake 掌握的更好。