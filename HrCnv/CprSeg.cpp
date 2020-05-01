#include "ParaSet.h"

extern IplImage* WhtBln(IplImage* src);
extern IplImage* RgbSeg(IplImage* src);
extern IplImage* ElpsSeg(IplImage* src);

IplImage* CprSeg(IplImage* src)
{
	/*白平衡*/
	IplImage* wtbln = WhtBln(src);

	/*用RGB域的矩形模型进行肤色分割*/
	IplImage* bnr_rgb = RgbSeg(wtbln);
#if 1
	/*用经光照变换的YCbCr域的椭圆模型进行肤色分割*/
	IplImage* bnr_elps = ElpsSeg(wtbln);

	/*合并两种方法分割后的图像*/
	IplImage* bnr_cpr = cvCreateImage(cvGetSize(src), 8, 1);
	cvZero(bnr_cpr);

	uchar light = (uchar)255;
	uchar* pbe, * pbr, * pb;
	for (int h = 0; h < bnr_rgb->height; h++)
	{
		pbe = (uchar*)bnr_elps->imageData + h * bnr_elps->widthStep;
		pbr = (uchar*)bnr_rgb->imageData + h * bnr_rgb->widthStep;
		pb = (uchar*)bnr_cpr->imageData + h * bnr_cpr->widthStep;
		for (int w = 0; w < bnr_rgb->width; w++)
		{
			if ((*pbe) || (*pbr))
				*pb = light;
			pbe++;
			pbr++;
			pb++;
		}
	}

	return bnr_cpr;
#endif

	//return bnr_rgb;
}
