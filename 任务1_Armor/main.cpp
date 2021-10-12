#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

//寻找周长最大的两个轮廓所用函数
void BubbleSort(int arr[], int n);
int array_find(int *a, int n, int key);
int find_max_index(int a[], int n);
int find_second_index(int a[], int n, int max_index);
void find_contours_index(vector<vector<Point>> cts, int *p_index_array);

//寻找轮廓顶点（方便画线）
int find_y_max_Point_index(vector<Point> a, int n);

//画出交点（打击点）
Point cross_point(Point, Point, Point, Point);



int main(int, char **)
{
	Mat in_frame, out_gray, out_binary;
	const char win1[] = "装甲板_1.avi";
	double fps = 30; //每秒的帧数
	char file_out[] = "装甲板_1.avi";
	VideoCapture inVideo("/home/sandwich/文档/RM实习/22视觉实习任务/任务1/装甲板_1.avi");

	if (!inVideo.isOpened())
	{ //检查错误
		cout << "Error! \n";
		return -1;
	}
	//获取输入视频的宽度和高度
	int width = (int)inVideo.get(CAP_PROP_FRAME_WIDTH);
	int height = (int)inVideo.get(CAP_PROP_FRAME_HEIGHT);
	//cout << "width = " << width << ", height = " << height << endl;
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
		//cvtColor(in_frame, out_gray, COLOR_BGR2GRAY);
		// out_frame1 = out_frame1 > 150;
		vector<Mat> imgChannels;
		split(in_frame, imgChannels);
		//获得目标颜色图像的二值图

		Mat out_gray = imgChannels.at(2) - imgChannels.at(0);

		threshold(out_gray, out_binary, 100, 255, THRESH_BINARY);

		//闭运算，消除扇叶上可能存在的小洞
		int structElementSize = 2;
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * structElementSize + 1, 2 * structElementSize + 1), Point(structElementSize, structElementSize));
		morphologyEx(out_binary, out_binary, MORPH_CLOSE, element);

		//threshold(out_gray, out_binary, 200, 255, 0);
		//定义轮廓和层次结构
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		//查找轮廓

		findContours(out_binary, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
		// cout << contours[2].size() << endl;
		// cout << contours[0].size() << endl;

		if (contours.size() > 2)
		{
			int index_array[2];
			find_contours_index(contours, index_array);
			// drawContours(in_frame, contours, index_array[0], (255, 0, 255), LINE_8, 8);
			// drawContours(in_frame, contours, index_array[1], (255, 0, 255), LINE_8, 8);
			vector<Point> dim_con1 = contours[index_array[0]];
			Point ptStart1 = dim_con1[0];
			Point ptEnd1 = dim_con1[find_y_max_Point_index(dim_con1, dim_con1.size())];
			vector<Point> dim_con2 = contours[index_array[1]];
			Point ptStart2 = dim_con2[0];
			Point ptEnd2 = dim_con2[find_y_max_Point_index(dim_con2, dim_con2.size())];
			line(in_frame, ptStart1, ptEnd2, Scalar(0, 255, 0), 2);
			line(in_frame, ptStart2, ptEnd1, Scalar(0, 255, 0), 2);

			Point cross = cross_point(ptStart1, ptStart2, ptEnd1, ptEnd2);
			if (cross.x != (-1) && cross.y != (-1))
			{
				circle(in_frame, cross, 3, (255, 0, 255), 5);
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
void InverseBubbleSort(int arr[], int n)
{
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = n - 1; j > 0; j--)
		{
			if (arr[j] > arr[j - 1])
			{
				std::swap(arr[j], arr[j - 1]);
			}
		}
	}
}
// 数组的查找程序，输入数组a和要查找的元素key，若成功查找，返回对应索引。若查找不到，返回-1.
int array_find(int *a, int n, int key)
{
	for (int *p = a; p < a + n; p++)
	{
		if (*p == key)
		{
			return p - a;
		}
	}
	return -1;
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


/**
 * @brief 寻找轮廓（以周长最大来算）
 * 
 * @param cts 轮廓
 * @param *p_index_array 轮廓地址
 */
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
		b1 = y1 * 1.0 - x1 * k1 * 1.0;	  // 整型转浮点型是关键
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

// int main()
// {
//     Mat src=imread("aa.jpg");
// 	Mat grey;
// 	if(src.empty())
// 	{
// 		printf("加载失败\n");
// 		return -1;
// 	}
// 	imshow("picture",src);
// 	cvtColor(src,grey,CV_BGR2GRAY);
// 	imshow("grey",grey);
// 	waitKey(0);
// 	return 0;

// VideoCapture capture(0);    //调用摄像头
// while (1)   //循环显示每一帧
// {
//     Mat frame;  //储存一帧图像
//     capture>>frame; //读取当前帧
//     imshow("调用摄像头", frame); //显示当前读入的一帧图像
//     waitKey(10);    //延时10ms

// }
// return 0;
