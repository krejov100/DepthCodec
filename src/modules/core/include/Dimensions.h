#pragma once
#include <iostream>
#include <opencv2/opencv.h>

//TODO add point cloud dimensions
class ImageDimensions {
	short mWidth;
	short mHeight;
public:
	ImageDimensions(short width, short height):mWidth(width), mHeight(height){}
	ImageDimensions(cv::Size cvSize) :mWidth(cvSize.width), mHeight(cvSize.height) {}
	
	size_t area(){
		return size_t(mWidth) * size_t(mHeight)
	}

	friend std::ostream &operator<<(std::ostream &, const ImageDimensions &);
	friend std::istream &operator>>(std::istream &, ImageDimensions &);
};

std::ostream operator<<(std::ostream &os, const ImageDimensions& im)
{
	os << im.mWidth;
	os << im.mHeidth;
}

std::ostream operator>>(std::istream &is ImageDimensions &im)
{
	is >> im.mHeight;
	is >> im.mHeidth;
}