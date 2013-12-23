// SmokeDetection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <iostream>

using namespace std;
using namespace cv;

Mat readImage(const string &filename, int flags=1);
uchar getChannelAvg(Mat chan);

int _tmain(int argc, _TCHAR* argv[])
{
	char *imagepath = "F:\\Download\\Smoke\\0080.jpg";
	Mat img = readImage(imagepath);
	assert(img.channels() == 3);


	Mat bin(img.rows, img.cols,CV_8UC1, 0);
	bin.create(img.rows, img.cols, CV_8UC1);
	Mat img_yuv;
    cvtColor(img, img_yuv, CV_BGR2YCrCb);
	
	vector<Mat> planes;
    split(img_yuv, planes); 
	
	int y_avg = getChannelAvg(planes[0]);
	int Cr_avg = getChannelAvg(planes[1]);
	int Cb_avg = getChannelAvg(planes[2]);

    for( int y = 0; y < img_yuv.rows; y++ )
    {
        uchar* ptr_y = planes[0].ptr<uchar>(y);
		uchar* ptr_Cr = planes[1].ptr<uchar>(y);
		uchar* ptr_Cb = planes[2].ptr<uchar>(y);

		uchar *ptr_bin = bin.ptr<uchar>(y);
        for( int x = 0; x < img_yuv.cols; x++ )
        {
			ptr_bin[x] = 255;
			if (ptr_y[x] <= ptr_Cb[x])
				ptr_bin[x] = 0;
			if (ptr_Cr[x] <= ptr_Cb[x])
				ptr_bin[x] = 0;
			if (ptr_y[x] <= y_avg)
				ptr_bin[x] = 0;
			if (ptr_Cb[x] >= Cb_avg)
				ptr_bin[x] = 0;
			if (ptr_Cr[x] <= Cr_avg)
				ptr_bin[x] = 0;
			if ((int)ptr_Cr[x] - (int)ptr_Cb[x] < 55)
				ptr_bin[x] = 0;
        }
    }
	namedWindow(imagepath, WINDOW_NORMAL);
	imshow(imagepath, img);

	namedWindow("FireBin", WINDOW_NORMAL);
	imshow("FireBin", bin);
	waitKey();
	return 0;
}

Mat readImage(const string &filename, int flags){
	Mat img = imread(filename, flags);
	if(img.empty())
    {
        fprintf(stderr, "Can not load image %s\n", filename);
        exit(-1);
    }
	if(!img.data) // check if the image has been loaded properly
	{
        fprintf(stderr, "No image data from %s\n", filename);
        exit(-1);
    }
	return img;
}

uchar getChannelAvg(Mat chan){
	assert(chan.channels() == 1);
	double sum = 0;
	for( int y = 0; y < chan.rows; ++y)
    {
        uchar* ptr = chan.ptr<uchar>(y);
		for (int x = 0; x < chan.cols; ++x){
			sum += ptr[x];
		}
	}
	return (uchar)(sum/chan.rows * chan.cols);
}