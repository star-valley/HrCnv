#include "ParaSet.h"

struct Defect {
	CvPoint front;
	CvPoint rear;
	CvPoint valley;
	double depth;
};//�ҿ��ɴ಻Ҫȱ�ݼ���8
int mod(int a, int b)
{
	if (a >= 0)
		return a % b;
	else
		return a % b + b;
}

class Hull {
public:
	Hull(curve contour);
	void gethull(IplImage* imhull);
	void getdefects(IplImage* imdefects);
	void getw();
	vector<float> cwdpths;
	CvPoint Dweb;

private:
	int length;
	vector<CvPoint> src;
	vector<int> srci;
	vector<Defect> defects;

	int N;
	curve contour;

	vector<double> depths;
	vector<CvPoint> valleys;
	int counter;

	void part(int idS, int idE);
};

Hull::Hull(curve contour)
{
	length = 0;
	counter = 0;
	this->contour = contour;
	//ǳ�������������ı�contour��û��ϵ
	N = contour.size();
}

void Hull::getdefects(IplImage* imdefects)
{
	Defect defect;
	CvPoint front, rear, valley;
	int idf, idr;
	int width = imdefects->width, height = imdefects->height;
	for (int i = 0; i < length - 1; i++)
	{
		front = src[i];
		rear = src[i + 1];
		idf = srci[i];
		idr = srci[i + 1];
		defect.front = front;
		defect.rear = rear;
		int xfr = rear.x - front.x;
		int yfr = rear.y - front.y;
		double d = 0.0, depth = 0.0;
		valley = front;
		CvPoint prs;
		//contour����Ŵ�����ˣ����ﲻ�õ���
		for (int j = idf; j < idr; j++)
		{
			prs = contour[j];
			int xfp = prs.x - front.x;
			int yfp = prs.y - front.y;
			d = fabs(xfp * yfr - yfp * xfr) / sqrt(xfr * xfr + yfr * yfr);
			if (d > depth)
			{
				depth = d;
				valley = prs;
			}
		}
		defect.valley = valley;
		defect.depth = depth;

		defects.push_back(defect);

		if (depth > 0 && front.x % (width - 2) && front.y % (height - 2))
		{
			depths.push_back(depth);
			valleys.push_back(valley);
			counter++;

			drclrcircle(imdefects, &valley, 4, 255, 0, 0);
			drclrcircle(imdefects, &front, 4, 0, 0, 255);
			drclrcircle(imdefects, &rear, 4, 0, 0, 255);
			drclrline(imdefects, &front, &valley, 0, 255, 0);
			drclrline(imdefects, &valley, &rear, 0, 255, 0);
		}
	}
}

void Hull::getw()
{
	/*������������*/
	int N_depths = counter;
	int idxm = 0;
	int mdepth = 0;
	for (int i = 0; i < N_depths; i++)
	{
		if (mdepth < depths[i])
		{
			idxm = i;
			mdepth = depths[i];
		}
	}
	Dweb = valleys[idxm];

	/*����Ԥ���������*/
	float t;
	for (int i = 0; i < 4; i++)
	{
		for (int j = N_depths - 1; j; j--)
		{
			if (depths[j] > depths[j - 1])
			{
				t = depths[j];
				depths[j] = depths[j - 1];
				depths[j - 1] = t;
			}
		}
		cwdpths.push_back(depths[i]);
	}
}

void Hull::part(int idS, int idE)
{
	CvPoint Vs = contour[idS];
	CvPoint Ve = contour[idE];
	src.push_back(Vs);
	srci.push_back(idS);
	length++;
	int lstart = length;

	int xse = Ve.x - Vs.x;
	int yse = -1 * (Ve.y - Vs.y);
	CvPoint prs, top, sec;
	int idx = (idS + 1) % N;//ע��������ѭ����
	while (idx != (idE + 1) % N)
	{
		prs = contour[idx];

		/*�ж��������Ƿ��ڱ�R��*/
		int xsp = prs.x - Vs.x, ysp = -1 * (prs.y - Vs.y);
		int crs_hypo = xsp * yse - xse * ysp;
		int cls_hypo = crs_hypo >= 0 ? 1 : 0;//�ڱ�R��������
		if (!cls_hypo)
		{
			idx = (idx + 1) % N;
			continue;
		}

		/*�������ڱ�R����ջֻ��ʼ����ѹ��*/
		if (length == lstart)
		{
			src.push_back(prs);
			srci.push_back(idx);
			length++;
			idx = (idx + 1) % N;
			continue;
		}

		/*����ջ�����͹���ԺͶ����ߵĵ����Ծ������������ȥ��*/
		top = src[length - 1], sec = src[length - 2];
		int x23 = prs.x - top.x, y23 = -1 * (prs.y - top.y);
		int x13 = prs.x - sec.x, y13 = -1 * (prs.y - sec.y);
		int x12 = top.x - sec.x, y12 = -1 * (top.y - sec.y);
		int crs_vtx = x13 * y12 - x12 * y13;
		int cl_vtx = crs_vtx >= 0 ? 1 : 0;
		//���������Դζ�������˳����ջ���㷺�����򵯳�ջ����
		//ע��߽װ������󣬼�ɾȥ����ζ����͹������ܱ���¶���εİ�����
		//���ջ�ε��͹�����в�ȷ������ֻ����ջ�������ѹ�������㣬�Ҳ�����
		if (cl_vtx)
		{
			src.pop_back();
			srci.pop_back();
			length--;
		}
		//���������Դζ��������棬�Ҷ����ߵ�������б����ͬ����ѹ��������
		else if (x23 * xse >= 0 && y23 * yse >= 0)
		{
			src.push_back(prs);
			srci.push_back(idx);
			length++;
			idx = (idx + 1) % N;
		}
		//���������Դζ��������棬�������ߵ�������б���෴��������������
		//��͹����εĶ�����֤�ǵ����㲻����͹�������ų�֮�����ǳ��ڼ��
		//����Ϊ��ֹ�ֲ�͹�İ�����Σ������Ӷ���εĳ���
		else
			idx = (idx + 1) % N;
	}

	//ֱ��ֵ����¼��src��srci������length-2����
}

void Hull::gethull(IplImage* imhull)
{
	CvPoint Vl = contour[0], Vb = Vl, Vr = Vl, Vt = Vl;
	int idL = 0, idB = 0, idR = 0, idT = 0;
	CvPoint prs = Vl;
	for (int idx = 0; idx < N; idx++)
	{
		prs = contour[idx];
		//���ظ��Ļ�ѡ��һ��
		if (prs.x < Vl.x)
		{
			Vl = prs;
			idL = idx;
		}
		if (prs.y > Vb.y)
		{
			Vb = prs;
			idB = idx;
		}
		if (prs.x > Vr.x)
		{
			Vr = prs;
			idR = idx;
		}
		if (prs.y < Vt.y)
		{
			Vt = prs;
			idT = idx;
		}
	}

	part(idL, idB);
	part(idB, idR);
	part(idR, idT);
	part(idT, idL);

	for (int i = 0; i < length - 1; i++)
		drline(imhull, &src[i], &src[i + 1]);
}

vector<float> HullDscr(curve contour, CvPoint& Dweb, IplImage* imhull, IplImage* imdefects)
{
	/*������������͹����*/
	Hull hull(contour);

	/*��͹������ͼ*/
	hull.gethull(imhull);

	/*���ȱ�ݲ���ͼ*/
	hull.getdefects(imdefects);

	/*��Ԥ����������������*/
	hull.getw();
	vector<float> cwdpths = hull.cwdpths;
	Dweb = hull.Dweb;

	return cwdpths;
}
