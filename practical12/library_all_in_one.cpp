// Library Management System - ALL-IN-ONE copy for online compilers (e.g. OnlineGDB).
// This is the same code as the multi-file project in src/, joined into one file.
// Use the multi-file version (src/*.cpp, src/*.h) for GitHub.

// ======================= Exceptions.h =======================
// Exceptions.h - custom exception hierarchy for the Library Management System.
// Every library-specific error derives from LibraryError, which derives from std::runtime_error.
// This lets callers catch one specific error, or all library errors with a single catch.

#include <stdexcept>
#include <string>

class LibraryError : public std::runtime_error {
public:
    explicit LibraryError(const std::string& message) : std::runtime_error(message) {}
};

// Thrown when user input or a record fails validation (empty title, ID <= 0, ...).
class InvalidInput : public LibraryError {
public:
    explicit InvalidInput(const std::string& reason) : LibraryError("Invalid input: " + reason) {}
};

// Thrown when a book ID does not exist in the library.
class BookNotFound : public LibraryError {
public:
    explicit BookNotFound(int id)
        : LibraryError("No book found with ID " + std::to_string(id) + ".") {}
};

// Thrown when adding a book whose ID is already used.
class DuplicateBook : public LibraryError {
public:
    explicit DuplicateBook(int id)
        : LibraryError("A book with ID " + std::to_string(id) + " already exists.") {}
};

// Thrown when issuing a book that is already issued.
class BookUnavailable : public LibraryError {
public:
    BookUnavailable(int id, const std::string& title)
        : LibraryError("Book " + std::to_string(id) + " (\"" + title + "\") is already issued.") {}
};

// Thrown when returning a book that is not currently issued.
class BookNotIssued : public LibraryError {
public:
    BookNotIssued(int id, const std::string& title)
        : LibraryError("Book " + std::to_string(id) + " (\"" + title +
                       "\") is not issued, so it cannot be returned.") {}
};

// Thrown when a file cannot be opened, read or written.
class FileError : public LibraryError {
public:
    explicit FileError(const std::string& reason) : LibraryError("File error: " + reason) {}
};

// ======================= TextUtil.h =======================
// TextUtil.h - small helper functions for text handling and CSV fields.

#include <string>
#include <vector>

namespace textutil {

std::string trim(const std::string& text);                 // remove leading/trailing whitespace
std::string toLower(const std::string& text);              // lower-case copy (for case-insensitive search)
bool parsePositiveInt(const std::string& text, int& out);  // true only for whole numbers > 0
std::string csvEscape(const std::string& field);           // quote a field if it has a comma or quote
std::vector<std::string> csvSplit(const std::string& line);// split one CSV line, honouring quotes

}  // namespace textutil

// ======================= TextUtil.cpp =======================
#include <algorithm>
#include <cctype>

namespace textutil {

std::string trim(const std::string& text) {
    const char* whitespace = " \t\r\n";
    std::size_t start = text.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    std::size_t end = text.find_last_not_of(whitespace);
    return text.substr(start, end - start + 1);
}

std::string toLower(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

bool parsePositiveInt(const std::string& text, int& out) {
    if (text.empty() || text.size() > 9) return false;  // 9 digits always fit in an int
    for (char c : text) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    out = std::stoi(text);
    return out > 0;
}

std::string csvEscape(const std::string& field) {
    if (field.find_first_of(",\"\n") == std::string::npos) return field;
    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') escaped += '"';  // a quote inside a quoted field is doubled
        escaped += c;
    }
    escaped += '"';
    return escaped;
}

std::vector<std::string> csvSplit(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    current += '"';  // doubled quote -> one quote character
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                current += c;
            }
        } else if (c == '"') {
            inQuotes = true;
        } else if (c == ',') {
            fields.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    fields.push_back(current);
    return fields;
}

}  // namespace textutil

// ======================= Book.h =======================
// Book.h - one book record. Data is private (encapsulation); the constructor validates it.

