# Practical 12 - Git steps (meaningful commit history)

The manual asks for a GitHub repository with **a meaningful commit history**, so add the files in small
logical steps instead of one big upload. Follow these steps in order.

## A. Create the repository (once)
1. On github.com click **New repository**.
2. Name: `library-management-system`. Visibility: Public.
3. **Do NOT tick "Add a README"** (an empty repo avoids conflicts). Click **Create repository**.
4. Copy the HTTPS URL, then in a terminal (VS Code: Ctrl+`):
```powershell
cd path\to\where\you\keep\projects
git clone https://github.com/shristisharma770/library-management-system.git
cd library-management-system
```

## B. Copy the project files into the empty folder
Unzip `library-management.zip`, open the `library-management` folder inside it, and copy **its contents**
(`src`, `data`, `docs`, `Makefile`, `README.md`, `.gitignore`) into the `library-management-system` folder
you just cloned. The cloned folder should look like this:
```
library-management-system/
  .gitignore   Makefile   README.md
  src/   data/   docs/
```
`.gitignore` starts with a dot and may be hidden. In File Explorer: View -> Show -> Hidden items.

Nothing is committed yet. Now do the commits below, one at a time.

## C. Nine commits (run these in the terminal, in this order)
```powershell
git add .gitignore
git commit -m "Add .gitignore for build output and generated files"

git add src/Exceptions.h
git commit -m "Add custom exception hierarchy for library errors"

git add src/TextUtil.h src/TextUtil.cpp
git commit -m "Add text and CSV helper functions"

git add src/Book.h src/Book.cpp
git commit -m "Add Book class with validation and issue/return logic"

git add src/Library.h src/Library.cpp
git commit -m "Add Library class using std::map with search and CSV save/load"

git add src/main.cpp Makefile
git commit -m "Add menu interface, demo mode and Makefile"

git add data/books.csv
git commit -m "Add sample library data"

git add docs/uml_class_diagram.png docs/sample_output.txt
git commit -m "Add UML class diagram and sample demo output"

git add README.md
git commit -m "Add README with features, build steps and usage"
```

## D. Push and check
```powershell
git push -u origin HEAD
git log --oneline
```
Sign in to GitHub in the browser window if it asks. `git log --oneline` should list 9 commits.
On GitHub, open the repository and click **N commits** (next to the green Code button) to see the history.

## E. Optional extra commit after you take screenshots
Save your screenshots in `docs/` (for example `docs/demo_output.png`) and run:
```powershell
git add docs
git commit -m "Add console screenshots"
git push
```

## F. Check that these are NOT in the repository
`library`, `library.exe`, `demo_books.csv`, `demo_bad.csv`, `books.csv` (the `.gitignore` blocks them).
`data/books.csv` IS meant to be there (it is the sample data).

## Evidence to collect for the manual
1. Repository main page (files and the latest commit)
2. Commit history page (list of the 9 commits)
3. Program output: demo run (normal and error cases)
4. Optionally one menu session (add, issue, return, an invalid ID)

## Running the program without a local compiler
Open `online_gdb/library_all_in_one.cpp`, paste it into onlinegdb.com/online_c++_compiler, click Run, type
`7` and press Enter (demo), then `0` and press Enter (exit). The demo output appears in the console.
