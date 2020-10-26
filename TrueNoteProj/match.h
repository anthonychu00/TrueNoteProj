#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "box.h"
#include "note.h"

using namespace cv;
using namespace std;

//images to template match against
extern vector<Mat> staffArr, quarterArr, sharpArr, flatArr, halfArr, wholeArr;


// scale our image by the percentage so that it can match one of the templates
// lower and upper define the percentage scaling
// threshold defines how close we require something to match said template.
extern int staffLower, sharpLower, flatLower, quarterLower, halfLower, wholeLower;
extern int staffUpper, sharpUpper, flatUpper, quarterUpper, halfUpper, wholeUpper;
extern float staffThres, sharpThres, flatThres, quarterThres, halfThres, wholeThres;


//auxiliary functions for matching against sheet music and other tasks related to it
float match(Mat img, const vector<Mat>& templates, int start, int stop, float thres, vector<vector<Point>>& matchedLocations);
vector<Box> containerizeMatches(Mat img, const vector<Mat>& templArray, int start, int stop, float thres);//put matches in a Box object to more conveniently access details on its location
vector<Box> removeIntersectingMatches(vector<Box>& matches, float thres);//Since we're using a threshold, some matches will appear close together and find the same note. We discard these.
vector<Box> mergeStaffs(vector<Box>& staffMatches, int imgWidth);//Find the most commonly occuring staff heights and make one bar out of them.
vector<Box> findSymbols(Mat img, const vector<Mat>& templArray, int start, int stop, float thres);
void drawMatches(Mat image, const vector<Box>& matches);
void findSymbolsInBar(const Box& bar, const vector<Box>& symbolMatches, String noteType, vector<Note>& notes, const vector<Note>& sharps = {}, const vector<Note>& flats = {});
void printNotes(const vector<Note>& notes);