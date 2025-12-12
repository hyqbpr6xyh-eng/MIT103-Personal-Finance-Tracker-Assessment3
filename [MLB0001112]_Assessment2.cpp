#include <iostream>
#include <string>
using namespace std;

const int MAX_TRANSACTIONS = 100;

// Transaction class (Part 5 of the assessment)
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

    string getDate() {
        return date;
    }

    string getCategory() {
        return category;
    }

    string getDescription() {
        return description;
    }

    float getAmount() {
        return amount;
    }

    void setDate(string d) {
        date = d;
    }

    void setCategory(string c) {
        category = c;
    }

    void setDescription(string desc) {
        description = desc;
    }

    void setAmount(float a) {
        amount = a;
    }
};

// Function prototypes (Part 4 of assessment) 
void showMenu();
int getValidMenuChoice(int min, int max);
void addTransaction(Transaction arr[], int &count);
void deleteTransaction(Transaction arr[], int &count);
void displayTransactions(Transaction arr[], int count);
void sortTransactions(Transaction arr[], int count);
void searchTransaction(Transaction arr[], int count);


// main
int main() {
    
    Transaction transactions[MAX_TRANSACTIONS];
    int transactionCount = 0;
    int choice;

    do 
    {
        showMenu();
        choice = getValidMenuChoice(1, 5);

        switch (choice) 
        {
            case 1:
                addTransaction(transactions, transactionCount);
                break;

            case 2:
                deleteTransaction(transactions, transactionCount);
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


// Menu + input validation (Part 1, 6) 
void showMenu() {
    cout << "\n==== Personal Finance Tracker ====\n";
    cout << "1 - Add new transaction\n";
    cout << "2 - Delete a transaction\n";
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
    return choice;
}


// Add transaction (Part 2, 4, 5, 6)
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

    cin.ignore(); // leftover newline

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


// Delete transaction (Part 2, 4)
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


// Display transactions (Part 2, 4)
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


// Sorting (Bubble sort) (Part 3)
void sortTransactions(Transaction arr[], int count) {

    if (count <= 1) return;

    for (int i = 0; i < count - 1; i++) 
    {
        for (int j = 0; j < count - 1 - i; j++) 
        {
            if (arr[j].getAmount() > arr[j + 1].getAmount()) {
                Transaction temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    cout << "Transactions have been sorted by amount.\n";
}


// Searching (Linear search) (Part 3)
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
