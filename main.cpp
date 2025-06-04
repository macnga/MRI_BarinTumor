#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;


// Total books
class Book {
	protected:
		int id;
		string title;
		string author;
		int quantity;
	
	public:
		Book(int id, const string& title, const string& author, int quantity) : id(id), title(title), author(author), quantity(quantity) {}
		virtual ~Book() = default;
		
		int getID() const {return id;}
		string getTitle() const {return title;}
		string getAuthor() const {return author;}
		int getQuantity() const {return quantity;}
		
		void increaseQuantity() {quantity++;} 
		void decreaseQuantity() {if (quantity > 0) quantity--;	}
		
		virtual void display() const {
		cout << "ID: " << id << "| Title: " << title << "| Author: " << author << "| Quantity: " << quantity << endl; 
		} 
		virtual string toFileString() const {
			stringstream ss;
			ss << "B," << id << "," << title << "," << author << "," << quantity;
			return ss.str();

		} 
}; 


// Borrowable books 
class BorrowableBook : public Book {
	private:
		string borrowDate;
	
	public:
		BorrowableBook(int id, const string& title, const string& author, int quantity) : Book(id, title, author, quantity) {	}
		void borrow(const string& date) {
			if (quantity > 0) {
				borrowDate = date;
				decreaseQuantity(); 
			} else {
				cout << "Book not available for borrow. \n" ;
			} 
		} 
		
		void returnBook() {
			increaseQuantity();
			borrowDate = ""; 
		}
		
		string getBorrowDate() const {return borrowDate;}
		
		void display() const override {
			Book::display();
			if (!borrowDate.empty()) 
				cout << " Last borrowed on: " << borrowDate << endl; 
		}
		
		string toFileString() const override {
			stringstream ss;
			ss << "BB," << id << "," << title << "," << author << "," << quantity;
			return ss.str();
 
		}
}; 


// manager all books
class Manager {
	private:
		vector<Book*> books;
		vector<BorrowedBook> borrowedList;
		int nextID = 1;
	
	public:
		~Manager() {
			for (auto b:books) {
				delete b; 
			}
		}
		
		void addBook (const string& title, const string& author, int quantity) {
			books.push_back(new BorrowableBook(nextID++, title, author, quantity));
			cout << "Book added. \n" ;
		}
		
		void viewBooks() const {
			if (books.empty())  {
				cout << "No books in library. \n";
				return; 
			}
			
			for (auto b:books) {
				b -> display(); 
			} 
		} ;
		
		void searchByTitle(const string& title) const {
			for (auto b:books) {
				if (b -> getTitle().find(title) != string::npos)  {
					b -> display(); 
				}
			} 
		} ;
		
		void searchByAuthor (const string& author) const {
			for (auto b:books) {
				if (b -> getAuthor().find(author) != string::npos) {
					b -> display(); 
				} 
			} 
		} 
		
		void issueBook (int id, const string& date) {
			for (auto b:books) {
				if (b -> getID() == id) {
					BorrowableBook* bb = dynamic_cast<BorrowableBook*> (b);
					if (bb) {
						bb -> borrow(date);
						cout << "Book issue. \n";
						return; 
					} 
				} 
			} 
			cout<< "Book not found. \n"; 
		} 
		
		void returnBook(int id) {
			for (auto b:books) {
				if (b -> getID() == id) {
					BorrowableBook* bb = dynamic_cast<BorrowableBook*> (b);
					if (bb) {
						bb -> returnBook();
						cout << "Book returned. \n";
						return; 
					} 
				} 
			} 
			cout << "Book not found.\n"; 
		}
		
		void borrowBook(int id, const string& phone) {
		    for (auto b : books) {
		        if (b->getID() == id && b->getQuantity() > 0) {
		            b->decreaseQuantity();
		            b->increaseBorrowCount();
		
		            borrowedList.push_back(BorrowedBook(b->getID(), b->getTitle(), phone));
		            cout << "Book borrowed successfully.\n";
		            return;
		        }
		    }
		    cout << "Book not found or out of stock.\n";
		}

		
		void saveToFile(const string& filename) const {
			ofstream out(filename);
			for(auto b:books) {
				out << b ->  toFileString() << endl; 
			} 
			out.close();
			cout << "Data saved. \n"; 
		}
		
