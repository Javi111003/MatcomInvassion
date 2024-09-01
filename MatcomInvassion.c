#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_SHOTS 100
#define MAX_ENEMIES 50
#define ALIEN_WIDTH 6

typedef struct {
    int health;
    int x,y;
    int width , height;
} SpaceShip;
typedef struct {
    int startX , startY;
    bool active;
} Shot;

typedef struct {
    int type;
    int x, y;
    int width, height;
    char** sprite;
    int active;
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

// Tipo 1
char* alien1_sprite[] = {
    " /\\ ",
    "/  \\",
    "\\  /",
    " \\/ "
};
const int alien1_width = 4, alien1_height = 4;

// Tipo 2
char* alien2_sprite[] = {
    " /\\ ",
    "/||\\",
    "\\  /",
    " \\/ "
};
const int alien2_width = 4, alien2_height = 4;

// Tipo 3
char* alien3_sprite[] = {
    " /\\/\\ ",
    "/|  |\\",
    "\\|  |/",
    " \\/\\/ "
};
const int alien3_width = 6, alien3_height = 4;

//Variables globales
WaveList wavesList;
SpaceShip myShip = {100, 10, 10, 9, 8}; // Actualizar el ancho y alto de la nave
Shot shots[MAX_SHOTS];
Enemy enemies[MAX_ENEMIES];
int width = 120;
int height = 30;
int score = 0;
int waveNumber = 1;
pthread_mutex_t lock;
pthread_t shootThread, inputThread , enemyThread ,waveThread ,collisionThread;

//Firmas de metodos
void initializeWaves();
void initializeShots();

void cleanup() {
    //Eliminar enemigos y disparos de la pantalla
    initializeShots();
    //Reiniciar oleada
    waveNumber=1;
    //Reiniciar puntuacion y vida del jugador
    score=0;
    myShip.health=100;
    // Destruir mutex
    pthread_mutex_destroy(&lock);
    // Limpiar la consola
    clear();
    refresh();
    // Cancelar hilos si es necesario
    pthread_cancel(shootThread);
    pthread_cancel(inputThread);
    pthread_cancel(enemyThread);
    pthread_cancel(waveThread);
    pthread_cancel(collisionThread);
    // Liberar la lista enlazada
    WaveNode *current = wavesList.head;
    while (current != NULL) {
        WaveNode *temp = current;
        current = current->next;
        free(temp);
    }
    wavesList.head = NULL;
    wavesList.tail = NULL;
    // Finalizar ncurses
    endwin();
}
void drawShip(const SpaceShip *ship, bool erase) {
    char shipDesign[][20] = {
        "     _    ",
        "    /_\\   ",
        "   |( )|  ",
        "   | _ |  ",
        "   ||-||  ",
        "  / |_| \\ ",
        " (MAT_COM)",
        "  (') (') "
    };
    char *blankDesign[] = {
        "          ",
        "          ",
        "          ",
        "          ",
        "          ",
        "          ",
        "          ",
        "          "
    };
    pthread_mutex_lock(&lock);
    for (int i = 0; i < ship->height; i++) {
        if (erase) {
            mvprintw(ship->y + i, ship->x, "%s", blankDesign[i]);
        } else {
            switch(i){
            case 7 :
                   attron(COLOR_PAIR(3));
                   break;
            default:
                   attron(COLOR_PAIR(1));
                   break;
            }
            mvprintw(ship->y + i, ship->x, "%s", shipDesign[i]);
            attroff(COLOR_PAIR(1));
            attroff(COLOR_PAIR(2));
            attroff(COLOR_PAIR(3));
        }
    }
    refresh();
    pthread_mutex_unlock(&lock);
}

void drawAlien(Enemy* enemy) {
    attron(COLOR_PAIR(2));
    for (int i = 0; i < enemy->height; i++)
        mvprintw(enemy->y + i, enemy->x, "%s", enemy->sprite[i]);

    attroff(COLOR_PAIR(2));
    refresh();
}

void eraseAlien(Enemy* enemy) {
    int i, j;
    for (i = 0; i < enemy->height; i++) {
        for (j = 0; j < enemy->width; j++) {
            mvaddch(enemy->y + i, enemy->x + j, ' ');
        }
    }
    refresh();
}

void animateAlienHit(Enemy* enemy) {
    // Ejemplo de animación simple: parpadeo del alien
    for (int i = 0; i < 3; i++) {
        eraseAlien(enemy);
        usleep(7000); // Esperar 0.7s
        drawAlien(enemy);
        usleep(7000); // Esperar 0.7s
    }
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
            int enemiesInWave = (waveNumber * rand()) % ((score/150)+1)+4;
            int nextFit = 0 ;
            int x = nextFit == 0? rand()%width : nextFit;
            newNode->wave.numEnemies = enemiesInWave;
            for (int i = 0; i < enemiesInWave; i++) {
                // Mejorar la generacion de la coordenada x de los enemigos para que no se generen en la misma posicion
                newNode->wave.enemies[i].type = i % 3 + 1;
                newNode->wave.enemies[i].active = newNode->wave.enemies[i].type;
                switch (newNode->wave.enemies[i].type) {
                    case 1:
                        newNode->wave.enemies[i].width = alien1_width;
                    newNode->wave.enemies[i].height = alien1_height;
                    newNode->wave.enemies[i].sprite = alien1_sprite;
                    break;
                    case 2:
                        newNode->wave.enemies[i].width = alien2_width;
                    newNode->wave.enemies[i].height = alien2_height;
                    newNode->wave.enemies[i].sprite = alien2_sprite;
                    break;
                    case 3:
                        newNode->wave.enemies[i].width = alien3_width;
                    newNode->wave.enemies[i].height = alien3_height;
                    newNode->wave.enemies[i].sprite = alien3_sprite;
                    break;
                }
                if (i == 0) {
                    newNode->wave.enemies[i].x = x-newNode->wave.enemies[i].width<=0||x+newNode->wave.enemies[i].width>=width-4?1+newNode->wave.enemies[i].width:x;
                }
                else {
                    int lastWidth = newNode->wave.enemies[i-1].width;
                    newNode->wave.enemies[i].x = newNode->wave.enemies[i-1].x + lastWidth + (newNode->wave.enemies[i].width/2);
                    if (newNode->wave.enemies[i].x + newNode->wave.enemies[i].width >= width-4) {//-4 por el espacio del marco
                        newNode->wave.enemies[i].x = 2 + newNode->wave.enemies[i].width;//al igual que aquí el +2
                    }
                }
                newNode->wave.enemies[i].y = 1;
            }
            nextFit = enemiesInWave==0 ? rand() % width : newNode->wave.enemies[enemiesInWave-1].x + newNode->wave.enemies[enemiesInWave-1].width;
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
        usleep(2500000-((score*1000)+1)); // Reduce the delay to make waves appear more frequently
    }
    return NULL;
}
void *moveWave(void* arg) {
    while(1){
        pthread_mutex_lock(&lock);
        WaveNode *current = wavesList.head;
        WaveNode *prev = NULL;
        while (current != NULL) {
            Wave *currentWave = &current->wave;
            bool allInactive = true ;
            for (int j = 0; j < currentWave->numEnemies; j++) {
                if (currentWave->enemies[j].active) {
                    allInactive = false;
                    eraseAlien(&currentWave->enemies[j]);
                    currentWave->enemies[j].y++;
                    if (currentWave->enemies[j].y >= height) {
                        currentWave->enemies[j].active = false;
                    } else {
                        drawAlien(&currentWave->enemies[j]);
                    }
                }
            }
            if (allInactive) {
                if (prev == NULL) {
                    wavesList.head = current->next;
                } else {
                    prev->next = current->next;
                }
                if (current == wavesList.tail) {
                    wavesList.tail = prev;
                }
                WaveNode *temp = current;
                current = current->next;
                free(temp);
            } else {
                prev = current;
                current = current->next;
            }
        }
        refresh();
        pthread_mutex_unlock(&lock);
        usleep(350000-((score*100)+1));
    }
    return NULL;
}

