# Company Management System
### CSE126 — Programming (1) | Faculty of Engineering, Alexandria University

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Project Structure](#2-project-structure)
3. [Data Structures](#3-data-structures)
4. [Global Variables](#4-global-variables)
5. [System Features](#5-system-features)
   - [LOAD](#51-load)
   - [ADD](#52-add)
   - [DELETE](#53-delete)
   - [MODIFY](#54-modify)
   - [SEARCH (QUERY)](#55-search-query)
   - [PRINT](#56-print)
   - [SAVE](#57-save)
   - [QUIT](#58-quit)
6. [Input Validation](#6-input-validation)
7. [Algorithms](#7-algorithms)
   - [Search Algorithm](#71-search-algorithm)
   - [Sort Algorithm](#72-sort-algorithm)
8. [Function Reference](#8-function-reference)
9. [File Format](#9-file-format)
10. [User Manual](#10-user-manual)
11. [Sample Runs](#11-sample-runs)
12. [Compilation & Execution](#12-compilation--execution)

---

## 1. Project Overview

The **Company Management System** is a console-based C application designed to create and maintain records of employees within a company. The system provides a full suite of data management operations — loading from file, adding, deleting, modifying, searching, sorting, and saving — all accessible through an interactive menu.

| Property | Value |
|---|---|
| Language | C (C99) |
| Interface | Console / Terminal |
| Storage | Comma-delimited text file (`.txt`) |
| Memory model | Dynamic allocation (`malloc` / `realloc`) |
| Build | `gcc -o company main.c Functions.c` |

---

## 2. Project Structure

```
project/
│
├── main.c          ← Entry point; shows welcome screen and runs the menu loop
├── Functions.h     ← Structs, global variable declarations, all function prototypes
└── Functions.c     ← Full implementation of every function
```

### Dependency Map

```
main.c
  └── Functions.h
        └── Functions.c
              ├── <stdio.h>
              ├── <stdlib.h>
              ├── <string.h>
              ├── <ctype.h>
              └── <time.h>
```

---

## 3. Data Structures

### `Date` struct
Stores a complete calendar date (day, month, year).

```c
typedef struct {
    int day;
    int month;
    int year;
} Date;
```

### `Employee` struct
The core data model. Every employee record holds the following fields:

```c
typedef struct {
    int    id;               // Unique employee identifier
    char  *name;             // Full name (heap-allocated string)
    float  salary;           // Monthly salary
    Date   birthDate;        // Date of birth  (DD-MM-YYYY)
    char  *address;          // Home address   (heap-allocated string)
    char  *mobile;           // Mobile number  (heap-allocated string)
    Date   enrollmentDate;   // Date of joining (set automatically)
    char  *email;            // Email address  (heap-allocated string)
} Employee;
```

> **Note:** String fields (`name`, `address`, `mobile`, `email`) are stored as heap-allocated pointers so that records of any length can be accommodated without wasting memory.

---

## 4. Global Variables

Global variables are declared in `Functions.c` and exposed via `extern` in `Functions.h`. This design allows every function to access the current state of the system without passing the array through every call.

```c
Employee *g_employees;      // Pointer to the dynamic employee array
int       g_count;          // Number of employees currently in memory
char      g_filename[256];  // Path of the loaded data file (used by SAVE)
```

---

## 5. System Features

### 5.1 LOAD

**Trigger:** Called automatically when the program starts.

Prompts the user for a file name, opens the file, and parses every line into an `Employee` struct. Lines that do not match the expected format are skipped with a warning message.

**Expected file format (comma-delimited):**
```
id, Full Name, salary, DD-MM-YYYY, Address, Mobile, DD-MM-YYYY, email
```

**Flow:**
```
Ask for filename
  → Open file
    → For each line:
        Parse id, name, salary, birthDate, address, mobile, enrollmentDate, email
        Trim whitespace from each token
        realloc the global array by +1
        Store the new Employee
  → Print "Loaded N employee(s)"
```

---

### 5.2 ADD

Prompts the user **field by field** and validates every input before accepting it.

| Field | Validation Rule |
|---|---|
| ID | Positive integer, must not already exist |
| Name | Letters and spaces only, non-empty |
| Salary | Positive decimal number |
| Birth Date | Valid calendar date in DD-MM-YYYY format |
| Address | Non-empty string |
| Mobile | 11 digits, starts with 010 / 011 / 012 / 015 |
| Email | Must contain exactly one `@`, a `.` after it, no spaces |
| Enrollment Date | **Set automatically** to the system's current date |

The user is re-prompted until a valid value is entered for each field. The ID uniqueness check prevents duplicate records from being inserted.

---

### 5.3 DELETE

Prompts for an employee **ID**, finds the matching record, asks for confirmation, then removes it.

**Steps:**
1. Read ID from user.
2. Linear search through `g_employees` for a matching `id`.
3. If not found → print error message and return.
4. If found → display the employee's name and ask for confirmation.
5. On confirmation: free all heap-allocated strings, shift the remaining elements one position to the left, decrement `g_count`.

> If the provided ID does not exist in the system, an appropriate error message is displayed and no data is modified.

---

### 5.4 MODIFY

Allows updating **five** fields of an existing employee: name, salary, mobile, address, and email.

The enrollment date and ID cannot be modified.

**Steps:**
1. Ask for the employee's ID.
2. Locate the record via linear search.
3. For each modifiable field, print the prompt and allow the user to skip by leaving the input blank.
4. Validate any non-blank input before applying the change.
5. Report how many fields were actually updated.

---

### 5.5 SEARCH (QUERY)

Performs a **case-insensitive partial-name search** across all employee records.

- The user supplies any substring (e.g., `"ah"` will match `"Ahmed"`, `"Haha"`, etc.).
- All matching employees are printed.
- If no match is found, a "no results" message is displayed.

---

### 5.6 PRINT

Prints **all** employee records sorted alphabetically by name (case-insensitive).

- A temporary copy of the global array is sorted so that the original insertion order in memory is preserved.
- The Bubble Sort algorithm is used (see Section 7.2).

---

### 5.7 SAVE

Writes the current in-memory employee records back to the **same file** that was loaded at startup, overwriting its contents.

The output format matches the input format exactly, ensuring that the file can be loaded again in a future session:

```
id, Full Name, salary, DD-MM-YYYY, address, mobile, DD-MM-YYYY, email
```

---

### 5.8 QUIT

Displays a warning that **all unsaved changes will be discarded**, then asks for confirmation before calling `exit(0)`. If the user declines, control returns to the menu.

---

## 6. Input Validation

### Mobile Number
```
Rules:
  • Exactly 11 characters
  • All characters must be digits
  • Must start with: 010 | 011 | 012 | 015
```

### Email Address
```
Rules:
  • No whitespace characters allowed
  • Must contain exactly one '@'
  • Must contain at least one '.' after the '@'
  • '@' cannot be the first character
  • '.' cannot immediately follow '@'
  • Must have at least one character after the final '.'
```

### Date
```
Rules:
  • Month must be between 1 and 12
  • Day must be valid for the given month (leap years handled)
  • Year must be between 1900 and 2100
```

### Employee ID
```
Rules:
  • Must be a positive integer
  • Must not already exist in the system (uniqueness enforced)
```

### Name
```
Rules:
  • Non-empty
  • May only contain alphabetic characters (A–Z, a–z) and spaces
```

---

## 7. Algorithms

### 7.1 Search Algorithm

**Type:** Linear Search (Sequential Search)

**Used in:** QUERY, DELETE, MODIFY

**Time complexity:** O(n)

**Description:**  
The array is scanned from index `0` to `g_count - 1`. For QUERY (name search), each employee's name is first converted to lowercase and compared against the lowercase query using `strstr()` to check for substring containment.

```
Procedure: queryEmployee
  Input : keyword (partial name string)
  Output: all matching employee records printed to screen

  convert keyword to lowercase → keyLower
  found ← 0

  for i from 0 to g_count - 1:
      convert g_employees[i].name to lowercase → nameLower
      if nameLower contains keyLower:
          print g_employees[i]
          found ← found + 1

  if found == 0:
      print "No employees found"
  else:
      print "Found N matching employee(s)"
```

---

### 7.2 Sort Algorithm

**Type:** Bubble Sort (Selection-style variant — compares all pairs)

**Used in:** PRINT

**Time complexity:** O(n²)

**Description:**  
A temporary copy of the employee array is sorted in-place. The comparison uses `strcasecmp()` so that uppercase and lowercase letters are treated equally. Swapping is done by exchanging entire `Employee` struct values (pointer swap, not deep copy).

```
Procedure: sortByName(arr, n)
  Input : arr[] — array of Employee structs, n — array length
  Output: arr[] sorted alphabetically by name (case-insensitive)

  for i from 0 to n - 2:
      for j from i + 1 to n - 1:
          if strcasecmp(arr[i].name, arr[j].name) > 0:
              swap(arr[i], arr[j])
```

> A **copy** of `g_employees` is sorted so that the original insertion order in memory is not disturbed.

---

## 8. Function Reference

### UI Functions

| Function | Description |
|---|---|
| `showWelcomeBox()` | Prints the ASCII welcome banner |
| `showMenu()` | Prints the 7-option numbered menu |
| `selectOption()` | Reads and validates a menu choice (1–7) |
| `featuresControl(option)` | Dispatches the chosen option to the correct feature function |

### Core Feature Functions

| Function | Description |
|---|---|
| `loadEmployees()` | Reads employees from a CSV file into `g_employees` |
| `addEmployee()` | Prompts for and validates a new employee record |
| `deleteEmployee()` | Finds an employee by ID and removes them |
| `modifyEmployee()` | Updates selected fields of an existing employee |
| `queryEmployee()` | Case-insensitive partial-name search |
| `printEmployees()` | Prints all employees sorted by name |
| `saveEmployees()` | Writes `g_employees` back to `g_filename` |
| `quitSystem()` | Warns user and exits |

### Helper / Validation Functions

| Function | Signature | Description |
|---|---|---|
| `readInt` | `int readInt(int *out)` | Safe integer input; returns 1 on success |
| `readLine` | `void readLine(char *buf, int size)` | Safe line input; strips trailing newline |
| `updateString` | `int updateString(char **field, const char *newVal)` | Frees old string and replaces with new copy |
| `toLowerStr` | `void toLowerStr(char *dst, const char *src, int max)` | Copies string to dst in lowercase |
| `trim` | `void trim(char *s)` | Removes leading and trailing whitespace in-place |
| `isValidMobile` | `int isValidMobile(const char *m)` | Returns 1 if mobile is valid |
| `isValidEmail` | `int isValidEmail(const char *e)` | Returns 1 if email is valid |
| `isUniqueId` | `int isUniqueId(int id)` | Returns 1 if id is not already in use |
| `isValidDate` | `int isValidDate(int d, int m, int y)` | Returns 1 if the date is a valid calendar date |
| `getCurrentDate` | `Date getCurrentDate(void)` | Returns today's date from the system clock |
| `sortByName` | `void sortByName(Employee *arr, int n)` | In-place bubble sort by name |
| `printEmployee` | `void printEmployee(Employee e)` | Prints a single formatted employee record |

---

## 9. File Format

### Structure
Each employee occupies **exactly one line**. Fields are separated by `, ` (comma + space).

```
<id>, <name>, <salary>, <DD-MM-YYYY>, <address>, <mobile>, <DD-MM-YYYY>, <email>
```

### Example
```
123, Steven Thomas, 2000.00, 10-06-1995, 26 Elhoreya Street, 01234567899, 15-09-2022, sthomas@gmail.com
456, Ahmed Ali, 5000.00, 22-03-1990, 14 Tahrir Square, 01012345678, 01-01-2023, aali@yahoo.com
789, Sara Mohamed, 3500.00, 05-11-1998, 7 Nile Corniche, 01156789012, 20-06-2024, smohamed@gmail.com
```

> **Important:** Field values must not contain commas, as the comma is used as the delimiter.

---

## 10. User Manual

### How to Start the System

```bash
# 1. Compile
gcc -o company main.c Functions.c

# 2. Run
./company
```

### Step-by-Step Usage

**On startup**, the system automatically asks you for a file name to load:
```
Enter file name to load: employees.txt
Loaded 3 employee(s) from 'employees.txt'.
```

If the file does not exist, the system starts with an empty database (you can still add employees and save them to a new file).

---

**The main menu** appears after loading:
```
--- MENU ---
1. ADD
2. DELETE
3. MODIFY
4. SEARCH
5. PRINT
6. SAVE
7. QUIT
Enter your choice:
```

---

### ADD (Option 1)
Enter each field when prompted. Press **Enter** to submit. Invalid input will be rejected with an error message and you will be re-prompted.

```
--- Add New Employee ---
Enter ID: 101
Enter full name: Mona Hassan
Enter salary: 4500
Enter birth date (DD-MM-YYYY): 14-07-1992
Enter address: 3 Garden City Road
Enter mobile number: 01098765432
Enter email: mhassan@gmail.com

Employee added successfully! (Enrollment date: 16-05-2026)
```

---

### DELETE (Option 2)
```
Enter employee ID to delete: 456
Found: Ahmed Ali (ID 456)
Confirm delete? (1 = Yes / 2 = No): 1
Employee deleted successfully.
```

---

### MODIFY (Option 3)
Leave any field **blank** to keep the current value unchanged.
```
Enter employee ID to modify: 123
Employee found: Steven Thomas

New name (leave blank to skip):
New salary (leave blank to skip): 2500
  Salary updated.
New mobile (leave blank to skip):
New address (leave blank to skip): 50 Corniche Road
  Address updated.
New email (leave blank to skip):

Modification completed successfully.
```

---

### SEARCH (Option 4)
```
Enter name (or part of name) to search: mo

Search Results:
==================================================
ID:              789
Name:            Sara Mohamed
...
--------------------------------------------------
Found 1 matching employee(s).
```

---

### PRINT (Option 5)
Prints all employees sorted alphabetically by name.

---

### SAVE (Option 6)
Writes all current data back to the same file that was loaded.
```
Data saved to 'employees.txt' successfully.
```

---

### QUIT (Option 7)
```
+--------------------------------------------------+
|  WARNING: All unsaved changes will be DISCARDED! |
+--------------------------------------------------+
Are you sure you want to quit? (1 = Yes / 2 = No): 1
Goodbye!
```

---

## 11. Sample Runs

### Sample Run 1 — Full Workflow

```
                        +------------------------------------------+
                        |                                          |
                        | WELCOME TO COMPANY MANAGEMENT SYSTEM     |
                        |                                          |
                        +------------------------------------------+

Enter file name to load: employees.txt
Loaded 3 employee(s) from 'employees.txt'.

--- MENU ---
1. ADD  2. DELETE  3. MODIFY  4. SEARCH  5. PRINT  6. SAVE  7. QUIT
Enter your choice: 5

All Employees (sorted by name):
==================================================
ID:              456
Name:            Ahmed Ali
Salary:          5000.00
Birth Date:      22-03-1990
Address:         14 Tahrir Square
Mobile:          01012345678
Enrollment Date: 01-01-2023
Email:           aali@yahoo.com
--------------------------------------------------
ID:              789
Name:            Sara Mohamed
...
--------------------------------------------------
ID:              123
Name:            Steven Thomas
...
--------------------------------------------------
Total: 3 employee(s).
```

---

### Sample Run 2 — Validation Errors

```
Enter your choice: 1
--- Add New Employee ---
Enter ID: abc
Invalid input. Please enter a number.
Enter ID: -5
ID must be a positive number.
Enter ID: 123
ID 123 already exists. Please enter a unique ID.
Enter ID: 999

Enter full name: John123
Name must contain letters and spaces only.
Enter full name: John Smith

Enter salary: -100
Invalid salary. Please enter a positive number.
Enter salary: 3000

Enter birth date (DD-MM-YYYY): 31-02-1990
Invalid date. Use DD-MM-YYYY format.
Enter birth date (DD-MM-YYYY): 15-06-1990

Enter address: 22 Ramses Street

Enter mobile number: 0991234567
Invalid mobile number.
Enter mobile number: 01112345678

Enter email: notanemail
Invalid email format.
Enter email: jsmith@company.com

Employee added successfully! (Enrollment date: 16-05-2026)
```

---

### Sample Run 3 — Search

```
Enter your choice: 4
Enter name (or part of name) to search: ali

Search Results:
==================================================
ID:              456
Name:            Ahmed Ali
Salary:          5000.00
Birth Date:      22-03-1990
Address:         14 Tahrir Square
Mobile:          01012345678
Enrollment Date: 01-01-2023
Email:           aali@yahoo.com
--------------------------------------------------
Found 1 matching employee(s).
```

---

## 12. Compilation & Execution

### Requirements
- GCC compiler (any version supporting C99 or later)
- Linux / macOS / Windows (with MinGW or WSL)

### Commands

```bash
# Compile
gcc -Wall -o company main.c Functions.c

# Run
./company

# Run with output saved to a log (optional)
./company | tee session.log
```

### Common Errors

| Error | Cause | Fix |
|---|---|---|
| `Cannot open 'file.txt'` | File does not exist | Check path, or create the file |
| `Warning: skipped malformed line N` | A line in the file does not match the expected format | Fix the line in the file |
| `ID already exists` | Duplicate ID entered during ADD | Use a different ID |

---

*Company Management System*  
*Faculty of Engineering, Alexandria University*
