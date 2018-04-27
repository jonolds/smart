#include<opencv2/opencv.hpp>
#include "VideoController.h"
#include "Alg.h"
using namespace std;
using namespace cv;

int main() {
	VideoController vid;
	vid.setInputVideo("p3.mp4");
	vid.setOutVidName("myOutputCarLanesVideo.avi");
	vid.setOutWindowName("live");
	vid.run();
	cout << utils::getThreadID() << "\n";
		return 0;
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

