//403106068
#define _GNU_SOURCE

#include<ncurses.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define MUSIC_FILE1 "music1.mp3"
#define MUSIC_FILE2 "music2.mp3"
#define MUSIC_FILE3 "music3.mp3"
#define FILENAME "users.txt"
#define SCORE_FILE "score.txt"
#define MAP_WIDTH 65
#define MAP_HEIGHT 24
#define PASSWORD_LENGTH 8

char map[MAP_HEIGHT][MAP_WIDTH];
int difficulty= 2;
int color= 2;
char *musicFiles[]= {MUSIC_FILE1, MUSIC_FILE2, MUSIC_FILE3};
int selectedMusic= 0;

typedef struct {
    int x, y, width, height;
} Room;

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
void generateRooms(Room rooms[], int *roomCount);
void connectRooms(Room rooms[], int roomCount);
void stair(Room rooms[], int roomCount);
int checkOverlap(Room rooms[], int roomCount, Room newRoom);
void generatePassword(char *password);
void getEmail(const char *username, char *email, size_t size);
void playerInfo(const char *username, Player *player);
void profileMenu(const char *username);

void createNewUserMenu();
void loginUserMenu();
void guestLogin();
void startNewGame();
void continueGame();
void initMap();
void renderMap();
void playMusic(int track);
void settingsMenu();
void applySettings();
void forgotPass();

int main(){
    playMusic(selectedMusic);
    initscr();
    setlocale(LC_ALL, "en_US.UTF-8");
    start_color();
    use_default_colors();
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
    // Mix_FreeMusic(music);
    // Mix_CloseAudio();
    // SDL_Quit();
    return 0;
}

void playMusic(int track) {
    Mix_HaltMusic();
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_Music *music = Mix_LoadMUS(musicFiles[track]);
    if (music) {
        Mix_PlayMusic(music, -1);
        Mix_VolumeMusic(MIX_MAX_VOLUME);
    }
}

void loginUserMenu() {
    char username[50], password[50];
    int choice;
    clear();
    mvprintw(1, 1, "---- User Login ----");
    mvprintw(3, 1, "1. Enter Username and Password");
    mvprintw(4, 1, "2. Forgot Password?");
    mvprintw(5, 1, "Enter choice: ");
    echo();
    scanw("%d", &choice);
    noecho();
    if (choice == 2) {
        forgotPass();
        return;
    }
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
        preGameMenu(username, 0);
    } else{
        mvprintw(6, 1, "Error: Invalid username or password.");
    }
    refresh();
    getch();
}

void forgotPass() {
    char username[50], email[100], fileUsername[50], filePassword[50], fileEmail[100];
    clear();
    mvprintw(1, 1, "---- Forgot Password ----");
    mvprintw(3, 1, "Enter your username: ");
    echo();
    getstr(username);
    noecho();
    mvprintw(4, 1, "Enter your registered email: ");
    echo();
    getstr(email);
    noecho();
    FILE *file= fopen(FILENAME, "r");
    int found= 0;
    while (fscanf(file, "%s %s %s", fileUsername, filePassword, fileEmail) != EOF) {
        if (strcmp(username, fileUsername) == 0 && strcmp(email, fileEmail) == 0) {
            mvprintw(7, 1, "Your password: %s", filePassword);
            found= 1;
            break;
        }
    }
    fclose(file);
    if (!found) {
        mvprintw(7, 1, "Error: Username or email incorrect.");
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
    mvprintw(5, 1, "3. Profile Menu");
    mvprintw(6, 1, "4. View Scoreboard");
    mvprintw(7, 1, "5. Settings");
    mvprintw(8, 1, "Enter your choice: ");
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
            profileMenu(username);
            break;
        case 4:
            scoreboard(username);
            break;
        case 5:
            settingsMenu(username);
            break;
        default:
            mvprintw(9, 1, "Invalid choice. Returning to main menu.");
            refresh();
            getch();
    }
}

