// Bible class function definitions
// Computer Science, MVNU
// Updated: 1/20/22

#include "Ref.h"
#include "Verse.h"
#include "Bible.h" 
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

Bible::Bible() { // Default constructor
	infile = "/home/class/csc3004/Bibles/web-complete";
	bool indexBuildStatus = buildTextIndexByRef();
	if (!indexBuildStatus) {
		exit(2);
	}
}

// Constructor – pass bible filename
Bible::Bible(const string s) { 
	infile = s;
	bool indexBuildStatus = buildTextIndexByRef();
	if (!indexBuildStatus) {
		exit(2);
	}
}

// Find issues with nonexistent references
// Used as a helper function for lookups
LookupResult Bible::findErrorInRef(const Ref ref) {
	LookupResult error = OTHER;
	int requestedBook = ref.getBook(),
		requestedChap = ref.getChap(),
		requestedVerse = ref.getVerse();

	Ref checkBookRef(requestedBook, 1, 1);
	if (refs.count(checkBookRef)) {
		// Book found, check chapter
		Ref checkChapRef(requestedBook, requestedChap, 1);
		if (refs.count(checkChapRef)) {
			// Chapter found, must be nonexistent verse
			error = NO_VERSE;
		}
		else {
			error = NO_CHAPTER;
		}
	}
	else {
		error = NO_BOOK;
	}

	return error;
}

// Open bible file
// True if file is successfully opened, false otherwise
bool Bible::openFile() {
	if (!instream.is_open()) {
		instream.open(infile.c_str(), ios::in);
		if (!instream) {
			return false;
		}
		instream.unsetf(ios::skipws); // include white space in read operations
		isOpen = true;
	}
	return true;
}

// Construct inverted index by Ref
// Returns 0 if file cannot be opened; 1 otherwise
bool Bible::buildTextIndexByRef() {
	int position;
	string line;

	if (!openFile()) {
		cerr << "Cannot open file " << infile << endl;
		return false;
	}

	while (!instream.fail() && instream.peek() != EOF) {
		// Get file position of line
		position = instream.tellg();
		// Read line and build Ref
		getline(instream, line);
		Ref reference(line);
		refs[reference] = position;
	} // end while loop

	instream.close();
	return true;
}

// Return index of references
map<Ref, int> Bible::getRefIndex() {
	return refs;
}

// Search index of Refs for a particular Ref
// Returns the bit offset associated with the Ref
// If not found, returns -1
int Bible::indexSearchByRef(const Ref ref) const {
	int offset = 0;
	auto it = refs.find(ref);
	if (it == refs.end()) {
		return -1;
	}
	else {
		offset = it->second;
	}
	return offset;
}

// lookup finds a given verse in this Bible
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

	// Attempt to find Ref in index
	int verseOffset = indexSearchByRef(ref);

	if (verseOffset >= 0) {
		status = SUCCESS;
		// Get and set file offset for verse
		instream.seekg(verseOffset);
		// Read line
		getline(instream, buffer);
		aVerse = Verse(buffer);
	}
	else { // Ref not found; find where error occurred
		status = findErrorInRef(ref);
	}

	/* Iterable version of lookup
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
	*/

    return aVerse;
}

// TODO: convert to using indexSearchByRef() function
// NOTE: I believe that this is already done; it should require only minor changes if not
// Return the next verse from the Bible file stream if the file is open.
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

// Return an error message string to describe status
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
	
// TODO: write using indexSearchByRef() function
// Return the reference after the given ref
Ref Bible::next(const Ref ref, LookupResult& status) { 
	auto it = refs.find(ref);
	if (it != refs.end()) {
		it++;
		if (it != refs.end()) {
			status = SUCCESS;
			return it->first;
		}
		else {
			status = findErrorInRef(ref);
		}
	}
	// TODO: what to return here?
	return Ref();
}

// TODO: write using indexSearchByRef() function
// Return the reference before the given ref
Ref Bible::prev(const Ref ref, LookupResult& status) { 
	auto it = refs.find(ref);
	if (it != refs.end()) {
		it--;
		if (it != refs.begin()) {
			status = SUCCESS;
			return it->first;
		}
		else {
			status = findErrorInRef(ref);
		}
	}
	// TODO: what to return here?
	return Ref();
}
