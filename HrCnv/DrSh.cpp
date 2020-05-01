#include "ParaSet.h"

void drpoint(IplImage* img, CvPoint* p)
{
	if (img->nChannels != 1)
	{
		cout << "输出图像不是灰度图！" << endl;
		return;
	}
	uchar* pimg;
	uchar light = (uchar)255;
	int x = p->x;
	int y = p->y;
	pimg = (uchar*)img->imageData + y * img->widthStep + x;
	*pimg = light;
}

void drclrpoint(IplImage* img, CvPoint* p, uchar R, uchar G, uchar B)
{
	if (img->nChannels != 3)
	{
		cout << "输入图象个不是三通道RGB图！" << endl;
		return;
	}
	uchar* pimg;
	int iR = 2, iG = 1, iB = 0;
	int x = p->x;
	int y = p->y;
	pimg = (uchar*)img->imageData + y * img->widthStep + 3 * x;
	pimg[iR] = R;
	pimg[iG] = G;
	pimg[iB] = B;
}

void drline(IplImage* img, CvPoint* p1, CvPoint* p2)
{
	if (img->nChannels != 1)
	{
		cout << "输出图像不是灰度图！" << endl;
		return;
	}
	CvPoint* p = new CvPoint;
	int x1 = p1->x, y1 = p1->y;
	int x2 = p2->x, y2 = p2->y;
	int xmin = x1 < x2 ? x1 : x2;
	int xmax = x1 > x2 ? x1 : x2;
	int ymin = y1 < y2 ? y1 : y2;
	int ymax = y1 > y2 ? y1 : y2;
	double dx = x1 - x2, dy = y1 - y2;
	if (x1 == x2 && y1 == y2)
	{
		drpoint(img, p1);
		return;
	}

	//若x差距大从x，即遍历x
	if (fabs(dx) > fabs(dy))
	{
		double k = dy / dx;
		for (int x = xmin; x <= xmax; x++)
		{
			p->x = x;
			p->y = y1 + k * (x - x1);
			drpoint(img, p);
		}
	}
	else
	{
		double m = dx / dy;
		for (int y = ymin; y <= ymax; y++)
		{
			p->y = y;
			p->x = x1 + m * (y - y1);
			drpoint(img, p);
		}
	}
}

void drclrline(IplImage* img, CvPoint* p1, CvPoint* p2, uchar R, uchar G, uchar B)
{
	if (img->nChannels != 3)
	{
		cout << "输入图象个不是三通道RGB图！" << endl;
		return;
	}
	CvPoint* p = new CvPoint;
	int x1 = p1->x, y1 = p1->y;
	int x2 = p2->x, y2 = p2->y;
	int xmin = x1 < x2 ? x1 : x2;
	int xmax = x1 > x2 ? x1 : x2;
	int ymin = y1 < y2 ? y1 : y2;
	int ymax = y1 > y2 ? y1 : y2;
	double dx = x1 - x2, dy = y1 - y2;
	if (x1 == x2 && y1 == y2)
	{
		drclrpoint(img, p1, R, G, B);
		return;
	}

	//若x差距大从x，即遍历x
	if (fabs(dx) > fabs(dy))
	{
		double k = dy / dx;
		for (int x = xmin; x <= xmax; x++)
		{
			p->x = x;
			p->y = y1 + k * (x - x1);
			drclrpoint(img, p, R, G, B);
		}
	}
	else
	{
		double m = dx / dy;
		for (int y = ymin; y <= ymax; y++)
		{
			p->y = y;
			p->x = x1 + m * (y - y1);
			drclrpoint(img, p, R, G, B);
		}
	}
}

void drclrcircle(IplImage* img, CvPoint* center, int radius, uchar R, uchar G, uchar B)
{
	if (img->nChannels != 3)
	{
		cout << "输入图象个不是三通道RGB图！" << endl;
		return;
	}
	CvSize size = cvGetSize(img);
	CvPoint* p = new CvPoint;
	int x, y;
	for (int dx = -1 * radius; dx <= radius; dx++)
	{
		for (int dy = -1 * radius; dy <= radius; dy++)
		{
			x = center->x + dx;
			y = center->y + dy;
			if (x < 0 || x >= size.width || y < 0 || y >= size.height)
				continue;
			if (dx * dx + dy * dy <= radius * radius)
			{
				p->x = x;
				p->y = y;
				drclrpoint(img, p, R, G, B);
			}
		}
	}
}
