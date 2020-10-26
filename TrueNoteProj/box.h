#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <math.h>
#pragma once

using namespace cv;
using namespace std;

class Box {
public:
	Box();
	Box(int x, int y, float w, float h);
	const int getX() const;
	const int getY() const;
	const float getWidth() const;
	const float getHeight() const;
	const float getCenterX() const;
	const float getCenterY() const;
	const float distance(Box b) const; //distance between 2 boxes
	const float intersection(Box b) const; //find how much two boxes intersect
	Box concat(Box b);
	void trace(Mat img, int b, int g, int r, int thickness) const;
private:
	int xPos, yPos;
	float width, height, centerX, centerY, area;
};
