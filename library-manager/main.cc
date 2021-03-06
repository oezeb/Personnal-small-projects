#include <bits/stdc++.h>

using namespace std;


class User {
	string id, password, name;
	unordered_map<string, int> books;
public:
	User(string id, string password) : User(id,password,"") {}

	User(string id, string password, string name) : id(id), password(password), name(name) {}

	vector<string> details() const { return { getid(), getpassword(), getname() }; }

	string getid() const { return id; }
	
	string getpassword() const { return password; }
	
	string getname() const { return name; }
        
	void borrow(string barcode) {
		unordered_map<string,int>::iterator it = books.find(barcode);
		if(it == books.end()) {
			books.insert(pair<string,int>(barcode,1));
		}
		else {
			it->second++;
		}
	}

	void back(string barcode) {
		unordered_map<string,int>::iterator it = books.find(barcode);
		if(it != books.end()) {
			if(--it->second == 0) {
				books.erase(barcode);
			}
		}
	}
};

class Book {
	string barcode, name, author;
	string* userid;
public:
	Book() : Book("","","") {}

	Book(string barcode, string name, string author) : barcode(barcode), name(name), author(author), userid(NULL) {}

	string getbarcode() const { return barcode; }

	string getname() const { return name; }

	string getauthor() const { return author; }

	string* getuserid() { return userid; }

	void setuserid(string* id) { userid = id; }

	vector<string> details() const { return { barcode, name, author }; }
};


class Library {
	unordered_map<string,Book*> books;
	unordered_map<string,User*> users;
	User* currUser;
public:
	Library() : Library("","") {}

	Library(string books, string users) : Library(*(new ifstream(books,ios::in)),*(new ifstream(users,ios::in))) { }

	Library(ifstream& bookstream, ifstream& userstream) : currUser(NULL) {
		string txt = filetostring(bookstream);
		for(auto& i: parsexml(txt,"book")) {
			string barcode = parsexml(i,"barcode")[0];
			string name = parsexml(i,"name")[0];
			string author = parsexml(i,"author")[0];
			books.insert(pair<string,Book*>(barcode,new Book(barcode,name,author)));
		}

		txt = filetostring(userstream);
		for(auto& i: parsexml(txt,"user")) {
			string id = parsexml(i,"id")[0];
			string name = parsexml(i,"name")[0];
			string password = parsexml(i,"password")[0];
			users.insert(pair<string,User*>(id,new User(id,password,name)));
		}
	}

	vector<string> parsexml(string& txt, string tag) {
		vector<string> collections;
		int start = 0;
		while(true) {
			start = txt.find("<"+tag,start); if(start == string::npos) break;
			start = txt.find(">",start); start++;

			int pos = txt.find("</"+tag,start); if(pos == string::npos) break;
			collections.push_back(txt.substr(start,pos-start));
			start = pos;
		}
		return collections;
	}

	string filetostring(ifstream& in) {
		string buffer = "";
		while(in.is_open() && !in.eof()) {
			string line;
			getline(in,line);
			buffer+=line;
		}
		return buffer;
	}


	void search(string key) {
		unordered_map<string,Book*>::iterator it = books.find(key);
		if(it == books.end()) {
			cout << "Not found" << endl;
		}
		else {
			Book* book = it->second;
			if(!book->getuserid()) {
				for(auto& e: book->details()) 
					cout << e << endl;
			}
		}
	}

	void borrow(string key) {
		if(loggedin()) {
			unordered_map<string,Book*>::iterator it = books.find(key);		
			if(it == books.end()) {
				cout << "Not found" << endl;
			}
			else {
				Book* book = it->second;
				if(!book->getuserid()) {
					book->setuserid(new string(currUser->getid()));
					currUser->borrow(book->getbarcode());
				}
				else {
					cout << "Book already out" << endl;
				}
			}
		}
		else {
			cout << "Not logged in" << endl;
		}
	}

	void back(string key) {
		unordered_map<string,Book*>::iterator it = books.find(key);
		if(it == books.end()) {
			cout << "Book Not found" << endl;
		}
		else {
			Book* book = it->second;
			string* userid = book->getuserid();
			if(userid) {
				unordered_map<string,User*>::iterator it = users.find(*userid);
				if(it != users.end()) {
					User* user = it->second;
					user->back(book->getbarcode());
					book->setuserid(NULL);
				}
			}
		}
	}

	bool login(string id, string password) {
		if(loggedin()) {
			cout << "Already logged in, logout first" << endl;
			return false;
		}
		
		unordered_map<string, User*>::iterator it = users.find(id);
		if(it != users.end() && it->second->getpassword() == password) {
			currUser = it->second;
			return true;
		}
		cout << "ID or Password not namtch !!!!!" << endl;
		return false;
	}

	bool login() {
		string id, password;
		cout << "Login" << endl << "ID : ";
		cin >> id;
		cout << "Password : ";
		cin >> password;
		return login(id,password);
	}

	bool loggedin() { return currUser != NULL; }

	void logout() { currUser = NULL; }

	vector<string> userDetails() {
		if(loggedin()) {
			return currUser->details();
		}
		else {
			return { "Not logged in" };
		}
	}
};


int main() {
	Library* lib = new Library("books.xml","users.xml");
	while(true) {
		int choice;
		string s;
		cout << "1. Search" << endl << "2. Query" << endl << "3. Borrow" << endl << "4. Return" << endl << "5.Exit" << endl;
		cin >> choice;
		switch(choice) {
			case 1:
				cout << "Search by Book Barcode No: ";
				cin >> s;
				lib->search(s);
				break;
			case 2:
				if(!lib->loggedin())
                                       while(!lib->login());
				for(auto& e : lib->userDetails())
				       cout << e << endl;
				break;
			case 3:
				if(!lib->loggedin())
                                       while(!lib->login());
				cout << "Enter the book Barcode No : ";
				cin >> s;
				lib->borrow(s);
				break;
			case 4:
				cout << "Enter book Barcode No : ";
				cin >> s;
				lib->back(s);
				break;
			case 5:
				return 0;

		default:
				break;
		}
	}
	return 0;
}
