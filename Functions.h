#ifndef COMPANY_FUNCTIONS_H
#define COMPANY_FUNCTIONS_H

#include <stdio.h>

/* ================================================
   ANSI Color Codes – for colorful console output
   ================================================ */
#define CUSTOMORANGE "\x1b[38;2;247;154;25m"
#define CUSTOMYELLOW "\x1b[38;2;255;229;42m"
#define CUSTOMRED    "\x1b[38;2;247;15;15m"
#define RED          "\x1b[31m"
#define GREEN        "\x1b[32m"
#define YELLOW       "\x1b[33m"
#define CYAN         "\x1b[36m"
#define RESET        "\x1b[0m"

/* ===========
   Structs
   =========== */

/* Date struct – holds day, month, and year */
typedef struct {
    int day;
    int month;
    int year;
} Date;

/* Employee struct */
typedef struct {
    int    id;
    char  *name;
    float  salary;
    Date   birthDate;
    char  *address;
    char  *mobile;
    Date   enrollmentDate;   /* set automatically to today on ADD */
    char  *email;
} Employee;

/* ==============================================
   Global Variables  (defined in Functions.c)
   ============================================== */
extern Employee *g_employees;
extern int       g_count;
extern char      g_filename[256];

/* =================
   UI Prototypes
   ================= */
void showWelcomeBox(void);
void showMenu(void);
int  selectOption(void);
void featuresControl(int option);

/* ===========================
   Core Feature Prototypes
   =========================== */
void loadEmployees(void);   /* 1. LOAD  – reads CSV file          */
void addEmployee(void);     /* 2. ADD   – prompt field by field    */
void deleteEmployee(void);  /* 3. DELETE – by ID                   */
void modifyEmployee(void);  /* 4. MODIFY – name/salary/mobile/...  */
void queryEmployee(void);   /* 5. QUERY – partial name search      */
void printEmployees(void);  /* 6. PRINT – sorted by name           */
void saveEmployees(void);   /* 7. SAVE  – write back to same file  */
void quitSystem(void);      /* 8. QUIT  – with unsaved-data warning*/

/* ==============================
   Helper Function Prototypes
   ============================== */
void sortByName(Employee *arr, int n);
void printEmployee(Employee e);
void toLowerStr(char *dst, const char *src, int max_len);
int  readInt(int *out);
void readLine(char *buf, int size);
int  updateString(char **field, const char *newVal);
int  isValidMobile(const char *m);
int  isValidEmail(const char *e);
int  isUniqueId(int id);
int  isValidDate(int d, int m, int y);
Date getCurrentDate(void);
void trimLeft(char *s);
void trimRight(char *s);
void trim(char *s);

#endif /* COMPANY_FUNCTIONS_H */