#include <string>

class Book {
public:
    // Throws InvalidInput if the ID is not positive or the title/author is empty.
    Book(int id, std::string title, std::string author, bool available = true);

    int getId() const { return id_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getAuthor() const { return author_; }
    bool isAvailable() const { return available_; }

    void issue();     // throws BookUnavailable if already issued
    void giveBack();  // throws BookNotIssued if it is not issued

    std::string toCsvRow() const;  // "id,title,author,available"

private:
    int id_;
    std::string title_;
    std::string author_;
    bool available_;
};

// ======================= Book.cpp =======================
Book::Book(int id, std::string title, std::string author, bool available)
    : id_(id),
      title_(textutil::trim(title)),
      author_(textutil::trim(author)),
      available_(available) {
    if (id_ <= 0) throw InvalidInput("Book ID must be a positive whole number.");
    if (title_.empty()) throw InvalidInput("Title cannot be empty.");
    if (author_.empty()) throw InvalidInput("Author cannot be empty.");
}

void Book::issue() {
    if (!available_) throw BookUnavailable(id_, title_);
    available_ = false;
}

void Book::giveBack() {
    if (available_) throw BookNotIssued(id_, title_);
    available_ = true;
}

std::string Book::toCsvRow() const {
    return std::to_string(id_) + "," + textutil::csvEscape(title_) + "," +
           textutil::csvEscape(author_) + "," + (available_ ? "1" : "0");
}

// ======================= Library.h =======================
// Library.h - manages all books. Uses std::map (ordered by book ID) for in-memory storage.

#include <cstddef>
#include <map>
#include <string>
#include <vector>

// Summary of reading a CSV file: how many rows were loaded or skipped, and why.
struct LoadResult {
    int loaded = 0;
    int skipped = 0;
    std::vector<std::string> warnings;
};

class Library {
public:
    void addBook(const Book& book);                      // throws DuplicateBook
    void issueBook(int id);                              // throws BookNotFound, BookUnavailable
    void returnBook(int id);                             // throws BookNotFound, BookNotIssued
    const Book& findById(int id) const;                  // throws BookNotFound
    std::vector<Book> search(const std::string& keyword) const;  // title or author, any case
    std::vector<Book> allBooks() const;                  // sorted by ID
    std::size_t size() const { return books_.size(); }

    void saveToFile(const std::string& path) const;      // throws FileError
    LoadResult loadFromFile(const std::string& path);    // throws FileError; skips bad rows

private:
    Book& find(int id);
    std::map<int, Book> books_;  // key = book ID
};

// ======================= Library.cpp =======================
#include <fstream>


void Library::addBook(const Book& book) {
    if (books_.count(book.getId()) > 0) throw DuplicateBook(book.getId());
    books_.emplace(book.getId(), book);
}

Book& Library::find(int id) {
    auto it = books_.find(id);
    if (it == books_.end()) throw BookNotFound(id);
    return it->second;
}

const Book& Library::findById(int id) const {
    auto it = books_.find(id);
    if (it == books_.end()) throw BookNotFound(id);
    return it->second;
}

void Library::issueBook(int id) { find(id).issue(); }

void Library::returnBook(int id) { find(id).giveBack(); }

std::vector<Book> Library::search(const std::string& keyword) const {
    const std::string key = textutil::toLower(textutil::trim(keyword));
    if (key.empty()) throw InvalidInput("Search keyword cannot be empty.");

    std::vector<Book> matches;
    for (const auto& entry : books_) {
        const Book& book = entry.second;
        if (textutil::toLower(book.getTitle()).find(key) != std::string::npos ||
            textutil::toLower(book.getAuthor()).find(key) != std::string::npos) {
            matches.push_back(book);
        }
    }
    return matches;
}

std::vector<Book> Library::allBooks() const {
    std::vector<Book> result;
    result.reserve(books_.size());
    for (const auto& entry : books_) result.push_back(entry.second);
    return result;
}

void Library::saveToFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out) throw FileError("Cannot open '" + path + "' for writing.");
    out << "id,title,author,available\n";
    for (const auto& entry : books_) out << entry.second.toCsvRow() << "\n";
    out.flush();
    if (!out) throw FileError("Writing to '" + path + "' failed.");
}

