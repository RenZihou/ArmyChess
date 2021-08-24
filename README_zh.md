# 军棋

清华大学计算机系 2021 年夏季小学期项目

***

## 编译

本项目开发使用`MinGW 8.1.0`与`Qt 6.1.2`，因此推荐使用这两个版本以免出现编译错误。如使用`Qt 5.x.x`，还需相应修改`CMakeLists.txt`。

本项目支持使用`cmake`或`qmake`编译，方法如下：

### 使用 cmake

需要在`CMakeLists.txt`中指定`CMAKE_PREFIX_PATH`，即由`Qt`提供的`cmake`前缀目录。这一目录通常形如`<Qt 安装路径>/<Qt 版本>/mingw81_64/lib/cmake`

完成设置后，在项目根目录开启命令行，执行如下命令：

```
> cmake -S . -B build -G "MinGW Makefiles"
> cmake --build build
```

编译好的可执行文件将会保存于`./build`目录。

### 使用 qmake

使用`qmake`无需过多设置，只需在项目根目录执行如下命令：

```
> mkdir build
> cd build
> qmake ..
> make
```

编译好的可执行文件将会保存于`./build/release`。

## 运行游戏

双击编译好的可执行文件即可开始游戏。由于需要网络通信，请确保您开启了相应的防火墙权限。

### 联机

### 游戏（规则说明）



## 设计文档

### 双端工作流程

### 通信协议

本项目采用`TCP`网络通信协议。

### 信号与槽机制设计

### GUI 界面设计



