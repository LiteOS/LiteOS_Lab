本问题现象，当添加完代码之后，发现编译时爆出大面的函数重定义错误，发生该情况的原因，很可能是因为在Makefile中重复包含了某些组件的mk文件，用户尤其会重复包含iot.mk文件。编译现象如下：

 ![](./redefines.png)

通过排查其Makefile，发现project.mk和Makefile中都包含了iot.mk文件

![](./Makefile_err.png)

![](./project_err.png)

当删除project.mk之后，问题修复。