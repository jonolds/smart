#include <opencv2/opencv.hpp>
#include <fstream>
#include "VideoController.h"

using namespace std;
using namespace cv;

void testEdgeSettings();

int main() {
	bool testing = true;

	if(testing)
		testEdgeSettings();
	else {
		VideoController vid;
		vid.setInputVideo("p3.mp4");
		vid.setOutVidName("myOutputCarLanesVideo.avi");
		vid.setOutWindowName("live");
		vid.run();
	}
	return 0;
}


void testEdgeSettings() {

	double  lowThresh	= 50;
	double  highThresh	= 100;
	bool	l2g			= false;
	int		houghThresh = 30;
	double	minLength	= 20;
	double	maxGap		= 20;
	
	for(int i = 0; i < 1; i++) {
		VideoController vid;
		vid.setInputVideo("p3.mp4");
		vid.setOutVidName("myOutputCarLanesVideo.avi");
		vid.setOutWindowName("live");
		vid.algPtr->setCanny(lowThresh, highThresh, l2g);
		vid.algPtr->setHough(houghThresh, minLength, maxGap);
		vid.run();
		cout << "lowThresh: " << lowThresh << "\n highThresh: " << highThresh << "\nl2g: " << l2g << "\n";
		cout << "houghThresh: " << houghThresh << "\nminLength: " << minLength << "\nmaxGap: " << maxGap << "\n";
		int bc = vid.algPtr->getBcount();
		int rc = vid.algPtr->getRcount();
		cout << "Total blueCount: " << vid.algPtr->getBcount() << "   Total redCount: " << vid.algPtr->getRcount() << "\n\n";
		
		ofstream out;
		out.open("setTests.csv", ios::out | ios::app);
		if (out.is_open()) {
			out << lowThresh << "," << highThresh << "," << l2g << "," << houghThresh << "," << minLength << "," << maxGap << "," << bc << "," << rc << endl;
			out.close();
		}
		else
			cout << "ofstream error\n";
		l2g = true;
	}
}

void merge() {
	Mat dst;
	for (int i = 1; i < 13; i += 3) {
		Mat row, img1, img2, img3;
		Mat tmp = imread(to_string(i) + ".jpg");
		resize(tmp, img1, Size(), .4, .4, INTER_CUBIC);
		tmp = imread(to_string(i + 1) + ".jpg");
		resize(tmp, img2, Size(), .4, .4, INTER_CUBIC);
		tmp = imread(to_string(i + 2) + ".jpg");
		resize(tmp, img3, Size(), .4, .4, INTER_CUBIC);
		hconcat(img1, img2, row);
		hconcat(row, img3, row);
		if (!dst.data)
			dst = row;
		else
			vconcat(dst, row, dst);
	}
	imwrite("finalLanes.jpg", dst);
	namedWindow("lanes", WINDOW_AUTOSIZE);
	imshow("lanes", dst);
	waitKeyEx(1);
	
}

