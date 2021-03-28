// 2021/3/27
// 作者: 1803070319高明
// 通过RGB对红绿通道提取并进行计算(处理为灰度图像)

#include "stdafx.h"
#include "Identify5VReadingsAPI.h"


int row = 0;
int col = 0;
int v = 0;
int b;
int g;
int r;
double v1;
double angle = 0;
int vv;
// 传入Mat对象进行加载 成功返回1，失败返回0
int loadImg(Mat inputImg)
{
	if (inputImg.data)
	{
		inputImg.copyTo(src);
		src_height = src.rows;
		src_width = src.cols;
		return 1;
	}
	else
	{
		return 0;
	}
}

// 传入图片路径进行加载，第一个参数为图片路径，第二个参数为是否显示传入的图片。
// 成功返回1，失败返回0。
int loadImg(string path, bool show)
{
	src = imread(path);
	if (!src.data)
	{
		MessageBox(NULL, _T("不能加载图片，请检查图片路径！"), _T("信息提示"), MB_OK);
		return 0;
	}
	else {
		if (show == 1)
		{
			namedWindow("图片显示", CV_WINDOW_AUTOSIZE);
			imshow("图片显示", src);
		}
		src_width = src.cols;
		src_height = src.rows;
	}

	return 1;
}

// 返回识别到的电压表表号。结果为0,1,2,3。返回0为未识别到电压表。
int returnVid()
{
	// 声明Mat对象用于存储绿通道的位置。
	Mat greenPoint(src_height, src_width, CV_8UC1);

	// 通过遍历Mat对象，对指定色彩范围的数据进行提取，转化为黑白单通道图像。
	for ( row = 0; row < src_height; row++) {
		for ( col = 0; col < src_width; col++) {

			 b = src.at<Vec3b>(row, col)[0];
			 g = src.at<Vec3b>(row, col)[1];
			 r = src.at<Vec3b>(row, col)[2];
			
			if (!((r > 70) && (r < 150) && (g - b > 30)))
			{
				greenPoint.at<uchar>(row, col) = 0;
			}
			else
			{
				greenPoint.at<uchar>(row, col) = 255;
			}
		}
	}

	// 开操作:去除较小的白色噪音。
	Mat kernel1 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));		
	morphologyEx(greenPoint, greenPoint, CV_MOP_OPEN, kernel1);

	// 膨胀:防止将同一连通域分开。
	Mat structureElement2 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
	dilate(greenPoint, greenPoint, structureElement2); 

	// 用于储存是否识别到第一个绿像素的状态。
	int isFirstGreen = 0;
	// 存储绿通道的第一个、最后一个绿像素的行列数。
	int green_max_row = 0, green_max_col = 0, green_min_row = 0, green_min_col = 0;
	// 遍历图像获取绿通道的第一个、最后一个绿像素的行列数。
	for ( row = 0; row < src_height; row++) {
		for ( col = 0; col < src_width; col++) {
			 v = greenPoint.at<uchar>(row, col);
			if (v == 0) {
				greenPoint.at<uchar>(row, col) = 0;
			}
			else
			{
				greenPoint.at<uchar>(row, col) = 255;
				if (isFirstGreen == 0)
				{
					green_min_row = row;
					green_min_col = col;
					isFirstGreen = 1;
				}
				else
				{
					green_max_row = row;
					green_max_col = col;
				}
			}
		}
	}
	// 计算出绿色通道的中心位置。
	center_y = (green_max_row + green_min_row) / 2;
	center_x = (green_max_col + green_min_col) / 2;

	// 显示中心位置。
	//circle(src, Point(center_x, center_y), 2, Scalar(255, 0, 0), 2, CV_AA);
	//imshow("src", src);

	// 判断是否识别到绿通道，getVk判断两个绿点的位置关系。
	if (green_max_col == 0)
	{
		return 0;
	}
	double getVk = (green_max_row - green_min_row) / (green_max_col - green_min_col);

	// 获取连通区数。白色代表有数据，黑色代表没有数据，所以图像输入之前要转换成"黑底白图".
	Mat labels, stats, centroids;
	int num = 0;
	connectedComponentsWithStats(greenPoint, labels, stats, centroids);
	num = centroids.rows - 1;

	// 判断图像是否处于中心位置。
	if (center_x >= src_width - 200 || center_x <= 200)
	{
		return 0;
	}

	// 判断连通域数以及绿点的位置关系确定电压表表号。
	if (num == 1)
	{
		VID = 1;
	}
	else if (num == 2)
	{
		if (getVk < 0)
		{
			VID = 2;
		}
		else
		{
			VID = 3;
		}
	}
	else
	{
		VID = 0;
	}
	return VID;
}

// 返回读取到的电压表值。
double twopointVvalue() {
	// 用于存储红通道的位置
	Mat src1(src_height, src_width, CV_8UC1);

	// 通过遍历Mat对象，对指定色彩范围的数据进行提取，转化为黑白单通道图像。
	for ( row = 0; row < src_height; row++) {
		for ( col = 0; col < src_width; col++) {
			 b = src.at<Vec3b>(row, col)[0];
			 g = src.at<Vec3b>(row, col)[1];
			 r = src.at<Vec3b>(row, col)[2];
			if (!(((b <= 120))		//120
				&& (g <= 115)		//115
				&& (r >= 200))) {  //200
				src1.at<uchar>(row, col) = 0;
			}
			else
			{
				src1.at<uchar>(row, col) = 255;
			}
		}
	}

	// 去除椒盐噪声
	medianBlur(src1, src1, 5);

	// 开操作取出多余像素点
	//Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	//morphologyEx(src1, src1, CV_MOP_OPEN, kernel);
	// 腐蚀操作
	//Mat structureElement2 = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	//erode(src1, src1, structureElement2, Point(-1, -1));
	//imshow("src1", src1);

	// 储存是否识别到第一个红点的状态
	int isFirstRed = 0;

	// 存储绿通道的第一个、最后一个绿像素的行列数。
	double red_max_row = 0, red_max_col = 0, red_min_row = 0, red_min_col = 0;
	// 遍历图像获取绿通道的第一个、最后一个绿像素的行列数。
	for ( row = 0; row < src_height; row++) {
		for ( col = 0; col < src_width; col++) {
			 v = src1.at<uchar>(row, col);
			if (v == 0) {
				src1.at<uchar>(row, col) = 255;
			}
			else
			{
				src1.at<uchar>(row, col) = 0;
				if (isFirstRed == 0)
				{
					red_min_row = row;
					red_min_col = col;
					isFirstRed = 1;
				}
				else
				{
					red_max_row = row;
					red_max_col = col;
				}
			}
		}
	}
	// 判断是否识别到红通道，red_k为两点之间斜率。
	if (red_max_row != 0)
	{
		red_k = (red_max_row - red_min_row) / (red_max_col - red_min_col);
	}
	else
	{
		return 0;
	}
	// 判断直线与水平线之间的角度。
	
	angle = atan(red_k) * 180 / PI;
	// 分两种情况讨论求出电压表值。
	if (red_k > 0)
	{
		v1 = (angle - baseAngle) / vangletotal * 5;
	}
	else
	{
		v1 = (angle - baseAngle + 180) / vangletotal * 5;
	}
	// 返回小数点后两位
	vv = (v1 * 100) + 0.5;
	v1 = (double)vv / 100;
	return v1;
}