LoadResult Library::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw FileError("Cannot open '" + path + "' for reading.");

    LoadResult result;
    std::map<int, Book> loaded;  // filled first, so a failure never leaves the library half-loaded
    std::string line;
    int lineNumber = 0;

    while (std::getline(in, line)) {
        ++lineNumber;
        const std::string text = textutil::trim(line);
        if (text.empty()) continue;
        if (lineNumber == 1 && textutil::toLower(text).rfind("id,", 0) == 0) continue;  // header row

        auto skip = [&](const std::string& reason) {
            ++result.skipped;
            result.warnings.push_back("Line " + std::to_string(lineNumber) + " skipped: " + reason);
        };

        const std::vector<std::string> fields = textutil::csvSplit(text);
        if (fields.size() != 4) {
            skip("expected 4 fields but found " + std::to_string(fields.size()));
            continue;
        }
        int id = 0;
        if (!textutil::parsePositiveInt(textutil::trim(fields[0]), id)) {
            skip("invalid book ID '" + fields[0] + "'");
            continue;
        }
        const std::string flag = textutil::trim(fields[3]);
        if (flag != "0" && flag != "1") {
            skip("availability must be 0 or 1");
            continue;
        }
        if (loaded.count(id) > 0) {
            skip("duplicate book ID " + std::to_string(id));
            continue;
        }
        try {
            loaded.emplace(id, Book(id, fields[1], fields[2], flag == "1"));
            ++result.loaded;
        } catch (const LibraryError& error) {
            skip(error.what());
        }
    }
    if (in.bad()) throw FileError("Error while reading '" + path + "'.");

    books_.swap(loaded);
    return result;
}

// ======================= main.cpp =======================
// main.cpp - console interface (menu) and a scripted demo for the Library Management System.
//
// Usage:
//   ./library              menu mode, data file "books.csv" in the current folder
//   ./library data/books.csv   menu mode with a chosen data file
//   ./library demo         runs the scripted demo (normal cases + error cases)
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>




using namespace std;

