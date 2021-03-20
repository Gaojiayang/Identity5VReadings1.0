// 2021/2/6 
// 作者: 1803070319高明

#include "stdafx.h"
#include "Identify5VReadingsAPI.h"


int loadImg(Mat inputImg)
{
	src = inputImg;
	return 0;
}

int loadImg(string path , bool show)
{
	src = imread(path);
	if (!src.data)
	{
		MessageBox(NULL, _T("不能加载图片，请检查图片路径！"), _T("信息提示"), MB_OK);
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

	return 0;
}

int loadImg(string path, bool show, const cv::String winname)
{
	src = imread(path);
	if (!src.data)
	{
		MessageBox(NULL, _T("不能加载图片，请检查图片路径！"), _T("信息提示"), MB_OK);
	}
	else
	{
		imshow(winname, src);
	}
	src_width = src.cols;
	src_height = src.rows;
	return 0;
}

int returnRows()
{
	return src_height;
}

int returnCols()
{
	return src_width;
}

int returnVid()
{
	Mat dst_green, dst_green_output;
	int isFirstGreen = 0;
	double green_max_row = 0, green_max_col = 0, green_min_row = 0, green_min_col = 0;

	cvtColor(src, dst_green, CV_BGR2HSV);

	for (int row = 0; row < src_height; row++) {
		for (int col = 0; col < src_width; col++) {

			int h = dst_green.at<Vec3b>(row, col)[0];
			int s = dst_green.at<Vec3b>(row, col)[1];
			int v = dst_green.at<Vec3b>(row, col)[2];

			if (!(((h >= 120 / 2 - 30 && h <= 120 / 2 + 30))	//颜色
				&& (s >= 255 * 0.3 && s <= 255)		//饱和度
				&& (v >= 255 * 0.1 && v <= 255))) {		//亮度

				dst_green.at<Vec3b>(row, col)[0] = 0;
				dst_green.at<Vec3b>(row, col)[1] = 0;
				dst_green.at<Vec3b>(row, col)[2] = 0;
			}
		}
	}
	// 开操作
	Mat kernel1 = getStructuringElement(MORPH_RECT, Size(9, 9), Point(-1, -1));
	morphologyEx(dst_green, dst_green_output, CV_MOP_OPEN, kernel1);

	for (int row = 0; row < src_height; row++) {
		for (int col = 0; col < src_width; col++) {
			int h = dst_green_output.at<Vec3b>(row, col)[0];
			int s = dst_green_output.at<Vec3b>(row, col)[1];
			int v = dst_green_output.at<Vec3b>(row, col)[2];
			if (h == 0 && s == 0 && v == 0) {
				dst_green_output.at<Vec3b>(row, col)[0] = 255;
				dst_green_output.at<Vec3b>(row, col)[1] = 255;
				dst_green_output.at<Vec3b>(row, col)[2] = 255;
			}
			else
			{
				dst_green_output.at<Vec3b>(row, col)[0] = 0;
				dst_green_output.at<Vec3b>(row, col)[1] = 0;
				dst_green_output.at<Vec3b>(row, col)[2] = 0;
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
	// 腐蚀
	double getVk = (green_max_row - green_min_row) / (green_max_col - green_min_col);
	Mat structureElement = getStructuringElement(MORPH_RECT, Size(4, 4), Point(-1, -1));
	erode(dst_green_output, dst_green_output, structureElement);
	//imshow("1", dst_green_output);
	// 获取连通区数
	Mat labels, stats, centroids, erzhi;
	int num = 0;
	cvtColor(dst_green_output, dst_green_output, CV_RGB2GRAY);
	//threshold(dst_green_output, erzhi, 10, 240, THRESH_BINARY);
	connectedComponentsWithStats(erzhi, labels, stats, centroids);
	num = centroids.rows;

	int center_x = (green_max_col + green_min_col) / 2;
	int center_y = (green_max_row + green_min_row) / 2;

	cout << "距离左侧边界值:" << center_x << endl;
	cout << "距离上侧边界值:" << center_y << endl;
	
	if (center_x <= (returnCols() / 2 - 150) || center_x >= (returnCols() / 2 + 150))
	{
		return 0;
	}
	
	if (num == 1)
	{
		VID = 1;
	}
	else if (num == 2)
	{
		if (getVk > 0)
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

double linearfittingVvalue() {
	Mat dst, dst1;
	cvtColor(src, dst, CV_BGR2HSV);
	// 判断红色
	for (int row = 0; row < src_height; row++) {
		for (int col = 0; col < src_width; col++) {
			int h = dst.at<Vec3b>(row, col)[0];
			int s = dst.at<Vec3b>(row, col)[1];
			int v = dst.at<Vec3b>(row, col)[2];
			if (!(((h >= 0 && h <= 10) || (h >= 160 && h <= 180))
				&& (s >= 255 * 0.3 && s <= 255)
				&& (v >= 255 * 0.4 && v <= 255))) {
				dst.at<Vec3b>(row, col)[0] = 0;
				dst.at<Vec3b>(row, col)[1] = 0;
				dst.at<Vec3b>(row, col)[2] = 0;
			}
		}
	}
	// 开操作取出多余像素点
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	morphologyEx(dst, dst1, CV_MOP_OPEN, kernel);
	
	int total_row = 0;
	int total_col = 0;
	double Dxx = 0, Dxy = 0, Dyy = 0;
	int red_point = 0;
	int counter = -1;
	// 黑白操作
	for (int row = 0; row < src_height; row++) {
		for (int col = 0; col < src_width; col++) {
			int h = dst1.at<Vec3b>(row, col)[0];
			int s = dst1.at<Vec3b>(row, col)[1];
			int v = dst1.at<Vec3b>(row, col)[2];
			if (h == 0 && s == 0 && v == 0) {
				dst1.at<Vec3b>(row, col)[0] = 255;
				dst1.at<Vec3b>(row, col)[1] = 255;
				dst1.at<Vec3b>(row, col)[2] = 255;
			}
			else
			{
				dst1.at<Vec3b>(row, col)[0] = 0;
				dst1.at<Vec3b>(row, col)[1] = 0;
				dst1.at<Vec3b>(row, col)[2] = 0;
				red_point++;
				// 直线拟合法
				total_row += row;
				total_col += col;
				point[counter++] = Point(row, col);
			}
		}
	}
	
	if (counter <= 10) {
		cvtColor(src, dst, CV_BGR2HSV);
		for (int row = 0; row < src_height; row++) {
			for (int col = 0; col < src_width; col++) {
				int h = dst.at<Vec3b>(row, col)[0];
				int s = dst.at<Vec3b>(row, col)[1];
				int v = dst.at<Vec3b>(row, col)[2];
				if (!(((h >= 0 && h <= 30) || (h >= 156 && h <= 180))
					&& (s >= 255 * 0.3 && s <= 255)
					&& (v >= 255 * 0.4 && v <= 255))) {
					dst.at<Vec3b>(row, col)[0] = 0;
					dst.at<Vec3b>(row, col)[1] = 0;
					dst.at<Vec3b>(row, col)[2] = 0;
				}
			}
		}
		Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
		morphologyEx(dst, dst1, CV_MOP_OPEN, kernel);
		for (int row = 0; row < src_height; row++) {
			for (int col = 0; col < src_width; col++) {
				int h = dst1.at<Vec3b>(row, col)[0];
				int s = dst1.at<Vec3b>(row, col)[1];
				int v = dst1.at<Vec3b>(row, col)[2];
				if (h == 0 && s == 0 && v == 0) {
					dst1.at<Vec3b>(row, col)[0] = 255;
					dst1.at<Vec3b>(row, col)[1] = 255;
					dst1.at<Vec3b>(row, col)[2] = 255;
				}
				else
				{
					dst1.at<Vec3b>(row, col)[0] = 0;
					dst1.at<Vec3b>(row, col)[1] = 0;
					dst1.at<Vec3b>(row, col)[2] = 0;
					red_point++;
					total_row += row;
					total_col += col;
					point[counter++] = Point(row, col);
				}
			}
		}
	}
	
	// 直线拟合法
	total_row /= red_point;
	total_col /= red_point;
	for (int i = 0; i < red_point - 1; i++)
	{
		Dxx = (point[i].x - total_row)*(point[i].x - total_row);
		Dxy = (point[i].x - total_row)*(point[i].y - total_col);
		Dyy = (point[i].y - total_col)*(point[i].y - total_col);
	}
	double lambda = ((Dxx + Dyy) - sqrt((Dxx - Dyy) * (Dxx - Dyy) + 4 * Dxy * Dxy)) / 2.0;
	double den = sqrt(Dxy * Dxy + (lambda - Dxx) * (lambda - Dxx));
	double a = Dxy / den;
	double b = (lambda - Dxx) / den;
	double c = -a * total_row - b * total_col;
	double k2 = -b / a;	
	double angle2 = atan(k2) * 180 / PI;
	double v2;
	if (k2 > 0)
	{
		v2 = (angle2 - baseAngle) / vangletotal * 5;
	}
	else
	{
		v2 = (angle2 - baseAngle + 180) / vangletotal * 5;
	}
	int vv = (v2 * 100) + 0.5;
	v2 =(double)vv / 100;
	return v2;
}

double twopointVvalue() {
	Mat dst, dst1;
	cvtColor(src, dst, CV_BGR2HSV);
	// 判断红色
	for (int row = 0; row < src_height; row++) {
		for (int col = 0; col < src_width; col++) {
			int h = dst.at<Vec3b>(row, col)[0];
			int s = dst.at<Vec3b>(row, col)[1];
			int v = dst.at<Vec3b>(row, col)[2];
			if (!(((h >= 0 && h <= 20) || (h >= 160 && h <= 180))
				&& (s >= 255 * 0.3 && s <= 255)
				&& (v >= 255 * 0.4 && v <= 255))) {
				dst.at<Vec3b>(row, col)[0] = 0;
				dst.at<Vec3b>(row, col)[1] = 0;
				dst.at<Vec3b>(row, col)[2] = 0;
			}
		}
	}
	// 开操作取出多余像素点
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	morphologyEx(dst, dst1, CV_MOP_OPEN, kernel);

	int isFirstRed = 0;
	int red_point = 0;
	double red_max_row, red_max_col, red_min_row, red_min_col;
	// 黑白操作
	for (int row = 0; row < src_height; row++) {
		for (int col = 0; col < src_width; col++) {
			int h = dst1.at<Vec3b>(row, col)[0];
			int s = dst1.at<Vec3b>(row, col)[1];
			int v = dst1.at<Vec3b>(row, col)[2];
			if (h == 0 && s == 0 && v == 0) {
				dst1.at<Vec3b>(row, col)[0] = 255;
				dst1.at<Vec3b>(row, col)[1] = 255;
				dst1.at<Vec3b>(row, col)[2] = 255;
			}
			else
			{
				dst1.at<Vec3b>(row, col)[0] = 0;
				dst1.at<Vec3b>(row, col)[1] = 0;
				dst1.at<Vec3b>(row, col)[2] = 0;
				red_point++;
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
	if (red_max_col != red_min_col)
	{
		red_k = (red_max_row - red_min_row) / (red_max_col - red_min_col);
	}
	else
	{
		return 0;
	}
	double angle = 0;
	angle = atan(red_k) * 180 / PI;
	double v;
	if (red_k > 0)
	{
		v = (angle - baseAngle) / vangletotal * 5;
	}
	else
	{
		v = (angle - baseAngle + 180) / vangletotal * 5;
	}
	int vv = (v * 100) + 0.5;
	v = (double)vv / 100;
	return v;
}
