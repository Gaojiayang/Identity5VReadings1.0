// 2021/3/27
// ����: 1803070319����
// ͨ��RGB�Ժ���ͨ����ȡ�����м���(����Ϊ�Ҷ�ͼ��)

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
// ����Mat������м��� �ɹ�����1��ʧ�ܷ���0
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

// ����ͼƬ·�����м��أ���һ������ΪͼƬ·�����ڶ�������Ϊ�Ƿ���ʾ�����ͼƬ��
// �ɹ�����1��ʧ�ܷ���0��
int loadImg(string path, bool show)
{
	src = imread(path);
	if (!src.data)
	{
		MessageBox(NULL, _T("���ܼ���ͼƬ������ͼƬ·����"), _T("��Ϣ��ʾ"), MB_OK);
		return 0;
	}
	else {
		if (show == 1)
		{
			namedWindow("ͼƬ��ʾ", CV_WINDOW_AUTOSIZE);
			imshow("ͼƬ��ʾ", src);
		}
		src_width = src.cols;
		src_height = src.rows;
	}

	return 1;
}

// ����ʶ�𵽵ĵ�ѹ���š����Ϊ0,1,2,3������0Ϊδʶ�𵽵�ѹ��
int returnVid()
{
	// ����Mat�������ڴ洢��ͨ����λ�á�
	Mat greenPoint(src_height, src_width, CV_8UC1);

	// ͨ������Mat���󣬶�ָ��ɫ�ʷ�Χ�����ݽ�����ȡ��ת��Ϊ�ڰ׵�ͨ��ͼ��
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

	// ������:ȥ����С�İ�ɫ������
	Mat kernel1 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));		
	morphologyEx(greenPoint, greenPoint, CV_MOP_OPEN, kernel1);

	// ����:��ֹ��ͬһ��ͨ��ֿ���
	Mat structureElement2 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
	dilate(greenPoint, greenPoint, structureElement2); 

	// ���ڴ����Ƿ�ʶ�𵽵�һ�������ص�״̬��
	int isFirstGreen = 0;
	// �洢��ͨ���ĵ�һ�������һ�������ص���������
	int green_max_row = 0, green_max_col = 0, green_min_row = 0, green_min_col = 0;
	// ����ͼ���ȡ��ͨ���ĵ�һ�������һ�������ص���������
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
	// �������ɫͨ��������λ�á�
	center_y = (green_max_row + green_min_row) / 2;
	center_x = (green_max_col + green_min_col) / 2;

	// ��ʾ����λ�á�
	//circle(src, Point(center_x, center_y), 2, Scalar(255, 0, 0), 2, CV_AA);
	//imshow("src", src);

	// �ж��Ƿ�ʶ����ͨ����getVk�ж������̵��λ�ù�ϵ��
	if (green_max_col == 0)
	{
		return 0;
	}
	double getVk = (green_max_row - green_min_row) / (green_max_col - green_min_col);

	// ��ȡ��ͨ��������ɫ���������ݣ���ɫ����û�����ݣ�����ͼ������֮ǰҪת����"�ڵװ�ͼ".
	Mat labels, stats, centroids;
	int num = 0;
	connectedComponentsWithStats(greenPoint, labels, stats, centroids);
	num = centroids.rows - 1;

	// �ж�ͼ���Ƿ�������λ�á�
	if (center_x >= src_width - 200 || center_x <= 200)
	{
		return 0;
	}

	// �ж���ͨ�����Լ��̵��λ�ù�ϵȷ����ѹ���š�
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

// ���ض�ȡ���ĵ�ѹ��ֵ��
double twopointVvalue() {
	// ���ڴ洢��ͨ����λ��
	Mat src1(src_height, src_width, CV_8UC1);

	// ͨ������Mat���󣬶�ָ��ɫ�ʷ�Χ�����ݽ�����ȡ��ת��Ϊ�ڰ׵�ͨ��ͼ��
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

	// ȥ����������
	medianBlur(src1, src1, 5);

	// ������ȡ���������ص�
	//Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	//morphologyEx(src1, src1, CV_MOP_OPEN, kernel);
	// ��ʴ����
	//Mat structureElement2 = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	//erode(src1, src1, structureElement2, Point(-1, -1));
	//imshow("src1", src1);

	// �����Ƿ�ʶ�𵽵�һ������״̬
	int isFirstRed = 0;

	// �洢��ͨ���ĵ�һ�������һ�������ص���������
	double red_max_row = 0, red_max_col = 0, red_min_row = 0, red_min_col = 0;
	// ����ͼ���ȡ��ͨ���ĵ�һ�������һ�������ص���������
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
	// �ж��Ƿ�ʶ�𵽺�ͨ����red_kΪ����֮��б�ʡ�
	if (red_max_row != 0)
	{
		red_k = (red_max_row - red_min_row) / (red_max_col - red_min_col);
	}
	else
	{
		return 0;
	}
	// �ж�ֱ����ˮƽ��֮��ĽǶȡ�
	
	angle = atan(red_k) * 180 / PI;
	// ������������������ѹ��ֵ��
	if (red_k > 0)
	{
		v1 = (angle - baseAngle) / vangletotal * 5;
	}
	else
	{
		v1 = (angle - baseAngle + 180) / vangletotal * 5;
	}
	// ����С�������λ
	vv = (v1 * 100) + 0.5;
	v1 = (double)vv / 100;
	return v1;
}
