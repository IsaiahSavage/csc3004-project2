/* Client-side program for Bible lookup using AJAX/CGI interface
 * By Isaiah Savage, March 2022
 * Mount Vernon Nazarene University
 */

#include "Bible.h"
#include "fifo.h"
#include "Ref.h"
#include "Verse.h"
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string.h>
using namespace std;

/* Required libraries for AJAX to function */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"
using namespace cgicc;

#define logging
#define LOG_FILENAME "/home/class/csc3004/tmp/ISASAVAGE-PROJECT2.log"
#include "logfile.h"

static const string receive_pipe = "bible_reply";
static const string send_pipe = "bible_request";

/**
* Print the results returned by the Bible lookup
*
* @param requestedVerseRef: The requested verse
* @param result: The fully-formatted string result
* @param numVerses: The requested number of verses
* @param search_type: The form field that contains the type of search performed
*/
void printResult(Ref requestedVerseRef, const int numVerses, const string result, const form_iterator search_type) {
	cout << "Search Type: <b>" << **search_type << "</b>" << endl; // search type
	cout << "<h2>" << requestedVerseRef.toString() // requested verse reference
		<< " - <em>" << numVerses << " Verse(s)</em></h2>" << endl; // requested number of verses
	cout << "<p>" << result << "</p>" << endl; // formatted results
}

int main() {

	#ifdef logging
		logFile.open(logFilename.c_str(), ios::out);
	#endif

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
	LookupResult result;
	Ref ref(bookNum, chapterNum, verseNum);
	string fullResult = ""; // formatted verse(s) to return

	Fifo recFifo(receive_pipe);
	Fifo sendFifo(send_pipe);

	// Send fifo request
	sendFifo.openwrite();
	string fifoRequest = "lookup " + ref.toStringNumber();
	log("sending request to server: " + fifoRequest);
	sendFifo.send(fifoRequest);

	// Retrieve fifo reply
	recFifo.openread();
	string fifoReply = recFifo.recv();
	log("received reply from server: " + fifoReply);

	// Parse reply, checking status of reply first
	int replyStatus = stoi(GetNextToken(fifoReply, " "));
	result = static_cast<LookupResult>(replyStatus);
	if (result == SUCCESS) {
		unique_ptr<Verse> replyVerse(new Verse(fifoReply));
		Ref replyRef = replyVerse->getRef();

		// Add book & chapter headings
		fullResult += "<b>" + replyRef.getBookName() + " " + to_string(replyRef.getChap()) + "</b><br>";
		fullResult += "<sup>" + to_string(replyRef.getVerse()) + "</sup>"; // add superscript verse number
		fullResult += replyVerse->getVerse(); // add verse

		// retrieve rest of verses if requested
		int expectedBook = replyRef.getBook(); // expected book; initially set to the book the results start in
		int expectedChap = replyRef.getChap(); // expected chapter; initially set to the chapter the results start in
		for (int i = 0; i < numVerses - 1 && validInput; i++) {

			// Send next fifo request
			fifoRequest = "next " + replyRef.toStringNumber();
			log("sending request to server: " + fifoRequest);
			sendFifo.send(fifoRequest);

			fifoReply = recFifo.recv();
			log("received reply from server: " + fifoReply);

			replyStatus = stoi(GetNextToken(fifoReply, " "));
			result = static_cast<LookupResult>(replyStatus);
			if (result == SUCCESS) {
				// verse found; request verse text from server
				Ref nextRef(fifoReply);
				fifoRequest = "lookup " + nextRef.toStringNumber();
				log("sending request to server: " + fifoRequest);
				sendFifo.send(fifoRequest);

				// retrieve server reply
				fifoReply = recFifo.recv();
				log("received reply from server: " + fifoReply);

				int replyStatus = stoi(GetNextToken(fifoReply, " "));
				result = static_cast<LookupResult>(replyStatus);

				if (result == SUCCESS) {
					// change pointer to next verse
					replyVerse.reset(new Verse(fifoReply));
					replyRef = replyVerse->getRef();

					// Add new book & chapter headings as needed
					if (replyRef.getBook() != expectedBook || replyRef.getChap() != expectedChap) {
						fullResult += "<br><br><b>" + replyRef.getBookName() + " " + to_string(replyRef.getChap()) + "</b><br>";
						// change expected book/chapter as needed
						expectedBook = replyRef.getBook();
						expectedChap = replyRef.getChap();
					}
					fullResult += " <sup>" + to_string(replyRef.getVerse()) + "</sup>";
					fullResult += replyVerse->getVerse();
				}
				else {
					log("invalid input: " + webBible.error(result));
					validInput = false;
				}
			}
			else {
				log("invalid input: " + webBible.error(result));
				validInput = false;
			}
		}
		// Final deallocation of memory
		replyVerse.reset(nullptr);
	}
	else {
		log("invalid input: " + webBible.error(result));
		validInput = false;
	}

	log("finished request. closing pipes...");
	recFifo.fifoclose();
	sendFifo.fifoclose();

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
