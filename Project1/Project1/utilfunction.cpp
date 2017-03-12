#include "utilfunction.h"


utilopencv::utilopencv() {

}
utilopencv::~utilopencv() {

}
	//img1 : 모폴로지를 적용할 이미지
	//img2 : 연산결과를 적용시킬 이미지
	//iterations : 반복횟수
	//size : 커널의 크기
	//type : 모폴로지연산의 타입
	//		MORPH_OPEN : open연산
	//		MORPH_CLOSE : close연산
	void utilopencv::morphology(Mat img1, Mat &img2, int iterations , int size, int type) {
		Size size2(size, size);
		Mat rectKernel = getStructuringElement(MORPH_RECT, size2);
		Point anchor(-1, -1);
		morphologyEx(img1, img2, type, rectKernel, anchor, iterations);
	}

	Mat utilopencv::resize(Mat img, Mat &dst, double size) {
		Mat image = img;
		if (size < 1)
			cv::resize(img, dst, Size((double)img.cols * size, (double)img.rows * size), 0, 0, INTER_AREA);
		else if (size >= 1)
			cv::resize(img, dst, Size((double)img.cols * size, (double)img.rows * size), 0, 0, INTER_LINEAR);
		return dst;
	}

	string utilopencv::OCRtext(Mat img, string &str) {
		Mat img_org = img;
		char *outText;
		//    const char *path = ".\\tessdata";  
		char *name;

		string data;
		char* result;

		if (img.type() != 0) {
			cvtColor(img, img, CV_BGR2GRAY);
		}

		resize(img, img, 5);

		Mat dst;
		GaussianBlur(img, img, Size(3, 3), 0);

		adaptiveThreshold(img, img, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 15, 3);
		img = ~img;

		morphology(img, img, 5, 2, MORPH_OPEN);
		morphology(img, img, 2, 2, MORPH_CLOSE);
		//이진화와 모폴로지 연산으로 보정이 적용된 이미지를 출력
		//imwrite("outmat2/"+to_string((unsigned) time(NULL))+".jpg", img);

		if (img.type() != 0) {
			cvtColor(img, img, CV_BGR2GRAY);
		}

		tesseract::TessBaseAPI api; 
		//학습 데이터의 경로, 적용시킬 언어
		api.Init("../libdata/tessdata", "kor", tesseract::OEM_DEFAULT);
		//api.Init("../libdata/tessdata", "kor+eng", tesseract::OEM_DEFAULT);
		//api.Init(NULL, "kor+eng", tesseract::OEM_DEFAULT); 
		api.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
		api.SetImage((uchar*)img.data, img.cols, img.rows, 1, img.cols);
		api.Recognize(0);
		str += api.GetUTF8Text();  
		return str;


	}

	//warptrans
	//img_org : 와핑할 이미지
	//img_dst : 저장 할 Mat
	//vector<Point> : 사각형 좌표를 저장 한 좌표, 4개의 Point가 필요
	//순서 : Top+Left, Top+Right, Bottom+Left, Bottom+Right
	//코드 출처 : http://miatistory.tistory.com/5

	void utilopencv::warptrans(Mat img_org, Mat &img_dst, vector<Point> qurd)
	{
		Point TopLeft = Point(qurd[0]);
		Point TopRight = Point(qurd[1]);
		Point BottomLeft = Point(qurd[2]);
		Point BottomRight = Point(qurd[3]);

		vector<Point> rect;
		rect.push_back(TopLeft);
		rect.push_back(TopRight);
		rect.push_back(BottomRight);
		rect.push_back(BottomLeft);

		double w1 = sqrt(pow(BottomRight.x - BottomLeft.x, 2)
			+ pow(BottomRight.x - BottomLeft.x, 2));
		double w2 = sqrt(pow(TopRight.x - TopLeft.x, 2)
			+ pow(TopRight.x - TopLeft.x, 2));

		double h1 = sqrt(pow(TopRight.y - BottomRight.y, 2)
			+ pow(TopRight.y - BottomRight.y, 2));
		double h2 = sqrt(pow(TopLeft.y - BottomLeft.y, 2)
			+ pow(TopLeft.y - BottomLeft.y, 2));

		double maxWidth = (w1 < w2) ? w1 : w2;
		double maxHeight = (h1 < h2) ? h1 : h2;

		Point2f src[4], dst[4];
		src[0] = Point2f(TopLeft.x, TopLeft.y);
		src[1] = Point2f(TopRight.x, TopRight.y);
		src[2] = Point2f(BottomRight.x, BottomRight.y);
		src[3] = Point2f(BottomLeft.x, BottomLeft.y);

		dst[0] = Point2f(0, 0);
		dst[1] = Point2f(maxWidth - 1, 0);
		dst[2] = Point2f(maxWidth - 1, maxHeight - 1);
		dst[3] = Point2f(0, maxHeight - 1);

		Mat transformMatrix;
		transformMatrix = getPerspectiveTransform(src, dst);

		Mat srcFrame, dstFrame;
		srcFrame = img_dst;
		warpPerspective(img_org, img_dst, transformMatrix, Size(maxWidth, maxHeight));
		return;
	}

	Rect utilopencv::Label(Mat img, Mat &dst, int thresh ) {
		Mat img_gray, img_color, img_binary;
		img.convertTo(img_gray, CV_8UC1);
		if (img_gray.channels() != 1)
			cvtColor(img_gray, img_gray, CV_BGR2GRAY);
		GaussianBlur(img_gray, img_gray, Size(3, 3), 0);
		adaptiveThreshold(img_gray, img_binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 5, 2);



		Mat img_labels, stats, centroids;
		int numOfLables = connectedComponentsWithStats(img_binary, img_labels,
			stats, centroids, 8, CV_32S);


		////라벨링된 이미지중 특정 라벨을 컬러로 표현해주기  
		//for (int y = 0; y<img_labels.rows; ++y) {

		//	int *label = img_labels.ptr<int>(y);
		//	Vec3b* pixel = img_color.ptr<Vec3b>(y);


		//	for (int x = 0; x < img_labels.cols; ++x) {
		//		if (label[x] == 3) {
		//			pixel[x][2] = 0;
		//			pixel[x][1] = 255;
		//			pixel[x][0] = 0;
		//		}
		//	}
		//}

		{

		}
		vector<Point> pt1;
		vector<Point> pt2;
		//cout << stats << endl;
		for (int i = 1; i < numOfLables; i++) {
			//cout << stats.at<int>(i, 4) << endl;

		}

		int Label = 0;
		double maxSca = 0;
		//라벨링 된 이미지에 각각 직사각형으로 둘러싸기 
		for (int j = 1; j < numOfLables; j++) {
			int area = stats.at<int>(j, CC_STAT_AREA);
			int left = stats.at<int>(j, CC_STAT_LEFT);
			int top = stats.at<int>(j, CC_STAT_TOP);
			int width = stats.at<int>(j, CC_STAT_WIDTH);
			int height = stats.at<int>(j, CC_STAT_HEIGHT);
			if (width*height < thresh)
				continue;
			pt1.push_back(Point(left, top));
			pt2.push_back(Point(left + width, top + height));

			//평균 Scalar 값을 추출
			//double mean = MatAve(img(Rect(Point(left, top), Point(left + width, top + height)))); 

			Point pp = stats.at<int>(j);

		}
		Rect roi(pt1[0], pt2[0]);
		dst = img_binary(roi);
		return roi;
	}

	//img1 : 선들을 검출할 gray이미지
	//img2 : 검출된 선들을 입힐 이미지
	//thres : 검출할 선의 임계값
	Mat utilopencv::HoughLines1(Mat img1, Mat &img2, int thres ) {
		Vec2f params;
		float rho, theta;
		float c, s;
		float x0, y0;
		vector<Vec2f> lines;
		//img2를 초기화
		img2 = Mat::zeros(img1.size(), CV_8UC3);


		HoughLines(img1, lines, 1, CV_PI / 180, thres);
		cout << "lines.size() = " << lines.size() << endl;

		for (int k = 0; k < lines.size(); k++) {
			params = lines[k];
			rho = params[0];
			theta = params[1];

			//draw
			c = cos(theta);
			s = sin(theta);
			x0 = rho * c;
			y0 = rho * s;
			if (acos(s) * 180 / CV_PI < 75)
				continue;
			Point pt1, pt2;


			pt1.x = cvRound(x0 + 10000 * (-s)) + 5;
			pt1.y = cvRound(y0 + 10000 * (c));
			pt2.x = cvRound(x0 - 10000 * (-s)) + 5;
			pt2.y = cvRound(y0 - 10000 * (c));

			line(img2, pt1, pt2, Scalar(0, 255, 255), 2);
		}
		return img2;
	}

	vector<vector<Point>> utilopencv::HoughLinesP(Mat img, Mat &dst, int thresh) {
		Mat edges;

		Canny(img, edges, 50, 100);
		vector<Vec4i> lines;
		cv:: HoughLinesP(edges, lines, 1, CV_PI / 180, thresh, 30, 10);
		Vec4i params;

		Point pt1, pt2;
		vector< vector<Point>> pt;
		for (int i = 0; i < lines.size(); i++) {
			vector<Point> lineP;
			params = lines[i];

			pt1.x = params[0];
			pt1.y = params[1];
			pt2.x = params[2];
			pt2.y = params[3];

			lineP.push_back(pt1);
			lineP.push_back(pt2);
			pt.push_back(lineP);
			line(dst, pt1, pt2, Scalar(0, 255, 255), 1);
			cout << pt1 << endl << pt2 << endl;
		}

		return pt;
	}

	double utilopencv::MatAve(Mat src) {
		double sum = 0;

		for (int i = 0; i < src.cols; i++)
			for (int j = 0; j < src.rows; j++)
				sum += (int)src.at<uchar>(j, i);
		return sum / src.total();
	}