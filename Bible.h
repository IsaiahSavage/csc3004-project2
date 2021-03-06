// Class Bible
// Computer Science, MVNU
//
// A Bible object represents a particular version of the Bible,
// and provides functions to lookup verses and references.
// A Bible object is constructed by giving it a file name string 
// that refers to a file containing the entire text of a version.
// Updated: 1/20/22

#ifndef Bible_H
#define Bible_H

#include "Ref.h"
#include "Verse.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

// status codes to be returned when looking up a reference
enum LookupResult { SUCCESS, NO_BOOK, NO_CHAPTER, NO_VERSE, OTHER };

class Bible {	// A class to represent a version of the bible
 private:
   string infile;		// file path name
   ifstream instream;	// input stream, used when file is open
   bool isOpen;			// true if file is open
   map<Ref, int> refs; // inverted index of references
   // OPTIONAL: you may add variables to keep track of
   // the current line and/or reference when scanning the file
   
   // Finds issues with nonexistent references
   // Used as a helper function for lookups
   LookupResult findErrorInRef(const Ref ref);

 public:
   Bible();	// Default constructor
   Bible(const string s); // Constructor – pass name of bible file

   // Opens infile. True if file is successfully opened, false if not.
   bool openFile();
   
   // Constructs inverted index by Ref
   bool buildTextIndexByRef();

   // Returns index of references
   map<Ref, int> getRefIndex();

   // Searches index of Refs for a particular Ref
   int indexSearchByRef(const Ref ref) const;

   // REQUIRED: Find and return a verse in this Bible, given a reference
   Verse lookup(const Ref ref, LookupResult& status);

   // REQUIRED:
   // Return the next verse from the Bible file stream if the file is open.
   // If the file is not open, open the file and return the first verse.
   Verse nextVerse(LookupResult& status);
   
   // Information functions (REQUIRED)
   // Return an error message string to describe status
   string error(LookupResult status);
   // Show the name of the bible file on cout
   void display();
   
   // OPTIONAL: Return the reference after the given parameter ref
   Ref next(const Ref ref, LookupResult& status);
   // OPTIONAL: Return the reference before the given parameter ref
   Ref prev(const Ref ref, LookupResult& status);
};
#endif //Bible_H
