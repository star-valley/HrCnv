#include "ParaSet.h"

double YccMdf(double Y, double C, int Compo);
int PxElpsSeg(uchar Y, uchar Cb, uchar Cr);
IplImage* rgb2ycc(IplImage* rgb);

/*ElpsSeg函数，根据修正的YCbCr空间肤色聚类椭圆模型
  对目标图像进行肤色分割，并返回分割后的二值图像*/
IplImage* ElpsSeg(IplImage* src)
{
	IplImage* bnr = cvCreateImage(cvGetSize(src), 8, 1);
	cvZero(bnr);

	IplImage* ycc = cvCreateImage(cvGetSize(src), 8, 3);
	cvZero(ycc);

	ycc = rgb2ycc(src);

	int Y = 0, Cb = 1, Cr = 2;

	int skin;
	uchar light = (uchar)255;
	uchar* pycc, * pbnr;
	for (int h = 0; h < src->height; h++)
	{
		pycc = (uchar*)ycc->imageData + h * ycc->widthStep;
		pbnr = (uchar*)bnr->imageData + h * bnr->widthStep;
		for (int w = 0; w < src->width; w++)
		{
			skin = PxElpsSeg(pycc[Y], pycc[Cb], pycc[Cr]);
			if (skin)
				*pbnr = light;

			pycc += 3;
			pbnr++;
		}
		//cout<<"line"<<h<<endl;
	}

	return bnr;
}



/*PxElpsSeg函数，判断一像素是否是皮肤像素*/
int PxElpsSeg(uchar Y, uchar Cb, uchar Cr)
{
	double Cbt, Crt;
	Cbt = YccMdf((double)Y, (double)Cb, 0);
	Crt = YccMdf((double)Y, (double)Cr, 1);

	int skin;
	double x, y;
	double Cbtt, Crtt;
	double cx = 109.38, cy = 152.02;
	Cbtt = Cbt - cx;
	Crtt = Crt - cy;
	double theta = 2.53;
	x = cos(theta) * Cbtt + sin(theta) * Crtt;
	y = cos(theta) * Crtt - sin(theta) * Cbtt;

	double ecx = 1.60, ecy = 2.41;
	double xt = x - ecx, yt = y - ecy;
	double a = 25.39, b = 14.03;
	double side = (xt * xt) / (a * a) + (yt * yt) / (b * b);
	if (side <= 1.00)
		skin = 1;
	else
		skin = 0;

	return skin;
}



/*YccMdf函数，根据光照分量Y对Cb和Cr分量进行修正*/
double YccMdf(double Y, double C, int Compo)
{
	double Ct = 0.00;
	double Kl = 125.00, Kh = 188.00;
	if (Y >= Kl && Y <= Kh)
	{
		Ct = C;
		return Ct;
	}

	int CompoCb = 0, CompoCr = 1;
	double Ymin = 16.00, Ymax = 235.00;

	double center = 0.00, spread = 0.00;
	double center_Kh = 0.00;
	double W0 = 0.00, Wl = 0.00, Wh = 0.00;
	if (Compo == CompoCb)
	{
		W0 = 46.97;
		Wl = 23.00;
		Wh = 14.00;
		center_Kh = 108.00;
		if (Y < Kl)
			center = 108.00 + (Kl - Y) * (118.00 - 108.00) / (Kl - Ymin);
		else
			center = 108.00 + (Y - Kh) * (118.00 - 108.00) / (Ymax - Kh);
	}
	else
	{
		W0 = 38.76;
		Wl = 20.00;
		Wh = 10.00;
		center_Kh = 154.00;
		if (Y < Kl)
			center = 154.00 - (Kl - Y) * (154.00 - 144.00) / (Kl - Ymin);
		else
			center = 154.00 + (Y - Kh) * (154.00 - 132.00) / (Ymax - Kh);
	}

	if (Y < Kl)
		spread = Wl + (Y - Ymin) * (W0 - Wl) / (Kl - Ymin);
	else
		spread = Wh + (Ymax - Y) * (W0 - Wh) / (Ymax - Kh);

	Ct = (C - center) * W0 / spread + center_Kh;
	return Ct;
}



IplImage* rgb2ycc(IplImage* rgb)
{
	IplImage* ycc = cvCreateImage(cvGetSize(rgb), 8, 3);
	cvZero(ycc);

	int R = 2, G = 1, B = 0;
	int Y = 0, Cb = 1, Cr = 2;

	uchar* prgb, * pycc;
	double r, g, b;
	double y, cb, cr;
	for (int h = 0; h < rgb->height; h++)
	{
		prgb = (uchar*)rgb->imageData + h * rgb->widthStep;
		pycc = (uchar*)ycc->imageData + h * ycc->widthStep;
		for (int w = 0; w < rgb->width; w++)
		{
			r = (double)prgb[R];
			g = (double)prgb[G];
			b = (double)prgb[B];
#if 0
			y = 0.257 * r + 0.564 * g + 0.098 * b + 16.00;
			cb = -0.148 * r - 0.291 * g + 0.439 * b + 128.00;
			cr = 0.439 * r - 0.368 * g - 0.071 * b + 128.00;

			y = 0.299 * r + 0.587 * g + 0.114 * b;
			cb = -0.169 * r - 0.331 * g + 0.500 * b;
			cr = 0.500 * r - 0.419 * g - 0.081 * b;

			y = 0.299 * r + 0.587 * g + 0.114 * b;
			cb = 0.564 * (b - y);
			cr = 0.713 * (r - y);
#endif
			y = 0.299 * r + 0.587 * g + 0.114 * b;
			cb = -0.168736 * r - 0.331264 * g + 0.500 * b + 128.00;
			cr = 0.500 * r - 0.418688 * g - 0.081312 * b + 128.00;
			pycc[Y] = (uchar)y;
			pycc[Cb] = (uchar)cb;
			pycc[Cr] = (uchar)cr;
			prgb += 3;
			pycc += 3;
		}
	}

	return ycc;
}
