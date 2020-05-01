#include "ParaSet.h"

struct Defect {
	CvPoint front;
	CvPoint rear;
	CvPoint valley;
	double depth;
};//我看干脆不要缺陷集了8
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
	//浅拷贝，不过不改变contour，没关系
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
		//contour的序号处理好了，这里不用担心
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
	/*求最深蹼坐标*/
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

	/*构建预蹼深度数组*/
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
	int idx = (idS + 1) % N;//注意索引的循环性
	while (idx != (idE + 1) % N)
	{
		prs = contour[idx];

		/*判断历经点是否在闭R内*/
		int xsp = prs.x - Vs.x, ysp = -1 * (prs.y - Vs.y);
		int crs_hypo = xsp * yse - xse * ysp;
		int cls_hypo = crs_hypo >= 0 ? 1 : 0;//在闭R外则跳过
		if (!cls_hypo)
		{
			idx = (idx + 1) % N;
			continue;
		}

		/*历经点在闭R内且栈只含始点则压入*/
		if (length == lstart)
		{
			src.push_back(prs);
			srci.push_back(idx);
			length++;
			idx = (idx + 1) % N;
			continue;
		}

		/*根据栈顶点的凸凹性和顶历线的单调性决定顶历两点的去留*/
		top = src[length - 1], sec = src[length - 2];
		int x23 = prs.x - top.x, y23 = -1 * (prs.y - top.y);
		int x13 = prs.x - sec.x, y13 = -1 * (prs.y - sec.y);
		int x12 = top.x - sec.x, y12 = -1 * (top.y - sec.y);
		int crs_vtx = x13 * y12 - x12 * y13;
		int cl_vtx = crs_vtx >= 0 ? 1 : 0;
		//顶历向量对次顶向量泛顺，即栈顶点泛凹，则弹出栈顶点
		//注意高阶凹的现象，即删去多边形顶点后，凸顶点可能变成新多边形的凹顶点
		//因此栈次点的凸凹性尚不确定，故只弹出栈顶点而不压入历经点，且不步进
		if (cl_vtx)
		{
			src.pop_back();
			srci.pop_back();
			length--;
		}
		//顶历向量对次顶向量严逆，且顶历线单调性与斜边相同，则压入历经点
		else if (x23 * xse >= 0 && y23 * yse >= 0)
		{
			src.push_back(prs);
			srci.push_back(idx);
			length++;
			idx = (idx + 1) % N;
		}
		//顶历向量对次顶向量严逆，但顶历线单调性与斜边相反，则跳过历经点
		//由凸多边形的定义易证非单调点不属于凸包，但排除之并不是出于简便
		//而是为防止局部凸的凹多边形，即复杂多边形的出现
		else
			idx = (idx + 1) % N;
	}

	//直极值点重录，src和srci遍历到length-2即可
}

void Hull::gethull(IplImage* imhull)
{
	CvPoint Vl = contour[0], Vb = Vl, Vr = Vl, Vt = Vl;
	int idL = 0, idB = 0, idR = 0, idT = 0;
	CvPoint prs = Vl;
	for (int idx = 0; idx < N; idx++)
	{
		prs = contour[idx];
		//有重复的话选第一个
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
	/*根据轮廓创建凸包类*/
	Hull hull(contour);

	/*求凸包并绘图*/
	hull.gethull(imhull);

	/*求各缺陷并绘图*/
	hull.getdefects(imdefects);

	/*求预蹼深度数组和最深蹼*/
	hull.getw();
	vector<float> cwdpths = hull.cwdpths;
	Dweb = hull.Dweb;

	return cwdpths;
}
