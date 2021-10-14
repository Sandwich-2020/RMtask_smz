#include <iostream>
#include <vector>
using namespace std;

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
double getDistance(Point A, Point B)
{
    double dis;
    dis = pow((A.x - B.x), 2) + pow((A.y - B.y), 2);
    return sqrt(dis);
}
int find_x_max_Point_index(vector<Point> a, int n);
int find_x_min_Point_index(vector<Point> a, int n);
int find_y_max_Point_index(vector<Point> a, int n);
int find_y_min_Point_index(vector<Point> a, int n);
Point cross_point(Point ptStart1, Point ptStart2, Point ptEnd1, Point ptEnd2);
int index_firstmatch(vector<vector<Point>> cts, int xl, int xr, int yb);
int index_secondmatch(vector<vector<Point>> cts, int xl, int xr, int yb, int first_idx);
int main(int, char **)
{
    Mat in_frame, out_gray, out_binary, out_gray2, out_binary2;
    const char win1[] = "装甲板_2.avi";
    double fps = 30; //每秒的帧数
    char file_out[] = "装甲板_2.avi";
    VideoCapture inVideo("../装甲板_2.avi");

    if (!inVideo.isOpened())
    { //检查错误
        cout << "Error! \n";
        return -1;
    }
    //获取输入视频的宽度和高度
    int width = (int)inVideo.get(CAP_PROP_FRAME_WIDTH);
    int height = (int)inVideo.get(CAP_PROP_FRAME_HEIGHT);
    // cout << "width = " << width << ", height = " << height << endl;
    VideoWriter recVid(file_out, VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, Size(width, height), 0);
    if (!recVid.isOpened())
    {
        cout << "Error! Video1 not opened...\n";
        return -1;
    }
    //为视频创建窗口
    namedWindow(win1);
    while (true)
    {
        //读取帧（抓取并解码）以流的形式进行
        inVideo >> in_frame;
        //将帧转换为灰度
        cvtColor(in_frame, out_gray, COLOR_BGR2GRAY);
        out_gray = out_gray > 150;

        //获得目标颜色图像的二值图

        // split(in_frame, imgChannels);
        // Mat out_gray = imgChannels.at(2) - imgChannels.at(0);

        threshold(out_gray, out_binary, 100, 255, THRESH_BINARY);

        //定义轮廓和层次结构
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;

        //查找轮廓
        findContours(out_binary, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
        // cout << contours[2].size() << endl;
        // cout << contours[0].size() << endl;

        // int index = 0;
        // for (; index >= 0; index = hierarchy[index][0])
        // {
        //     Scalar color(rand() & 255, rand() & 255, rand() & 255);
        //     drawContours(in_frame, contours, index, color, FILLED, 8, hierarchy);
        // }
        if (contours.size() > 0)
        {
            unsigned long long max_contour = 0;
            double max_area = cv::contourArea(cv::InputArray(contours[0]), false);

            for (unsigned long long i = 1; i < contours.size(); i++)
            {
                double temp_area = cv::contourArea(cv::InputArray(contours[i]), false);
                if (max_area < temp_area)
                {
                    max_area = temp_area;
                    max_contour = i;
                }
            }
            if (max_area > 1200)
            {
                Mat imageContours0 = Mat::zeros(out_binary.size(), CV_8UC1); //最小外接正矩形画布

                vector<double> g_dConArea(contours.size());
                for (int i = 0; i < contours.size(); i++)
                {
                    //绘制轮廓
                    /*drawContours(imageContours0, contours, i, Scalar(255), 1, 8, hierarchy);*/
                    //计算轮廓的面积
                    g_dConArea[i] = contourArea(contours[i]);
                }
                //寻找面积最大的部分
                int max = 0;
                for (int i = 1; i < contours.size(); i++)
                {
                    if (g_dConArea[i] > g_dConArea[max])
                    {
                        max = i;
                    }
                }

                vector<Point> dim_con1 = contours[max];
                Point ptStart1 = dim_con1[find_x_min_Point_index(dim_con1, dim_con1.size())];
                Point ptEnd1 = dim_con1[find_x_max_Point_index(dim_con1, dim_con1.size())];
                Point ptChoose1 = dim_con1[find_y_max_Point_index(dim_con1, dim_con1.size())];
                // line(in_frame, ptStart1, ptEnd1, Scalar(0, 255, 0), 2);

                //绘制轮廓
                // drawContours(in_frame, contours, max, Scalar(255, 0, 0), FILLED, 8, hierarchy);
                cvtColor(in_frame, out_gray2, COLOR_BGR2GRAY);

                vector<Mat> imgChannels;

                //获得目标颜色图像的二值图

                split(in_frame, imgChannels);
                Mat out_gray2 = imgChannels.at(2) - imgChannels.at(0);

                threshold(out_gray2, out_binary2, 100, 255, THRESH_BINARY);

                //定义轮廓和层次结构
                vector<vector<Point>> contours2;
                vector<Vec4i> hierarchy2;

                //查找轮廓
                findContours(out_binary2, contours2, hierarchy2, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
                // cout << contours[2].size() << endl;
                // cout << contours[0].size() << endl;

                // int index = 0;
                // for (; index >= 0; index = hierarchy2[index][0])
                // {
                //     Scalar color(rand() & 255, rand() & 255, rand() & 255);
                //     drawContours(in_frame, contours2, index, color, FILLED, 8, hierarchy2);
                // }
                int first_idx = index_firstmatch(contours2, ptStart1.x, ptEnd1.x, ptChoose1.y);
                int second_idx = index_secondmatch(contours2, ptStart1.x, ptEnd1.x, ptChoose1.y, first_idx);
                vector<Point> first_cts = contours2.at(first_idx);
                vector<Point> second_cts = contours2.at(second_idx);
                Point ptStart2 = first_cts[find_y_min_Point_index(first_cts, first_cts.size())];
                Point ptEnd2 = first_cts[find_y_max_Point_index(first_cts, first_cts.size())];
                Point ptStart3 = second_cts[find_y_min_Point_index(second_cts, second_cts.size())];
                Point ptEnd3 = second_cts[find_y_max_Point_index(second_cts, second_cts.size())];

                line(in_frame, ptStart2, ptEnd3, Scalar(0, 255, 0), 2);
                line(in_frame, ptStart3, ptEnd2, Scalar(0, 255, 0), 2);
                Point cross = cross_point(ptStart2, ptStart3, ptEnd2, ptEnd3);
                if (cross.x != (-1) && cross.y != (-1))
                {
                    circle(in_frame, cross, 3, (255, 0, 255), 5);
                }

                // for (int k = 0; k < (int)contours2.size(); k++)
                // {

                //     vector<Point> dim_con1 = contours2.at(k);
                //     for (int t = 0; t < (int)dim_con1.size(); t++)
                //     {
                //         Point dim_con2 = dim_con1[t];
                //         if (dim_con2.x > ptStart1.x && dim_con2.x < ptEnd1.x && dim_con2.y > ptChoose1.y + 30)
                //         {
                //             // drawContours(in_frame, contours2, k, Scalar(255, 0, 0), FILLED);
                //             Point ptStart2 = dim_con1[find_y_min_Point_index(dim_con1, dim_con1.size())];
                //             Point ptEnd2 = dim_con1[find_y_max_Point_index(dim_con1, dim_con1.size())];
                //             line(in_frame, ptStart2, ptEnd2, Scalar(0, 255, 0), 2);
                //         }
                //     }
                // }
            }
        }

        //将幀写入视频文件（编码并保存）以流的形式进行
        recVid << in_frame;
        imshow(win1, in_frame); // 在窗口中显示帧
        // imshow(win2, out_frame1); // 在窗口中显示帧
        if (in_frame.empty())
            break;
        else if (waitKey(1000 / fps) >= 0)
            break;
    }

    inVideo.release();
    return 0;
}
int find_x_min_Point_index(vector<Point> a, int n)
{
    int idx = 0;
    for (int i = 1; i < n; i++)
    {
        if (a[idx].x > a[i].x)
        {
            idx = i;
        }
    }
    return idx;
}
int find_x_max_Point_index(vector<Point> a, int n)
{
    int idx = 0;
    for (int i = 1; i < n; i++)
    {
        if (a[idx].x < a[i].x)
        {
            idx = i;
        }
    }
    return idx;
}
int find_y_max_Point_index(vector<Point> a, int n)
{
    int idx = 0;
    for (int i = 1; i < n; i++)
    {
        if (a[idx].y < a[i].y)
        {
            idx = i;
        }
    }
    return idx;
}
int find_y_min_Point_index(vector<Point> a, int n)
{
    int idx = 0;
    for (int i = 1; i < n; i++)
    {
        if (a[idx].y > a[i].y)
        {
            idx = i;
        }
    }
    return idx;
}

int index_firstmatch(vector<vector<Point>> cts, int xl, int xr, int yb)
{
    for (int k = 0; k < (int)cts.size(); k++)
    {

        vector<Point> temp_ct = cts.at(k);
        for (int t = 0; t < (int)temp_ct.size(); t++)
        {
            Point pt = temp_ct[t];
            if (pt.x > xl && pt.x < xr && pt.y > yb + 30)
            {
                return k;
                // // drawContours(in_frame, cts, k, Scalar(255, 0, 0), FILLED);
                // Point ptStart2 = temp_ct[find_y_min_Point_index(dim_con1, temp_ct.size())];
                // Point ptEnd2 = temp_ct[find_y_max_Point_index(dim_con1, temp_ct.size())];
                // line(in_frame, ptStart2, ptEnd2, Scalar(0, 255, 0), 2);
            }
        }
    }
    return -1;
}
int index_secondmatch(vector<vector<Point>> cts, int xl, int xr, int yb, int first_idx)
{
    for (int k = first_idx + 1; k < (int)cts.size(); k++)
    {

        vector<Point> temp_ct = cts.at(k);
        for (int t = 0; t < (int)temp_ct.size(); t++)
        {
            Point pt = temp_ct[t];
            if (pt.x > xl && pt.x < xr && pt.y > yb + 30)
            {
                return k;
                // // drawContours(in_frame, cts, k, Scalar(255, 0, 0), FILLED);
                // Point ptStart2 = temp_ct[find_y_min_Point_index(dim_con1, temp_ct.size())];
                // Point ptEnd2 = temp_ct[find_y_max_Point_index(dim_con1, temp_ct.size())];
                // line(in_frame, ptStart2, ptEnd2, Scalar(0, 255, 0), 2);
            }
        }
    }
    return -1;
}

Point cross_point(Point ptStart1, Point ptStart2, Point ptEnd1, Point ptEnd2)
{

    //是否存在交点
    bool exist = false;
    Point cross(0, 0);
    int x = 0;
    int y = 0;
    int x1 = ptStart1.x; // 取四点坐标
    int y1 = ptStart1.y;
    int x2 = ptEnd2.x;
    int y2 = ptEnd2.y;

    int x3 = ptStart2.x; // 取四点坐标
    int y3 = ptStart2.y;
    int x4 = ptEnd1.x;
    int y4 = ptEnd1.y;
    bool k1_exist = true;
    bool k2_exist = true;
    double k1, b1, k2, b2;
    if (x2 - x1 == 0)
    {
        k1_exist = false;
    }
    else
    {
        k1 = (y2 - y1) * 1.0 / (x2 - x1); // 计算k1,由于点均为整数，需要进行浮点数转化
        b1 = y1 * 1.0 - x1 * k1 * 1.0;    // 整型转浮点型是关键
    }
    if ((x4 - x3) == 0) // L2直线斜率不存在操作
    {
        k2_exist = false;
        b2 = 0;
    }
    else
    {
        k2 = (y4 - y3) * 1.0 / (x4 - x3); // 斜率存在操作
        b2 = y3 * 1.0 - x3 * k2 * 1.0;
    }

    if (!k1_exist)
    {
        if (k2_exist)
        {
            x = x1;
            y = k2 * x1 + b2;
            exist = true;
        }
    }
    else if (!k2_exist)
    {
        x = x3;
        y = k1 * x3 + b1;
        exist = true;
    }
    else if (k2 != k1)
    {
        x = (b2 - b1) * 1.0 / (k1 - k2);
        y = k1 * x * 1.0 + b1 * 1.0;
        exist = true;
    }

    if (exist)
    {
        cross.x = x;
        cross.y = y;
    }
    else
    {
        cross.x = -1;
        cross.y = -1;
    }
    return cross;
}
