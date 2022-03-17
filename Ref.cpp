// Ref class function definitions
// Computer Science, MVNU
// Updated: 1/20/22

#include "Ref.h"
#include <iostream>
#include <string>
using namespace std;

// GetNextToken - general routine for getting next token from a string
// Parameters
//    str - string to search. search is destructive, i.e., token is removed.
//    delimiters - string, a list of characters to use a delimiters between tokens
// Returns: string token (removed from str)

string GetNextToken(string& str, const string& delimiters = " ") {
  // Skip delimiters at beginning                                          
  string::size_type startPos = str.find_first_not_of(delimiters, 0);
  // Find position of delimiter at end of token                                           
  string::size_type endPos = str.find_first_of(delimiters, startPos);

  // Found a token, remove it from string, and return it   
  string next = str.substr(startPos, endPos - startPos);
  string rest = str.substr(endPos - startPos + 1, string::npos);
  str = rest;
  return(next);
}

// Ref member functions

Ref::Ref() {book = 0; chap = 0; verse = 0;}  	// Default constructor

Ref::Ref(const string s) { // Parse constructor - receives a line "34:5:7 text"
    string rtext = s; // make local copy of string to avoid modifying parameter
    // parse the reference - notice, currently there is no error checking!
    // Get book number
    string strbook = GetNextToken(rtext,":");
    book = atoi(strbook.c_str());
    // Get the chapter number
    string strchap = GetNextToken(rtext,":");
    chap = atoi(strchap.c_str());
    // Get the verse number
    string strverse = GetNextToken(rtext," ");
    verse = atoi(strverse.c_str());
}

Ref::Ref(const int b, const int c, const int v) { 	// Construct Ref from three integers
	book = b;
	chap = c;
	verse = v;
}

// Accessors
int Ref::getBook() const {return book;}	 // Access book number
int Ref::getChap() const {return chap;}	 // Access chapter number
int Ref::getVerse() const {return verse;}; // Access verse number

// TO DO: implement comparison functions
// REQUIRED: == comparison
bool Ref::operator==(const Ref r) const { 
    return (book == r.getBook() && chap == r.getChap() && verse == r.getVerse());
}

// OPTIONAL: define < and > comparisons
bool Ref::operator<(const Ref r) const {
    bool isLessThan = false;
    if (book < r.getBook()) {
        isLessThan = true;
    }
    else if (book == r.getBook()) {
        if (chap < r.getChap()) {
            isLessThan = true;
        }
        else if (chap == r.getChap()) {
            if (verse < r.getVerse()) {
                isLessThan = true;
            }
        }
    }
    return isLessThan;
}
bool Ref::operator>(const Ref r) const {
    bool isGreaterThan = false;
    if (book > r.getBook()) {
        isGreaterThan = true;
    }
    else if (book == r.getBook()) {
        if (chap > r.getChap()) {
            isGreaterThan = true;
        }
        else if (chap == r.getChap()) {
            if (verse > r.getVerse()) {
                isGreaterThan = true;
            }
        }
    }
    return isGreaterThan;
}

void Ref::display() { 	// Display Reference
     cout << getBookName() << " " << chap << ":" << verse;
}

// returns reference with book name (e.g., Genesis 1:1)
string Ref::toString() {
    return getBookName() + " " + to_string(chap) + ":" + to_string(verse);
}

// returns integer version of reference (e.g., 1:1:1)
string Ref::toStringNumber() {
    return to_string(book) + ":" + to_string(chap) + ":" + to_string(verse);
}

string Ref::getBookName() {
    string bookNames[66] = {
         "Genesis",
         "Exodus",
         "Leviticus",
         "Numbers",
         "Deuteronomy",
         "Joshua",
         "Judges",
         "Ruth",
         "1 Samuel",
         "2 Samuel",
         "1 Kings",
         "2 Kings",
         "1 Chronicles",
         "2 Chronicles",
         "Ezra",
         "Nehemiah",
         "Esther",
         "Job",
         "Psalm",
         "Proverbs",
         "Ecclesiastes",
         "Song of Solomon",
         "Isaiah",
         "Jeremiah",
         "Lamentations",
         "Ezekiel",
         "Daniel",
         "Hosea",
         "Joel",
         "Amos",
         "Obadiah",
         "Jonah",
         "Micah",
         "Nahum",
         "Habakkuk",
         "Zephaniah",
         "Haggai",
         "Zechariah",
         "Malachi",
         "Matthew",
         "Mark",
         "Luke",
         "John",
         "Acts",
         "Romans",
         "1 Corinthians",
         "2 Corinthians",
         "Galatians",
         "Ephesians",
         "Philippians",
         "Colossians",
         "1 Thessalonians",
         "2 Thessalonians",
         "1 Timothy",
         "2 Timothy",
         "Titus",
         "Philemon",
         "Hebrews",
         "James",
         "1 Peter",
         "2 Peter",
         "1 John",
         "2 John",
         "3 John",
         "Jude",
         "Revelation"
    };
    return bookNames[book - 1];
}
