// testReader.cpp
// Command line driver program for testing Ref index
// Author: Isaiah Savage
// Last Updated: 2/17/22

#include "Bible.h"
#include "Ref.h"
#include <iostream>
#include <map>
using namespace std;

int main(int argc, char** argv) {
	Bible webBible;
	map<Ref, int> refs = webBible.getRefIndex();

	cout << "References added: " << refs.size() << endl;
	auto lastElementIt = refs.end();
	lastElementIt--;
	cout << "Offset of last reference added to index: " << (*lastElementIt).second << endl;

	switch (argc) {
	case 1:
		cerr << "Usage: " << argv[0] << " <book_number (1-66)> <chapter_number> <verse_number> <num_of_verses (optional)>" << endl;
		exit(2);
	case 2:
		cerr << "Missing chapter and verse numbers." << endl;
		exit(2);
	case 3:
		cerr << "Missing verse number." << endl;
		exit(2);
	default:
		break;
	}
	int b, c, v, n;
	b = atoi(argv[1]);
	c = atoi(argv[2]);
	v = atoi(argv[3]);
	n = argc == 5 ? atoi(argv[4]) : 1;
	// preemptive check of book number
	if (b < 1 || b > 66) {
		cerr << "Error: book must be in range 1-66" << endl;
		exit(2);
	}
	// Create a reference from the numbers
	Ref ref(b, c, v);
	Verse verse;

	// Use the Bible object to retrieve the verse by reference
	cout << "Looking up reference: ";
	ref.display();
	cout << endl;

	LookupResult result;
	verse = webBible.lookup(ref, result);
	// Error handling
	if (result == SUCCESS) {
		verse.display();
		cout << endl;
		// display multiple verses
		for (int i = 0; i < n - 1; i++) {
			webBible.nextVerse(result).display();
			cout << endl;
		}
	}
	else {
		cout << webBible.error(result) << endl;
		exit(2);
	}
}