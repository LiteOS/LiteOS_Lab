### 文件说明
|文件|说明|  
|:------|:------|
|mem/test_los_memory|内存的测试案例，文件内部的每个TEST_F()代表一个测试用例|
|main.cpp|测试实例的main文件，main函数是入口函数|
|Makefile|
|run_test.sh|执行用例的脚本，包括部署测试框架，编译被测对象库，编译测试用例并执行等|

### 使用说明
#### run_test.sh
> * sh run_test.sh ：部署测试框架，重新编译被测对象库，重新编译测试用例并执行
> * sh run_test.sh clean : 清除测试产生的中间文件，还原现场

#### Makefile
> * make ：链接被测对象库，编译测试用例，生成可执行程序
> * make run ：执行测试用例
> * make cov ：统计覆盖率
> * make memcheck ：内存泄露检测
> * make clean ：清除编译测试用例产生的中间文件
> * make distclean : 清除测试产生的中间文件（包括被测对象库编译插产生的中间文件）