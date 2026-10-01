#include "Functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

/* Called when stdin is closed (Ctrl+D / Ctrl+Z / end of piped input).
   Without this, every input loop would spin forever on EOF. */
static void exitOnEOF(void) {
    printf(RED "\nInput closed. Exiting (unsaved changes are discarded).\n" RESET);
    exit(0);
}

/* ============================================================
   Global Variables
   ============================================================ */
Employee *g_employees  = NULL;
int       g_count      = 0;
char      g_filename[256] = "";

/* ============================================================
   UI Functions
   ============================================================ */

void showWelcomeBox(void) {
    printf(CUSTOMRED  "\t\t\t+------------------------------------------+\n");
    printf(            "\t\t\t|                                          |\n");
    printf(            "\t\t\t|" CUSTOMYELLOW " WELCOME TO COMPANY MANAGEMENT SYSTEM " CUSTOMRED "   |\n");
    printf(            "\t\t\t|                                          |\n");
    printf(            "\t\t\t+------------------------------------------+\n\n" RESET);
}

void showMenu(void) {
    printf(CUSTOMRED "--- " CUSTOMYELLOW "MENU" CUSTOMRED " ---\n" RESET);
    printf(CUSTOMORANGE
           "1. ADD\n"
           "2. DELETE\n"
           "3. MODIFY\n"
           "4. SEARCH\n"
           "5. PRINT\n"
           "6. SAVE\n"
           "7. QUIT\n" RESET);
    printf(CYAN "Enter your choice: " RESET);
}

/* Validates and returns a menu option */
int selectOption(void) {
    char choice[100];
    int  optionNumber;

    while (1) {
        if (!fgets(choice, sizeof(choice), stdin)) exitOnEOF();
        {
            /* Detect buffer overflow (very long input) */
            if (strchr(choice, '\n') == NULL) {
                printf(CYAN "Please enter a valid number: " RESET);
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }
            char *flag = NULL;
            optionNumber = (int)strtol(choice, &flag, 10);
            if (*flag == '\n' && optionNumber >= 1 && optionNumber <= 7)
                break;
            printf(CYAN "Please enter a valid number (1-7): " RESET);
        }
    }
    return optionNumber;
}

/* Dispatches menu option to the correct function */
void featuresControl(int option) {
    switch (option) {
        case 1: addEmployee();    break;
        case 2: deleteEmployee(); break;
        case 3: modifyEmployee(); break;
        case 4: queryEmployee();  break;
        case 5: printEmployees(); break;
        case 6: saveEmployees();  break;
        case 7: quitSystem();     break;
        default:
            printf(RED "Invalid option.\n" RESET);
    }
}

/* ==================
   Helper Functions 
   ================== */

/* Remove leading whitespace in-place */
void trimLeft(char *s) {
    if (!s) return;
    int i = 0;
    while (s[i] == ' ' || s[i] == '\t') i++;
    if (i > 0) memmove(s, s + i, strlen(s) - i + 1);
}

/* Remove trailing whitespace / newlines in-place */
void trimRight(char *s) {
    if (!s) return;
    int len = (int)strlen(s);
    while (len > 0 && (s[len-1] == ' '  || s[len-1] == '\t' ||
                       s[len-1] == '\n' || s[len-1] == '\r'))
        s[--len] = '\0';
}

void trim(char *s) { trimRight(s); trimLeft(s); }

/* Safe integer reader */
int readInt(int *out) {
    char  buffer[50];
    char *end;

    if (!fgets(buffer, sizeof(buffer), stdin)) exitOnEOF();

    if (!strchr(buffer, '\n')) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
    }

    long val = strtol(buffer, &end, 10);
    while (isspace((unsigned char)*end)) end++;
    if (*end != '\0' && *end != '\n') return 0;

    *out = (int)val;
    return 1;
}

/* Safe line reader – strips the trailing newline */
void readLine(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; exitOnEOF(); }
    if (!strchr(buf, '\n')) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    buf[strcspn(buf, "\n")] = '\0';
}

