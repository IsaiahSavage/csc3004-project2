// Verse class function definitions
// Computer Science, MVNU
// Updated: 1/20/22

#include "Verse.h"
#include <iostream>
using namespace std;

Verse::Verse() {  	// Default constructor
    verseText = "Uninitialized Verse!";
    verseRef = Ref();
} 

// REQUIRED: Parse constructor - pass verse string from file
Verse::Verse(const string s) {
	// use Ref constructor to create verseRef
	// store the remainder of the string in verseText
    // not the prettiest, but gets around tight coupling to Ref's nextToken()
    verseRef = Ref(s);
    string refLengthStr(to_string(verseRef.getBook()) + to_string(verseRef.getChap()) + to_string(verseRef.getVerse()));
    size_t verseRefOffset = refLengthStr.length() + 3;
    verseText = s.substr(verseRefOffset);
}

// REQUIRED: Accessors
string Verse::getVerse() {
    return verseText;
}

Ref Verse::getRef() {
    return verseRef;
}

// display reference and verse
void Verse::display() {
    verseRef.display();
    cout << " " << verseText;
 }
