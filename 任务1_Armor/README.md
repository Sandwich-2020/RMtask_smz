#任务一



# 使用方法：（在该项目第一级文件夹下运行）

mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
make -j4
sudo make install
./test1


#简单介绍
本代码主要通过二值化对视频进行灰度处理，在识别轮廓后，寻找周长最大轮廓，并寻找最低点和最高点，连线找打击点。

# 版本说明
第1版：通过将python代码修改成cpp类型代码（区别是真的大）注意点：opencv后续更新较多，18年前的代码基本需要修改才能用
第2版：存在bug：1.误识别 2.视频不能正常退出
第3版：解决bug1：用红色通道，减少误识别
