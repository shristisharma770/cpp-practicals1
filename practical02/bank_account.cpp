// Practical 02: Classes, Access Specifiers and Member Functions
// Real-world entity: BankAccount
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

class BankAccount {
private:
    // PRIVATE: data members. Outside code cannot read or change them directly,
    // so nobody can set the balance to a wrong value (e.g. -5000).
    // The only way to change them is through the public functions below,
    // which check the values first. This is called encapsulation.
    int    accountNumber;
    string holderName;
    double balance;

public:
    // PUBLIC: the interface that main() and other code is allowed to use.

    // Constructor: runs automatically when an object is created.
    // It initialises the object and validates the values.
    BankAccount(int accNo, const string& name, double openingBalance) {
        accountNumber = (accNo > 0) ? accNo : 0;          // account number must be positive
        holderName    = name.empty() ? "Unknown" : name;  // name must not be empty
        balance       = (openingBalance >= 0) ? openingBalance : 0.0; // no negative opening balance
        if (accNo <= 0 || name.empty() || openingBalance < 0)
            cout << "  [Warning] Invalid value corrected while creating the account.\n";
    }

    // Setter: changes the holder name, but only if it is valid
    void setHolderName(const string& name) {
        if (!name.empty()) holderName = name;
        else cout << "  [Error] Name cannot be empty.\n";
    }

    // Getter: gives read-only access to a private value
    double getBalance() const { return balance; }

    // Operation 1: deposit money
    void deposit(double amount) {
        if (amount <= 0) {
            cout << "  [Error] Deposit amount must be positive.\n";
            return;
        }
        balance += amount;
        cout << "  Deposited " << amount << ". New balance: " << balance << "\n";
    }

    // Operation 2: withdraw money
    void withdraw(double amount) {
        if (amount <= 0) {
            cout << "  [Error] Withdrawal amount must be positive.\n";
        } else if (amount > balance) {
            cout << "  [Error] Insufficient balance. Available: " << balance << "\n";
        } else {
            balance -= amount;
            cout << "  Withdrew " << amount << ". New balance: " << balance << "\n";
        }
    }

    // Display all details of the account
    void display() const {
        cout << "  Account No : " << accountNumber << "\n"
             << "  Holder     : " << holderName << "\n"
             << "  Balance    : Rs. " << balance << "\n";
    }
};

int main() {
    cout << fixed << setprecision(2);   // show money with 2 decimal places

    // ---- Object 1 ----
    cout << "=== Account 1 ===\n";
    BankAccount acc1(1001, "Aarav Patil", 5000);
    acc1.display();
    acc1.deposit(2500);
    acc1.withdraw(1000);
    acc1.withdraw(10000);      // fails: more than balance
    acc1.display();

    // ---- Object 2 ----
    cout << "\n=== Account 2 ===\n";
    BankAccount acc2(1002, "Meera Kulkarni", 12000);
    acc2.display();
    acc2.deposit(-500);        // fails: negative amount
    acc2.withdraw(2000);
    acc2.setHolderName("Meera Deshmukh");
    acc2.display();

    // Direct access to private data is NOT allowed. Uncommenting the next line gives a compile error:
    // acc1.balance = 999999;   // error: 'double BankAccount::balance' is private

    cout << "\nFinal balances -> Account 1: " << acc1.getBalance()
         << ", Account 2: " << acc2.getBalance() << "\n";
    return 0;
}
