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
int verifyLogin(const char *username, const char *password);
void preGameMenu(const char *username, int isGuest);

void createNewUserMenu();
void loginUserMenu();
void guestLogin();
void startNewGame();
void continueGame();

int main(){
    initscr();
    noecho();
    cbreak();
    int choice;

    do {
        clear();
        mvprintw(1, 1, "---- User Menu ----");
        mvprintw(3, 1, "1. Create New User");
        mvprintw(4, 1, "2. Login");
        mvprintw(4, 1, "3. Guest Login");
        mvprintw(5, 1, "4. Exit");
        mvprintw(7, 1, "Enter your choice: ");
        echo();
        scanw("%d", &choice);
        noecho();
        switch (choice) {
            case 1:
                createNewUserMenu();
                break;
            case 2:
                loginUserMenu();
                break;
            case 3:
                guestLogin();
                break;
            case 4:
                mvprintw(9, 1, "Exiting...");
                refresh();
                getch();
                break;
            default:
                mvprintw(9, 1, "Invalid choice. Try again.");
                refresh();
                getch();
        }
    } while(choice != 4);

    endwin();
    return 0;
}

void loginUserMenu() {
    char username[50], password[50];
    clear();
    mvprintw(1, 1, "---- User Login ----");
    mvprintw(3, 1, "Enter username: ");
    echo();
    getstr(username);
    noecho();
    mvprintw(4, 1, "Enter password: ");
    echo();
    getstr(password);
    noecho();
    if (verifyLogin(username, password)){
        mvprintw(6, 1, "Login successful! Welcome, %s.", username);
    } else{
        mvprintw(6, 1, "Error: Invalid username or password.");
    }
    refresh();
    getch();
}

void preGameMenu(const char *username, int isGuest) {
    int choice;
    clear();
    mvprintw(1, 1, "---- PreGame Menu ----");
    if (isGuest) {
        mvprintw(3, 1, "Guest mode: Starting a new game...");
        refresh();
        getch();
        startNewGame(username);
        return;
    }
    mvprintw(3, 1, "1. Start New Game");
    mvprintw(4, 1, "2. Continue Previous Game");
    mvprintw(5, 1, "Enter your choice: ");
    echo();
    scanw("%d", &choice);
    noecho();
    if(choice == 1){
        startNewGame(username);
    } else if (choice == 2){
        continueGame(username);
    } else {
        mvprintw(6, 1, "Invalid choice. Returning to main menu.");
        refresh();
        getch();
    }
}

void createNewUserMenu(){
    char username[100], password[100], email[150];
    clear();
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
    fprintf(file, "%s %s %s\n", username, password, email);
    fclose(file);
}

int verifyLogin(const char *username, const char *password) {
    FILE *file = fopen(FILENAME, "r");
    char line[200], fileUsername[50], filePassword[50];
    while (fgets(line, sizeof(line), file)){
        sscanf(line, "%s %s", fileUsername, filePassword);
        if (strcmp(fileUsername, username) == 0 && strcmp(filePassword, password) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void guestLogin(){
    clear();
    mvprintw(1, 1, "---- Guest Login ----");
    mvprintw(3, 1, "You are logged in as a guest.");
    mvprintw(5, 1, "Note: Your game will not be saved.");
    refresh();
    getch();
    preGameMenu("Guest", 1);
}

void startNewGame() {
    clear();
    mvprintw(1, 1, "Starting a new game...");
    refresh();
    getch();
}

void continueGame() {
    clear();
    mvprintw(1, 1, "Continuing previous game...");
    refresh();
    getch();
}
