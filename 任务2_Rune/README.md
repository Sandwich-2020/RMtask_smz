# 任务二



## 使用方法：（在该项目第一级文件夹下运行）

mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
make -j4
sudo make install
./test1

## 简单介绍
本代码主要通过二值化对视频进行灰度处理，对轮廓进行模板匹配，寻找大风车的打击扇叶。

## 版本说明
第1版：通过修改任务一颜色通道代码。识别能量机关轮廓
第2版：对能量机关进行模板匹配，识别需要打击的扇叶
第3版：目前存在bug：视频不能正常退出，同时存在误识别，完成后续任务后回来继续琢磨（10.5）



可以考虑任务一二合并！！

