#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <fstream>
using namespace std;

const int MAX_TRANSACTIONS = 100;
const int MAX_USERS = 10;
const string ENCRYPTION_KEY = "mySecretKey123";


//  Transaction Class (same as base code) 
class Transaction {
private:
    string date;
    string category;
    string description;
    float amount;

public:
    Transaction() {
        date = "";
        category = "";
        description = "";
        amount = 0.0;
    }

    Transaction(string d, string c, string desc, float a) {
        date = d;
        category = c;
        description = desc;
        amount = a;
    }

    string getDate() { return date; }
    string getCategory() { return category; }
    string getDescription() { return description; }
    float getAmount() { return amount; }

    void setDate(string d) { date = d; }
    void setCategory(string c) { category = c; }
    void setDescription(string desc) { description = desc; }
    void setAmount(float a) { amount = a; }
};


//  User Struct for Authentication 
struct User {
    string username;
    string passwordHash;
    string role;   // admin or user
};


//  Function Prototypes 
void showMenu();
int getValidMenuChoice(int min, int max);

void addTransaction(Transaction arr[], int &count);
void deleteTransaction(Transaction arr[], int &count);
void displayTransactions(Transaction arr[], int count);
void sortTransactions(Transaction arr[], int count);
void searchTransaction(Transaction arr[], int count);

// Authentication
void initUsers(User users[], int &userCount);
User* login(User users[], int userCount);
string hashPassword(const string &password);

// File handling
string encryptDecrypt(const string &text, const string &key);
void saveTransactionsToFile(Transaction arr[], int count);
void loadTransactionsFromFile(Transaction arr[], int &count);


//  MAIN 
int main() {

    // === Load users + login ===
    User users[MAX_USERS];
    int userCount = 0;
    initUsers(users, userCount);

    User* currentUser = NULL;
    int attempts = 0;

    cout << "Welcome to Personal Finance Tracker (Auth + File Version)\n";

    while (attempts < 3 && currentUser == NULL) {
        currentUser = login(users, userCount);
        if (currentUser == NULL) {
            attempts++;
            cout << "Login failed. Attempts left: " << (3 - attempts) << "\n";
        }
    }

    if (currentUser == NULL) {
        cout << "Too many failed login attempts. Exiting program.\n";
        return 0;
    }

    bool isAdmin = (currentUser->role == "admin");

    cout << "Logged in as: " << currentUser->username
         << " (role: " << currentUser->role << ")\n";


    //  Transactions (array-based, with file persistence) 
    Transaction transactions[MAX_TRANSACTIONS];
    int transactionCount = 0;

    loadTransactionsFromFile(transactions, transactionCount);

    int choice;

    do 
    {
        showMenu();
        choice = getValidMenuChoice(1, 5);

        switch (choice) 
        {
            case 1:
                addTransaction(transactions, transactionCount);
                saveTransactionsToFile(transactions, transactionCount);
                break;

            case 2:
                if (!isAdmin) {
                    cout << "Only admins can delete transactions.\n";
                } else {
                    deleteTransaction(transactions, transactionCount);
                    saveTransactionsToFile(transactions, transactionCount);
                }
                break;

            case 3:
                searchTransaction(transactions, transactionCount);
                break;

            case 4:
                sortTransactions(transactions, transactionCount);
                displayTransactions(transactions, transactionCount);
                break;

            case 5:
                cout << "Exiting program..." << endl;
                break;

            default:
                cout << "Invalid choice." << endl;
        }

    } while (choice != 5);

    return 0;
}



//  Menu + Validation
void showMenu() {
    cout << "\n==== Personal Finance Tracker ====\n";
    cout << "1 - Add new transaction\n";
    cout << "2 - Delete a transaction (admin only)\n";
    cout << "3 - Search for a transaction\n";
    cout << "4 - Display all transactions (sorted by amount)\n";
    cout << "5 - Exit\n";
    cout << "Enter your choice: ";
}

int getValidMenuChoice(int min, int max) {
    int choice;
    cin >> choice;

    while (cin.fail() || choice < min || choice > max) 
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Please enter a number between "
             << min << " and " << max << ": ";
        cin >> choice;
    }

    cin.ignore(1000, '\n');
    return choice;
}



//  AUTHENTICATION FUNCTIONS 

string hashPassword(const string &password) {
    hash<string> hasher;
    size_t h = hasher(password);
    stringstream ss;
    ss << h;
    return ss.str();
}

void initUsers(User users[], int &userCount) {
    userCount = 0;

    // Admin
    users[userCount].username = "admin";
    users[userCount].passwordHash = hashPassword("admin123");
    users[userCount].role = "admin";
    userCount++;

    // Regular user
    users[userCount].username = "user";
    users[userCount].passwordHash = hashPassword("user123");
    users[userCount].role = "user";
    userCount++;
}

