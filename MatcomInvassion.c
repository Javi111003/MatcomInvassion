#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_SHOTS 100
#define MAX_ENEMIES 50
#define MAX_WAVES 20

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
typedef struct {
    int x;
    int y;
    bool active;
} Enemy;
typedef struct {
    Enemy enemies[MAX_ENEMIES];
    int numEnemies;
} Wave;
// Define the new structures
typedef struct WaveNode {
    Wave wave;
    struct WaveNode *next;
} WaveNode;
typedef struct {
    WaveNode *head;
    WaveNode *tail;
} WaveList;

//Variables globales
WaveList wavesList;
SpaceShip myShip = {100, 10, 10, 5, 3};
Shot shots[MAX_SHOTS];
Enemy enemies[MAX_ENEMIES];
int width = 120;
int height = 30;
int score = 0;
int waveNumber = 1;
pthread_mutex_t lock;
pthread_t shootThread, inputThread , enemyThread ,waveTrhead;

//Firmas de metodos
void initializeWaves();
void initializeShots();

void cleanup() {
    //Eliminar enemigos y disparos de la pantalla
    initializeShots();
    //Reiniciar oleada
    waveNumber=1;
    //Reiniciar puntuuacion
    score=0;
    // Finalizar ncurses
    endwin();
    // Destruir mutex
    pthread_mutex_destroy(&lock);
    // Limpiar la consola
    system("clear");
    // Cancelar hilos si es necesario
    pthread_cancel(shootThread);
    pthread_cancel(inputThread);
    pthread_cancel(enemyThread);
    pthread_cancel(waveTrhead);
    // Liberar la lista enlazada
    WaveNode *current = wavesList.head;
    while (current != NULL) {
        WaveNode *temp = current;
        current = current->next;
        free(temp);
    }
    initializeWaves();
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
    pthread_mutex_lock(&lock);
    for (int i = 0; i < ship->height; i++) {
        if (erase) {
            mvprintw(ship->y + i, ship->x, "%s", blankDesign[i]);
        } else {
            attron(COLOR_PAIR(1));
            mvprintw(ship->y + i, ship->x, "%s", shipDesign[i]);
            attroff(COLOR_PAIR(1));
        }
    }
    refresh();
    pthread_mutex_unlock(&lock);
}
void updateUI() {
    pthread_mutex_lock(&lock);
    mvprintw(0, 2, "Score: %d", score);
    mvprintw(0, 20, "Wave: %d", waveNumber);
    mvprintw(0, 40, "Health: %d", myShip.health);
    refresh();
    pthread_mutex_unlock(&lock);
}
void initializeShots() {
    for (int i = 0; i < MAX_SHOTS; i++) {
        shots[i].active = false;
    }
}
// Initialize the linked list
void initializeWaves() {
    wavesList.head = NULL;
    wavesList.tail = NULL;
}
void *generateWave(void* arg) {
    while(1){
        pthread_mutex_lock(&lock);
        WaveNode *newNode = (WaveNode *)malloc(sizeof(WaveNode));
        if (newNode != NULL) {
            int enemiesInWave = (waveNumber * rand()) % 10;
            newNode->wave.numEnemies = enemiesInWave;
            for (int i = 0; i < enemiesInWave; i++) {
            //Mejorar la generacion de la coordenada x de los enemigos para que no se generen en la misma posicion
                int x = rand() % width;
                newNode->wave.enemies[i].x = x==0?1:x;
                newNode->wave.enemies[i].y = 1;
                newNode->wave.enemies[i].active = true;
            }
            newNode->next = NULL;
            if (wavesList.tail != NULL) {
                wavesList.tail->next = newNode;
            } else {
                wavesList.head = newNode;
            }
            wavesList.tail = newNode;
            waveNumber++;
        }
        pthread_mutex_unlock(&lock);
        usleep(1000000); // Reduce the delay to make waves appear more frequently
    }
    return NULL;
}
void *moveWave(void* arg) {
    while(1){
        pthread_mutex_lock(&lock);
        WaveNode *current = wavesList.head;
        while (current != NULL) {
            Wave *currentWave = &current->wave;
            for (int j = 0; j < currentWave->numEnemies; j++) {
                if (currentWave->enemies[j].active) {
                    mvprintw(currentWave->enemies[j].y, currentWave->enemies[j].x, " ");
                    currentWave->enemies[j].y++;
                    if (currentWave->enemies[j].y >= height) {
                        currentWave->enemies[j].active = false;
                    } else {
                        mvprintw(currentWave->enemies[j].y, currentWave->enemies[j].x, "E");
                    }
                }
            }
            current = current->next;
        }
        refresh();
        pthread_mutex_unlock(&lock);
        usleep(350000);
    }
    return NULL;
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
                    usleep(30000); // Reduce the delay to increase shot speed
                    mvaddch(shotY, shots[i].startX, ' ');
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
        updateUI();
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
                for (int i = 0; i < MAX_SHOTS; i++) {
                    if (!shots[i].active) {
                        shots[i].startX = myShip.x + 2;
                        shots[i].startY = myShip.y;
                        shots[i].active = true;
                        break;
                    }
                }
                score++;
                break;
        }
        drawShip(&myShip, false);
        usleep(20000); // Reduce the delay to improve movement speed
    }
    cleanup();
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
    curs_set(0);
    raw();
    box(stdscr, 0, 0);
    halfdelay(1); // Usar halfdelay para mejorar la respuesta del teclado
    updateUI();
    srand(time(NULL));
    refresh();

    pthread_mutex_init(&lock, NULL);

    initializeWaves();
    initializeShots();

    pthread_create(&shootThread, NULL, shoot, NULL);
    pthread_create(&inputThread, NULL, moveAndShoot, NULL);
    pthread_create(&enemyThread, NULL, moveWave, NULL);
    pthread_create(&waveTrhead, NULL, generateWave, NULL);
    pthread_join(inputThread, NULL);

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