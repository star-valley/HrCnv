#include "ParaSet.h"

CvPoint neb(CvPoint center, int ori)
{
	CvPoint neighbor;
	int dx = 0, dy = 0;
	switch (ori)
	{
	case 0:dx = 1, dy = 0; break;
	case 1:dx = 1, dy = 1; break;
	case 2:dx = 0, dy = 1; break;
	case 3:dx = -1, dy = 1; break;
	case 4:dx = -1, dy = 0; break;
	case 5:dx = -1, dy = -1; break;
	case 6:dx = 0, dy = -1; break;
	case 7:dx = 1, dy = -1; break;
	default:cout << "orint code illegal!" << endl;
	}
	neighbor.x = center.x + dx;
	neighbor.y = center.y - dy;

	return neighbor;
}

int next(int fori)
{
	int pori;
	//����˳ʱ����ת90�㣬ż��˳ʱ����ת45��
	if (fori % 2)
		pori = (fori + 6) % 8;
	else
		pori = (fori + 7) % 8;
	return pori;
}

uchar at(IplImage* img, CvPoint p)
{
	int width = img->width, height = img->height;
	int x = p.x, y = p.y;
	if (x < 0 || x > width - 1 || y < 0 || y > height - 1)
	{
		cout << "at error!" << endl;
		return -1;
	}
	uchar* pointer = (uchar*)img->imageData + y * img->widthStep + x;
	uchar value = *pointer;
	return value;
}

void assign(IplImage* img, CvPoint p, uchar value)
{
	int width = img->width, height = img->height;
	int x = p.x, y = p.y;
	if (x < 0 || x > width - 1 || y < 0 || y > height - 1)
	{
		cout << "assign error!" << endl;
		return;
	}
	uchar* pointer = (uchar*)img->imageData + y * img->widthStep + x;
	*pointer = value;
}

int isframe(IplImage* img, CvPoint p)
{
	int width = img->width, height = img->height;
	int jd = !(p.x % (width - 1)) || !(p.y % (height - 1));
	return jd;
}

int isbp(IplImage* img, CvPoint p)
{
	if (!at(img, p))
		return 0;
	if (isframe(img, p))
		return 1;
	int jdsr = 0;
	//��4-��ͨ����߽��
	for (int ori = 0; ori < 8; ori += 2)
	{
		if (!at(img, neb(p, ori)))
		{
			jdsr = 1;
			break;
		}
	}
	return jdsr;
}

int offframe(IplImage* img, CvPoint p)
{
	int width = img->width, height = img->height;
	int jd = p.x < 0 || p.x > width - 1 || p.y < 0 || p.y > height - 1;
	return jd;
}

curve ContourExtr(IplImage* bnr,IplImage* imcontour)
{
	/*��ͼ�������̬ѧ���Ͳ���*/
	IplImage* img = cvCreateImage(cvGetSize(bnr), 8, 1);
	cvZero(img);
	cvDilate(bnr, img, 0, 1);

	/*��������*/
	curve contour;
	int Mreca = 0;
	int mark = 1;
	uchar* pimg = (uchar*)img->imageData;
	int xs = 0, ys = 0;
	//���ܴ��ڷǼ򵥱�����
	//����ѧ������ѧ.jpg
	while (1)
	{
		curve contour_prs;
		CvPoint p;
		p.x = 0;
		p.y = 0;
		int exi = 0;

		/*Ѱ������������ʼ��*/
		int scan = 1;
		int w = xs, h = ys;
		//�������������Ϊ���������Ч��
		for (w = xs; w < img->width; w++)
		{
			if (scan)
			{
				if (*pimg == (uchar)255)
				{
					p.x = w;
					p.y = ys;
					exi = 1;
					break;
				}
				else
					scan = !(*pimg == 129);
			}
			else
				scan = *pimg == 129;
			pimg++;
		}
		if (!exi)
		{
			for (h = ys + 1; h < img->height; h++)
			{
				pimg = (uchar*)img->imageData + h * img->widthStep;
				for (int w = 0; w < img->width; w++)
				{
					if (scan)
					{
						if (*pimg == (uchar)255)
						{
							p.x = w;
							p.y = h;
							exi = 1;
							break;
						}
						else
							scan = !(*pimg == 129);
					}
					else
						scan = *pimg == 129;
					pimg++;
				}
				if (exi)
					break;
			}
		}

		//���Ҳ�����ʼ�㣬�������������Ѽ������ֹ�������
		if (!exi)
			break;

		/*����������*/
		int ori = 7;
		int ori_prs;
		int lin, rin, horid;
		CvPoint p_prs;
		xs = p.x, ys = p.y;
		int closed = 0;
		int xm = xs, xM = xs, ym = ys, yM = ys;
		int rectarea;
		while (!closed)
		{
			contour_prs.push_back(p);
			xm = p.x < xm ? p.x : xm;
			xM = p.x > xM ? p.x : xM;
			ym = p.y < ym ? p.y : ym;
			yM = p.y > yM ? p.y : yM;
			p_prs = p;
			ori_prs = ori;

			/*ȷ����һ���ٷ������һ���ٵ�*/
			ori = next(ori);
			while (offframe(img, neb(p, ori)) || (!isbp(img, neb(p, ori))))
				ori = (ori + 1) % 8;
			p = neb(p, ori);

			/*��¼��ֵ�*/
			if (ori_prs == 0 || ori == 4)
				lin = 1;
			else
				lin = (ori + 4) % 8 > ori_prs;
			if (ori_prs == 4 || ori == 0)
				rin = 1;
			else
				rin = ori > (ori_prs + 4) % 8;
			horid = lin ^ rin;
			if (at(img, p_prs) == 255)
				assign(img, p_prs, (uchar)(mark + 128 * horid));
			else if (at(img, neb(p_prs, 0)) || at(img, neb(p_prs, 4)))
				assign(img, p_prs, (uchar)(mark + 128 * 1));
			else
				assign(img, p_prs, (uchar)(mark));

			/*���������ٵ���ֹ����Ϊ�����պ�*/
			if (p.x == xs && p.y == ys)
			{
				if (ori == 4)
					assign(img, p, (uchar)(mark + 128 * 1));
				else
					assign(img, p, (uchar)(mark));
				closed = 1;
			}

		}

		//����ɸѡ
		rectarea = (xM - xm) * (yM - ym);
		if (rectarea > Mreca)
		{
			Mreca = rectarea;
			contour = contour_prs;
		}
	}

	/*��������*/
	int N = contour.size();
	CvPoint pdr;
	for (int i = 0; i < N; i++)
	{
		pdr = contour[i];
		drpoint(imcontour, &pdr);
	}

	return contour;
}

//ÿ�к������������ֵ㣬������ı߽�㣬�򲦶�����
//������߽�㲻�����ؿ����ڵ㡢���ͱ߽��Ķ���ó�
