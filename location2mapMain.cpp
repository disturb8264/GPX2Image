#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

int getLoncation(vector<float> &lats, vector<float> &lons, const char* path) {
	FILE* FP = fopen(path, "rt");
	int nl;
	fscanf(FP, "%d", &nl);

	for (int i = 0; i < nl; i++) {
		float lat, lon;
		fscanf(FP, "%f%f", &lat, &lon);
		lats.push_back(lat);
		lons.push_back(lon);		
	}
	fclose(FP);
	return 0;
}


int getElevation(vector<float> &eles, const char* path) {
	FILE* FP = fopen(path, "rt");
	int nl;
	fscanf(FP, "%d", &nl);

	for (int i = 0; i < nl; i++) {
		float ele;
		fscanf(FP, "%f", &ele);
		eles.push_back(ele);		
	}
	fclose(FP);
	return 0;
}

int getTime(vector<string> &times, const char* path) {
	FILE* FP = fopen(path, "rt");
	int nl;
	fscanf(FP, "%d", &nl);
	
	char txt[100];
	for (int i = 0; i < nl; i++) {		
		fscanf(FP, "%s", &txt);
		times.push_back(txt);
	}
	fclose(FP);
	return 0;
}

int main()
{
	vector<float> lats;
	vector<float> lons;
	vector<float> eles;
	vector<string> times;
	getLoncation(lats, lons, "location.txt");
	getElevation(eles, "ele.txt");
	getTime(times, "time.txt");

	vector<float> clats;
	vector<float> clons;
	float minlats = 999;
	float maxlats = 0;
	float minlons = 999;
	float maxlons = 0;
	for (int i = 0; i < lats.size(); i++) {
		if (lats[i] < minlats)
			minlats = lats[i];
		if (lats[i] > maxlats)
			maxlats = lats[i];
		if (lons[i] < minlons)
			minlons = lons[i];
		if (lons[i] > maxlons)
			maxlons = lons[i];
	}


	Size szImage(500,500);
	
	float factorLats = (maxlats - minlats) / szImage.height;
	float factorLons = (maxlons - minlons) / szImage.width;
	float sizeFactor = max(factorLats, factorLons);
	for (int i = 0; i < lats.size(); i++) {
		clats.push_back((lats[i] - minlats)/ sizeFactor);
		clons.push_back((lons[i] - minlons)/ sizeFactor);
	}

	int marginX = szImage.width - (maxlons - minlons) / sizeFactor;
	int marginY = szImage.height - (maxlats - minlats) / sizeFactor;
	Point marginPos(marginX / 2, marginY / 2);

	Mat img(szImage, CV_8UC3, Scalar(0, 0, 0));

	char txt[100];
	for (int i = 1; i < clats.size(); i++) {
		Point preP(clons[i - 1] - marginPos.x, szImage.height - clats[i - 1] - marginPos.y);
		Point curP(clons[i] - marginPos.x, szImage.height - clats[i] - marginPos.y);
		line(img, preP, curP, Scalar(255, 255, 255), 2);
		Mat pimg = img.clone();
		circle(pimg, curP, 5, Scalar(255, 255, 0), 2);

		
		putText(pimg, times[i].c_str(), Point(10, 50), CV_FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 1);

		sprintf(txt, "%.1fm", eles[i]);
		putText(pimg, txt, Point(20, szImage.height - 20), CV_FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255), 1);
		imshow("map", pimg);
		waitKey(10);
	}

	waitKey(0);
	return 0;
}

