# Company Management System

**CSE126 · Programming (1)**  
Alexandria University — Faculty of Engineering

A console application in C for managing employee records stored in a comma-delimited text file. Records are kept in a dynamically allocated array (`malloc` / `realloc`), and every input is validated.

---

## Features

| Menu option | What it does |
|-------------|--------------|
| *(at startup)* **LOAD** | Reads the employee file you name; malformed lines are skipped with a warning |
| **1. ADD** | Adds an employee field by field; the enrollment date is set to today automatically |
| **2. DELETE** | Deletes by ID, with confirmation |
| **3. MODIFY** | Updates name, salary, mobile, address, or email (leave a field blank to skip it) |
| **4. SEARCH** | Case-insensitive partial-name search |
| **5. PRINT** | Prints all employees sorted by name |
| **6. SAVE** | Writes everything back to the loaded file |
| **7. QUIT** | Exits, warning that unsaved changes are discarded |

## Input validation

| Field | Rule |
|-------|------|
| ID | Positive integer, unique |
| Name | Letters and spaces only |
| Salary | Positive, finite number |
| Birth date | `DD-MM-YYYY`, real calendar date (leap years handled) |
| Address | Not empty, no commas |
| Mobile | 11 digits, starting with `010`, `011`, `012`, or `015` |
| Email | One `@`, a dot after it, no spaces, no commas |

Commas are rejected in free-text fields because the data file uses `,` as its delimiter. Closing the input stream (Ctrl+D, or Ctrl+Z on Windows) exits cleanly instead of looping.

## Build & Run

```bash
make
./company
```

or without `make`:

```bash
gcc -std=gnu11 -Wall -Wextra -O2 -o company main.c Functions.c
```

At the prompt, enter a file name. Try the included sample:

```
Enter file name to load: employees_sample.txt
```

## File format

One employee per line, fields separated by `, `:

```
id, Full Name, salary, DD-MM-YYYY, address, mobile, DD-MM-YYYY, email
```

## Project structure

```
.
├── main.c                # Entry point and menu loop
├── Functions.c           # All features and helpers
├── Functions.h           # Structs, globals, prototypes
├── Makefile
├── employees_sample.txt  # Example data file
├── Documentation.md      # Full design documentation
└── README.md
```

See [`Documentation.md`](Documentation.md) for the data structures, algorithms, and function reference.

## Notes

- Uses `strdup` and `strcasecmp` (POSIX); they are available with GCC on Linux, macOS, and MinGW on Windows.
- Colored output uses ANSI escape codes; use a terminal that supports them (Windows Terminal, VS Code, most Linux and macOS terminals).

## Author

Omar El-Banna, Computer and Communication Engineering, Faculty of Engineering, Alexandria University.

## License

Released under the MIT License. Developed as a course project for Programming (1), Faculty of Engineering, Alexandria University.
