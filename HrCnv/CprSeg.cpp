#include "ParaSet.h"

extern IplImage* WhtBln(IplImage* src);
extern IplImage* RgbSeg(IplImage* src);
extern IplImage* ElpsSeg(IplImage* src);

IplImage* CprSeg(IplImage* src)
{
	/*��ƽ��*/
	IplImage* wtbln = WhtBln(src);

	/*��RGB��ľ���ģ�ͽ��з�ɫ�ָ�*/
	IplImage* bnr_rgb = RgbSeg(wtbln);
#if 1
	/*�þ����ձ任��YCbCr�����Բģ�ͽ��з�ɫ�ָ�*/
	IplImage* bnr_elps = ElpsSeg(wtbln);

	/*�ϲ����ַ����ָ���ͼ��*/
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
