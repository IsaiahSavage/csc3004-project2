/* Demo server program for Bible lookup using AJAX/CGI interface
 * By James Skon, Febrary 10, 2011
 * updated by Bob Kasper, January 2020
 * updated by Isaiah Savage February 2022
 * Mount Vernon Nazarene University
 * 
 * This sample program works using the cgicc AJAX library to
 * allow live communication between a web page and a program running on the
 * same server that hosts the web server.
 *
 * This program is run by a request from the associated html web document.
 * A Javascript client function invokes an AJAX request,
 * passing the input form data as the standard input string.
 *
 * The cgi.getElement function parses the input string, searching for the matching 
 * field name, and returing a "form_iterator" oject, which contains the actual
 * string the user entered into the corresponding field. The actual values can be
 * accessed by dereferencing the form iterator twice, e.g. **verse
 *     refers to the actual string entered in the form's "verse" field.
 */

#include "Bible.h"
#include "Ref.h"
#include "Verse.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

/* Required libraries for AJAX to function */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"

using namespace cgicc;

/**
* Print the results returned by the Bible lookup
*
* @param requestedVerseRef: The requested verse
* @param result: The fully-formatted string result
* @param numVerseS: The requested number of verses
* @param search_type: The form field that contains the type of search performed
*/
void printResult(Ref requestedVerseRef, const int numVerses, const string result, const form_iterator search_type) {
	cout << "Search Type: <b>" << **search_type << "</b>" << endl; // search type
	cout << "<h2>" << requestedVerseRef.toString() // requested verse reference
		<< " - <em>" << numVerses << " Verse(s)</em></h2>" << endl; // requested number of verses
	cout << "<p>" << result << "</p>" << endl; // formatted results
}

int main() {
  /* A CGI program must send a response header with content type
   * back to the web client before any other output.
   * For an AJAX request, our response is not a complete HTML document,
   * so the response type is just plain text to insert into the web page.
   */
  cout << "Content-Type: text/plain\n\n";
  
  Cgicc cgi;  // create object used to access CGI request data

  // GET THE INPUT DATA
  // browser sends us a string of field name/value pairs from HTML form
  // retrieve the value for each appropriate field name
  form_iterator st = cgi.getElement("search_type");
  form_iterator book = cgi.getElement("book");
  form_iterator chapter = cgi.getElement("chapter");
  form_iterator verse = cgi.getElement("verse");
  form_iterator nv = cgi.getElement("num_verse");

  // Convert and check input data
  int chapterNum, verseNum, bookNum, numVerses;
  bool validInput = false;
  if (chapter != cgi.getElements().end()) {
	 chapterNum = chapter->getIntegerValue();
	 if (chapterNum > 150) {
		 cout << "<p>The chapter number (" << chapterNum << ") is too high.</p>" << endl;
	 }
	 else if (chapterNum <= 0) {
		 cout << "<p>The chapter must be a positive number.</p>" << endl;
	 }
	 else
		 validInput = true;
  }
  
  if (validInput) {
	  validInput = false;
	  if (verse != cgi.getElements().end()) {
		  verseNum = verse->getIntegerValue();
		  if (verseNum > 176) {
			  cout << "<p>The verse number (" << verseNum << ") is too high.</p>" << endl;
		  }
		  else if (verseNum <= 0) {
			  cout << "<p>The verse number must be a positive number.</p>" << endl;
		  }
		  else {
			  validInput = true;
		  }
	  }
  }

  if (validInput) {
	  validInput = false;
	  if (book != cgi.getElements().end()) {
		  bookNum = book->getIntegerValue();
		  if (bookNum > 66 || bookNum <= 0) {
			  cout << "<p>The book number must be between 1 and 66.</p>" << endl;
		  }
		  else {
			  validInput = true;
		  }
	  }
  }

  if (validInput) {
	  if (nv != cgi.getElements().end()) {
		  numVerses = nv->getIntegerValue();
		  if (numVerses > 31102 || numVerses <= 0) {
			  numVerses = 1;
		  }
	  }
	  else {
		  numVerses = 1;
	  }
  }

  Bible webBible("/home/class/csc3004/Bibles/web-complete");
  Verse requestedVerse;
  LookupResult result;
  Ref ref(bookNum, chapterNum, verseNum);
  string fullResult = ""; // formatted verse(s) to return

  requestedVerse = webBible.lookup(ref, result);
  if (result == SUCCESS) {
	  // Add book & chapter headings
	  fullResult += "<b>" + ref.getBookName() + " " + to_string(ref.getChap()) + "</b><br>";
	  fullResult += "<sup>" + to_string(ref.getVerse()) + "</sup>"; // add superscript verse number
	  fullResult += requestedVerse.getVerse(); // add verse

	  // retrieve rest of verses if requested
	  int expectedBook = ref.getBook(); // expected book; initially set to the book the results start in
	  int expectedChap = ref.getChap(); // expected chapter; initially set to the chapter the results start in
	  for (int i = 0; i < numVerses - 1 && validInput; i++) {
		  Verse nextVerse = webBible.nextVerse(result);
		  if (result == SUCCESS) {
			  // Add book & chapter headings
			  if (nextVerse.getRef().getBook() != expectedBook || nextVerse.getRef().getChap() != expectedChap) {
				  fullResult += "<br><br><b>" + nextVerse.getRef().getBookName() + " " + to_string(nextVerse.getRef().getChap()) + "</b><br>";
				  // change expected book/chapter as needed
				  expectedBook = nextVerse.getRef().getBook();
				  expectedChap = nextVerse.getRef().getChap();
			  }
			  fullResult += " <sup>" + to_string(nextVerse.getRef().getVerse()) + "</sup>";
			  fullResult += nextVerse.getVerse();
		  }
		  else {
			  validInput = false;
		  }
	  }
  }
  else {
	  validInput = false;
  }

  /* SEND BACK THE RESULTS
   * Finally we send the result back to the client on the standard output stream
   * in HTML text format.
   * This string will be inserted as is inside a container on the web page, 
   * so we must include HTML formatting commands to make things look presentable!
   */
  if (validInput) {
	  printResult(ref, numVerses, fullResult, st);
  }
  else {
	  cout << "<p>Invalid Input: <em>" << webBible.error(result) << "</em></p>" << endl;
  }
  return 0;
}
