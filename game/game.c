//403106068
#include<ncurses.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define FILENAME "users.txt"

int ValidPassword(const char *password);
int ValidEmail(const char *email);
int UsernameUnique(const char *username);
void saveUser(const char *username, const char *password, const char *email);
void createNewUserMenu();

int main(){
    initscr();
    noecho();
    cbreak();
    createNewUserMenu();

    endwin();
    return 0;
}

void createNewUserMenu(){
    char username[100], password[100], email[150];
    mvprintw(1, 1, "---- Create New User ----");
    mvprintw(3, 1, "Enter username: ");
    echo();
    getstr(username);
    noecho();
    if (!UsernameUnique(username)) {
        mvprintw(5, 1, "Error: Username already exists.");
        refresh();
        getch();
        return;
    }

    mvprintw(4, 1, "Enter password: ");
    echo();
    getstr(password);
    noecho();
    if (!ValidPassword(password)) {
        mvprintw(6, 1, "Error: Password must be at least 7 characters long and include a number, an uppercase letter, and a lowercase letter.");
        refresh();
        getch();
        return;
    }

    mvprintw(5, 1, "Enter email: ");
    echo();
    getstr(email);
    noecho();
    if (!ValidEmail(email)) {
        mvprintw(7, 1, "Error: Invalid email format.");
        refresh();
        getch();
        return;
    }

    saveUser(username, password, email);
    mvprintw(7, 1, "User created successfully!");
    refresh();
    getch();
}

int ValidPassword(const char *password) {
    int hasUpper= 0, hasLower= 0, hasDigit= 0;
    if (strlen(password) < 7) return 0;

    for (int i= 0; password[i] != '\0'; i++) {
        if (isupper(password[i])) hasUpper= 1;
        if (islower(password[i])) hasLower= 1;
        if (isdigit(password[i])) hasDigit= 1;
    }
    return hasUpper && hasLower && hasDigit;
}

int ValidEmail(const char *email) {
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');
    return at && dot && at < dot && dot - at > 1 && strlen(dot) > 1;
}

int UsernameUnique(const char *username) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return 1;

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char existingUsername[50];
        sscanf(line, "%s", existingUsername);
        if (strcmp(existingUsername, username) == 0) {
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 1;
}

void saveUser(const char *username, const char *password, const char *email) {
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        mvprintw(8, 1, "Error: Could not open file for writing.");
        refresh();
        getch();
        exit(1);
    }

    fprintf(file, "%s %s %s\n", username, password, email);
    fclose(file);
}