void generatePassword(char *password) {
    const char *chars= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int hasUpper= 0, hasLower= 0, hasDigit= 0;
    while (!(hasUpper && hasLower && hasDigit)) {
        hasUpper= hasLower= hasDigit= 0;
        for (int i=0; i < PASSWORD_LENGTH; i++) {
            password[i]= chars[rand() % strlen(chars)];
            if (isupper(password[i])) hasUpper= 1;
            if (islower(password[i])) hasLower= 1;
            if (isdigit(password[i])) hasDigit= 1;
        }
        password[PASSWORD_LENGTH]= '\0';
    }
}

void createNewUserMenu(){
    char username[100], password[100], email[150];
    int choice;
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
    mvprintw(4, 1, "Choose Password:");
    mvprintw(5, 1, "1. Enter manually");
    mvprintw(6, 1, "2. Generate random password");
    mvprintw(8, 1, "Enter choice: ");
    echo();
    scanw("%d", &choice);
    noecho();
    if (choice == 1) {
        mvprintw(9, 1, "Enter password: ");
        echo();
        getstr(password);
        noecho();
        if (!ValidPassword(password)) {
            mvprintw(11, 1, "Error: Password must be at least 7 characters long and include a number, an uppercase letter, and a lowercase letter.");
            refresh();
            getch();
            return;
        }
    } else if(choice == 2){
        srand(time(NULL));
        generatePassword(password);
        mvprintw(9, 1, "Generated password: %s", password);
        refresh();
        getch();
    } else {
        mvprintw(9, 1, "Invalid choice.");
        refresh();
        getch();
        return;
    }
    mvprintw(10, 1, "Enter email: ");
    echo();
    getstr(email);
    noecho();
    if (!ValidEmail(email)) {
        mvprintw(12, 1, "Error: Invalid email format.");
        refresh();
        getch();
        return;
    }
    saveUser(username, password, email);
    mvprintw(12, 1, "User created successfully!");
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
    if(!at || !dot) return 0;
    else if(at == email) return 0;
    else if(dot-at == 1) return 0;
    else if(*(dot+1) == '\0') return 0;
    else return 1;
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
    Room rooms[10];
    int roomCount= 0;
    initMap();
    generateRooms(rooms, &roomCount);
    connectRooms(rooms, roomCount);
    stair(rooms, roomCount);
    renderMap();
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
    sortPlayers(players, count);
    int row = 3;
    mvprintw(1, 1, "---- Scoreboard ----");
    for (int i=0; i < count; i++){
        if (i == 0) attron(COLOR_PAIR(1));
        else if (i == 1) attron(COLOR_PAIR(2));
        else if (i == 2) attron(COLOR_PAIR(3));
        else attron(COLOR_PAIR(4));
        if (strcmp(players[i].username, currentUser) == 0) attron(A_BOLD);
        char displayName[50];
        wchar_t *symbol = L"";
        if (i == 0) { strcpy(displayName, "khafan"); symbol = L"🏆"; attron(A_ITALIC);} 
        else if (i == 1) { strcpy(displayName, "mashti"); symbol = L"🥈"; attron(A_ITALIC); } 
        else if (i == 2) { strcpy(displayName, "bahal"); symbol = L"🥉"; attron(A_ITALIC);} 
        else {strcpy(displayName, players[i].username); attroff(A_ITALIC);}
        char line[100];
        snprintf(line, sizeof(line), "%d. %s - Score: %d, XP: %d, Gold: %d, Games: %d",
                 i+1, displayName, players[i].score,
                 players[i].experience, players[i].gold, players[i].gamesPlayed);
        mvaddstr(row, 1, line);
        mvaddwstr(row++,1+ strlen(line)+1, symbol);
        attroff(A_BOLD);
        attroff(A_ITALIC);
        attroff(COLOR_PAIR(1));
        attroff(COLOR_PAIR(2));
        attroff(COLOR_PAIR(3));
        attroff(COLOR_PAIR(4));
    }
    fclose(file);
    refresh();
    getch();
}

void sortPlayers(Player players[], int count){
    for (int i=0; i < count-1; i++){
        for (int j= i+1; j < count; j++){
            if (players[j].score > players[i].score){
                Player temp= players[i];
                players[i]= players[j];
                players[j]= temp;
            }
        }
    }
}