		void loadFromFile (const string& filename) {
			ifstream in(filename);
			string line;
			while (getline(in, line)) {
				stringstream ss(line);
				string type;
				getline(ss, type, ',');
				
				int id, quantity;
				string title, author, date;
				
				getline(ss, line, ','); id = stoi(line);
				getline(ss, title, ',');
				getline(ss, author, ',');
				getline(ss, line, ','); quantity = stoi(line);
				
				if (type == "B") {
					books.push_back(new Book(id, title, author, quantity));
				} else {
					getline(ss, date);
					BorrowableBook* bb = new BorrowableBook(id, title, author, quantity);
					if (!date.empty()) bb -> borrow(date);
					books.push_back(bb);
				}
				
				nextID = max(nextID, id+1);
			} 
			
			in.close();
			cout << "Data loaded. \n";
		} 
}; 

// Manager borrowed books 

class BorrowedBook {
    int bookID;
    string title;
    string borrowerPhone;
    time_t borrowDate;
    time_t dueDate;

public:
    BorrowedBook(int bookID, string title, string phone)
        : bookID(bookID), title(title), borrowerPhone(phone) {
        time(&borrowDate);

        // Tính ngày tr? (2 tháng = 60 ngày)
        dueDate = borrowDate + 60 * 24 * 60 * 60;
    }

    bool isOverdue() const {
        time_t now;
        time(&now);
        return now > dueDate;
    }

    void display() const {
        cout << "Book ID: " << bookID
             << ", Title: " << title
             << ", Borrower: " << borrowerPhone
             << ", Due: " << formatDate(dueDate)
             << (isOverdue() ? " [OVERDUE]" : "")
             << endl;
    }

    static string formatDate(time_t t) {
        tm* tm_info = localtime(&t);
        char buffer[20];
        strftime(buffer, 20, "%Y-%m-%d", tm_info);
        return string(buffer);
    }
};



// Main
int main() {
	Manager lib;
	lib.loadFromFile("library.txt");
	
	int choice;
	do {
		cout << "\n=====Library Management==== \n";
		cout << "1. Add Book \n";
		cout << "2. View All Books \n";
		cout << "3. Search by Title \n";
		cout << "4. Search by Author \n";
		cout << "5. Issue Book \n";
		cout << "6. Return Book \n";
		cout << "7. Save & Exit \n";
		cout << "Enter your choice: ";
		cin >> choice;
		cin.ignore();
		
		if (choice == 1) {
			string title, author;
			int qty;
			cout << "Enter title: "; getline(cin, title);
			cout << "Enter author: "; getline(cin, author);
			cout << "Enter quantity: "; cin >> qty;
			lib.addBook(title, author, qty);
		} else if (choice == 2) {
			lib.viewBooks();
		} else if (choice == 3) {
			string title;
			cout << "Enter title to search: "; getline(cin >> ws, title);
			lib.searchByTitle(title);
		} else if (choice == 4) {
			string author;
			cout << "Enter author to search: "; getline(cin >> ws, author);
			lib.searchByAuthor(author);
		} else if (choice == 5) {
			int id;
			string date;
			cout << "Enter book ID to issue: "; cin >> id;
			cout << "Enter borrow date (DD-MM-YYYY): "; cin >> date;
			lib.issueBook(id, date);
		} else if (choice == 6) {
			int id;
			cout << "Enter book ID to return: "; cin >> id;
			lib.returnBook(id);
		} else if (choice == 7) {
			lib.saveToFile("library.txt");
		} else {
			cout << "Invalid choice! \n";
		}
	} while (choice != 7);
	
	return 0;
}
