//403106068
#include<ncurses.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define FILENAME "users.txt"
#define SCORE_FILE "score.txt"

typedef struct {
    char username[50];
    int score;
    int gold;
    int gamesPlayed;
    int experience;
} Player;

int ValidPassword(const char *password);
int ValidEmail(const char *email);
int UsernameUnique(const char *username);
void saveUser(const char *username, const char *password, const char *email);
int verifyLogin(const char *username, const char *password);
void preGameMenu(const char *username, int isGuest);
void scoreboard(const char *currentUser);
void sortPlayers(Player players[], int count);
int settingsMenu(const char *username);

void createNewUserMenu();
void loginUserMenu();
void guestLogin();
void startNewGame();
void continueGame();

int main(){
    initscr();
    start_color();
    init_pair(1, COLOR_YELLOW, -1);
    init_pair(2, COLOR_CYAN, -1);
    init_pair(3, COLOR_GREEN, -1);
    init_pair(4, COLOR_WHITE, -1);
    init_pair(5, COLOR_RED, -1);
    noecho();
    cbreak();
    int choice;
    do {
        clear();
        mvprintw(1, 1, "---- User Menu ----");
        mvprintw(3, 1, "1. Create New User");
        mvprintw(4, 1, "2. Login");
        mvprintw(5, 1, "3. Guest Login");
        mvprintw(6, 1, "4. Exit");
        mvprintw(8, 1, "Enter your choice: ");
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
        mvprintw(3, 1, "1. Start Game");
        mvprintw(4, 1, "2. View Scoreboard");
        mvprintw(5, 1, "3. Settings");
        mvprintw(7, 1, "Enter your choice: ");
        echo();
        scanw("%d", &choice);
        noecho();
        switch (choice){
        case 1:
            startNewGame(username);
            break;
        case 2:
            scoreboard(username);
            break;
        case 3:
            settingsMenu(username);
            break;
        default:
            mvprintw(7, 1, "Invalid choice. Returning to main menu.");
            refresh();
            getch();
        }
        return;
    }
    mvprintw(3, 1, "1. Start New Game");
    mvprintw(4, 1, "2. Continue Previous Game");
    mvprintw(5, 1, "3. View Scoreboard");
    mvprintw(6, 1, "4. Settings");
    mvprintw(7, 1, "Enter your choice: ");
    echo();
    scanw("%d", &choice);
    noecho();
    switch (choice){
        case 1:
            startNewGame(username);
            break;
        case 2:
            continueGame(username);
            break;
        case 3:
            scoreboard(username);
            break;
        case 4:
            settingsMenu(username);
            break;
        default:
            mvprintw(8, 1, "Invalid choice. Returning to main menu.");
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

void scoreboard(const char *currentUser) {
    FILE *file = fopen(SCORE_FILE, "r");
    Player players[100];
    int count= 0;
    while (fscanf(file, "%s %d %d %d %d", players[count].username, &players[count].score, &players[count].gold, &players[count].gamesPlayed, &players[count].experience) != EOF){
        count++;
        if (count >= 100) break;
    }
    fclose(file);
    sortPlayers(players, count);
    int row = 3;
    mvprintw(1, 1, "---- Scoreboard ----");
    for (int i=0; i < count; i++){
        if (i == 0) attron(COLOR_PAIR(1));
        else if (i == 1) attron(COLOR_PAIR(2));
        else if (i == 2) attron(COLOR_PAIR(3));
        else attron(COLOR_PAIR(4));
        if (strcmp(players[i].username, currentUser) == 0) attron(A_BOLD);
        mvprintw(row++, 1, "%d. %s%s - XP: %d, Score: %d, Gold: %d, Games: %d", i+1, (i<3 ? "🏆" : ""), players[i].username, players[i].experience, players[i].score, players[i].gold, players[i].gamesPlayed);
        attroff(A_BOLD);
        attroff(COLOR_PAIR(1));
        attroff(COLOR_PAIR(2));
        attroff(COLOR_PAIR(3));
        attroff(COLOR_PAIR(4));
    }
    refresh();
    getch();
}

void sortPlayers(Player players[], int count){
    for (int i=0; i < count-1; i++){
        for (int j= i+1; j < count; j++){
            if (players[j].experience > players[i].experience){
                Player temp= players[i];
                players[i]= players[j];
                players[j]= temp;
            }
        }
    }
}

int settingsMenu(const char *username){
    int difficulty, color;
    clear();
    mvprintw(1, 1, "---- Settings Menu ----");
    mvprintw(3, 1, "Select Difficulty:");
    mvprintw(4, 1, "1. Easy");
    mvprintw(5, 1, "2. Medium");
    mvprintw(6, 1, "3. Hard");
    mvprintw(8, 1, "Enter choice: ");
    echo();
    scanw("%d", &difficulty);
    noecho();
    clear();
    mvprintw(1, 1, "---- Settings Menu ----");
    mvprintw(3, 1, "Select Character Color:");
    mvprintw(4, 1, "1. Green");
    mvprintw(5, 1, "2. Blue");
    mvprintw(6, 1, "3. Yellow");
    mvprintw(8, 1, "Enter choice: ");
    echo();
    scanw("%d", &color);
    noecho();
    mvprintw(10, 1, "Settings saved! Returning to menu...");
    refresh();
    getch();
    return difficulty, color;
}