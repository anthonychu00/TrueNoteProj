#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <math.h>
#include <string>
#include "box.h"
#pragma once

using namespace cv;
using namespace std;

class Note {
public:
	Note();
	Note(Box b, String type, Box bar, const vector<Note>& sharps, const vector<Note>& flats);
	const String getName();
	const String getType();
	const Box getContainer() const;
private:
	Box container;
	String noteType, noteLetter;
	float center, absoluteY;
};