void *checkCollisions(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);
        WaveNode *current = wavesList.head;
        while (current != NULL) {
            Wave *currentWave = &current->wave;
            for (int j = 0; j < currentWave->numEnemies; j++) {
                if (currentWave->enemies[j].active) {
                    // Detectar colisiones con disparos
                    for (int k = 0; k < MAX_SHOTS; k++) {
                        if (shots[k].active && shots[k].startX >= currentWave->enemies[j].x && shots[k].startX < currentWave->enemies[j].x + currentWave->enemies[j].width && shots[k].startY >= currentWave->enemies[j].y && shots[k].startY < currentWave->enemies[j].y + currentWave->enemies[j].height) {
                            shots[k].active = false;
                            currentWave->enemies[j].active -= 1;
                            animateAlienHit(&currentWave->enemies[j]);
                            if(!currentWave->enemies[j].active){
                            eraseAlien(&currentWave->enemies[j]);
                            score += currentWave->enemies[j].type*10; // Incrementar la puntuación
                            }
                        }
                    }

                    // Detectar colisiones con la nave
                    if (currentWave->enemies[j].x < myShip.x + myShip.width && currentWave->enemies[j].x + currentWave->enemies[j].width > myShip.x && currentWave->enemies[j].y < myShip.y + myShip.height && currentWave->enemies[j].y + currentWave->enemies[j].height > myShip.y) {
                        myShip.health -= currentWave->enemies[j].type*10; // Disminuir la vida de la nave
                        currentWave->enemies[j].active = false;
                        eraseAlien(&currentWave->enemies[j]);
                    }
                }
            }
            current = current->next;
        }
        pthread_mutex_unlock(&lock);
        usleep(10000); // Ajustar el tiempo de espera según sea necesario
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
    while ((ch = getch()) != 'q' && myShip.health>0) {
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
                        shots[i].startX = myShip.x + 5;
                        shots[i].startY = myShip.y;
                        shots[i].active = true;
                        break;
                    }
                }
                break;
        }
        drawShip(&myShip, false);
        usleep(20000); // Reduce the delay to improve movement speed
    }
    system("clear");
    printf("Game Over...You're dead dude (T_T) \n");
    sleep(2);
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
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // Space Ship Body Color
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Alien Body Color
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Fire Color
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
    pthread_create(&waveThread, NULL, generateWave, NULL);
    pthread_create(&collisionThread, NULL, checkCollisions, NULL);
    pthread_join(inputThread, NULL);

    cleanup();
}

