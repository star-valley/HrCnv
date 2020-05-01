#include "ParaSet.h"

int main()
{
	string HgName, index;
	cout << "Please input the HgName:" << endl;
	cin >> HgName;
	cout << "Please input the index" << endl;
	cin >> index;

	string folder = "D:\\photos_expr\\HrCnv\\";
	string ex = ".bmp";
	string file = HgName + index;
	string path = folder + file + ex;
	string path_bnr = folder + file + "_bnr" + ex;
	string path_contour = folder + file + "_contour" + ex;
	string path_hull = folder + file + "_hull" + ex;
	string path_defects = folder + file + "_defects" + ex;
	string path_result = folder + HgName + "_result" + ".txt";
	
	/*肤色分割(二值图像)->边缘检测(二值图像)
	 *->轮廓提取(点数组的数组)->轮廓筛选(点数组)
	 *->凸包描述(数组和点)->手势类别判定(枚举成员)*/

	/*读取图像*/
	IplImage* src = cvLoadImage(path.c_str(), 1);
	if (!src)
	{
		cout << "文件不存在或加载失败！您可以检查：" << endl;
		cout << "1.文件路径是否正确" << endl << "2.读取函数是否使用正常" << endl;
		return -1;
	}

	/*肤色分割*/
	IplImage* bnr = CprSeg(src);
	cvSaveImage(path_bnr.c_str(), bnr);

	/*轮廓提取*/
	IplImage* imcontour = cvCreateImage(cvGetSize(src), 8, 1);
	cvZero(imcontour);
	curve contour = ContourExtr(bnr, imcontour);

	/*凸包描述*/
	CvPoint Dweb;
	IplImage* imdefects = cvCreateImage(cvGetSize(src), 8, 3);
	cvZero(imdefects);
	IplImage* imhull = cvCloneImage(imcontour);
	vector<float> cwdpths = HullDscr(contour, Dweb, imhull, imdefects);

	/*手势判定*/
	Gesture gesture = GesDtm(cwdpths, Dweb, contour);

	/*手势名和判断结果输出*/
	string gesture_name, result_name;
	NameShow(gesture, HgName, gesture_name, result_name);

	//存储和输出部分
	cvSaveImage(path_bnr.c_str(), bnr);
	cvSaveImage(path_contour.c_str(), imcontour);
	cvSaveImage(path_hull.c_str(), imhull);
	cvSaveImage(path_defects.c_str(), imdefects);

	ofstream record;
	record.open(path_result.c_str(), ios::app);
	record << file << "：\t" << result_name << "\t" << gesture_name << endl;

	cout << "The gesture is:" << endl << gesture_name << endl;
	cout << "The result is:" << endl << result_name << endl;

	cout << "Finished" << endl;

	return 0;
}
