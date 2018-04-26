#ifndef _ALG_H_
#define  _ALG_H_
#include <opencv2/core/mat.hpp>
#include <vector>
#include <deque>

using namespace std;
using namespace cv;

class Alg {
	int y_offset;
	deque<Vec4i> blue2, red2;
	deque<pair<Point, Point>> farthestSlope;

public:
	Alg():y_offset(0){}
	void process(Mat& src, Mat& out);
	void drawOrAndYel(Mat& out);
	Mat cannyAndHough(Mat &src, Mat &out, vector<Vec4i> &lines);
	void sortLines(Mat& img, vector<Vec4i> &lines, vector<Vec4i> &blues, vector<Vec4i> &reds);
	void combineLines(vector<Vec4i> &blues, vector<Vec4i> &reds, vector<Vec4i> &blueFrameLines, vector<Vec4i> &redFrameLines);
	void static drawLines(Mat&out, vector<Vec4i> blues, vector<Vec4i> reds);
	void addText(Mat &tmp, double slope, vector<Vec4i> lines, int i);
	void setYoffset(int i);
};
#endif