void settingsMenu() {
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
    clear();
    mvprintw(1, 1, "---- Settings Menu ----");
    mvprintw(3, 1, "Select Background Music:");
    mvprintw(4, 1, "1. Track 1");
    mvprintw(5, 1, "2. Track 2");
    mvprintw(6, 1, "3. Track 3");
    mvprintw(7, 1, "0. Stop Music");
    mvprintw(9, 1, "Enter choice: ");
    echo();
    scanw("%d", &selectedMusic);
    noecho();
    selectedMusic= selectedMusic-1;
    applySettings();
    mvprintw(14, 1, "Settings saved! Returning to menu...");
    refresh();
    getch();
}

void applySettings() {
    start_color();
    if (color == 1) init_pair(6, COLOR_GREEN, -1);
    else if (color == 2) init_pair(7, COLOR_BLUE, -1);
    else if (color == 3) init_pair(8, COLOR_YELLOW, -1);
    attron(COLOR_PAIR(6 + color - 1));
    attroff(COLOR_PAIR(6 + color - 1));
    playMusic(selectedMusic);
}

void initMap(){
    for (int y=0; y < MAP_HEIGHT; y++){
        for (int x=0; x < MAP_WIDTH; x++){
            map[y][x] = ' ';
        }
    }
}

int checkOverlap(Room rooms[], int roomCount, Room newRoom) {
    for (int i=0; i < roomCount; i++) {
        if (!(newRoom.x + newRoom.width < rooms[i].x ||
              newRoom.x > rooms[i].x + rooms[i].width ||
              newRoom.y + newRoom.height < rooms[i].y ||
              newRoom.y > rooms[i].y + rooms[i].height)){
            return 1;
        }
    }
    return 0;
}

void generateRooms(Room rooms[], int *roomCount) {
    *roomCount = 6 + rand() % (10 - 5);
    int count= 0;
    while (count < *roomCount) {
        int w= 4 + rand() % 6;
        int h= 4 + rand() % 6;
        int x= rand() % (MAP_WIDTH - w-1) + 1;
        int y= rand() % (MAP_HEIGHT - h-1) + 1;
        Room newRoom = {x, y, w, h};

        if (!checkOverlap(rooms, count, newRoom)) {
            rooms[count]= newRoom;
            (count)++;            
            for (int j=y; j < y+h; j++) {
                for (int k=x; k < x+w; k++) {
                    if (j == y || j == y+h-1){
                        map[j][k] = '-'; 
                    } else if(k == x || k == x+w-1){
                        map[j][k]= '|';
                    } else{
                        map[j][k] = '.'; 
                    }
                }
            }
            int numDoors = 1 + rand() % 4;
            for (int d=0; d < numDoors; d++){
                int usedSides[4] = {0, 0, 0, 0};
                for (int dr=0; dr < numDoors; dr++) {
                    int doorSide;
                    do {
                        doorSide = rand() % 4;
                    } while (usedSides[doorSide]);
                    usedSides[doorSide] = 1;
                    int doorX, doorY;
                    switch (doorSide) {
                        case 0:
                            doorX = x + rand() % w;
                            doorY = y;
                            break;
                        case 1:
                            doorX = x + rand() % w;
                            doorY = y + h - 1;
                            break;
                        case 2:
                            doorX = x;
                            doorY = y + rand() % h;
                            break;
                        case 3:
                            doorX = x + w - 1;
                            doorY = y + rand() % h;
                            break;
                    }
                    // if (doorX > 0 && doorX < MAP_WIDTH - 1 && doorY > 0 && doorY < MAP_HEIGHT - 1){
                    //     map[doorY][doorX] = '+';
                    // }
                    if (doorX > 2 && doorX < MAP_WIDTH - 3 && doorY > 2 && doorY < MAP_HEIGHT - 3) {
                        map[doorY][doorX] = '+';
                        int hallX = doorX, hallY = doorY;
                        switch (doorSide) {
                            case 0: hallY--; break;
                            case 1: hallY++; break;
                            case 2: hallX--; break;
                            case 3: hallX++; break;
                        }
                        if (hallX > 1 && hallX < MAP_WIDTH - 2 && hallY > 1 && hallY < MAP_HEIGHT - 2 && map[hallY][hallX] == ' ') {
                            map[hallY][hallX] = '#';
                        }
                    }
                }
            }
        }
    }
}

