#include <stdio.h>
#include <stdlib.h>
#include "Functions.h"

int main() {

    showWelcomeBox();

    /* LOAD is called by default at startup */
    loadEmployees();

    /* Main menu loop */
    while (1) {
        showMenu();
        featuresControl(selectOption());
    }

    return 0;
}
