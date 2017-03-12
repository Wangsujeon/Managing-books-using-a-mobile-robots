#include "utilfunction.h"
  

int main() { 

	//1920x1080 이미지에 최적화 되어 있습니다.
	Mat img_org = imread("image/resize/001.jpg", IMREAD_COLOR);
	Mat img_blur;	//블러 이미지
	Mat img_gray;	//그레이 이미지
	Mat img_mop;	//모폴로지 이미지
	Mat img_thr;	//임계값 이미지
	Mat img_sobel;	//편미분 이미지
	Mat img_bil;	//양방향 필터 이미지
	Mat img_houg;	//허프변환이미지
	Mat img_dst;	//출력 이미지
	Mat img_clone;

	utilopencv util;
	clock_t time;
	time = clock();
	if (img_org.empty())
		return -1;
	resize(img_org, img_org, Size(1920, 1080));
	util.resize(img_org, img_org, 0.5);
	imwrite("pt/img_org.jpg", img_org);
	//전체적인 이미지의 평활화를 위한 블러링
	{
		int border = 3;	//7x7
		Size ksize(border * 2 + 1, border * 2 + 1);
		boxFilter(img_org, img_dst, -1, ksize);
		//imshow("dst1", img_dst1);

		int d = ksize.width;
		double sigmaColor = 10.0 * 8;
		double sigmaSpace = 10.0 * 8;
		bilateralFilter(img_org, img_bil, d, sigmaColor, sigmaSpace);
	}
	imwrite("ppt/img_blur.jpg", img_bil);
	imshow("img_blur", img_bil);
	//기타 잡 영상을 제거하기 위한 모폴로지 연산
	util.morphology(img_bil, img_mop, 2, 3, MORPH_CLOSE);
	imwrite("ppt/img_mop1.jpg", img_mop);

	cvtColor(img_mop, img_gray, CV_BGR2GRAY);
	//imshow("img_gray", img_gray);

	//전체적인 이미지의 경계를 추출 후 모폴로지 연산으로 잡영 제거
	adaptiveThreshold(img_gray, img_thr, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 21, 5);
	imwrite("ppt/img_thr.jpg", img_thr);
	util.morphology(img_thr, img_mop, 2, 3, MORPH_CLOSE);
	imwrite("ppt/img_mop2.jpg", img_mop);

	//sobel 연산으로 x축 편미분 수행
	Sobel(img_mop, img_sobel, 0, 1, 0);
	imwrite("ppt/img_sobel1.jpg", img_sobel);

	//편미분 후 생긴 잡영 제거
	util.morphology(img_sobel, img_mop, 2, 3, MORPH_CLOSE);
	imwrite("ppt/img_mop3.jpg", img_mop);

	//대략적인 선 모양, 하나의 책 라인에 여러 선이 중첩된다.
	util.HoughLines1(img_mop, img_clone, 180);
	//imshow("lines1", img_clone);
	imwrite("ppt/img_clone.jpg", img_clone);
	imshow("imgsi", img_clone);
	waitKey();
	//중첩된 근접한 선들을 지우기 위한 모폴로지 연산
	util.morphology(img_clone, img_mop, 8, 3, MORPH_CLOSE);
	imwrite("ppt/img_mop4.jpg", img_mop);
	imshow("imgsi", img_mop);

	Sobel(img_mop, img_sobel, 0, 1, 0, 1, 1, 0, BORDER_REPLICATE);
	imwrite("ppt/img_sobel2.jpg", img_sobel);
	imshow("img", img_sobel);

	// 대략적인 위치를 추출
	vector<Point> middlep, bottomp;
	for (int i = 0; i < img_sobel.cols; i++) {
		int pos = (i);
		Vec3b* pixel = img_sobel.ptr<Vec3b>(img_sobel.rows / 2);
		if (pixel[pos][0] == 0 && pixel[pos][1] == 255 && pixel[pos][2] == 255) {
			std::cout << "middle : " << i << endl;
			circle(img_sobel, Point(pos, img_sobel.rows / 2), 2, Scalar(255, 255, 0), 3);
			middlep.push_back(Point2f(pos, img_sobel.rows / 2));
		}
	}
	for (int i = 0; i < img_sobel.cols; i++) {
		int pos = (i);
		Vec3b* pixel = img_sobel.ptr<Vec3b>(img_sobel.rows - 1);
		if (pixel[pos][0] == 0 && pixel[pos][1] == 255 && pixel[pos][2] == 255) {
			std::cout << "bottom : " << i << endl;
			circle(img_sobel, Point(pos, img_sobel.rows - 1), 2, Scalar(255, 255, 0), 3);
			bottomp.push_back(Point2f(pos, img_sobel.rows - 1));
		}
	}
	imwrite("ppt/Points.jpg", img_sobel);


	imshow("dilsob", img_sobel);
	waitKey();

	//ofstream outfile("OCRtext.txt", ios::app);
	Mat img_warp; 
	Mat img_range;
	for (int i = 0; i < middlep.size() - 1; i++) {
		vector<Point> qurdp;
		//상단에서 구한 각 좌표값을 vector qurdp에 저장, 순서 : Top+Left, Top+Right, Bottom+Left, Bottom+Right
		qurdp.push_back(middlep[i]);
		qurdp.push_back(middlep[i + 1]);
		qurdp.push_back(bottomp[i]);
		qurdp.push_back(bottomp[i + 1]);
		//적용 할 이미지와 와핑으로 저장할 Mat 변수, 좌표 지정
		img_warp.zeros(Size(0, 0), CV_16UC3);
		util.warptrans(img_org, img_warp, qurdp);
		imwrite("ppt/warp" + to_string(i) + ".jpg", img_warp);

		Scalar lowerb(170, 170, 170);
		Scalar upperb(255, 255, 255);
		inRange(img_warp, lowerb, upperb, img_range);
		img_range = ~img_range;
		Rect mark;
		Mat img_label;
		mark = util.Label(img_range, img_label, 1000);
		if (mark.area() == 0) {
			cout << "Fail is Mark detected " << endl;

		}
		img_label = ~img_label;
		imwrite("ppt/label" + to_string(i) + ".jpg", img_warp(mark));
		imshow("label", img_warp(mark));
		
		
		fstream fs;
		//fs.open("OCRText.txt", ios::out | ios::trunc);
		//이어쓰기
		fs.open("OCRText.txt", ios::out|ios::app);
		
		string ocr;
		util.OCRtext(img_warp(mark), ocr);
		fs << ocr << endl;
		fs.close();

	
	}
	 
}

