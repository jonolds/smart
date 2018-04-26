#include "Alg.h"
#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
Scalar colGreen = Scalar(0, 255, 0), colRed = Scalar(0, 0, 255), colBlue = Scalar(255, 0, 0), colYellow = Scalar(0, 255, 255), colOrange = Scalar(0, 165, 255);

void Alg::process(Mat& src, Mat& out) {
	vector<Vec4i> lines, blues, reds, blueFrameLines, redFrameLines;
	Mat img = cannyAndHough(src, out, lines);
	sortLines(img, lines, blues, reds);
	combineLines(blues, reds, blueFrameLines, redFrameLines);
	drawLines(out, blues, reds);
	drawOrAndYel(out);
}

void Alg::combineLines(vector<Vec4i> &blues, vector<Vec4i> &reds, vector<Vec4i> &blueFrameLines, vector<Vec4i> &redFrameLines) {

}

Mat Alg::cannyAndHough(Mat &src, Mat &out, vector<Vec4i> &lines) {
	lines.clear();
	Mat img;
	out = src.clone();
	cvtColor(src, img, COLOR_BGR2GRAY);
	Mat im_edge(img.size(), CV_8U, Scalar::all(0));
	Canny(img, im_edge, 100, 300, 3);
	// Rect(x,y,w,h) w->width=cols;h->rows
	Mat roi = im_edge(Rect(0, int(0.6*im_edge.rows), im_edge.cols, im_edge.rows - int(0.6*im_edge.rows)));
	// 1st best {30, 20, 20} > {30,10,20}>{40, 20, 10} 
	HoughLinesP(roi, lines, 1, 1 * CV_PI / 180, 30, 20, 40);
	/* the detected hough lines are for the roi image- must add offset y to detected lines to display full image */
	setYoffset(int(.6*im_edge.rows));
	return img;
}

void Alg::sortLines(Mat &img, vector<Vec4i> &lines, vector<Vec4i> &blues, vector<Vec4i> &reds) {
	Point maxBlue(0, img.size().height), maxRed(0, img.size().height);
	for (size_t i = 0; i < lines.size(); i++) {
		lines[i][1] += y_offset, lines[i][3] += y_offset;
		int x1 = lines[i][0], y1 = lines[i][1], x2 = lines[i][2], y2 = lines[i][3];
		double slope = x2 - x1 != 0 ? (tan(double(y2 - y1) / double(x2 - x1)) * 180) / CV_PI : 99.0;
		while (slope > 360) //slope reference to origin at botton left/mathematical orignal
			slope -= 360;
		while (slope < -360)
			slope += 360;
		//lane lines are close to +/- 45 degree; horizontal lane lines have slope~0
		if (!((abs(abs(slope) - 45) > 14.5) || cvIsNaN(slope) || (slope < 0 && (x2 > img.size().width*0.55 || x1 > img.size().width*0.55))
			|| (slope > 0 && (x2 < img.size().width*0.4 || x1 < img.size().width*0.4)))) {
			if (slope > 0) {
				reds.emplace_back(lines[i]);
				if (y1 < maxRed.y || y2 < maxBlue.y)
					maxRed = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
			}
			else {
				blues.emplace_back(lines[i]);
				if (y1 < maxRed.y || y2 < maxBlue.y)
					maxBlue = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
			}
			cout << ((slope > 0) ? "RED " : "BLUE ") << x1 << " " << x2 << " " << " " << slope << "\n";
		}
	}
	farthestSlope.push_front(make_pair(maxBlue, maxRed));
}

void Alg::drawLines(Mat&out, vector<Vec4i> blues, vector<Vec4i> reds) {
	for(size_t i = 0; i < blues.size(); i++)
		line(out, Point(blues[i][0], blues[i][1]), Point(blues[i][2], blues[i][3]), colBlue, 2, 8);
	for (size_t i = 0; i < reds.size(); i++)
		line(out, Point(reds[i][0], reds[i][1]), Point(reds[i][2], reds[i][3]), colRed, 2, 8);
}

void Alg::drawOrAndYel(Mat& out) {
	line(out, farthestSlope.at(0).first, farthestSlope.at(0).second, colYellow, 2, LINE_8, 0);
	size_t num = 6;
	if(farthestSlope.size() > num) {
		farthestSlope.pop_back();
		pair<Point, Point> bridge = farthestSlope.at(0);
		for (size_t i = 0; i < num - 1; i++)
			bridge = make_pair((bridge.first + farthestSlope.at(i + 1).first), (bridge.second + farthestSlope.at(i+1).second));
		bridge = make_pair(bridge.first / int(num), bridge.second / int(num));
		line(out, bridge.first, bridge.second, colOrange, 2, LINE_8, 0);
	}
}

void Alg::addText(Mat &tmp, double slope, vector<Vec4i> lines, int i) {
	int y_offset2 = -5, x_offset2 = slope > 0 ? -250 : 60, fontFace = CV_FONT_HERSHEY_DUPLEX, thickness = 3;
	double fontScale = 1;
	int baseline = 0;

	string text = "slope = " + to_string(slope);
	Point org(30, 30);
	Size textSz = getTextSize(text, fontFace, fontScale, thickness, &baseline);
	rectangle(tmp, org + Point(0, 5), Point(30 + textSz.width, 25 - textSz.height), CV_RGB(0, 0, 0), CV_FILLED);
	putText(tmp, "slope = " + to_string(slope), Point(30, 30), fontFace, fontScale, colGreen, thickness, LINE_8, false);

	text = "1(" + to_string(lines[i][0]) + ", " + to_string(lines[i][1]) + ")";
	org = Point(lines[i][0] + x_offset2, lines[i][1] + y_offset2);
	textSz = getTextSize(text, fontFace, fontScale, thickness, &baseline);
	rectangle(tmp, org + Point(0, 5), Point(org.x + textSz.width, org.y - textSz.height), CV_RGB(0, 0, 0), CV_FILLED);
	putText(tmp, text, org, fontFace, fontScale, colGreen, thickness, LINE_8, false);

	text = "2(" + to_string(lines[i][2]) + ", " + to_string(lines[i][3]) + ")";
	org = Point(lines[i][2] + x_offset2, lines[i][3] + y_offset2);
	textSz = getTextSize(text, fontFace, fontScale, thickness, &baseline);
	rectangle(tmp, org + Point(0, 5), Point(org.x + textSz.width, org.y - textSz.height), CV_RGB(0, 0, 0), CV_FILLED);
	putText(tmp, text, org, fontFace, fontScale, colGreen, thickness, LINE_8, false);
}

void Alg::setYoffset(int i) {
	y_offset = i;
}