/* Free old string and replace with a new copy */
int updateString(char **field, const char *newVal) {
    if (!newVal || !*newVal) return 0;
    char *tmp = malloc(strlen(newVal) + 1);
    if (!tmp) { printf(RED "Memory allocation error.\n" RESET); return 0; }
    strcpy(tmp, newVal);
    free(*field);
    *field = tmp;
    return 1;
}

/* Copy src to dst in lowercase */
void toLowerStr(char *dst, const char *src, int max_len) {
    int i = 0;
    if (!dst || !src || max_len <= 0) return;
    while (*src && i < max_len - 1)
        dst[i++] = (char)tolower((unsigned char)*src++);
    dst[i] = '\0';
}

/* Validate Egyptian mobile number (11 digits, starts with 010/011/012/015) */
int isValidMobile(const char *m) {
    if (!m || strlen(m) != 11) return 0;
    if (strncmp(m, "010", 3) && strncmp(m, "011", 3) &&
        strncmp(m, "012", 3) && strncmp(m, "015", 3)) return 0;
    for (int i = 0; m[i]; i++)
        if (!isdigit((unsigned char)m[i])) return 0;
    return 1;
}

/* Validate email – must have exactly one @, a dot after it, no spaces */
int isValidEmail(const char *e) {
    if (!e || !*e) return 0;
    int index = 0;
    const char *at  = NULL;
    const char *dot = NULL;

    for (int i = 0; e[i]; i++) {
        if (isspace((unsigned char)e[i]) || e[i] == ',') return 0;
        if (e[i] == '@') { if (at) return 0; at = e + i; index = i; }
        if (e[i] == '.') { if (i == 0 || e[i-1] == '.') return 0; dot = e + i; }
    }
    if (!at || !dot)         return 0;
    if (at == e)             return 0;   /* nothing before @ */
    if (*(at + 1) == '.')   return 0;   /* @ followed by dot */
    if (dot < at)            return 0;   /* dot is before @ */
    if (*(dot + 1) == '\0') return 0;   /* nothing after dot */
    if (index > 0 && e[index-1] == '.') return 0; /* dot right before @ */
    return 1;
}

/* Returns 1 if the id is NOT already used */
int isUniqueId(int id) {
    for (int i = 0; i < g_count; i++)
        if (g_employees[i].id == id) return 0;
    return 1;
}

/* Returns 1 if the date is a valid calendar date */
int isValidDate(int d, int m, int y) {
    if (m < 1 || m > 12)     return 0;
    if (d < 1 || d > 31)     return 0;
    if (y < 1900 || y > 2100) return 0;
    int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    /* Leap year check */
    if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) days[2] = 29;
    return d <= days[m];
}

/* Returns today's date using system clock */
Date getCurrentDate(void) {
    Date d;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    d.day   = t->tm_mday;
    d.month = t->tm_mon + 1;
    d.year  = t->tm_year + 1900;
    return d;
}

/* Print one employee record */
void printEmployee(Employee e) {
    printf(YELLOW  "ID:              " RESET "%d\n",     e.id);
    printf(YELLOW  "Name:            " RESET "%s\n",     e.name);
    printf(YELLOW  "Salary:          " RESET "%.2f\n",   e.salary);
    printf(YELLOW  "Birth Date:      " RESET "%02d-%02d-%04d\n",
           e.birthDate.day, e.birthDate.month, e.birthDate.year);
    printf(YELLOW  "Address:         " RESET "%s\n",     e.address);
    printf(YELLOW  "Mobile:          " RESET "%s\n",     e.mobile);
    printf(YELLOW  "Enrollment Date: " RESET "%02d-%02d-%04d\n",
           e.enrollmentDate.day, e.enrollmentDate.month, e.enrollmentDate.year);
    printf(YELLOW  "Email:           " RESET "%s\n",     e.email);
    printf("--------------------------------------------------\n");
}

