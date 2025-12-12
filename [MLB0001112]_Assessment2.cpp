#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <stdexcept>

using namespace std;

const int MAX_RECENT = 10;   // I just put 10 because that seemed reasonable
const int MAX_USERS = 10;    // not expecting more users 
const string ENCRYPTION_KEY = "mySecretKey123";  // just a simple key for XOR (not real security haha)


//Transaction class (from Assessment 2, reused) 
class Transaction {
private:
    string date;
    string category;
    string description;
    float amount;

public:

    // default constructor
    Transaction() {
        date = "";
        category = "";
        description = "";
        amount = 0.0;
    }

    // constructor with params
    Transaction(string d, string c, string desc, float a) {
        date = d;
        category = c;
        description = desc;
        amount = a;
    }

    // getters
    string getDate() { return date; }
    string getCategory() { return category; }
    string getDescription() { return description; }
    float getAmount() { return amount; }

    // setters (I didn’t really use these much but keeping them )
    void setDate(string d) { date = d; }
    void setCategory(string c) { category = c; }
    void setDescription(string desc) { description = desc; }
    void setAmount(float a) { amount = a; }
};


// User and Data Structures for Assessment 3 
struct User {
    string username;
    string passwordHash;
    string role; // "admin" or "user" 
};

struct Node {   // linked list node
    Transaction data;
    Node *next;
};

// stack to store recent transactions
struct RecentStack {
    Transaction arr[MAX_RECENT];
    int top;
};


//Function Prototypes 

void showMenu(bool isAdmin);
int getValidMenuChoice(int min, int max);

string hashPassword(const string &password);
string encryptDecrypt(const string &text, const string &key);

void initUsers(User users[], int &userCount);
User* login(User users[], int userCount);

void addTransaction(Node* &head, int &count, RecentStack &recent);
void deleteTransaction(Node* &head, int &count);
void displayTransactions(Node* head);
void sortTransactions(Node* head);
void searchTransaction(Node* head);

void pushRecent(RecentStack &stack, const Transaction &t);
void showRecent(RecentStack &stack);

void saveTransactionsToFile(Node* head);
void loadTransactionsFromFile(Node* &head, int &count, RecentStack &recent);


// MAIN 
int main() {

    // loading user data + login
    User users[MAX_USERS];
    int userCount = 0;

    initUsers(users, userCount);

    User *currentUser = NULL;

    cout << "Welcome to Personal Finance Tracker (Secure Version)\n";

    int attempts = 0;
    // retry system
    while (attempts < 3 && currentUser == NULL) {
        currentUser = login(users, userCount);
        if (currentUser == NULL) {
            attempts++;
            cout << "Login failed. Attempts left: " << (3 - attempts) << "\n";
        }
    }

    if (currentUser == NULL) {
        cout << "Too many failed login attempts. Exiting program.\n";
        return 0;   // I just stop the program here
    }

    bool isAdmin = (currentUser->role == "admin");

    cout << "Logged in as: " << currentUser->username
         << "  (role: " << currentUser->role << ")\n";

    // linked list start
    Node *head = NULL;
    int transactionCount = 0;
    RecentStack recent;
    recent.top = -1;  // empty

    loadTransactionsFromFile(head, transactionCount, recent);

    int choice;

    do {
        showMenu(isAdmin);
        choice = getValidMenuChoice(1, 6);

        switch (choice) {
            case 1:
                addTransaction(head, transactionCount, recent);
                saveTransactionsToFile(head);  // save immediately
                break;

            case 2:
                if (!isAdmin) {
                    cout << "Only admins are allowed to delete transactions.\n";
                } else {
                    deleteTransaction(head, transactionCount);
                    saveTransactionsToFile(head);
                }
                break;

            case 3:
                searchTransaction(head);
                break;

            case 4:
                sortTransactions(head); // bubble sort - slow but okay
                displayTransactions(head);
                break;

            case 5:
                showRecent(recent);
                break;

            case 6:
                cout << "Exiting program...\n";
                break;

            default:
                cout << "Invalid choice\n";
        }

    } while (choice != 6);

    return 0;
}


//Helper Functions 

void showMenu(bool isAdmin) {

    // menu is same as assessment 2 but updated
    cout << "\n==== Personal Finance Tracker ====\n";
    cout << "1 - Add new transaction\n";
    cout << "2 - Delete a transaction (admin only)\n";
    cout << "3 - Search for a transaction\n";
    cout << "4 - Display all transactions (sorted by amount)\n";
    cout << "5 - Show recent transactions (stack)\n";
    cout << "6 - Exit\n";
    cout << "Enter your choice: ";
}