void printObservatory(){
    printf("     *   .         '       .   *    *   .         '       .   *   \n");
    printf("        .         '       .   *         .         '       .   *     \n");
    printf("        *   .         '       .   *    *   .         '       .   *\n");
    printf("              `  .    '             . *   .    +    '    \n");
    printf("   *   .         '       .   *    *   .         '       .   *   \n");
    printf("    *   .         '       .   *    *   .         '       .   *   \n");
    printf("  .    _     *       \\|/   .       .      -*-              +    \n");
    printf("    .' \\`.     +    -*-     *   .         '       .   *         \n");
    printf(" .  |__''_|  .       /|\\ +         .    +       .           |   \n");
    printf("    |     | .                                        .     -*-  \n");
    printf("    |     |           `  .    '             . *   .    +    '   \n");
    printf("  _.'-----'-._     *                  .                        \n");
    printf("/          apc\\__.__.--._______________--______________---_----_ \n");
}
void printInstructions() {
    printf("Instructions\n");
    printf("1. Use 'w', 'a', 's', 'd' to move the spaceship\n");
    printf("2. Press 'space' to shoot\n");
    printf("3. Press 'q' to quit the game\n");
    printf("\n");
}

void printMainMenu() {
    printf(" __  __       _        _____           _       _   _                 \n");
    printf("|  \\/  |     (_)      |_   _|         (_)     | | (_)                \n");
    printf("| \\  / | __ _ _ _ __    | |  _ ____   ___  ___| |_ _  ___  _ __  ___ \n");
    printf("| |\\/| |/ _` | | '_ \\   | | | '_ \\ \\ / / |/ __| __| |/ _ \\| '_ \\/ __|\n");
    printf("| |  | | (_| | | | | | _| |_| | | \\ V /| | (__| |_| | (_) | | | \\__ \\\n");
    printf("|_|  |_|\\__,_|_|_| |_||_____|_| |_|\\_/ |_|\\___|\\__|_|\\___/|_| |_|___/\n");
    printf("Main Menu\n");
    printf("1. Start Game\n");
    printf("2. Exit\n");
    printObservatory();
    printf("\n");
    printf("Enter your choice: ");
}

int main() {
    int choice;
    int validInput = 0;
    do {
        system("clear");
        printMainMenu();
        usleep(50000);
        validInput = scanf("%d", &choice);
        if (validInput != 1 || (choice != 1 && choice != 2)) {
            printf("Invalid choice. Please try again.\n");
            sleep(1);
            while (getchar() != '\n'); // Limpiar el buffer de entrada
            validInput = 0;
        } else {
            switch (choice) {
                case 1:
                    printInstructions();
                    sleep(4);
                    StartGame();
                    break;
                case 2:
                    printf("Exiting...\n");
                    sleep(1);
                    system("clear");
                    cleanup();
                    break;
            }
        }
    } while (choice != 2);

    return 0;
}