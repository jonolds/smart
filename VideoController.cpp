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
	while (!stop) { // read each frame in video	
		if (!vidreader.read(currentFrame)) // read next frame
			break;
		int initialTime = int(getTickCount());
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
	}

	Scalar m = mean(Mat(frProcTimeVec));
	cout << "\nMean frame proc time " << sum(sum(m)) << "  frames: " << frProcTimeVec.size() << "\n";
	writer.release();
	vidreader.release();
	cvDestroyAllWindows();
}