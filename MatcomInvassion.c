#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct {
    int health;
    int x;
    int y;
    int width;
    int height;
} SpaceShip;

typedef struct {
    int startX;
    int startY;
    bool active;
} Shot;

#define MAX_SHOTS 100

SpaceShip myShip = {100, 10, 10, 5, 3};
Shot shots[MAX_SHOTS];
int width = 99;
int height = 30;
int score = 0;
pthread_mutex_t lock;
pthread_t shotThread;

void cleanup() {
    // Finalizar ncurses
    endwin();

    // Destruir mutex
    pthread_mutex_destroy(&lock);

    // Limpiar la consola
    system("clear");

    // Cancelar hilos si es necesario
    pthread_cancel(shotThread);
}

void drawFrame() {
    for (int x = 0; x < width; x++) {
        mvprintw(0, x, "-");
        mvprintw(height - 1, x, "-");
    }
    for (int y = 0; y < height; y++) {
        mvprintw(y, 0, "|");
        mvprintw(y, width - 1, "|");
    }
    mvprintw(0, 0, "+");
    mvprintw(0, width - 1, "+");
    mvprintw(height - 1, 0, "+");
    mvprintw(height - 1, width - 1, "+");
}

void drawShip(const SpaceShip *ship, bool erase) {
    char shipDesign[][6] = {
        "  ^  ",
        " /|\\ ",
        "( |-)",
        " \\|/ "
    };
    char *blankDesign[] = {
        "     ",
        "     ",
        "     "
    };
    for (int i = 0; i < ship->height; i++) {
        if (erase) {
            mvprintw(ship->y + i, ship->x, "%s", blankDesign[i]);
        } else {
            attron(COLOR_PAIR(1));
            mvprintw(ship->y + i, ship->x, "%s", shipDesign[i]);
            attroff(COLOR_PAIR(1));
        }
    }
}

void updateScore() {
    mvprintw(0, 2, "Score: %d", score);
}

void *shoot(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock);
        for (int i = 0; i < MAX_SHOTS; i++) {
            if (shots[i].active) {
                int shotY = shots[i].startY - 1;
                if (shotY > 0) {
                    mvprintw(shotY, shots[i].startX, "|");
                    refresh();
                    usleep(10000); // Reduce the delay to increase shot speed
                    mvprintw(shotY, shots[i].startX, " ");
                    shots[i].startY--;
                } else {
                    shots[i].active = false;
                }
            }
        }
        pthread_mutex_unlock(&lock);
        usleep(10000); // Reduce the delay to increase shot speed
    }
    return NULL;
}

void *moveAndShoot(void *arg) {
    int ch;
    while ((ch = getch()) != 'q') {
        drawShip(&myShip, true);
        switch (ch) {
            case 'w':
                if (myShip.y > 1) myShip.y--;
                break;
            case 's':
                if (myShip.y < height - myShip.height - 1) myShip.y++;
                break;
            case 'a':
                if (myShip.x > 1) myShip.x--;
                break;
            case 'd':
                if (myShip.x < width - myShip.width - 1) myShip.x++;
                break;
            case ' ':
                pthread_mutex_lock(&lock);
                for (int i = 0; i < MAX_SHOTS; i++) {
                    if (!shots[i].active) {
                        shots[i].startX = myShip.x + 2;
                        shots[i].startY = myShip.y;
                        shots[i].active = true;
                        break;
                    }
                }
                score++;
                updateScore();
                pthread_mutex_unlock(&lock);
                break;
        }
        drawShip(&myShip, false);
        refresh();
        usleep(20000); // Reduce the delay to improve movement speed
    }
    return NULL;
}

void StartGame() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color(); // Inicializar colores
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); // Definir par de colores

    halfdelay(1); // Usar halfdelay para mejorar la respuesta del teclado

    drawFrame();
    updateScore();
    refresh();

    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < MAX_SHOTS; i++) {
        shots[i].active = false;
    }

    pthread_create(&shotThread, NULL, shoot, NULL);

    pthread_t moveAndShootThread;
    pthread_create(&moveAndShootThread, NULL, moveAndShoot, NULL);

    pthread_join(moveAndShootThread, NULL);

    cleanup();
}

int main() {
    printf(" __  __       _        _____           _       _   _                 \n");
    printf("|  \\/  |     (_)      |_   _|         (_)     | | (_)                \n");
    printf("| \\  / | __ _ _ _ __    | |  _ ____   ___  ___| |_ _  ___  _ __  ___ \n");
    printf("| |\\/| |/ _` | | '_ \\   | | | '_ \\ \\ / / |/ __| __| |/ _ \\| '_ \\/ __|\n");
    printf("| |  | | (_| | | | | | _| |_| | | \\ V /| | (__| |_| | (_) | | | \\__ \\\n");
    printf("|_|  |_|\\__,_|_|_| |_||_____|_| |_|\\_/ |_|\\___|\\__|_|\\___/|_| |_|___/\n");
    printf("\n");

    int choice;
    int validInput = 0;
    do {
        printf("Main Menu\n");
        printf("1. Start Game\n");
        printf("2. Exit\n");
        printf("Enter your choice: ");
        validInput = scanf("%d", &choice);

        if (validInput != 1 || (choice != 1 && choice != 2)) {
            printf("Invalid choice. Please try again.\n");
            while (getchar() != '\n'); // Limpiar el buffer de entrada
            validInput = 0;
        } else {
            switch (choice) {
                case 1:
                    StartGame();
                    break;
                case 2:
                    printf("Exiting...\n");
                    cleanup();
                    break;
            }
        }
    } while (choice != 2);

    return 0;
}