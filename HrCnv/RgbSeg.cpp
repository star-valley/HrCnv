#include "ParaSet.h"

IplImage* RgbSeg(IplImage* src)
{
	IplImage* bnr = cvCreateImage(cvGetSize(src), 8, 1);
	cvZero(bnr);

	int R = 2, G = 1, B = 0;

	uchar light = (uchar)255;
	uchar* prgb, * pbnr;
	for (int h = 0; h < src->height; h++)
	{
		prgb = (uchar*)src->imageData + h * src->widthStep;
		pbnr = (uchar*)bnr->imageData + h * bnr->widthStep;
		for (int w = 0; w < src->width; w++)
		{
			if (prgb[R] > 95 && prgb[G] > 40 && prgb[B] > 20 && prgb[R] - prgb[B] > 0 && prgb[R] - prgb[G] > 15)
			{
				*pbnr = light;
			}
			prgb += 3;
			pbnr++;
		}
	}

	return bnr;
}