namespace {

// Thrown when standard input ends (Ctrl+D / Ctrl+Z), so the menu can stop cleanly.
struct InputClosed {};

string readLine(const string& prompt) {
    cout << prompt << flush;
    string text;
    if (!getline(cin, text)) throw InputClosed{};
    return textutil::trim(text);
}

// Keeps asking until the user types a positive whole number.
int readPositiveInt(const string& prompt) {
    for (;;) {
        int value = 0;
        if (textutil::parsePositiveInt(readLine(prompt), value)) return value;
        cout << "  [Error] Please enter a positive whole number.\n";
    }
}

string shorten(const string& text, size_t width) {
    if (text.size() <= width) return text;
    return text.substr(0, width - 3) + "...";
}

void printBooks(const vector<Book>& books) {
    if (books.empty()) {
        cout << "  (no books to show)\n";
        return;
    }
    cout << "  " << left << setw(5) << "ID" << setw(36) << "Title" << setw(24) << "Author"
         << "Status\n";
    cout << "  " << string(74, '-') << "\n";
    for (const Book& book : books) {
        cout << "  " << left << setw(5) << book.getId() << setw(36) << shorten(book.getTitle(), 34)
             << setw(24) << shorten(book.getAuthor(), 22)
             << (book.isAvailable() ? "Available" : "Issued") << "\n";
    }
}

void printWarnings(const LoadResult& result) {
    for (const string& warning : result.warnings) cout << "    [Warning] " << warning << "\n";
}

// ---------------------------------------------------------------- demo
// Runs one action and prints either "[OK] message" or "[Error] message".
template <typename Action>
void attempt(const string& label, Action action) {
    cout << "  > " << label << "\n";
    try {
        const string message = action();  // run first: if it throws, nothing is printed as [OK]
        cout << "    [OK] " << message << "\n";
    } catch (const LibraryError& error) {
        cout << "    [Error] " << error.what() << "\n";
    }
}

void runDemo() {
    const string csvFile = "demo_books.csv";
    Library library;
    cout << "=== Library Management System - Demo ===\n";

    cout << "\n--- 1. Add books (normal cases) ---\n";
    struct Seed {
        int id;
        const char* title;
        const char* author;
    };
    const Seed seeds[] = {
        {101, "Clean Code", "Robert C. Martin"},
        {102, "The C++ Programming Language", "Bjarne Stroustrup"},
        {103, "Effective Modern C++", "Scott Meyers"},
        {104, "Introduction to Algorithms", "Thomas H. Cormen"},
        {105, "Wings of Fire", "A. P. J. Abdul Kalam"},
        {106, "Computer Networks, 5th Edition", "Andrew S. Tanenbaum"},
    };
    for (const Seed& seed : seeds) {
        attempt("Add book " + to_string(seed.id), [&] {
            library.addBook(Book(seed.id, seed.title, seed.author));
            return string("Added \"") + seed.title + "\" by " + seed.author;
        });
    }

    cout << "\n--- 2. List all books ---\n";
    printBooks(library.allBooks());

    cout << "\n--- 3. Search (case-insensitive, title or author) ---\n";
    cout << "  > Search for \"c++\"\n";
    printBooks(library.search("c++"));
    cout << "  > Search for \"KALAM\"\n";
    printBooks(library.search("KALAM"));
    cout << "  > Search for \"zzz\" (no match)\n";
    printBooks(library.search("zzz"));

    cout << "\n--- 4. Issue and return (normal cases) ---\n";
    attempt("Issue book 101", [&] {
        library.issueBook(101);
        return "Issued \"" + library.findById(101).getTitle() + "\"";
    });
    attempt("Issue book 103", [&] {
        library.issueBook(103);
        return "Issued \"" + library.findById(103).getTitle() + "\"";
    });
    cout << "  Status after issuing:\n";
    printBooks(library.allBooks());
    attempt("Return book 101", [&] {
        library.returnBook(101);
        return "Returned \"" + library.findById(101).getTitle() + "\"";
    });

    cout << "\n--- 5. Error cases (handled with exceptions) ---\n";
    attempt("Issue book 103 again (already issued)", [&] {
        library.issueBook(103);
        return string("Issued");
    });
    attempt("Issue book 999 (unknown ID)", [&] {
        library.issueBook(999);
        return string("Issued");
    });
    attempt("Return book 101 (not issued)", [&] {
        library.returnBook(101);
        return string("Returned");
    });
    attempt("Add book 102 again (duplicate ID)", [&] {
        library.addBook(Book(102, "Another Title", "Another Author"));
        return string("Added");
    });
    attempt("Add a book with an empty title", [&] {
        library.addBook(Book(110, "   ", "Some Author"));
        return string("Added");
    });
    attempt("Add a book with ID 0", [&] {
        library.addBook(Book(0, "Zero Book", "Some Author"));
        return string("Added");
    });
    attempt("Search with an empty keyword", [&] {
        library.search("  ");
        return string("Searched");
    });

    cout << "\n--- 6. Save and reload (file I/O) ---\n";
    attempt("Save the library to " + csvFile, [&] {
        library.saveToFile(csvFile);
        return "Saved " + to_string(library.size()) + " books";
    });
    Library reloaded;
    LoadResult loadResult;
    attempt("Load " + csvFile + " into a new, empty Library", [&] {
        loadResult = reloaded.loadFromFile(csvFile);
        return "Loaded " + to_string(loadResult.loaded) + " books, skipped " +
               to_string(loadResult.skipped);
    });
    printBooks(reloaded.allBooks());
    cout << "  (book 103 is still Issued, and the comma in book 106's title survived)\n";

    cout << "\n--- 7. File error cases ---\n";
    attempt("Load a file that does not exist", [&] {
        reloaded.loadFromFile("no_such_file.csv");
        return string("Loaded");
    });
    {
        ofstream bad("demo_bad.csv");
        bad << "id,title,author,available\n"
            << "201,Valid Book One,Author A,1\n"
            << "abc,Bad ID,Author B,1\n"
            << "202,Missing Fields,Author C\n"
            << "203,Valid Book Two,Author D,0\n"
            << "201,Duplicate ID,Author E,1\n";
    }
    Library tolerant;
    LoadResult badResult;
    attempt("Load demo_bad.csv (contains malformed rows)", [&] {
        badResult = tolerant.loadFromFile("demo_bad.csv");
        return "Loaded " + to_string(badResult.loaded) + " good rows, skipped " +
               to_string(badResult.skipped) + " bad rows";
    });
    printWarnings(badResult);

    cout << "\n=== Demo finished ===\n";
}

// ---------------------------------------------------------------- menu
void printMenu() {
    cout << "\n====== Library Management System ======\n"
         << " 1. List all books\n"
         << " 2. Search books (title or author)\n"
         << " 3. Add a book\n"
         << " 4. Issue a book\n"
         << " 5. Return a book\n"
         << " 6. Save now\n"
         << " 7. Run demo (normal and error cases)\n"
         << " 0. Save and exit\n";
}

void runMenu(Library& library, const string& path) {
    for (;;) {
        printMenu();
        const string choice = readLine("Enter choice: ");
        try {
            if (choice == "1") {
                printBooks(library.allBooks());
            } else if (choice == "2") {
                const string keyword = readLine("  Search for: ");
                printBooks(library.search(keyword));
            } else if (choice == "3") {
                const int id = readPositiveInt("  Book ID: ");
                const string title = readLine("  Title: ");
                const string author = readLine("  Author: ");
                library.addBook(Book(id, title, author));
                cout << "  [OK] Book added.\n";
            } else if (choice == "4") {
                const int id = readPositiveInt("  Book ID to issue: ");
                library.issueBook(id);
                cout << "  [OK] Issued \"" << library.findById(id).getTitle() << "\".\n";
            } else if (choice == "5") {
                const int id = readPositiveInt("  Book ID to return: ");
                library.returnBook(id);
                cout << "  [OK] Returned \"" << library.findById(id).getTitle() << "\".\n";
            } else if (choice == "6") {
                library.saveToFile(path);
                cout << "  [OK] Saved " << library.size() << " books to " << path << ".\n";
            } else if (choice == "7") {
                runDemo();
            } else if (choice == "0") {
                return;
            } else {
                cout << "  [Error] Please choose a number from the menu.\n";
            }
        } catch (const LibraryError& error) {
            cout << "  [Error] " << error.what() << "\n";
        }
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc > 1 && string(argv[1]) == "demo") {
        runDemo();
        return 0;
    }

    const string path = (argc > 1) ? argv[1] : "books.csv";
    Library library;

    try {
        const LoadResult result = library.loadFromFile(path);
        cout << "Loaded " << result.loaded << " books from " << path;
        if (result.skipped > 0) cout << " (" << result.skipped << " bad rows skipped)";
        cout << ".\n";
        printWarnings(result);
    } catch (const FileError& error) {
        cout << error.what() << "\nStarting with an empty library.\n";
    }

    try {
        runMenu(library, path);
    } catch (const InputClosed&) {
        cout << "\nInput closed.\n";
    }

    try {
        library.saveToFile(path);
        cout << "Saved " << library.size() << " books to " << path << ". Goodbye!\n";
    } catch (const FileError& error) {
        cout << "[Error] " << error.what() << "\n";
        return 1;
    }
    return 0;
}

