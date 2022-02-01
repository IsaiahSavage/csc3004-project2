// Bible class function definitions
// Computer Science, MVNU
// Updated: 1/20/22

#include "Ref.h"
#include "Verse.h"
#include "Bible.h" 
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

Bible::Bible() { // Default constructor
	infile = "/home/class/csc3004/Bibles/web-complete";
}

// Constructor – pass bible filename
Bible::Bible(const string s) { infile = s; }

// open bible file. True if file is successfully opened, false otherwise.
bool Bible::openFile() {
	instream.open(infile.c_str(), ios::in);
	if (!instream) {
		return false;
	}
	instream.unsetf(ios::skipws); // include white space in read operations
	isOpen = true;
	return true;
}

// REQUIRED: lookup finds a given verse in this Bible
Verse Bible::lookup(Ref ref, LookupResult& status) { 
    // scan the file to retrieve the line that holds ref ...
    // update the status variable
	status = OTHER; // placeholder until retrieval is attempted
	// create and return the verse object
	Verse aVerse;   // default verse, to be replaced by a Verse object
	                // that is constructed from a line in the file.
	if (!openFile()) {
		cerr << "Error - can't open input file: " << infile << endl;
		exit(2);
	}

	//search file
	string buffer;
	int requestedBook = ref.getBook(), 
		requestedChap = ref.getChap(), 
		requestedVerse = ref.getVerse();

	while (status == OTHER && instream.peek() != EOF && !instream.fail()) {
		getline(instream, buffer);
		aVerse = Verse(buffer);
		// SUCCESS if current verse matches requested one
		if (aVerse.getRef() == ref) {
			status = SUCCESS;
		}
		else if (aVerse.getRef().getBook() > requestedBook) {
			// Book is valid, but chapter did not exist
			status = NO_CHAPTER;
		}
		else if (aVerse.getRef().getBook() == requestedBook && aVerse.getRef().getChap() > requestedChap) {
			// Book is correct and chapter is valid, but verse did not exist
			status = NO_VERSE;
		}
	}
	// End of file edge case check
	if (instream.peek() == EOF) {
		if (aVerse.getRef().getBook() == requestedBook) {
			if (aVerse.getRef().getChap() == requestedChap) {
					status = NO_VERSE;
			}
			else {
				status = NO_CHAPTER;
			}
		}
		else {
			status = NO_BOOK;
		}
	}
    return aVerse;
}

// REQUIRED: Return the next verse from the Bible file stream if the file is open.
// If the file is not open, open the file and return the first verse.
Verse Bible::nextVerse(LookupResult& status) {
	string buffer;
	Verse aVerse;
	if (!isOpen && !openFile()) {
		status = OTHER;
		cerr << " There was an error with opening input file: " << infile << endl; // Can be removed for production
		exit(2);
	}
	// Check for end of file
	if (instream.peek() == EOF) {
		status = OTHER;
		cerr << " Reached end of file; there are no more verses." << endl; // Can be removed for production
		exit(2); 
	}
	getline(instream, buffer);
	aVerse = Verse(buffer);
	return aVerse;
}

// REQUIRED: Return an error message string to describe status
string Bible::error(LookupResult status) {
	string msg = "";
	switch (status) {
	case NO_BOOK:
		msg = "The requested book does not exist.";
		break;
	case NO_CHAPTER:
		msg = "The requested chapter does not exist.";
		break;
	case NO_VERSE:
		msg = "The requested verse does not exist.";
		break;
	case OTHER:
		msg = "An issue came up while retrieving your verse.";
		break;
	}
	return msg;
}

void Bible::display() {
	cout << "Bible file: " << infile << endl;
}
	
// OPTIONAL access functions
// OPTIONAL: Return the reference after the given ref
Ref Bible::next(const Ref ref, LookupResult& status) { return Ref(); }

// OPTIONAL: Return the reference before the given ref
Ref Bible::prev(const Ref ref, LookupResult& status) { return Ref(); }
