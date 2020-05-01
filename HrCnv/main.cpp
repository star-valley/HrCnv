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
	
	/*��ɫ�ָ�(��ֵͼ��)->��Ե���(��ֵͼ��)
	 *->������ȡ(�����������)->����ɸѡ(������)
	 *->͹������(����͵�)->��������ж�(ö�ٳ�Ա)*/

	/*��ȡͼ��*/
	IplImage* src = cvLoadImage(path.c_str(), 1);
	if (!src)
	{
		cout << "�ļ������ڻ����ʧ�ܣ������Լ�飺" << endl;
		cout << "1.�ļ�·���Ƿ���ȷ" << endl << "2.��ȡ�����Ƿ�ʹ������" << endl;
		return -1;
	}

	/*��ɫ�ָ�*/
	IplImage* bnr = CprSeg(src);
	cvSaveImage(path_bnr.c_str(), bnr);

	/*������ȡ*/
	IplImage* imcontour = cvCreateImage(cvGetSize(src), 8, 1);
	cvZero(imcontour);
	curve contour = ContourExtr(bnr, imcontour);

	/*͹������*/
	CvPoint Dweb;
	IplImage* imdefects = cvCreateImage(cvGetSize(src), 8, 3);
	cvZero(imdefects);
	IplImage* imhull = cvCloneImage(imcontour);
	vector<float> cwdpths = HullDscr(contour, Dweb, imhull, imdefects);

	/*�����ж�*/
	Gesture gesture = GesDtm(cwdpths, Dweb, contour);

	/*���������жϽ�����*/
	string gesture_name, result_name;
	NameShow(gesture, HgName, gesture_name, result_name);

	//�洢���������
	cvSaveImage(path_bnr.c_str(), bnr);
	cvSaveImage(path_contour.c_str(), imcontour);
	cvSaveImage(path_hull.c_str(), imhull);
	cvSaveImage(path_defects.c_str(), imdefects);

	ofstream record;
	record.open(path_result.c_str(), ios::app);
	record << file << "��\t" << result_name << "\t" << gesture_name << endl;

	cout << "The gesture is:" << endl << gesture_name << endl;
	cout << "The result is:" << endl << result_name << endl;

	cout << "Finished" << endl;

	return 0;
}
