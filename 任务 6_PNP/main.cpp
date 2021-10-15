#include <iostream>
// #include <Eigen/Dense>
#include <opencv2/opencv.hpp>
// #include <opencv2/core/eigen.hpp>
#include <math.h>

//视频中装甲板实际参数，长67.5mm，宽：26.5mm
#define HALF_WIDTH 33.75
#define HALF_HEIGHT 13.15

using namespace cv;
using namespace std;
// using namespace Eigen;
void find_contours_index(vector<vector<Point>> cts, int *p_index_array);
int find_max_index(int a[], int n);
int find_second_index(int a[], int n, int max_index);
//寻找轮廓顶点（方便画线）
int find_y_max_Point_index(vector<Point> a, int n);
int main()
{

	FileStorage fs2("../cameraparam.yml", FileStorage::READ);
	Mat cameraMatrix2, distCoeffs2;
	fs2["camera_matrix"] >> cameraMatrix2;
	fs2["distortion_coefficients"] >> distCoeffs2;
	// cout << cameraMatrix2 << endl;
	// cout << distCoeffs2 << endl;

	VideoCapture inVideo("../装甲板_1.avi");
	double fps = 30; //每秒的帧数
	if (!inVideo.isOpened())
	{ //检查错误
		cout << "Error! \n";
		return -1;
	}
	Mat in_frame, out_gray, out_binary;
	while (true)
	{
		//读取帧（抓取并解码）以流的形式进行
		inVideo >> in_frame;

		vector<Mat> imgChannels;
		split(in_frame, imgChannels);
		//获得目标颜色图像的二值图

		Mat out_gray = imgChannels.at(2) - imgChannels.at(0);

		threshold(out_gray, out_binary, 100, 255, THRESH_BINARY);

		//闭运算，消除扇叶上可能存在的小洞
		int structElementSize = 2;
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * structElementSize + 1, 2 * structElementSize + 1), Point(structElementSize, structElementSize));
		morphologyEx(out_binary, out_binary, MORPH_CLOSE, element);

		//定义轮廓和层次结构
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		//查找轮廓

		findContours(out_binary, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

		if (contours.size() > 2)
		{
			int index_array[2];
			find_contours_index(contours, index_array);
			vector<Point> dim_con1 = contours[index_array[0]];
			Point ptStart1 = dim_con1[0];
			Point ptEnd1 = dim_con1[find_y_max_Point_index(dim_con1, dim_con1.size())];
			vector<Point> dim_con2 = contours[index_array[1]];
			Point ptStart2 = dim_con2[0];
			Point ptEnd2 = dim_con2[find_y_max_Point_index(dim_con2, dim_con2.size())];
			vector<Point2f> pnts;
			//保证pnts中的点为顺时针顺序
			if (ptStart1.x < ptStart2.x)
			{
				pnts.push_back(Point2f(ptStart1));
				pnts.push_back(Point2f(ptStart2));
				pnts.push_back(Point2f(ptEnd2));
				pnts.push_back(Point2f(ptEnd1));
			}
			else
			{
				pnts.push_back(Point2f(ptStart2));
				pnts.push_back(Point2f(ptStart1));
				pnts.push_back(Point2f(ptEnd1));
				pnts.push_back(Point2f(ptEnd2));
			}
			// //自定义装甲板的世界坐标
			// vector<Point3f> obj = vector<Point3f>{
			// 	Point3f(-HALF_WIDTH, -HALF_HEIGHT, 0), //tl
			// 	Point3f(HALF_WIDTH, -HALF_HEIGHT, 0),  //tr
			// 	Point3f(HALF_WIDTH, HALF_HEIGHT, 0),   //br
			// 	Point3f(-HALF_WIDTH, HALF_HEIGHT, 0)   //bl
			// };
			//自定义装甲板的世界坐标
			vector<Point3f> obj = vector<Point3f>{
				Point3f(0, 0, 0),		//tl
				Point3f(67.5, 0, 0),	//tr
				Point3f(67.5, 26.5, 0), //br
				Point3f(0, 26.5, 0)		//bl
			};
			Mat rVec = Mat::zeros(3, 1, CV_64FC1); //init rvec
			Mat tVec = Mat::zeros(3, 1, CV_64FC1); //init tvec

			//解算位移向量与旋转向量
			solvePnP(obj, pnts, cameraMatrix2, distCoeffs2, rVec, tVec, false, SOLVEPNP_ITERATIVE);
			cout << "tvec: " << tVec << endl;
			double rm[9];
			Mat rotMat(3, 3, CV_64FC1, rm);
			Rodrigues(rVec, rotMat);
			//旋转向量转旋转矩阵

			cout << "the distance to the center of Armor:" << sqrt(tVec.at<double>(0, 0) * tVec.at<double>(0, 0) + tVec.at<double>(1, 0) * tVec.at<double>(1, 0) + tVec.at<double>(2, 0) * tVec.at<double>(2, 0)) << endl;
			// 转换格式
			rotMat.convertTo(rotMat, CV_64FC1);
			tVec.convertTo(tVec, CV_64FC1);
			cout << "rot matrix:" << rotMat << endl;
			//根据旋转矩阵求出坐标旋转角
			float picth,yaw,roll;
			picth = atan2(rotMat.at<double>(2, 1), rotMat.at<double>(2, 2));
			yaw = atan2(-rotMat.at<double>(2, 0),
							sqrt(rotMat.at<double>(2, 1) * rotMat.at<double>(2, 1) + rotMat.at<double>(2, 2) * rotMat.at<double>(2, 2)));
			roll = atan2(rotMat.at<double>(1, 0), rotMat.at<double>(0, 0));

			//将弧度转化为角度
			picth = picth * (180 /  CV_PI);
			yaw = yaw * (180 /  CV_PI);
			roll = roll * (180 /  CV_PI);
			// // 转成Eigen下的矩阵
			// Matrix3f Rotated_matrix;
			// Vector3f Tran_vector;
			// cv2eigen(rotMat, Rotated_matrix);
			// cv2eigen(tVec, Tran_vector);
			// Vector3f euler_angles = Rotated_matrix.eulerAngles(0, 1, 2);
			// float picth,yaw,roll;
			// picth = euler_angles[0] * 180 / CV_PI;
			// yaw = euler_angles[1] * 180 / CV_PI;
			// roll = euler_angles[2] * 180 / CV_PI;
			cout<< "picth,yaw,roll:"<<picth<<","<<yaw<<","<<roll  << endl;
			// distance = (COEFF_K * sqrt(Tran_vector.transpose() * Tran_vector) + COEFF_B) * cosf(pitch * CV_PI / 180.f);
		}

		if (in_frame.empty())
			break;
		else if (waitKey(1000 / fps) >= 0)
			break;
	}

	inVideo.release();
	return 0;
}

