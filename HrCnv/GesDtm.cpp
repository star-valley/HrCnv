#include "ParaSet.h"

Gesture vori(CvPoint Dweb, curve contour);

Gesture GesDtm(vector<float> cwdpths, CvPoint Dweb, curve contour)
{
	Gesture gesture;
	int Nweb = 1;
	for (int i = 1; i < 4; i++)
	{
		if (cwdpths[i] > cwdpths[0] / 2)
		{
			Nweb++;
		}
	}

	switch (Nweb)
	{
	case 4:gesture = pause; break;
	case 3:gesture = on; break;
	case 2:gesture = confirm; break;
	case 1:gesture = vori(Dweb, contour); break;
	default:gesture = illegal;
	}

	return gesture;
}

Gesture vori(CvPoint Dweb, curve contour)
{
	int ycenter;
	int N = contour.size();
	int sum_y = 0;
	for (int idx = 0; idx < N; idx++)
		sum_y += contour[idx].y;

	ycenter = sum_y / N;

	return Dweb.y < ycenter ? up : down;
	//注意图像的像素是从上往下排的
}