// input validation
int getValidMenuChoice(int min, int max) {
    int choice;
    cin >> choice;

    while (cin.fail() || choice < min || choice > max) {

        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Invalid input. Please enter a number between "
             << min << " and " << max << ": ";

        cin >> choice;
    }

    cin.ignore(1000, '\n'); // forgot this earlier once and it broke getline
    return choice;
}


// basic password hashing (not real encryption)
string hashPassword(const string &password) {
    std::hash<string> hasher;
    size_t h = hasher(password);
    stringstream ss;
    ss << h;
    return ss.str();
}


// XOR encryption / decryption
string encryptDecrypt(const string &text, const string &key) {

    string result = text;

    for (size_t i = 0; i < text.size(); i++) {
        result[i] = text[i] ^ key[i % key.size()];
    }

    return result;
}


// Authentication 

void initUsers(User users[], int &userCount) {

    userCount = 0;

    ifstream infile("users.txt");

    if (!infile) {
        // create admin if no file
        ofstream outfile("users.txt");

        string defaultUser = "admin";
        string defaultPass = "admin123";

        string hash = hashPassword(defaultPass);

        outfile << defaultUser << "|" << hash << "|admin\n";
        outfile.close();

        cout << "Default admin created (username: admin, password: admin123)\n";

        infile.open("users.txt");
    }

    string line;

    // reading users file
    while (getline(infile, line) && userCount < MAX_USERS) {

        if (line.size() == 0) continue;

        stringstream ss(line);
        string u, h, r;

        getline(ss, u, '|');
        getline(ss, h, '|');
        getline(ss, r, '|');

        users[userCount].username = u;
        users[userCount].passwordHash = h;
        users[userCount].role = r;

        userCount++;
    }

    infile.close();
}


User* login(User users[], int userCount) {

    // normal login function
    string uname, pwd;

    cout << "\n--- Login ---\n";

    cout << "Username: ";
    getline(cin, uname);

    cout << "Password: ";
    getline(cin, pwd);

    string hashed = hashPassword(pwd);

    for (int i = 0; i < userCount; i++) {
        if (users[i].username == uname && users[i].passwordHash == hashed) {
            return &users[i];
        }
    }

    return NULL;
}


//Recent Stack 
void pushRecent(RecentStack &stack, const Transaction &t) {

    // pushing like a normal stack
    if (stack.top < MAX_RECENT - 1) {
        stack.top++;
        stack.arr[stack.top] = t;
    } else {
        // stack full, shift everything down
        for (int i = 0; i < MAX_RECENT - 1; i++) {
            stack.arr[i] = stack.arr[i + 1];
        }
        // put newest at top
        stack.arr[MAX_RECENT - 1] = t;
    }
}


// print recent transactions
void showRecent(RecentStack &stack) {

    if (stack.top < 0) {
        cout << "No recent transactions.\n";
        return;
    }

    cout << "\n--- Recent Transactions (most recent first) ---\n";

    // printing backwards like stack
    for (int i = stack.top; i >= 0; i--) {

        cout << (stack.top - i + 1)
             << ". Date: " << stack.arr[i].getDate()
             << ", Category: " << stack.arr[i].getCategory()
             << ", Description: " << stack.arr[i].getDescription()
             << ", Amount: " << stack.arr[i].getAmount() << endl;
    }
}


//Linked List Operations 

// add transaction to list
void addTransaction(Node* &head, int &count, RecentStack &recent) {

    string date, cat, desc, amountText;
    float amount;

    // basic user input prompts
    cout << "Enter date (DD/MM/YYYY): ";
    getline(cin, date);

    cout << "Enter category (e.g. Food, Rent, Transport): ";
    getline(cin, cat);

    cout << "Enter description: ";
    getline(cin, desc);

    cout << "Enter amount (positive for income, negative for expense): ";
    getline(cin, amountText);

    try {
        amount = stof(amountText);
    } catch (...) {
        cout << "Invalid amount entered. Transaction not added.\n";
        return;
    }

    Transaction t(date, cat, desc, amount);

    Node *newNode = new Node;
    newNode->data = t;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
    } else {
        Node *temp = head;

        // just go to end of list
        while (temp->next != NULL) {
            temp = temp->next;
        }

        temp->next = newNode;
    }

    count++;

    pushRecent(recent, t); // keep stack updated

    cout << "Transaction added successfully.\n";
}


// delete by index
void deleteTransaction(Node* &head, int &count) {

    if (head == NULL || count == 0) {
        cout << "No transactions to delete.\n";
        return;
    }

    displayTransactions(head);

    int index;

    cout << "Enter the transaction number to delete (1 to " << count << "): ";
    cin >> index;

    while (cin.fail() || index < 1 || index > count) {

        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Invalid number. Enter again (1 to " << count << "): ";
        cin >> index;
    }

    cin.ignore(1000, '\n');

    if (index == 1) {
        Node *temp = head;
        head = head->next;
        delete temp;
    } else {

        Node *prev = head;

        for (int i = 1; i < index - 1; i++) {
            prev = prev->next;
        }

        Node *toDelete = prev->next;
        prev->next = toDelete->next;

        delete toDelete;
    }

    count--;

    cout << "Transaction deleted.\n";
}


