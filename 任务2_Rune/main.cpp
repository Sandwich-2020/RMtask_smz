/**
 * @file main.cpp
 * @author Sandwich (1293716870@qq.com)
 * @brief 
 * @version 0.1
 * @date 2021-10-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>
#include <chrono>
#include <typeinfo>
using namespace cv;
using namespace cv::ml;
using namespace std;
double getDistance(Point A, Point B);
double TemplateMatch(cv::Mat image, cv::Mat tepl, cv::Point &point, int method);
int main(int, char **)
{
	Mat in_frame, out_gray;
	const char win[] = "Task_2";
	double fps = 30; //每秒的帧数
	char file_out[] = "Task_2.avi";

	VideoCapture inVid("../in/能量机关_1.avi");

	if (!inVid.isOpened())
	{ //检查错误
		cout << "Error! \n";
		return -1;
	}
	//获取输入视频的宽度和高度
	int width = (int)inVid.get(CAP_PROP_FRAME_WIDTH);
	int height = (int)inVid.get(CAP_PROP_FRAME_HEIGHT);
	//cout << "width = "<<width<<", height = "<<height<<endl;
	VideoWriter recVid(file_out, VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, Size(width, height), 0);

	//创建窗口
	namedWindow(win);
	while (true)
	{
		//读取帧（抓取并解码）以流的形式进行
		inVid >> in_frame;
		//将帧转换为灰度
		//cvtColor(in_frame, out_gray, COLOR_BGR2GRAY);
		vector<Mat> imgChannels;
		split(in_frame, imgChannels);
		//获得目标颜色图像的二值图

		Mat midImage2 = imgChannels.at(0) - imgChannels.at(2);

		threshold(midImage2, midImage2, 100, 255, THRESH_BINARY);

		//闭运算，消除扇叶上可能存在的小洞
		int structElementSize = 2;
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * structElementSize + 1, 2 * structElementSize + 1), Point(structElementSize, structElementSize));
		morphologyEx(midImage2, midImage2, MORPH_CLOSE, element);

		//查找轮廓
		vector<vector<Point>> contours2;
		vector<Vec4i> hierarchy2;
		findContours(midImage2, contours2, hierarchy2, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

		// int index = 0;
		// for (; index >= 0; index = hierarchy2[index][0])
		// {
		// 	Scalar color(rand() & 255, rand() & 255, rand() & 255);
		// 	drawContours(in_frame, contours2, index, color, FILLED, 8, hierarchy2);
		// }
		RotatedRect rect_tmp2;
		bool findTarget = 0;
		//遍历轮廓
		if (hierarchy2.size())
			for (int i = 0; i >= 0; i = hierarchy2[i][0])
			{
				//找出轮廓的最小外接矩形
				rect_tmp2 = minAreaRect(contours2[i]);
				//将矩形的四个点保存在P中
				Point2f P[4];
				rect_tmp2.points(P);

				Point2f srcRect[4];
				Point2f dstRect[4];

				double width;
				double height;

				//矫正提取的叶片的宽高
				width = getDistance(P[0], P[1]);
				height = getDistance(P[1], P[2]);
				if (width > height)
				{
					srcRect[0] = P[0];
					srcRect[1] = P[1];
					srcRect[2] = P[2];
					srcRect[3] = P[3];
				}
				else
				{
					swap(width, height);
					srcRect[0] = P[1];
					srcRect[1] = P[2];
					srcRect[2] = P[3];
					srcRect[3] = P[0];
				}

				//Scalar color(rand() & 255, rand() & 255, rand() & 255);
				//drawContours(in_frame, contours2, i, color, 4, 8, hierarchy2);

				//通过面积筛选
				double area = height * width;
				if (area > 5000 && area < 6200)
				{
					//对矩形顶点进行赋值
					dstRect[0] = Point2f(0, 0);
					dstRect[1] = Point2f(width, 0);
					dstRect[2] = Point2f(width, height);
					dstRect[3] = Point2f(0, height);
					// 应用透视变换，矫正成规则矩形
					Mat transform = getPerspectiveTransform(srcRect, dstRect);
					Mat perspectMat;
					warpPerspective(midImage2, perspectMat, transform, midImage2.size());

					//imshow("warpdst", perspectMat);

					// 提取扇叶图片
					Mat testim;
					testim = perspectMat(Rect(0, 0, width, height));
					// int cnnt = 0;
					// //用于保存扇叶图片，以便接下来训练svm
					// int tem = 0;
					// tem++;
					// string s = "leaf" + to_string(tem) + ".jpg";
					// imwrite("/home/sandwich/文档/学习/task_2/img/" + s, testim);
					// imshow("testim", testim);

					
					

					//if (testim.empty())
					//{
					//	cout << "filed open" << endl;
					//	return -1;
					//}

					cv::Point matchLoc;
					double value;
					Mat tmp1;
					resize(testim, tmp1, Size(42, 20));

					//imshow("temp1", tmp1);

					vector<double> Vvalue1;
					vector<double> Vvalue2;
					Mat templ[9];



					for (int i = 1; i <= 8; i++)
					{
						templ[i] = imread("../template/template" + to_string(i) + ".jpg", IMREAD_GRAYSCALE);
					}
					//模板匹配
					for (int j = 1; j <= 6; j++)
					{
						value = TemplateMatch(tmp1, templ[j], matchLoc, TM_CCOEFF_NORMED);
						Vvalue1.push_back(value);
					}
					for (int j = 7; j <= 8; j++)
					{
						value = TemplateMatch(tmp1, templ[j], matchLoc, TM_CCOEFF_NORMED);
						Vvalue2.push_back(value);
					}
					int maxv1 = 0, maxv2 = 0;

					for (int t1 = 0; t1 < 6; t1++)
					{
						if (Vvalue1[t1] > Vvalue1[maxv1])
						{
							maxv1 = t1;
						}
					}
					for (int t2 = 0; t2 < 2; t2++)
					{
						if (Vvalue2[t2] > Vvalue2[maxv2])
						{
							maxv2 = t2;
						}
					}

					// cout << Vvalue1[maxv1] << endl;
					// 	cout << Vvalue2[maxv2] << endl;

					//转化为svm所要求的格式
					//Mat test = get(testim);

					//预测是否是要打击的扇叶

					if (Vvalue1[maxv1] > Vvalue2[maxv2] && Vvalue1[maxv1] > 0.6)

					

					{
						findTarget = true;
						//查找装甲板
						if (hierarchy2[i][2] >= 0)
						{
							RotatedRect rect_tmp = minAreaRect(contours2[hierarchy2[i][2]]);
							Point2f Pnt[4];
							rect_tmp.points(Pnt);
							const float maxHWRatio =1;
							const float maxArea = 2000;
							const float minArea = 600;

							float width = rect_tmp.size.width;
							float height = rect_tmp.size.height;
							if (height > width)
								swap(height, width);
							float area = width * height;

							if (height / width > maxHWRatio || area > maxArea || area < minArea)
							{
								continue;
							}
							Point centerP = rect_tmp.center;
						
							
							for (int j = 0; j < 4; ++j)
							{
								line(in_frame, Pnt[j], Pnt[(j + 1) % 4],Scalar(0, 0, 255), 4);
							}
						}
					}
				}
			}

		//将幀写入视频文件（编码并保存）以流的形式进行
		recVid << in_frame;
		imshow(win, in_frame); // 在窗口中显示帧

		if (waitKey(1000 / fps) >= 0)
			break;
	}

	return 0;
}

//获取点间距离
double getDistance(Point A, Point B)
{
	double dis;
	dis = pow((A.x - B.x), 2) + pow((A.y - B.y), 2);
	return sqrt(dis);
}

//模板匹配
double TemplateMatch(cv::Mat image, cv::Mat tepl, cv::Point &point, int method)
{
	int result_cols = image.cols - tepl.cols + 1;
	int result_rows = image.rows - tepl.rows + 1;
	//    cout <<result_cols<<" "<<result_rows<<endl;
	cv::Mat result = cv::Mat(result_cols, result_rows, CV_32FC1);
	cv::matchTemplate(image, tepl, result, method);
        //最小位置指针和最大位置指针
	double minVal, maxVal;
	
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	switch (method)
	{
	case TM_SQDIFF:
	case TM_SQDIFF_NORMED:
		point = minLoc;
		return minVal;

	default:
		point = maxLoc;
		return maxVal;
	}
}
