#include "note.h"

unordered_map<int, String> notePositions = {
	{-4, "G5"}, {-3, "F5"}, {-2, "E5"}, {-1, "D5"}, {0, "C5"},
	{1, "B4"}, {2, "A4"}, {3, "G4"}, {4, "F4"}, {5, "E4"}, {6, "D4"}, {7, "C4"},
	{8, "B3"}, {9, "A3"}, {10, "G3"}, {11, "F3"}, {12, "E3"}, {13, "D3"}, {14, "C3"},
	{15, "B2"}, {16, "A2"}, {17, "F2"}
};

Note::Note() {}

Note::Note(Box b, String type, Box bar, const vector<Note>& sharps, const vector<Note>& flats) {
	noteType = type;
	container = b;
	center = b.getHeight() / 2.0f + b.getY();//postion in relation to the bar it's in
	absoluteY = (center - bar.getY()) / bar.getHeight();
	noteLetter = notePositions[(int)(absoluteY / .0625 + .5)];

	//applies key signature to notes
	for (Note n : sharps) {
		if (n.noteLetter.compare(0, 1, noteLetter, 0, 1) == 0 && b.getX() < n.container.getX()) {
			noteLetter.append("#");
		}
	}

	for (Note n : flats) {
		if (n.noteLetter.compare(0, 1, noteLetter, 0, 1) == 0 && b.getX() < n.container.getX()) {
			noteLetter.append("b");
		}
	}
}

const String Note::getName() {
	return noteLetter;
}

const String Note::getType() {
	return noteType;
}

const Box Note::getContainer() const {
	return container;
}