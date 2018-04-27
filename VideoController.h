#ifndef _VIDEOCONTROLLER_H_
#define _VIDEOCONTROLLER_H_
#include<opencv2/opencv.hpp>
#include<opencv2/videoio.hpp>
#include "Alg.h"

using namespace cv;
using namespace std;

class VideoController {
	VideoCapture vidreader;  VideoWriter writer;
	Alg * algPtr;
	int fps, frameCount;
	string outWindowName = "Output", outVidName;
	bool stop = false, isVidOpen, isWriterInitialized;
	vector<double> frProcTimeVec;
	bool isOutputVideoSaveReqd();
	void initWriter();
	void makeKillWindow();
	static void killWindows(int event, int x, int y, int, void*);
public:
	VideoController();
	~VideoController();
	void setOutVidName(string name);
	void setOutWindowName(string name);
	void setInputVideo(string ipVideoName);
	void run();
};
#endif