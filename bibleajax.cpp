/* Demo server program for Bible lookup using AJAX/CGI interface
 * By James Skon, Febrary 10, 2011
 * updated by Bob Kasper, January 2020
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
  
  /* TO DO: OTHER INPUT VALUE CHECKS ARE NEEDED ... but that's up to you! */
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
	  validInput = false;
	  if (nv != cgi.getElements().end()) {
		  numVerses = nv->getIntegerValue();
		  if (numVerses > 31102 || numVerses <= 0) {
			  cout << "<p>The number of verses must be between 1 and 31,102, the total number of verses in the Bible!</p>" << endl;
		  }
		  else {
			  validInput = true;
		  }
	  }
  }
  /* TO DO: PUT CODE HERE TO CALL YOUR BIBLE CLASS FUNCTIONS
   *        TO LOOK UP THE REQUESTED VERSES
   */

  Bible webBible("/home/class/csc3004/Bibles/web-complete");
  Verse requestedVerse;
  LookupResult result;
  Ref ref(bookNum, chapterNum, verseNum);

  if (validInput) {
	  requestedVerse = webBible.lookup(ref, result);
	  if (result == SUCCESS) {
		// TODO: concatenate multiple verses together and 
		// return the result
		/*for (int i = 0; i < numVerses - 1; i++) {
			  verse = webBible.nextVerse(result);
		  }*/
	  }
	  else {
		  validInput = false;
		  cout << "<p>" << webBible.error(result) << "</p>" << endl;
	  }
  }

  /* SEND BACK THE RESULTS
   * Finally we send the result back to the client on the standard output stream
   * in HTML text format.
   * This string will be inserted as is inside a container on the web page, 
   * so we must include HTML formatting commands to make things look presentable!
   */
  if (validInput) {
	cout << "Search Type: <b>" << **st << "</b>" << endl;
	cout << "<p>" << requestedVerse.getRef().toString() << " " << requestedVerse.getVerse() << "</p>" << endl;
  }
  else {
	  cout << "<p>Invalid Input: <em>" << webBible.error(result) << "</em></p>" << endl;
  }
  return 0;
}
