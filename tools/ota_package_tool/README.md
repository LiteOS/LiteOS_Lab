ota_package_tool工具开发以及编译说明

#开发需要的工具

python3.7,pyqt5,pyqt5-tools, PyCryptodome,pyinstaller,pycharm，mysys2

1）python3.7（https://www.python.org/downloads/windows/）

	注意：python安装时，勾选pip，pyqt5,pyqt5-tools和pyinstaller是通过pip安装的，通过pip安装比较简单

2）pyqt5-tools （https://pypi.org/project/pyqt5-tools/#files）支持的python最高版本3.7

    pyqt5-tools包含pyqt5开发需要的各种工具，比如designer等

    安装pyqt5-tools默认自动安装对应版本的pyqt5

3） PyCryptodome （https://pycryptodome.readthedocs.io/en/latest/index.html）

    强大的python开源加密库
   

4）pyinstaller （https://pypi.org/project/PyInstaller/#description）支持的最高版本3.7

	打包工具，将py打包成exe可执行文件
 

5）pycharm（https://www.jetbrains.com/pycharm/）
	
	python的IDE

6）mysys2 

	hdiffz的windows编译工具


#开发工具安装配置

上面章节提到的工具只需要手动下载python和pycharm，其他工具通过pip下载，自动适配对应版本

1）下载python3.7 windows 32位

2）安装python-3.7.0.exe (https://www.python.org/downloads/windows/)

3）pip安装其他组件（如果pip下载速度过慢，请换国内源比如https://pypi.tuna.tsinhua.edu.cn/simple）

	pip install sip
	pip install pyqt5-tools
	pip install pyinstaller

4）安装pycharm 
 下载地址https://www.jetbrains.com/pycharm/download/#section=windows
 打开PyCharm，点击右下角Configure—>Settings—>External Tools,点击加号来添加自己的工具，做如下配置：
 
	Name:QtDesigner 
  	Group:Qt 
  	Programs:各位自己的designer路径 
  	Working directory：$ProjectFileDir$ 

  	Name:Pyuic 
 	Group:Qt 
 	Program:F:\anaconda\python.exe(各位自己的python路径) 
 	Arguments：-m PyQt5.uic.pyuic  $FileName$ -o $FileNameWithoutExtension$.py 
 	Working directory：$FileDir$ 

#编译打包

1）编译差分动态库
	
	打开mysys2，进入工具的GCC目录tools\ota_package_tool\GCC
	mingw32-make.exe

2）打包exe

	进入工具目录tools\ota_package_tool
	只需批处理文件build.bat
	打包的可执行文件在dist目录