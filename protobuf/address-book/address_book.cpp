#include <iostream>
#include <fstream>
#include <vld.h>
#include "addressbook.pb.h"

using namespace std;
void PromptForAddress(tutorial::Person* person)
{
	cout << "Enter person ID number: ";
	
	int id;
	cin >> id;
	person->set_id(id);
	cin.ignore(256, '\n');

	cout << "Enter name: ";
	getline(cin, *person->mutable_name());

	cout << "Enter email address (blank for none): ";
	string email;
	getline(cin, email);
	if (!email.empty()) {
		person->set_email(email);
	}

	while (true) {
		cout << "Enter a phone number (or leave blank to finish): ";
		string number;
		getline(cin, number);
		if (number.empty()) {
			break;
		}

		auto phone_number = person->add_phones();
		phone_number->set_number(number);

		cout << "Is this a mobile, home, or work phone? ";
		string type;
		getline(cin, type);
		if (type == "mobile") {
			phone_number->set_type(tutorial::Person::MOBILE);
		}
		else if (type == "home") {
			phone_number->set_type(tutorial::Person::HOME);
		}
		else if (type == "work") {
			phone_number->set_type(tutorial::Person::WORK);
		}
		else {
			cout << "Unknown phone type.  Using default." << endl;
		}
	}
}

void ListPeople(const tutorial::AddressBook& addr)
{
	for (int i = 0; i < addr.people_size(); ++i) {
		auto const& p = addr.people(i);
		cout << "Person ID: " << p.id() << endl;
		cout << "Name: " << p.name() << endl;
		if (p.has_email()) {
			cout << "Email: " << p.email() << endl;
		}

		for (int j = 0; j < p.phones_size(); j++) {
			const tutorial::Person::PhoneNumber& phone_number = p.phones(j);

			switch (phone_number.type()) {
			case tutorial::Person::MOBILE:
				cout << "  Mobile phone #: ";
				break;
			case tutorial::Person::HOME:
				cout << "  Home phone #: ";
				break;
			case tutorial::Person::WORK:
				cout << "  Work phone #: ";
				break;
			}
			cout << phone_number.number() << endl;
		}
	}
}

int main(int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " ADDRESS_BOOK_FILE" << endl;
		return EXIT_FAILURE;
	}

	tutorial::AddressBook addr;
	{
		// Read the existing address book
		fstream input(argv[1], ios::in | ios::binary);
		if (!input) {
			cout << argv[1] << ": File not found. Creating a new file." << endl;
		}
		else if (!addr.ParseFromIstream(&input)) {
			cerr << "Failed to parse address book." << endl;
			return EXIT_FAILURE;
		}
	}

	// aAdd an address
	/*
	PromptForAddress(addr.add_people());
	{
		// Write the new address book back to disk
		fstream output(argv[1], ios::out | ios::trunc | ios::binary);
		if (!addr.SerializeToOstream(&output)) {
			cerr << "Failed to write address book." << endl;
			return EXIT_FAILURE;
		}
	}
	*/

	// read address book
	ListPeople(addr);

	// Optional: Delete all global objects allocated by libprotobuf
	google::protobuf::ShutdownProtobufLibrary();
	return EXIT_SUCCESS;
}