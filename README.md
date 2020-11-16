# SLAM_ZYNQ
2020年第四届全国大学生FPGA创新设计竞赛参赛作品

## 项目说明
本项目基于开源的SLAM框架ORB_SLAM2，项目中主要完成的工作有:
1. 将ORB_SLAM2移植到ZYNQ平台上，具体包括：
   * 完成OpenCV、DBoW2、g2o等库在PS端ARM处理器上的交叉编译；
   * 对ORB-SLAM2代码进行修改，删减了其中调用Pangolin库进行地图和轨迹进行实时绘制的代码，最终编译出可在PS端使用的库文件；
   * 打包所有依赖库并编写CMakeLists.txt，提供完整的交叉编译环境；
2. 利用ZYNQ的PL部分对ORB_SLAM2中前端的特征提取部分进行加速，其中：
   * PL端代码使用HLS编写，实现了ORB特征提取中所需的FAST角点提取、高斯模糊、BRIEF描述符计算及构建图形金字塔所需的图像缩放功能；
   * PL端与PS端通过DMA进行数据交互，为此使用C++编写了PS端的DMA相关驱动程序；
   * 修改ORB_SLAM2中特征提取相关代码，使其可以利用PL进行特征提取加速

## 开发环境
* 电脑端
    * 操作系统： Ubuntu 18.04
    * 交叉编译工具链：arm-linux-gnueabihf（gcc 版本 7.2.1 20171011 (Linaro GCC 7.2-2017.11-rc1) 
* 开发板
    * 板卡型号：PYNQ-Z2
    * PS端操作系统：使用[PYNQ-Z2官方系统镜像](https://d2m32eurp10079.cloudfront.net/Download/pynq_z2_v2.5.zip)构建

## 主要文件介绍
* Examples：开发过程中使用的测试用例，其中的RGB-D目录下为最终测试整个项目使用的可执行文件代码；
* Thirdparty：OpenCV等第三方依赖库的头文件及交叉编译好的动态链接库等；
* Vocabulary：ORB_SLAM2项目中带有的字典文件，供特征点检索的词袋模型使用
* include：修改后的ORB_SLAM2头文件
* src：修改后的ORB_SLAM2源文件
* lib：项目中编译得到的动态链接库
* utils：目前其中存放了用于将vivado产生的bit格式比特流转换为fpga manager使用的bin格式的Python脚本
* on-board files：开发板上用到的文件，其中包括用于烧写PL端的bin文件、测试使用的可执行文件、用于评测执行结果的程序及方便自动运行的shell脚本
* PL: PL端工程文件
    * ip：IP核的HLS源码及生成IP核使用的tcl脚本
    * project_1: 用于重建vivado工程的tcl脚本

## 工程重建方法
需使用**Vivado 2018.3**
### 构建IP核
在HLS Command Prompt中进入PL/ip目录下输入以下命令：
```
vivado_hls -f build_ip.tcl
```
### 重建Vivado工程
在Vivado TCL Shell中进入PL/project_1目录，输入以下命令：
```
source orb_fpga.tcl
```
之后会在project_1目录下生成myproj目录，使用Vivado打开其中的project_1.xpr项目文件，为block design创建HDL wrapper后即可开始生成bitstream
