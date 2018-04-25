#ifndef _ALG_H_
#define  _ALG_H_
#include <opencv2/core/mat.hpp>

using namespace std;
using namespace cv;

class Alg {
public:
	void process(Mat& src, Mat& out);
	void process2(Mat& src, Mat& out);
	static void drawOrange(Mat& out);
	Mat cannyAndHough(Mat &src, Mat &out, vector<Vec4i> &lines);
	void linesDraw(Mat& img, Mat& out, vector<Vec4i> &lines);
	void linesDraw2(Mat& img, Mat& out, vector<Vec4i> &lines);
	void addText(Mat &tmp, double slope, vector<Vec4i> lines, int i);
};
#endif