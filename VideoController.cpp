#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "VideoController.h"

using namespace std;
using namespace cv;
Scalar vcColGreen = Scalar(0, 255, 0);
VideoController::VideoController() : algPtr(new Alg()), fps(0), frameCount(0), isVidOpen(false), isWriterInitialized(false) {
}

VideoController::~VideoController() {
	writer.release(); vidreader.release();
}
void VideoController::makeKillWindow() {
	Mat kill(100, 100, CV_8UC4);
	namedWindow("kill window", WINDOW_GUI_EXPANDED | WINDOW_NORMAL | WINDOW_FREERATIO);
	circle(kill, Point(50, 50), 50, Scalar(255, 255, 255), CV_FILLED, 8, 0);
	putText(kill, "KILL IT KILL IT", Point(50, 50), FONT_HERSHEY_COMPLEX, 1, vcColGreen, 3,LINE_8, false);

	setMouseCallback("kill window", killWindows, 0);
	saveWindowParameters("kill window");
	loadWindowParameters("kill window");
	imshow("kill window", kill);
}

void VideoController::killWindows(int event, int x, int y, int, void* ) {
	destroyAllWindows();
}


void VideoController::setInputVideo(string ipVideoName) {
	vidreader.release();

	vidreader.open(ipVideoName);
	if (!vidreader.isOpened()) {
		cout << " Invalid video file read " << endl;
		vidreader.release();
		CV_Assert(false);
	}
	vidreader.set(CAP_PROP_FRAME_WIDTH, 432);
	vidreader.set(CAP_PROP_FRAME_HEIGHT, 240);
	isVidOpen = true;
	fps = int(vidreader.get(CAP_PROP_FPS));
	cout << "fps: " << fps << "\n";
}

void VideoController::setOutVidName(string name) {
	outVidName = name;
}
void VideoController::setOutWindowName(string name) {
	outWindowName = name;
}

bool VideoController::isOutputVideoSaveReqd() {
	return (!outVidName.empty());
}

void VideoController::initWriter() {
	if (isOutputVideoSaveReqd()) {
		writer.release(); // release any previous instance of writer object
		int codec = static_cast<int>(vidreader.get(CAP_PROP_FOURCC));
		Size sz = Size(int(vidreader.get(CAP_PROP_FRAME_WIDTH)), int(vidreader.get(CAP_PROP_FRAME_HEIGHT)));
		writer.open(outVidName, codec, fps, sz, true);
		if (!writer.isOpened()) {
			cout << " Error while calling the cv::VideoWriter.open(" << outVidName << ")" << endl;
			writer.release();
			CV_Assert(false);
		}
		isWriterInitialized = true;
	}
}

void VideoController::run() {
	CV_Assert(isVidOpen);
	Mat currentFrame, outputFrame;
	startWindowThread();
	namedWindow(outWindowName, WINDOW_GUI_EXPANDED | WINDOW_FREERATIO);
	initWriter();
	frameCount = 0;
	while (!stop && frameCount < 21) { // read each frame in video	
		if (!vidreader.read(currentFrame)) // read next frame
			break;
		int initialTime = int(getTickCount());
		//if (frameCount > 10 && frameCount < 20)
		cout << "frame: " << frameCount << "\n";
		algPtr->process(currentFrame, outputFrame);
		double frameProcessTime = ((double(getTickCount()) - initialTime) / getTickFrequency()) * 1000;
		frProcTimeVec.push_back(frameProcessTime);
		imshow(outWindowName, outputFrame);
		if (isWriterInitialized)
			writer.write(outputFrame);
		int elaspedTime = int((getTickCount() - initialTime) / (1000 * getTickFrequency()));//from start of frame read
		int remainingTime = (1000 / fps) - (elaspedTime);//used to prevent early play/process of next frame
		frameCount++;
		if (remainingTime > 1) // if positive wait for the remaining time
			waitKey(remainingTime);
		else
			waitKey(1); // delay for 1 ms if elaspedtime>delay
		//waitKey();
	}

	Scalar m = mean(Mat(frProcTimeVec));
	cout << "\nMean frame proc time " << sum(sum(m)) << "  frames: " << frProcTimeVec.size() << "\n";
	writer.release();
	vidreader.release();
	waitKey();
	cvDestroyAllWindows();
	waitKey(1);
	waitKey(1);
	waitKey(1);
	waitKey(1);
	waitKey(1);

}