/* Bubble sort employees by name (case-insensitive) */
void sortByName(Employee *arr, int n) {
    Employee temp;
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (strcasecmp(arr[i].name, arr[j].name) > 0) {
                temp   = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
}

/* ============================================================
   Core Feature Implementations
   ============================================================ */

/* ----------------------------------------------------------
   LOAD  – reads a comma-delimited file into g_employees
   Format per line:
     id, Full Name, salary, DD-MM-YYYY, address, mobile, DD-MM-YYYY, email
   ---------------------------------------------------------- */
void loadEmployees(void) {
    printf(CYAN "Enter file name to load: " RESET);
    readLine(g_filename, sizeof(g_filename));

    if (strlen(g_filename) == 0) {
        printf(RED "No file name entered. Starting with empty system.\n" RESET);
        return;
    }

    FILE *f = fopen(g_filename, "r");
    if (!f) {
        printf(RED "Cannot open '%s'. Starting with empty system.\n" RESET, g_filename);
        return;
    }

    /* Free any previously loaded data */
    free(g_employees);
    g_employees = NULL;
    g_count     = 0;

    char line[512];
    int  lineNum = 0;

    while (fgets(line, sizeof(line), f)) {
        lineNum++;
        trim(line);
        if (strlen(line) == 0) continue;   /* skip blank lines */

        char    tmp[512];
        strcpy(tmp, line);                 /* strtok modifies the string */

        Employee e;
        char    *tok;
        int      ok = 1;

        /* id */
        tok = strtok(tmp, ",");
        if (!tok) { ok = 0; goto bad_line; }
        trim(tok);
        e.id = (int)strtol(tok, NULL, 10);

        /* name */
        tok = strtok(NULL, ",");
        if (!tok) { ok = 0; goto bad_line; }
        trim(tok);
        e.name = strdup(tok);

        /* salary */
        tok = strtok(NULL, ",");
        if (!tok) { free(e.name); ok = 0; goto bad_line; }
        trim(tok);
        e.salary = strtof(tok, NULL);

        /* birth date  DD-MM-YYYY */
        tok = strtok(NULL, ",");
        if (!tok) { free(e.name); ok = 0; goto bad_line; }
        trim(tok);
        if (sscanf(tok, "%d-%d-%d",
                   &e.birthDate.day,
                   &e.birthDate.month,
                   &e.birthDate.year) != 3)
            { free(e.name); ok = 0; goto bad_line; }

        /* address */
        tok = strtok(NULL, ",");
        if (!tok) { free(e.name); ok = 0; goto bad_line; }
        trim(tok);
        e.address = strdup(tok);

        /* mobile */
        tok = strtok(NULL, ",");
        if (!tok) { free(e.name); free(e.address); ok = 0; goto bad_line; }
        trim(tok);
        e.mobile = strdup(tok);

        /* enrollment date  DD-MM-YYYY */
        tok = strtok(NULL, ",");
        if (!tok) { free(e.name); free(e.address); free(e.mobile); ok = 0; goto bad_line; }
        trim(tok);
        if (sscanf(tok, "%d-%d-%d",
                   &e.enrollmentDate.day,
                   &e.enrollmentDate.month,
                   &e.enrollmentDate.year) != 3)
            { free(e.name); free(e.address); free(e.mobile); ok = 0; goto bad_line; }

        /* email */
        tok = strtok(NULL, ",");
        if (!tok) { free(e.name); free(e.address); free(e.mobile); ok = 0; goto bad_line; }
        trim(tok);
        e.email = strdup(tok);

        /* Append to global array */
        Employee *t = realloc(g_employees, (g_count + 1) * sizeof(Employee));
        if (!t) { printf(RED "Memory error on line %d.\n" RESET, lineNum); break; }
        g_employees = t;
        g_employees[g_count++] = e;
        continue;

bad_line:
        if (!ok)
            printf(RED "Warning: skipped malformed line %d.\n" RESET, lineNum);
    }

    fclose(f);
    printf(GREEN "Loaded %d employee(s) from '%s'.\n" RESET, g_count, g_filename);
}

/* ----------------------------------------------------------
   SAVE  – writes g_employees back to the same file
   ---------------------------------------------------------- */
void saveEmployees(void) {
    if (strlen(g_filename) == 0) {
        printf(CYAN "Enter file name to save: " RESET);
        readLine(g_filename, sizeof(g_filename));
        if (strlen(g_filename) == 0) {
            printf(RED "No file name entered. Changes not saved.\n" RESET);
            return;
        }
    }

    FILE *f = fopen(g_filename, "w");
    if (!f) {
        printf(RED "Cannot open '%s' for writing.\n" RESET, g_filename);
        return;
    }

    for (int i = 0; i < g_count; i++) {
        Employee *e = &g_employees[i];
        fprintf(f, "%d, %s, %.2f, %02d-%02d-%04d, %s, %s, %02d-%02d-%04d, %s\n",
                e->id,
                e->name,
                e->salary,
                e->birthDate.day,  e->birthDate.month,  e->birthDate.year,
                e->address,
                e->mobile,
                e->enrollmentDate.day, e->enrollmentDate.month, e->enrollmentDate.year,
                e->email);
    }

    fclose(f);
    printf(GREEN "Data saved to '%s' successfully.\n" RESET, g_filename);
}

/* ----------------------------------------------------------
   ADD  – prompts field-by-field, enrollment date = today
   ---------------------------------------------------------- */
void addEmployee(void) {
    Employee e;
    char     buf[300];
    int      ival;

    printf(CUSTOMYELLOW "\n--- Add New Employee ---\n" RESET);

    /* ----- ID ----- */
    while (1) {
        printf(CYAN "Enter ID: " RESET);
        if (!readInt(&ival)) { printf(RED "Invalid input. Please enter a number.\n" RESET); continue; }
        if (ival <= 0)        { printf(RED "ID must be a positive number.\n" RESET); continue; }
        if (!isUniqueId(ival)){ printf(RED "ID %d already exists. Please enter a unique ID.\n" RESET, ival); continue; }
        e.id = ival;
        break;
    }

    /* ----- Name ----- */
    while (1) {
        printf(CYAN "Enter full name: " RESET);
        readLine(buf, sizeof(buf));
        if (strlen(buf) == 0) { printf(RED "Name cannot be empty.\n" RESET); continue; }
        int valid = 1;
        for (int i = 0; buf[i]; i++)
            if (!isalpha((unsigned char)buf[i]) && buf[i] != ' ')
                { valid = 0; break; }
        if (!valid) { printf(RED "Name must contain letters and spaces only.\n" RESET); continue; }
        e.name = strdup(buf);
        break;
    }

    /* ----- Salary ----- */
    while (1) {
        printf(CYAN "Enter salary: " RESET);
        readLine(buf, sizeof(buf));
        char  *end;
        float  sal = strtof(buf, &end);
        if (*end != '\0' || !(sal > 0) || !isfinite(sal))
            { printf(RED "Invalid salary. Please enter a positive number.\n" RESET); continue; }
        e.salary = sal;
        break;
    }

    /* ----- Birth Date ----- */
    while (1) {
        printf(CYAN "Enter birth date (DD-MM-YYYY): " RESET);
        readLine(buf, sizeof(buf));
        int d, m, y;
        if (sscanf(buf, "%d-%d-%d", &d, &m, &y) != 3 || !isValidDate(d, m, y))
            { printf(RED "Invalid date. Use DD-MM-YYYY format (e.g. 15-06-1995).\n" RESET); continue; }
        e.birthDate.day   = d;
        e.birthDate.month = m;
        e.birthDate.year  = y;
        break;
    }

    /* ----- Address ----- */
    while (1) {
        printf(CYAN "Enter address: " RESET);
        readLine(buf, sizeof(buf));
        if (strlen(buf) == 0) { printf(RED "Address cannot be empty.\n" RESET); continue; }
        if (strchr(buf, ',')) { printf(RED "Address cannot contain commas (the data file is comma-delimited).\n" RESET); continue; }
        e.address = strdup(buf);
        break;
    }

    /* ----- Mobile ----- */
    while (1) {
        printf(CYAN "Enter mobile number (11 digits, starts with 010/011/012/015): " RESET);
        readLine(buf, sizeof(buf));
        if (!isValidMobile(buf))
            { printf(RED "Invalid mobile number.\n" RESET); continue; }
        e.mobile = strdup(buf);
        break;
    }

    /* ----- Email ----- */
    while (1) {
        printf(CYAN "Enter email (example@domain.com): " RESET);
        readLine(buf, sizeof(buf));
        if (!isValidEmail(buf))
            { printf(RED "Invalid email format.\n" RESET); continue; }
        e.email = strdup(buf);
        break;
    }

    /* ----- Enrollment Date = today (automatic) ----- */
    e.enrollmentDate = getCurrentDate();

    /* ----- Append to global array ----- */
    Employee *t = realloc(g_employees, (g_count + 1) * sizeof(Employee));
    if (!t) { printf(RED "Memory error. Employee not added.\n" RESET); return; }
    g_employees = t;
    g_employees[g_count++] = e;

    printf(GREEN "\nEmployee added successfully! "
                 "(Enrollment date: %02d-%02d-%04d)\n" RESET,
           e.enrollmentDate.day, e.enrollmentDate.month, e.enrollmentDate.year);
}

/* ----------------------------------------------------------
   DELETE  – removes the employee with the given ID
   ---------------------------------------------------------- */
void deleteEmployee(void) {
    if (g_count == 0) { printf(RED "No employees in the system.\n" RESET); return; }

    int id;
    printf(CYAN "Enter employee ID to delete: " RESET);
    if (!readInt(&id)) { printf(RED "Invalid input.\n" RESET); return; }

    int index = -1;
    for (int i = 0; i < g_count; i++)
        if (g_employees[i].id == id) { index = i; break; }

    if (index == -1) {
        printf(RED "No employee with ID %d was found.\n" RESET, id);
        return;
    }

    /* Confirm */
    printf(CUSTOMYELLOW "Found: %s (ID %d)\n" RESET,
           g_employees[index].name, g_employees[index].id);
    printf(CYAN "Confirm delete? (1 = Yes / 2 = No): " RESET);
    char buf[10];
    readLine(buf, sizeof(buf));
    if (buf[0] != '1') { printf(CUSTOMORANGE "Delete cancelled.\n" RESET); return; }

    /* Free heap strings */
    free(g_employees[index].name);
    free(g_employees[index].address);
    free(g_employees[index].mobile);
    free(g_employees[index].email);

    /* Shift remaining elements left */
    for (int i = index; i < g_count - 1; i++)
        g_employees[i] = g_employees[i + 1];
    g_count--;

    printf(GREEN "Employee deleted successfully.\n" RESET);
}

/* ----------------------------------------------------------
   MODIFY  – can change: name, salary, mobile, address, email
   ---------------------------------------------------------- */
void modifyEmployee(void) {
    if (g_count == 0) { printf(RED "No employees in the system.\n" RESET); return; }

    int id;
    printf(CYAN "Enter employee ID to modify: " RESET);
    if (!readInt(&id)) { printf(RED "Invalid input.\n" RESET); return; }

    Employee *e = NULL;
    for (int i = 0; i < g_count; i++)
        if (g_employees[i].id == id) { e = &g_employees[i]; break; }

    if (!e) { printf(RED "No employee with ID %d was found.\n" RESET, id); return; }

    printf(GREEN "\nEmployee found: %s\n\n" RESET, e->name);

    char buf[300];
    int  changed = 0;

    /* Name */
    printf(CYAN "New name (leave blank to skip): " RESET);
    readLine(buf, sizeof(buf));
    if (strlen(buf) > 0) {
        int valid = 1;
        for (int i = 0; buf[i]; i++)
            if (!isalpha((unsigned char)buf[i]) && buf[i] != ' ')
                { valid = 0; break; }
        if (valid) { updateString(&e->name, buf); printf(GREEN "Name updated.\n" RESET); changed++; }
        else         printf(RED "Invalid name, skipped.\n" RESET);
    }

    /* Salary */
    printf(CYAN "New salary (leave blank to skip): " RESET);
    readLine(buf, sizeof(buf));
    if (strlen(buf) > 0) {
        char  *end;
        float  sal = strtof(buf, &end);
        if (*end == '\0' && sal > 0 && isfinite(sal))
            { e->salary = sal; printf(GREEN "Salary updated.\n" RESET); changed++; }
        else
            printf(RED "Invalid salary, skipped.\n" RESET);
    }

    /* Mobile */
    printf(CYAN "New mobile (leave blank to skip): " RESET);
    readLine(buf, sizeof(buf));
    if (strlen(buf) > 0) {
        if (isValidMobile(buf))
            { updateString(&e->mobile, buf); printf(GREEN "Mobile updated.\n" RESET); changed++; }
        else
            printf(RED "Invalid mobile number, skipped.\n" RESET);
    }

    /* Address */
    printf(CYAN "New address (leave blank to skip): " RESET);
    readLine(buf, sizeof(buf));
    if (strlen(buf) > 0) {
        if (strchr(buf, ','))
            printf(RED "Address cannot contain commas, skipped.\n" RESET);
        else
            { updateString(&e->address, buf); printf(GREEN "Address updated.\n" RESET); changed++; }
    }

    /* Email */
    printf(CYAN "New email (leave blank to skip): " RESET);
    readLine(buf, sizeof(buf));
    if (strlen(buf) > 0) {
        if (isValidEmail(buf))
            { updateString(&e->email, buf); printf(GREEN "Email updated.\n" RESET); changed++; }
        else
            printf(RED "Invalid email format, skipped.\n" RESET);
    }

    if (changed > 0) printf(GREEN "\nModification completed successfully.\n" RESET);
    else             printf(RED   "\nNo changes were made.\n" RESET);
}

/* ----------------------------------------------------------
   QUERY  – searches by partial name (case-insensitive)
   ---------------------------------------------------------- */
void queryEmployee(void) {
    if (g_count == 0) { printf(RED "No employees in the system.\n" RESET); return; }

    char keyword[200], keyLower[200], nameLower[300];

    printf(CYAN "Enter name (or part of name) to search: " RESET);
    readLine(keyword, sizeof(keyword));

    if (strlen(keyword) == 0) { printf(RED "Search keyword cannot be empty.\n" RESET); return; }

    toLowerStr(keyLower, keyword, sizeof(keyLower));

    int found = 0;
    printf(GREEN "\nSearch Results:\n" RESET);
    printf("==================================================\n");

    for (int i = 0; i < g_count; i++) {
        toLowerStr(nameLower, g_employees[i].name, sizeof(nameLower));
        if (strstr(nameLower, keyLower)) {
            printEmployee(g_employees[i]);
            found++;
        }
    }

    if (!found)
        printf(RED "No employees found matching \"%s\".\n" RESET, keyword);
    else
        printf(GREEN "Found %d matching employee(s).\n" RESET, found);
}

/* ----------------------------------------------------------
   PRINT  – prints all employees sorted by name
   ---------------------------------------------------------- */
void printEmployees(void) {
    if (g_count == 0) { printf(RED "No employees in the system.\n" RESET); return; }

    /* Sort a temporary copy so the original order is preserved */
    Employee *copy = malloc(g_count * sizeof(Employee));
    if (!copy) { printf(RED "Memory error.\n" RESET); return; }
    memcpy(copy, g_employees, g_count * sizeof(Employee));

    sortByName(copy, g_count);

    printf(GREEN "\nAll Employees (sorted by name):\n" RESET);
    printf("==================================================\n");
    for (int i = 0; i < g_count; i++)
        printEmployee(copy[i]);

    printf(GREEN "Total: %d employee(s).\n" RESET, g_count);
    free(copy);
}

/* ----------------------------------------------------------
   QUIT  – warns user and exits (without saving)
   ---------------------------------------------------------- */
void quitSystem(void) {
    printf(CUSTOMRED
           "\n+--------------------------------------------------+\n"
           "|  WARNING: All unsaved changes will be DISCARDED! |\n"
           "+--------------------------------------------------+\n" RESET);
    printf(CYAN "Are you sure you want to quit? (1 = Yes / 2 = No): " RESET);

    char buf[10];
    readLine(buf, sizeof(buf));

    if (buf[0] == '1') {
        printf(GREEN "Goodbye!\n" RESET);
        exit(0);
    }
    printf(CUSTOMORANGE "Returning to menu...\n" RESET);
}
