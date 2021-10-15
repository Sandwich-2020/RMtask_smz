# 任务六



## 使用方法：（在该项目第一级文件夹下运行）

mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
make -j4
sudo make install
./test1



## 说明

在计算角度时，以装甲板中心作为世界坐标系原点，再通过cv::solvePnP结合所给的相机内参与畸变参数解算出世界坐标系相对于相机坐标系的位移向量与旋转向量，进一步可求姿态与距离。



