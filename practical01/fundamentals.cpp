// Practical 01: C++ Fundamentals - data types, constants, variables, operators, I/O
#include <iostream>
#include <string>
using namespace std;

int main() {
    // Constants: values that must never change
    const int    SUBJECTS     = 3;
    const double MAX_MARKS    = 100.0;
    const double PASS_PERCENT = 40.0;

    // Variables of different data types
    string name;          // text
    int    age;           // whole number
    double m1, m2, m3;    // marks (decimal)
    char   grade;         // single character

    // Input
    cout << "Enter student name: ";
    getline(cin, name);
    cout << "Enter age: ";
    cin >> age;
    cout << "Enter marks in " << SUBJECTS << " subjects (out of 100): ";
    cin >> m1 >> m2 >> m3;

    // Arithmetic operators: + / *
    double total      = m1 + m2 + m3;
    double average    = total / SUBJECTS;
    double percentage = (total / (MAX_MARKS * SUBJECTS)) * 100;
    int    ageNextYear = age + 1;
    int    ageMod      = age % 2;   // % gives remainder

    // Relational operators: >= < ==   |   Logical operators: && || !
    bool passed      = percentage >= PASS_PERCENT;
    bool allSubjects = (m1 >= 35) && (m2 >= 35) && (m3 >= 35);
    bool finalResult = passed && allSubjects;
    bool needsHelp   = !finalResult || (percentage < 50);

    if      (percentage >= 75) grade = 'A';
    else if (percentage >= 60) grade = 'B';
    else if (percentage >= 40) grade = 'C';
    else                       grade = 'F';

    // Output
    cout << "\n----- RESULT -----\n";
    cout << "Name        : " << name << "\n";
    cout << "Age         : " << age << " (next year: " << ageNextYear
         << ", age % 2 = " << ageMod << ")\n";
    cout << "Total       : " << total << " / " << MAX_MARKS * SUBJECTS << "\n";
    cout << "Average     : " << average << "\n";
    cout << "Percentage  : " << percentage << "%\n";
    cout << "Grade       : " << grade << "\n";
    cout << "Status      : " << (finalResult ? "PASS" : "FAIL") << "\n";
    cout << "Needs extra help? " << (needsHelp ? "Yes" : "No") << "\n";
    return 0;
}