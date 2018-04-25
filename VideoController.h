#ifndef _VIDEOCONTROLLER_H_
#define  _VIDEOCONTROLLER_H_
#include "VideoController.h"
#include "Alg.h"
using namespace std;
using namespace cv;


class VideoController {
	VideoCapture vidreader;
	VideoWriter writer;
	Alg * algPtr;
	int fps;
	string outWindowName, outVidName;
	bool stop = false, isVidOpen, isWriterInitialized;
	vector<double> frameProcessingTime; //used for storing frameprocessing times(in ms

	bool isOutputVideoSaveReqd() { return (!outVidName.empty()); }
	void initWriter() {
		if (isOutputVideoSaveReqd()) {
			writer.release(); // release any previous instance of writer object
			int codec = static_cast<int>(vidreader.get(CAP_PROP_FOURCC));
			Size sz = Size(int(vidreader.get(CAP_PROP_FRAME_WIDTH)), int(vidreader.get(CAP_PROP_FRAME_HEIGHT)));
			writer.open(outVidName.c_str(), codec, fps, sz, true);
			if (!writer.isOpened()) {
				cout << " Error while calling the cv::VideoWriter.open(" << outVidName << ")" << endl;
				writer.release();
				CV_Assert(false);
			}
			isWriterInitialized = true;
		}
	}
public:
	VideoController() : algPtr(nullptr), fps(0), outWindowName("Output"), isVidOpen(false), isWriterInitialized(false) {}
	~VideoController() { writer.release(); vidreader.release(); }
	void setOutVidName(string name) { outVidName = name; }
	void setOutWindowName(string name) { outWindowName = name; }
	void setInputVideo(string ipVideoName) {
		vidreader.release();
		vidreader.open(ipVideoName);
		if (!vidreader.isOpened()) {
			cout << " Invalid video file read " << endl;
			vidreader.release();
			CV_Assert(false);
		}
		isVidOpen = true;
		fps = int(vidreader.get(CAP_PROP_FPS));
		cout << "fps: " << fps << "\n";
	}
	void run() {
		CV_Assert(isVidOpen);
		Mat currentFrame, outputFrame;
		namedWindow(outWindowName);
		initWriter();
		int frameCount = 0;
		while (!stop && frameCount < 247) { // read each frame in video	
			if (!vidreader.read(currentFrame)) // read next frame
				break;
			cout << frameCount << "\n";
			int initialTime = int(getTickCount());
			algPtr->process(currentFrame, outputFrame);
			double frameProcessTime = ((double(getTickCount()) - initialTime) / getTickFrequency()) * 1000;
			frameProcessingTime.push_back(frameProcessTime);
			imshow(outWindowName, outputFrame);
			if (isWriterInitialized)
				writer.write(outputFrame);
			int elaspedTime = int((getTickCount()-initialTime)/(1000*getTickFrequency()));//from start of frame read
			int remainingTime = (1000/fps) - (elaspedTime);//used to prevent early play/process of next frame
			frameCount++;
			if (remainingTime > 1) // if positive wait for the remaining time
				waitKey(remainingTime);
			else
				waitKey(1); // delay for 1 ms if elaspedtime>delay
		}
		
		Scalar m = mean(Mat(frameProcessingTime));
		cout << endl << " mean frame processing time " << sum(sum(m)) << "   frames: " << frameProcessingTime.size() << "\n";
		writer.release();
		vidreader.release();
		waitKey();
	}
};
#endif