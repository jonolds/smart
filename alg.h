#ifndef _ALG_H_
#define  _ALG_H_
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

class Alg {
	int y_offset, frameCount2 = 0, blueCount = 0, redCount = 0;
	vector<Vec4i> farthestSlopeVec;
	
	double  lowThresh = 100;
	double  highThresh = 300;
	bool	l2gradient = false;
	int		houghThresh = 30;
	double	minLength = 30;
	double	maxGap = 20;

public:
	Alg();
	void process(Mat& src, Mat& out);
	void drawOrAndYel(Mat& out);
	Mat cannyAndHough(Mat &src, Mat &out, vector<Vec4i> &lines);
	void sortLines(Mat& img, vector<Vec4i> &lines, vector<Vec4i> &blues, vector<Vec4i> &reds);
	void combineLines(vector<Vec4i>& blues, vector<Vec4i> &reds, vector<Vec4i> &blueFrameLines, vector<Vec4i> &redFrameLines);
	void static drawLines(Mat&out, vector<Vec4i> blues, vector<Vec4i> reds);
	void addText(Mat &tmp, double slope, vector<Vec4i> lines, int i);
	void setYoffset(int i);
	void drawBlueWarning(Mat& out);
	void setCanny(double low, double high, bool l2g);
	void setHough(int hThresh, double minLen, double gapMax);
	int getBcount();
	int getRcount();
};
#endif