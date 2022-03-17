// biblelookupserver.cpp
// Server-side lookup for Ref objects
// Author: Isaiah Savage
// Last Updated: 3/13/22

#include "Bible.h"
#include "fifo.h"
#include "Ref.h"
#include <iostream>
#include <map>
using namespace std;

static const string receive_pipe = "bible_request";
static const string send_pipe = "bible_reply";

int main() {
	Bible webBible;
	map<Ref, int> refs = webBible.getRefIndex();

	cout << "References added: " << refs.size() << endl;
	auto lastElementIt = refs.end();
	lastElementIt--;
	cout << "Offset of last reference added to index: " << (*lastElementIt).second << endl;

	Fifo recFifo(receive_pipe);
	Fifo sendFifo(send_pipe);

	recFifo.openread();

	while (1) {
		string request = recFifo.recv();

		cout << "Received request: " << request << endl;

		// Split request into requested method and Ref
		string requestMethod = GetNextToken(request, " ");
		Ref requestRef(request);

		sendFifo.openwrite();

		cout << "looking up reference: " << requestRef.toStringNumber() << endl;

		// Use the Bible object to retrieve the verse by reference
		LookupResult result;

		if (requestMethod == "lookup") {
			Verse verse = webBible.lookup(requestRef, result);
			string reply = to_string(result) + " ";
			if (result == SUCCESS) {
				reply += verse.getRef().toStringNumber() + " ";
				reply += verse.getVerse();
			}
			cout << "sending reply: " << reply << endl;
			sendFifo.send(reply);
		}
		else if (requestMethod == "next") {
			Ref nextRef = webBible.next(requestRef, result);
			string reply = to_string(result) + " " + nextRef.toStringNumber();
			cout << "sending reply: " << reply << endl;
			sendFifo.send(reply);
		}
		else if (requestMethod == "prev") {
			Ref prevRef = webBible.prev(requestRef, result);
			string reply = to_string(result) + " " + prevRef.toStringNumber();
			cout << "sending reply: " << reply << endl;
			sendFifo.send(reply);
		}
		else {
			result = OTHER;
			cout << "sending reply: " << result << endl;
			sendFifo.send(to_string(result));
		}

		sendFifo.fifoclose();
	}


}