void connectRooms(Room rooms[], int roomCount) {
    for (int i=0; i < roomCount-1; i++) {
        int doorX1= -1, doorY1= -1;
        int doorX2= -1, doorY2= -1;
        Room r1= rooms[i];
        for (int y= r1.y; y < r1.y + r1.height; y++) {
            for (int x= r1.x; x < r1.x + r1.width; x++) {
                if (map[y][x] == '+') {
                    doorX1= x;
                    doorY1= y;
                    break;
                }
            }
            if (doorX1 != -1) break;
        }
        Room r2= rooms[i + 1];
        for (int y= r2.y; y < r2.y + r2.height; y++) {
            for (int x= r2.x; x < r2.x + r2.width; x++) {
                if (map[y][x] == '+') {
                    doorX2= x;
                    doorY2= y;
                    break;
                }
            }
            if (doorX2 != -1) break;
        }
        int pivotX = doorX1 + (rand() % abs(doorX2 - doorX1+1)) * ((doorX2 > doorX1) ? 1 : -1);
        int pivotY = doorY1 + (rand() % abs(doorY2 - doorY1+1)) * ((doorY2 > doorY1) ? 1 : -1);
        int curX= doorX1, curY= doorY1;
        while (curX != pivotX) {
            if (map[curY][curX] == ' ') {
                map[curY][curX] = '#';
            }
            curX+= (pivotX > curX) ? 1 : -1;
        }
        while (curY != pivotY) {
            if (map[curY][curX] == ' ') {
                map[curY][curX] = '#';
            }
            curY+= (pivotY > curY) ? 1 : -1;
        }
        while (pivotX != doorX2) {
            if (map[curY][curX] == ' '){
                map[curY][curX] = '#';
            }
            pivotX+= (doorX2 > pivotX) ? 1 : -1;
        }
        while (pivotY != doorY2) {
            if (map[curY][curX] == ' '){
                map[curY][curX] = '#';
            }
            pivotY+= (doorY2 > pivotY) ? 1 : -1;
        }
    }
}

void stair(Room rooms[], int roomCount) {
    int index = rand() % roomCount;
    int x = rooms[index].x + rooms[index].width / 2;
    int y = rooms[index].y + rooms[index].height / 2;
    map[y][x] = '<';
}

void renderMap() {
    clear();
    for (int y=0; y < MAP_HEIGHT; y++) {
        for (int x=0; x < MAP_WIDTH; x++) {
            mvaddch(y, x, map[y][x]);
        }
    }
    refresh();
}

void profileMenu(const char *username) {
    clear();  
    Player player;
    char email[100];
    playerInfo(username, &player);
    getEmail(username, email, sizeof(email));

    mvprintw(1, 1, " ----User Profile---- ");

    mvprintw(3, 1, "Name: %s", username);
    mvprintw(4, 1, "Email: %s", email);
    mvprintw(5, 1, "Score: %d", player.score);
    mvprintw(6, 1, "Gold: %d", player.gold);
    mvprintw(7, 1, "XP: %d", player.experience);
    mvprintw(8, 1, "Games played: %d", player.gamesPlayed);

    mvprintw(10, 1, "Press any key to exit...");

    refresh();
    getch();
}

void getEmail(const char *username, char *email, size_t size) {
    FILE *file = fopen(FILENAME, "r");
    char fileUsername[50], filePassword[50], fileEmail[100];
    while (fscanf(file, "%s %s %s", fileUsername, filePassword, fileEmail) != EOF) {
        if (strcmp(username, fileUsername) == 0) {
            strncpy(email, fileEmail, size-1);
            email[size-1] = '\0';
            fclose(file);
            return ;
        }
    }
    fclose(file);
}

void playerInfo(const char *username, Player *player) {
    FILE *file = fopen(SCORE_FILE, "r");
    char fileUsername[50];
    int fileScore, fileGold, fileGames, fileExperience ;
    while (fscanf(file, "%s %d %d %d %d", fileUsername, &fileScore, &fileGold, &fileGames, &fileExperience) != EOF) {
        if (strcmp(username, fileUsername) == 0) {
            player->score= fileScore;
            player->gold= fileGold;
            player->gamesPlayed= fileGames;
            player->experience= fileExperience;
            fclose(file);
            return;
        }
    }
}