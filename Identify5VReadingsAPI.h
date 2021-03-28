// 作者: 1803070319高明

//  2021/3/23/  15:46


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
const double PI = 3.1415926;
const double baseAngle = 41.5;
const double vangletotal = 133 - baseAngle;	
int center_x = 0, center_y = 0;


// 加载图片
int loadImg(string path, bool show);
int loadImg(Mat inputImg);

// 识别电压表
int returnVid();

// 识别指针示数
double twopointVvalue();
