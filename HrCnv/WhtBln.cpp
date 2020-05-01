#include "ParaSet.h"

IplImage* WhtBln(IplImage* src)
{
	IplImage* bln = cvCreateImage(cvGetSize(src), 8, 3);
	cvZero(bln);

	int R = 2, G = 1, B = 0;
	int Npx = 0;
	double avR = 0.00, avG = 0.00, avB = 0.00;
	uchar* prgb;
	for (int h = 0; h < src->height; h++)
	{
		prgb = (uchar*)src->imageData + h * src->widthStep;
		for (int w = 0; w < src->width; w++)
		{
			avR += (double)prgb[R];
			avG += (double)prgb[G];
			avB += (double)prgb[B];
			Npx++;
			prgb += 3;
		}
	}
	avR /= (double)Npx;
	avG /= (double)Npx;
	avB /= (double)Npx;

	double K = (avR + avG + avB) / 3;
	double Kr = K / avR;
	double Kg = K / avG;
	double Kb = K / avB;

	uchar* psrc, * pbln;
	for (int h = 0; h < src->height; h++)
	{
		psrc = (uchar*)src->imageData + h * src->widthStep;
		pbln = (uchar*)bln->imageData + h * bln->widthStep;
		for (int w = 0; w < src->width; w++)
		{
			pbln[R] = (uchar)(Kr * psrc[R]);
			pbln[G] = (uchar)(Kg * psrc[G]);
			pbln[B] = (uchar)(Kb * psrc[B]);
			psrc += 3;
			pbln += 3;
		}
	}

	return bln;
}