int find_max_index(int a[], int n)
{
	int idx = 0;
	for (int i = 1; i < n; i++)
	{
		if (a[idx] < a[i])
		{
			idx = i;
		}
	}
	return idx;
}
int find_second_index(int a[], int n, int max_index)
{

	int idx = 0;
	int second_value = a[0];
	//判断数组中是否存在两个最大值。
	for (int i = 0; i < n; i++)
	{
		//当i累加到max_index时，直接跳过。
		if (i == max_index)
		{
			continue;
		}
		//对于其他非max_index元素，依次判断是否有元素与最大值相同
		else if (a[i] == a[max_index])
		{
			idx = i;
			return idx;
		}
	}
	//次大与最大不相等时
	for (int i = 1; i < n; i++)
	{
		if (a[idx] < a[i] && a[i] < a[max_index])
		{
			idx = i;
		}
	}
	return idx;
}

void find_contours_index(vector<vector<Point>> cts, int *p_index_array)
{
	int num = cts.size();
	int length_array[num];
	// 计算周长并存到length_array
	for (int i = 0; i < num; i++)
	{
		int x = arcLength(cts[i], true);
		length_array[i] = x;
	}

	// 排序并返回拥有最大周长的两个轮廓对应的下标
	// InverseBubbleSort(length_array, num);
	// array_find(length_array,num,key);
	int max_idx = find_max_index(length_array, num);
	int second_idx = find_second_index(length_array, num, max_idx);

	*p_index_array = max_idx;
	p_index_array++;
	*p_index_array = second_idx;
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