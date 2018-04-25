#include "Alg.h"
#include<iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Scalar colGreen = Scalar(0, 255, 0), colRed = Scalar(0, 0, 255), colBlue = Scalar(255, 0, 0), colYellow = Scalar(0, 255, 255), colOrange = Scalar(0, 165, 255);
deque<Vec4i> slopeOfBridge, negSlopePts, posSlopePts;
int y_offset;

void Alg::process(Mat& src, Mat& out) {
	vector<Vec4i> lines;
	Mat img = cannyAndHough(src, out, lines);
	linesDraw(img, out, lines);
}

void Alg::process2(Mat& src, Mat& out) {
	vector<Vec4i> lines;
	Mat img = cannyAndHough(src, out, lines);
	linesDraw2(img, out, lines);
}

void Alg::linesDraw(Mat &img, Mat&out, vector<Vec4i> &lines) {
	Point maxLeft(0, img.size().height), maxRight(0, img.size().height);
	for (size_t i = 0; i < lines.size(); i++) {
		bool skipFlagSlope = false;
		lines[i][1] += y_offset;
		lines[i][3] += y_offset;
		// calculated slope is with reference to origin at botton left, aka mathematical orignal
		int x1 = lines[i][0], y1 = lines[i][1], x2 = lines[i][2], y2 = lines[i][3];
		double slope = x2 - x1 != 0 ? (tan(double(y2 - y1) / double(x2 - x1)) * 180) / CV_PI : 99.0;
		while (slope > 360)
			slope -= 360;
		while (slope < -360)
			slope += 360;
		//lane lines are close to +/- 45 degree; horizontal lane lines have slope~0
		//cout << x1 << " " << x2 << " " << " " << slope << " " << (float)img.size().width << " " << (float)img.size().width*0.6 << "\n";
		if ((abs(abs(slope) - 45) > 14.5) || cvIsNaN(slope) || (slope < 0 && x2 > img.size().width*0.55) || (slope < 0 && x1 > img.size().width*0.55)
			|| (slope > 0 && x2 < img.size().width*0.4) || (slope > 0 && x1 < img.size().width*0.4))
			skipFlagSlope = true;
		if (!skipFlagSlope) {
			if (slope > 0) {
				line(out, Point(x1, y1), Point(x2, y2), colRed, 2, 8);
				if (y1 < maxRight.y || y2 < maxLeft.y)
					maxRight = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
			}

			else {
				line(out, Point(x1, y1), Point(x2, y2), colBlue, 2, 8);
				if (y1 < maxRight.y || y2 < maxLeft.y)
					maxLeft = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
			}
			cout << ((slope > 0) ? "RED " : "BLUE ") << x1 << " " << x2 << " " << " " << slope << "\n";
		}
	}
	slopeOfBridge.push_front(Vec4i(maxLeft.x, maxLeft.y, maxRight.x, maxRight.y));
	drawOrange(out);
}
void Alg::linesDraw2(Mat &img, Mat&out, vector<Vec4i> &lines) {
	Point maxLeft(0, img.size().height), maxRight(0, img.size().height);
	for (size_t i = 0; i < lines.size(); i++) {
		bool skipFlagSlope = false;
		lines[i][1] += y_offset;
		lines[i][3] += y_offset;
		// calculated slope is with reference to origin at botton left, aka mathematical orignal
		int x1 = lines[i][0], y1 = lines[i][1], x2 = lines[i][2], y2 = lines[i][3];
		double slope = x2 - x1 != 0 ? (tan(double(y2 - y1) / double(x2 - x1)) * 180) / CV_PI : 99.0;
		while (slope > 360)
			slope -= 360;
		while (slope < -360)
			slope += 360;
		//lane lines are close to +/- 45 degree; horizontal lane lines have slope~0
		if ((abs(abs(slope) - 45) > 14.5) || cvIsNaN(slope) || (slope < 0 && x2 > img.size().width*0.55) || (slope < 0 && x1 > img.size().width*0.55)
			|| (slope > 0 && x2 < img.size().width*0.4) || (slope > 0 && x1 < img.size().width*0.4))
			skipFlagSlope = true;
		if (!skipFlagSlope) {
			if (slope > 0) {
				line(out, Point(x1, y1), Point(x2, y2), colRed, 2, 8);
				if (y1 < maxRight.y || y2 < maxLeft.y)
					maxRight = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
			}
			else {
				line(out, Point(x1, y1), Point(x2, y2), colBlue, 2, 8);
				if (y1 < maxRight.y || y2 < maxLeft.y)
					maxLeft = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
			}
			cout << ((slope > 0) ? "RED " : "BLUE ") << x1 << " " << x2 << " " << " " << slope << "\n";
		}
	}
	slopeOfBridge.push_front(Vec4i(maxLeft.x, maxLeft.y, maxRight.x, maxRight.y));
	drawOrange(out);
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
	y_offset = int(.6*im_edge.rows);
	return img;
}

void Alg::drawOrange(Mat& out) {
	line(out, Point(slopeOfBridge[0][0], slopeOfBridge[0][1]), Point(slopeOfBridge[0][2], slopeOfBridge[0][3]), colYellow, 2, LINE_8, 0);
	if (slopeOfBridge.size() > 6) {
		double xLave = 0.0, yLave = 0.0, xRave = 0.0, yRave = 0.0, num = 6.0;
		slopeOfBridge.pop_back();

		for (int i = 0; i < num; i++) {
			xLave += slopeOfBridge[i][0];
			yLave += slopeOfBridge[i][1];
			xRave += slopeOfBridge[i][2];
			yRave += slopeOfBridge[i][3];
		}
		line(out, Point(int(xLave / num), int(yLave / num)), Point(int(xRave / num), int(yRave / num)), colOrange, 2, LINE_8, 0);
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