// print everything
void displayTransactions(Node* head) {

    if (head == NULL) {
        cout << "No transactions recorded.\n";
        return;
    }

    cout << "\n---- All Transactions ----\n";

    Node *temp = head;
    int i = 1;

    while (temp != NULL) {

        cout << i << ". Date: " << temp->data.getDate()
             << ", Category: " << temp->data.getCategory()
             << ", Description: " << temp->data.getDescription()
             << ", Amount: " << temp->data.getAmount() << endl;

        temp = temp->next;
        i++;
    }
}


// bubble sort (very slow but fine for small lists)
void sortTransactions(Node* head) {

    if (head == NULL || head->next == NULL) return;

    bool swapped;

    do {
        swapped = false;

        Node *cur = head;

        while (cur->next != NULL) {

            if (cur->data.getAmount() > cur->next->data.getAmount()) {
                Transaction tmp = cur->data;
                cur->data = cur->next->data;
                cur->next->data = tmp;

                swapped = true;
            }

            cur = cur->next;
        }

    } while (swapped);

    cout << "Transactions have been sorted by amount.\n";
}


// search by date or category
void searchTransaction(Node* head) {

    if (head == NULL) {
        cout << "No transactions to search.\n";
        return;
    }

    int opt;

    cout << "Search by:\n1 - Date\n2 - Category\nEnter option: ";
    cin >> opt;

    while (cin.fail() || (opt != 1 && opt != 2)) {

        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Invalid choice. Enter 1 or 2: ";
        cin >> opt;
    }

    cin.ignore(1000, '\n');

    string key;

    cout << "Enter search value: ";
    getline(cin, key);

    bool found = false;

    cout << "\nSearch results:\n";

    Node *temp = head;
    int i = 1;

    while (temp != NULL) {

        bool match = false;

        if (opt == 1 && temp->data.getDate() == key) {
            match = true;
        } else if (opt == 2 && temp->data.getCategory() == key) {
            match = true;
        }

        if (match) {
            cout << i << ". Date: " << temp->data.getDate()
                 << ", Category: " << temp->data.getCategory()
                 << ", Description: " << temp->data.getDescription()
                 << ", Amount: " << temp->data.getAmount() << endl;
            found = true;
        }

        temp = temp->next;
        i++;
    }

    if (!found) {
        cout << "No matching transactions found.\n";
    }
}



//File Handling + Encryption 

// save everything to file
void saveTransactionsToFile(Node* head) {

    ofstream outfile("transactions.txt");

    if (!outfile) {
        cout << "Error opening transactions.txt for writing.\n";
        return;
    }

    try {

        Node *temp = head;

        while (temp != NULL) {

            stringstream ss;
            ss << temp->data.getAmount();

            string line = temp->data.getDate() + "|" +
                          temp->data.getCategory() + "|" +
                          temp->data.getDescription() + "|" +
                          ss.str();

            string encrypted = encryptDecrypt(line, ENCRYPTION_KEY);

            outfile << encrypted << "\n";

            temp = temp->next;
        }

        outfile.close();

    } catch (exception &e) {
        cout << "Error saving transactions: " << e.what() << endl;
    }
}



// load saved data
void loadTransactionsFromFile(Node* &head, int &count, RecentStack &recent) {

    ifstream infile("transactions.txt");

    if (!infile) {
        cout << "No existing transactions file found. Starting with empty list.\n";
        return;
    }

    string line;

    try {

        while (getline(infile, line)) {

            if (line.size() == 0) continue;

            string decrypted = encryptDecrypt(line, ENCRYPTION_KEY);

            stringstream ss(decrypted);

            string d, c, desc, amountStr;

            getline(ss, d, '|');
            getline(ss, c, '|');
            getline(ss, desc, '|');
            getline(ss, amountStr, '|');

            float amount = 0;

            try {
                amount = stof(amountStr);
            } catch (...) {
                cout << "Skipping invalid transaction line in file.\n";
                continue;
            }

            Transaction t(d, c, desc, amount);

            Node *newNode = new Node;
            newNode->data = t;
            newNode->next = NULL;

            if (head == NULL) {
                head = newNode;
            } else {
                Node *tmp = head;
                while (tmp->next != NULL) tmp = tmp->next;

                tmp->next = newNode;
            }

            count++;

            pushRecent(recent, t);  // rebuild recent list
        }

        infile.close();

    } catch (exception &e) {
        cout << "Error while reading transactions file: " << e.what() << endl;
    }
}
