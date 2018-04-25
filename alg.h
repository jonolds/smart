#ifndef _ALG_H_
#define  _ALG_H_
using namespace std;
using namespace cv;

void inline drawOrange(Mat& out);
Mat inline cannyAndHough(Mat &src, Mat &out);
void inline linesDraw(Mat& img, Mat& out);
void inline linesDraw2(Mat& img, Mat& out);

Scalar colorGreen = Scalar(0, 255, 0), colorRed = Scalar(0, 0, 255), colorBlue = Scalar(255, 0, 0), colorYellow = Scalar(0, 255, 255), colorOrange = Scalar(0, 165, 255);
deque<Vec4i> slopeOfBridge, negSlopePts, posSlopePts;
Point lastOleft, lastOright;
int y_offset;
vector<Vec4i> lines;

class LaneFinder : public Alg {
public:
	void process(Mat& src, Mat& out) override {
		Mat img = cannyAndHough(src, out);
		linesDraw(img, out);
	}

	void process2(Mat& src, Mat& out) override {
		Mat img = cannyAndHough(src, out);
		linesDraw2(img, out);
	}
};

void inline linesDraw(Mat &img, Mat&out) {
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
				line(out, Point(x1, y1), Point(x2, y2), colorRed, 2, 8);
				if (y1 < maxRight.y || y2 < maxLeft.y)
					maxRight = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
			}

			else {
				line(out, Point(x1, y1), Point(x2, y2), colorBlue, 2, 8);
				if (y1 < maxRight.y || y2 < maxLeft.y)
					maxLeft = (y1 > y2) ? Point(x2, y2) : Point(x1, y1);
			}
			cout << ((slope > 0) ? "RED " : "BLUE ") << x1 << " " << x2 << " " << " " << slope << "\n";
		}
	}
	slopeOfBridge.push_front(Vec4i(maxLeft.x, maxLeft.y, maxRight.x, maxRight.y));
	drawOrange(out);
}
void inline linesDraw2(Mat &img, Mat&out) {
	int  maxXl = 0, maxYl = img.size().height, maxXr = 0, maxYr = img.size().height;
	//	cout << img.size().width << " " << img.size().height << " " << "init max height: " << maxY << "\n";
	for (size_t i = 0; i < lines.size(); i++) {
		bool skipFlagSlope = false;
		lines[i][1] += y_offset;
		lines[i][3] += y_offset;
		// calculated slope is with reference to origin at botton left, aka mathematical orignal
		int x1 = lines[i][0], y1 = lines[i][1], x2 = lines[i][2], y2 = lines[i][3];
		double slope = x2 - x1 != 0 ? (tan(double(y2 - y1) / double(x2 - x1)) * 180) / CV_PI : 99.0;
		if (slope > 360)
			slope -= 360;
		if (slope < -360)
			slope += 360;
		//lane lines are close to +/- 45 degree; horizontal lane lines have slope~0
		//cout << x1 << " " << x2 << " " << " " << slope << " " << (float)img.size().width << " " << (float)img.size().width*0.6 << "\n";
		if ((abs(abs(slope) - 45) > 14.5) || cvIsNaN(slope) || (slope < 0 && x2 > img.size().width*0.55) || (slope < 0 && x1 > img.size().width*0.55)
			|| (slope > 0 && x2 < img.size().width*0.4) || (slope > 0 && x1 < img.size().width*0.4))
			skipFlagSlope = true;
		if (!skipFlagSlope) {
			if (slope > 0) {
				//cout <<"RED "<<x1<<" "<<x2<<" "<<" "<<slope<<"\n";
				line(out, Point(x1, y1), Point(x2, y2), colorRed, 2, 8);
			}
			else {
				//cout<<"BLUE "<<x1<<" "<<x2<<" "<<" "<<slope<<"\n";
				line(out, Point(x1, y1), Point(x2, y2), colorBlue, 2, 8);
			}

			if (slope < 0) {
				if (y1 < maxYl || y2 < maxYl) {
					maxYl = (y1 > y2) ? y2 : y1;
					maxXl = (y1 > y2) ? x2 : x1;
				}
			}
			else {
				if (y1 < maxYr || y2 < maxYr) {
					maxYr = (y1 > y2) ? y2 : y1;
					maxXr = (y1 > y2) ? x2 : x1;
				}
			}
		}
	}
	line(out, Point(maxXl, maxYl), Point(maxXr, maxYr), colorYellow, 2, LINE_8, 0);
	slopeOfBridge.push_front(Vec4i(maxXl, maxYl, maxXr, maxYr));
	if (slopeOfBridge.size() > 6)
		drawOrange(out);
}

Mat inline cannyAndHough(Mat &src, Mat &out) {
	lines.clear();
	Mat img;
	out = src.clone();
	cvtColor(src, img, COLOR_BGR2GRAY);
	Mat im_edge(img.size(), CV_8U, Scalar::all(0));
	Canny(img, im_edge, 100, 300, 3);

	// Rect(x,y,w,h) w->width=cols;h->rows
	Mat roi = im_edge(Rect(0, 0.6*im_edge.rows, im_edge.cols, im_edge.rows - 0.6*im_edge.rows));
	// 1st best {30, 20, 20} > {30,10,20}>{40, 20, 10} 
	HoughLinesP(roi, lines, 1, 1 * CV_PI / 180, 30, 20, 40);
	/* the detected hough lines are for the roi image- must add offset y to detected lines to display full image */
	y_offset = int(.6*im_edge.rows);
	return img;
}

void inline drawOrange(Mat& out) {
	line(out, Point(slopeOfBridge[0][0], slopeOfBridge[0][1]), Point(slopeOfBridge[0][2], slopeOfBridge[0][3]), colorYellow, 2, LINE_8, 0);
	if (slopeOfBridge.size() > 6) {
		double xLave = 0.0, yLave = 0.0, xRave = 0.0, yRave = 0.0, num = 6.0;
		slopeOfBridge.pop_back();

		for (int i = 0; i < num; i++) {
			xLave += slopeOfBridge[i][0];
			yLave += slopeOfBridge[i][1];
			xRave += slopeOfBridge[i][2];
			yRave += slopeOfBridge[i][3];
		}
		line(out, Point(int(xLave / num), int(yLave / num)), Point(int(xRave / num), int(yRave / num)), colorOrange, 2, LINE_8, 0);
	}
}
#endif