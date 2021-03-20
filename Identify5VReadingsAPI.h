
// 作者: 1803070319高明

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <time.h>
#include <windows.h>

using namespace cv;
using namespace std;

Mat src;
int src_height = 0, src_width = 0;
int VID = 0;
double red_k = 0;
clock_t loadimg, getgreen, getred;
Point point[500];
const double PI = 3.1415926;
const double baseAngle = 42.5;	//原41.0
const double vangletotal = 133 - baseAngle;		

// 加载图片
int loadImg(string path, bool show, const cv::String winname);
int loadImg(string path, bool show);
int loadImg(Mat inputImg);

int returnRows();
int returnCols();

// 识别电压表
int returnVid();

// 识别指针示数
double linearfittingVvalue();
double twopointVvalue();
