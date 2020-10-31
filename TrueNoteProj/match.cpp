#include "match.h"

vector<Mat> staffArr = { imread("images/templates/staff.png", 0), imread("images/templates/staff2.png", 0) };
vector<Mat> quarterArr = { imread("images/templates/quarter.png", 0), imread("images/templates/solid-note.png", 0) };
vector<Mat> sharpArr = { imread("images/templates/sharp.png", 0) };
vector<Mat> flatArr = { imread("images/templates/flat-line.png", 0), imread("images/templates/flat-space.png", 0) };
vector<Mat> halfArr = { imread("images/templates/half-space.png", 0), imread("images/templates/half-note-line.png", 0),
					imread("images/templates/half-line.png", 0) , imread("images/templates/half-note-space.png", 0) };
vector<Mat> wholeArr = { imread("images/templates/whole-space.png", 0) , imread("images/templates/whole-note-line.png", 0),
					imread("images/templates/whole-line.png", 0) , imread("images/templates/whole-note-space.png", 0) };

int staffLower = 50, sharpLower = staffLower, flatLower = staffLower, quarterLower = staffLower, halfLower = staffLower, wholeLower = staffLower;
int staffUpper = 150, sharpUpper = staffUpper, flatUpper = staffUpper, quarterUpper = staffUpper, halfUpper = staffUpper, wholeUpper = staffUpper;
float staffThres = .77f, sharpThres = .70f, flatThres = .77f, quarterThres = .7f, halfThres = .7f, wholeThres = .7f;

float match(Mat img, const vector<Mat>& templates, int start, int stop, float thres, vector<vector<Point>>& matchedLocations) {
	vector<vector<Point>> locations;// matches for each template allocated in one vector
	vector<Point> templLocations;//individual matches for each template
	Mat result;
	size_t maxMatchedLocations = 0; // vector sizes are unsigned, so can't compare to int, size_t is unsigned so no negatives
	int imgWidth = img.cols, imgHeight = img.rows;
	float closestScale = 0.0f;

	for (int i = start; i < stop + 1; i += 3) {
		Mat newTempl;
		size_t currentMatches = 0;
		float currentScale = i / 100.0f;//We scale our template by different percentages to find the best size

		for (Mat templ : templates) {
			//resize template for different size sheets
			if (currentScale >= 1) {
				resize(templ, newTempl, Size(), currentScale, currentScale, INTER_CUBIC);
			}
			else if (currentScale < 1) {
				resize(templ, newTempl, Size(), currentScale, currentScale, INTER_AREA);
			}

			//create result matrix
			result.create(img.rows - newTempl.rows + 1, img.cols - newTempl.cols + 1, CV_32FC1);

			//template matching and normalization for match discarding
			matchTemplate(img, newTempl, result, TM_CCOEFF_NORMED);

			//find where the normalized template score is above a threshold in result matrix
			for (int i = 0; i < result.rows; ++i) {
				const float* p = result.ptr<float>(i);
				for (int j = 0; j < result.cols; ++j) {
					if (p[j] >= thres) {
						templLocations.push_back(Point(j, i));
					}
				}
			}

			currentMatches += templLocations.size();
			locations.push_back(templLocations);

			result = Mat();
			templLocations.clear();
		}
		//Determine if currrent scale has found more matches
		if (currentMatches > maxMatchedLocations) {
			maxMatchedLocations = currentMatches;
			closestScale = currentScale;
			matchedLocations = locations;
		}

		locations.clear();
		currentMatches = 0;
	}
	return closestScale;
}

vector<Box> containerizeMatches(Mat img, const vector<Mat>& templArray, int start, int stop, float thres) {
	vector<Box> matchBoxes;
	vector<vector<Point>> matchPoints;//a vector for each template's matches
	float optimalScale = match(img, templArray, start, stop, thres, matchPoints);

	if (matchPoints.empty()) {
		return matchBoxes;
	}

	//regular for-loop used to be able to access templArray in sequence in addition to matchPoints
	for (int i = 0; i < templArray.size(); i++) {
		float templWidth = templArray.at(i).cols;
		float templHeight = templArray.at(i).rows;
		templWidth *= optimalScale;
		templHeight *= optimalScale;

		for (const Point& p : matchPoints.at(i)) {
			matchBoxes.push_back(Box(p.x, p.y, templWidth, templHeight));
		}
	}
	return matchBoxes;
}

vector<Box> removeIntersectingMatches(vector<Box>& matches, float thres) {
	vector<Box> cleanedMatches;

	while (matches.size() > 0) {
		Box b = matches.back();
		matches.pop_back();
		sort(matches.begin(), matches.end(), [b](const Box& b1, const Box& b2) {
			return b1.distance(b) > b2.distance(b); //> = descending order, < = ascending order
			});

		bool contiguous = true;

		while (contiguous) {
			contiguous = false;
			//iterate in reverse to more efficiently call erase()
			for (auto it = matches.rbegin(); it != matches.rend();) {
				if (b.intersection(*it) > thres || it->intersection(b) > thres) {
					b = b.concat(*it);
					it = decltype(it)(matches.erase(next(it).base()));
					contiguous = true;
				}
				else if (it->distance(b) > b.getWidth() / 2.0f + it->getWidth() / 2.0f) {
					break;
				}
				else {
					++it;
				}
			}
		}
		cleanedMatches.push_back(b);
	}
	return cleanedMatches;
}

vector<Box> mergeStaffs(vector<Box>& staffMatches, int imgWidth) {
	vector<Box> staffMerged;
	int currentY;
	float currentH;

	sort(staffMatches.begin(), staffMatches.end(), [](const Box& b1, const Box& b2) {
		return b1.getY() < b2.getY(); //> = descending order, < = ascending order
		});

	currentY = staffMatches.at(0).getY();
	currentH = staffMatches.at(0).getHeight();
	for (const Box& b : staffMatches) {
		if (b.getY() - currentY > 2) {
			staffMerged.push_back(Box(0, currentY, (float)imgWidth, currentH));
			currentY = b.getY();
			currentH = b.getHeight();
		}
	}
	staffMerged.push_back(Box(0, currentY, (float)imgWidth, currentH));
	return staffMerged;
}

vector<Box> findSymbols(Mat img, const vector<Mat>& templArray, int start, int stop, float thres) {
	vector<Box> matches = containerizeMatches(img, templArray, start, stop, thres);
	matches = removeIntersectingMatches(matches, .5f);
	return matches;
}

void drawMatches(Mat image, const vector<Box>& matches) {
	for (const Box& b : matches) {
		b.trace(image, 0, 0, 255, 2);
	}
}

void findSymbolsInBar(const Box& bar, const vector<Box>& symbolMatches, String noteType, vector<Note>& notes, const vector<Note>& sharps, const vector<Note>& flats) {

	for (const Box& match : symbolMatches) {
		if (abs(match.getCenterY() - bar.getCenterY()) < bar.getHeight() * 5.0f * .125f) {
			notes.push_back(Note(match, noteType, bar, sharps, flats));
		}
	}
}

void printNotes(const vector<Note>& notes) {
	for (Note n : notes) {
		cout << n.getName() << " " << n.getType() << endl;
	}
}