#pragma once
#include <api/baseapi.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <time.h>	 

using namespace std;
using namespace cv;

class utilopencv {
public:
	utilopencv();
	virtual ~utilopencv();

	double MatAve(Mat src);

	vector<vector<Point>> HoughLinesP(Mat img, Mat &dst, int thresh);

	Mat HoughLines1(Mat img1, Mat &img2, int thres = 100);


	//img1 : 선들을 검출할 gray이미지
	//img2 : 검출된 선들을 입힐 이미지
	//thres : 검출할 선의 임계값
	//반환값 : 조건에 맞는 0번째 좌표값
	Rect Label(Mat img, Mat &dst, int thresh = 0);

	//warptrans
	//img_org : 와핑할 이미지
	//img_dst : 저장 할 Mat
	//vector<Point> : 사각형 좌표를 저장 한 좌표, 4개의 Point가 필요
	//순서 : Top+Left, Top+Right, Bottom+Left, Bottom+Right
	//코드 출처 : http://miatistory.tistory.com/5
	void warptrans(Mat img_org, Mat &img_dst, vector<Point> qurd);

	//OCRtext
	//이미지를 입력하면 텍스트를 출력합니다
	//GRAYSCALE 이미지를 입력하는것을 추천합니다.
	//OCRtext : 추출 할 이미지
	//str : 출력 할 문자열
	string OCRtext(Mat img, string &str);

	//resize
	//img : 입력 할 이미지
	//dst : 출력 할 이미지
	//size : 출력 할 이미지의 곱
	//(2배 : size = 2, 0.5배 : size = 0.5 )
	Mat resize(Mat img, Mat &dst, double size);


	//img1 : 모폴로지를 적용할 이미지
	//img2 : 연산결과를 적용시킬 이미지
	//iterations : 반복횟수
	//size : 커널의 크기
	//type : 모폴로지연산의 타입
	//		MORPH_OPEN : open연산
	//		MORPH_CLOSE : close연산
	void morphology(Mat img1, Mat &img2, int iterations = 1, int size = 1, int type = MORPH_CLOSE);

};