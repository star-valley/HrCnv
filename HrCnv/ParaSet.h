#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#if 0
#include "cv.h"
#include "cxcore.h"
#include "_cxcore.h"
#include "c6x.h"
#endif

#if 1
#include "cv.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
using namespace cv;
#endif

using namespace std;

typedef vector<CvPoint> curve;
typedef vector<curve> cluster;

typedef enum gesture_enum
{
	pause = 0,
	on = 1,
	up = 2,
	down = 3,
	confirm = 4,
	illegal = 5
}Gesture;
typedef enum result_enum
{
	True = 1,
	False = 0
}Result;

extern IplImage* CprSeg(IplImage* src);
extern curve ContourExtr(IplImage* edge, IplImage* imcontour);
extern vector<float> HullDscr(curve contour, CvPoint& Dweb, IplImage* imhull, IplImage* imdefects);
extern Gesture GesDtm(vector<float> cwdpths, CvPoint Dweb, curve contour);
extern void NameShow(Gesture gesture, string HgName, string& gesture_name, string& result_name);

extern void drpoint(IplImage* img, CvPoint* p);
extern void drclrpoint(IplImage* img, CvPoint* p, uchar R, uchar G, uchar B);
extern void drline(IplImage* img, CvPoint* p1, CvPoint* p2);
extern void drclrline(IplImage* img, CvPoint* p1, CvPoint* p2, uchar R, uchar G, uchar B);
extern void drclrcircle(IplImage* img, CvPoint* center, int radius, uchar R, uchar G, uchar B);
