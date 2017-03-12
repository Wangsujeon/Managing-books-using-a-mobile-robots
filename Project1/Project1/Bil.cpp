#include "utilfunction.h"



Mat resize(Mat img, Mat &dst, double size) {
	Mat image = img;
	if (size < 1)
		resize(img, dst, Size((double)img.cols * size, (double)img.rows * size), 0, 0, INTER_AREA);
	else if (size >= 1)
		resize(img, dst, Size((double)img.cols * size, (double)img.rows * size), 0, 0, INTER_LINEAR);
	return dst;
}
void onChange(int pos, void *param);

int main() {
	Mat img[2];
	img[0] = imread("Image/resize/001.jpg",IMREAD_COLOR);
	img[1].create(img[0].size(), CV_8UC3);
	int pos = 3;
	onChange(pos, (void *)img);

	//imshow의 이름과 같아야지 그 창에 표시된다.
	createTrackbar("Threshold", "img", &pos, 20, onChange, (void*)img);

	waitKey();
	return 0;
}

void onChange(int pos, void* param) {
	Mat* pMat = (Mat*)param;
	Mat img_org = Mat(pMat[0]);
	Mat img_aft = Mat(pMat[1]);
	int nSize = pos;
	int s;
	s = pos;
	img_aft.setTo(Scalar::all(255));

	int border = 3;	//7x7
	Size ksize(border * 3 + 1, border * 3 + 1);
	int d = -1;
	double sigmaColor = 3.0*pos;
	double sigmaSpace = 3.0 ;
	bilateralFilter(img_org, img_aft, 3, d, sigmaColor, sigmaSpace);

	imshow("img", resize(img_aft,img_aft,0.5));
}