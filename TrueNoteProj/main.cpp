#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <unordered_map>
#include <cmath>
#include <iterator>
#include "match.h" //linker will link the matching .cpp file
#include "box.h"
#include "note.h"

using namespace cv;
using namespace std;

int main()
{
	Mat grayImage;
	Mat image = imread("resources/samples/emblem.jpg");
	vector<Box> staffMatches, sharpMatches, flatMatches, quarterMatches, halfMatches, wholeMatches;
	vector<Note> allNotes, barNotes, sharps, flats;
	unordered_map<int, int> heightMap;
	int width, height;

	//convert image to grayscale for easier template matching
	cvtColor(image, grayImage, COLOR_BGR2GRAY);

	width = grayImage.cols;
	height = grayImage.rows;

	//If pixel value is less than 127, set to 0. Greater than 255, set to 255.
	threshold(grayImage, grayImage, 127, 255, THRESH_BINARY);

	//finding possible locations of measures
	cout << "Finding measure locations..." << endl;
	staffMatches = containerizeMatches(grayImage, staffArr, 95, 119, staffThres);

	//find the average occurences of heights (how long our staves are)
	cout << "Finding measure heights..." << endl;
	for (const Box& b : staffMatches) {
		int currentHeight = b.getY();

		if (heightMap.find(currentHeight) == heightMap.end()) {
			heightMap[currentHeight] = 1;
		}
		else {
			heightMap[currentHeight]++;
		}
	}

	//discard bad staff matches
	cout << "Discarding false positives..." << endl;
	int sum = 0, total = 1; //total starts at 1 to slightly lower the threshold needed to not be discarded
	double avg;
	for (auto& [key, value] : heightMap) {
		sum += value;
		total++;
	}

	avg = (double)sum / (double)total;

	for (auto it = staffMatches.begin(); it != staffMatches.end();) {
		//cout << it->getY() << endl;
		if (heightMap.at(it->getY()) < avg) {
			it = staffMatches.erase(it);
		}
		else {
			++it;
		}
	}

	//combine staff matches in the same line
	cout << "Constructing bar lines..." << endl;
	staffMatches = mergeStaffs(staffMatches, width);

	//get notes, sharps, flats
	cout << "Determining key signatures..." << endl;
	sharpMatches = findSymbols(grayImage, sharpArr, 95, 119, sharpThres);
	flatMatches = findSymbols(grayImage, flatArr, 95, 119, flatThres);

	cout << "Finding notes..." << endl;
	quarterMatches = findSymbols(grayImage, quarterArr, 95, 119, quarterThres);
	halfMatches = findSymbols(grayImage, halfArr, 95, 119, halfThres);
	wholeMatches = findSymbols(grayImage, wholeArr, 95, 119, wholeThres);

	//drawing matches on image
	drawMatches(image, sharpMatches);
	drawMatches(image, flatMatches);
	drawMatches(image, quarterMatches);
	drawMatches(image, halfMatches);
	drawMatches(image, wholeMatches);

	//Get notes' letter
	cout << "Matching notes to measures..." << endl;
	for (const Box& bar : staffMatches) {
		findSymbolsInBar(bar, sharpMatches, "#", sharps);
		findSymbolsInBar(bar, flatMatches, "b", flats);
		findSymbolsInBar(bar, quarterMatches, "Quarter", barNotes, sharps, flats);
		findSymbolsInBar(bar, halfMatches, "Half", barNotes, sharps, flats);
		findSymbolsInBar(bar, wholeMatches, "Whole", barNotes, sharps, flats);

		sort(barNotes.begin(), barNotes.end(), [](const Note& n1, const Note& n2) {
			return n1.getContainer().getX() < n2.getContainer().getX(); //> = descending order, < = ascending order
			});

		allNotes.insert(allNotes.end(), barNotes.begin(), barNotes.end());
		barNotes.clear();
		sharps.clear();
		flats.clear();
	}

	printNotes(allNotes);

	String windowName = "Template Matches";
	namedWindow(windowName, WINDOW_NORMAL);
	resizeWindow(windowName, width, height);
	imshow(windowName, image);
	waitKey(0);
	destroyWindow(windowName);
	return 0;
}