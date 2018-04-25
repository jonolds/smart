#ifndef _ALG_H_
#define  _ALG_H_
#include <opencv2/core/mat.hpp>
#include <deque>

using namespace std;
using namespace cv;

class Alg {
	int y_offset;
	deque<Vec4i> slopeOfBridge, negSlopePts, posSlopePts;
public:
	Alg():y_offset(0){}
	void process(Mat& src, Mat& out);
	void process2(Mat& src, Mat& out);
	void drawOrange(Mat& out);
	Mat cannyAndHough(Mat &src, Mat &out, vector<Vec4i> &lines);
	void linesDraw(Mat& img, Mat& out, vector<Vec4i> &lines);
	void addText(Mat &tmp, double slope, vector<Vec4i> lines, int i);
	void setYoffset(int i);
};
#endif