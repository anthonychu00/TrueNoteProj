#include "box.h"


//implement box member functions here
Box::Box() {}

Box::Box(int x, int y, float w, float h) {
	xPos = x;
	yPos = y;
	width = w;
	height = h;
	centerX = x + w / 2.0f;
	centerY = y + h / 2.0f;
	area = w * h;
}

const int Box::getX() const {
	return xPos;
}

const int Box::getY() const {
	return yPos;
}

const float Box::getWidth() const {
	return width;
}

const float Box::getHeight() const {
	return height;
}

const float Box::getCenterX() const {
	return centerX;
}
const float Box::getCenterY() const {
	return centerY;
}

const float Box::distance(Box b) const {
	float distanceX = centerX - b.centerX;
	float distanceY = centerY - b.centerY;
	return sqrt(distanceX * distanceX + distanceY * distanceY);

}

const float Box::intersection(Box b) const {
	float intersectX = max(0.0f, min(xPos + width, b.xPos + b.width) - max(xPos, b.xPos));
	float intersectY = max(0.0f, min(yPos + height, b.yPos + b.height) - max(yPos, b.yPos));
	float totalIntersection = intersectX * intersectY;
	return totalIntersection / area;
}

Box Box::concat(Box b) {
	int concatX = min(xPos, b.xPos);
	int concatY = min(yPos, b.yPos);
	float mergedW = max(xPos + width, b.xPos + b.width) - concatX;
	float mergedH = max(yPos + height, b.yPos + b.height) - concatY;

	return Box(concatX, concatY, mergedW, mergedH);
}

void Box::trace(Mat img, int b, int g, int r, int thickness) const {
	int tracePosX = xPos;
	int tracePosY = yPos;
	int xSize = (int)(xPos + width);
	int ySize = (int)(yPos + height);
	rectangle(img, Point(tracePosX, tracePosY), Point(xSize, ySize), Scalar(b, g, r), thickness);

}