User* login(User users[], int userCount) {

    string uname, pwd;

    cout << "\n--- Login ---\n";
    cout << "Username: ";
    cin >> uname;

    cout << "Password: ";
    cin >> pwd;

    string hashed = hashPassword(pwd);

    for (int i = 0; i < userCount; i++) {
        if (users[i].username == uname && users[i].passwordHash == hashed) {
            return &users[i];
        }
    }

    return NULL;
}



//  FILE HANDLING FUNCTIONS 

string encryptDecrypt(const string &text, const string &key) {

    string result = text;

    for (size_t i = 0; i < text.size(); i++) {
        result[i] = text[i] ^ key[i % key.size()];
    }

    return result;
}

void saveTransactionsToFile(Transaction arr[], int count) {

    ofstream outfile("transactions.txt");

    if (!outfile) {
        cout << "Error opening transactions.txt for writing.\n";
        return;
    }

    for (int i = 0; i < count; i++) {

        stringstream ss;
        ss << arr[i].getAmount();

        string line = arr[i].getDate() + "|" +
                      arr[i].getCategory() + "|" +
                      arr[i].getDescription() + "|" +
                      ss.str();

        string encrypted = encryptDecrypt(line, ENCRYPTION_KEY);

        outfile << encrypted << "\n";
    }

    outfile.close();
}

void loadTransactionsFromFile(Transaction arr[], int &count) {

    ifstream infile("transactions.txt");

    if (!infile) {
        cout << "No existing transactions file found. Starting empty.\n";
        return;
    }

    string line;

    while (getline(infile, line) && count < MAX_TRANSACTIONS) {

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

        arr[count] = t;
        count++;
    }

    infile.close();
}



// Original Assessment 2 Functionalities (array-based) 

// Add transaction
void addTransaction(Transaction arr[], int &count) {

    if (count >= MAX_TRANSACTIONS) {
        cout << "Transaction list is full. Cannot add more.\n";
        return;
    }

    string date, category, description;
    string amountText;
    float amount;

    cout << "Enter date (DD/MM/YYYY): ";
    cin >> date;

    cout << "Enter category (e.g. Food, Rent, Transport): ";
    cin >> category;

    cin.ignore();

    cout << "Enter description: ";
    getline(cin, description);

    cout << "Enter amount (positive for income, negative for expense): ";
    cin >> amountText;

    try {
        amount = stof(amountText);
    } catch (...) {
        cout << "Invalid amount entered. Transaction not added.\n";
        return;
    }

    Transaction t(date, category, description, amount);
    arr[count] = t;
    count++;

    cout << "Transaction added successfully.\n";
}


// Delete transaction
void deleteTransaction(Transaction arr[], int &count) {

    if (count == 0) {
        cout << "No transactions to delete.\n";
        return;
    }

    displayTransactions(arr, count);

    int index;
    cout << "Enter the transaction number to delete (1 to " << count << "): ";
    cin >> index;

    while (cin.fail() || index < 1 || index > count) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid number. Enter again (1 to " << count << "): ";
        cin >> index;
    }

    for (int i = index - 1; i < count - 1; i++) {
        arr[i] = arr[i + 1];
    }

    count--;
    cout << "Transaction deleted.\n";
}


// Display
void displayTransactions(Transaction arr[], int count) {

    if (count == 0) {
        cout << "No transactions recorded.\n";
        return;
    }

    cout << "\n---- All Transactions ----\n";

    for (int i = 0; i < count; i++) {

        cout << (i + 1) << ". Date: " << arr[i].getDate()
             << ", Category: " << arr[i].getCategory()
             << ", Description: " << arr[i].getDescription()
             << ", Amount: " << arr[i].getAmount() 
             << endl;
    }
}


// Sort (bubble)
void sortTransactions(Transaction arr[], int count) {

    if (count <= 1) return;

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {

            if (arr[j].getAmount() > arr[j + 1].getAmount()) {
                Transaction temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    cout << "Transactions have been sorted by amount.\n";
}


// Search
void searchTransaction(Transaction arr[], int count) {

    if (count == 0) {
        cout << "No transactions to search.\n";
        return;
    }

    int option;
    cout << "Search by:\n1 - Date\n2 - Category\nEnter option: ";
    cin >> option;

    while (cin.fail() || (option != 1 && option != 2)) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid choice. Enter 1 or 2: ";
        cin >> option;
    }

    string key;
    cout << "Enter search value: ";
    cin >> key;

    bool found = false;

    cout << "\nSearch results:\n";

    for (int i = 0; i < count; i++) {
        if ((option == 1 && arr[i].getDate() == key) ||
            (option == 2 && arr[i].getCategory() == key)) {

            cout << (i + 1) << ". Date: " << arr[i].getDate()
                 << ", Category: " << arr[i].getCategory()
                 << ", Description: " << arr[i].getDescription()
                 << ", Amount: " << arr[i].getAmount()
                 << endl;

            found = true;
        }
    }

    if (!found) {
        cout << "No matching transactions found.\n";
    }
}
