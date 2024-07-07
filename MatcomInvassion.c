#include<stdio.h>
#include<ncurses.h>
#include<unistd.h>
#include<pthread.h>
typedef struct {
    int healt;
    int x;
    int y;
} Alien;
typedef struct {
    int health;
    int x;
    int y;
    int width;
    int height;
} SpaceShip;
void shoot(int startX, int startY) {
    int shotY = startY - 1; // Comienza un poco arriba de la nave
    while (shotY > 0) {
        mvprintw(shotY, startX, "|"); // Dibuja el disparo
        refresh();
        usleep(20000); // Espera un poco para simular el movimiento del disparo
        mvprintw(shotY, startX, " "); // Borra el disparo anterior
        shotY--; // Mueve el disparo hacia arriba
    }
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
        mvprintw(ship->y + i, ship->x, "%s", erase ? blankDesign[i] : shipDesign[i]);
    }
}

    void StartGame() {
        initscr();
        noecho();
        curs_set(FALSE);
        keypad(stdscr, TRUE);//activando las flechas del teclado 
        nodelay(stdscr,TRUE);//getch no loqueante
        timeout(50); // Establece un tiempo de espera de 50 ms para getch()

        SpaceShip myShip = {100, 10, 10, 5, 3};

        pthread_t shootThread;

        int ch;
        while ((ch = getch()) != 'q') {
            drawShip(&myShip, true); // Borra la nave en la posición actual
            switch (ch) {
                case 'w': myShip.y--; break;
                case 's': myShip.y++; break;
                case 'a': myShip.x--; break;
                case 'd': myShip.x++; break;
                case ' ': // Barra espaciadora para disparar
                shoot(myShip.x + 2, myShip.y); // Asume que el disparo sale del centro de la nave
                break;
            }
            drawShip(&myShip, false); // Dibuja la nave en la nueva posición
            refresh();
        }

        endwin();
    }

int main() {
    
    printf(" __  __       _        _____           _       _   _                 \n");
    printf("|  \\/  |     (_)      |_   _|         (_)     | | (_)                \n");
    printf("| \\  / | __ _ _ _ __    | |  _ ____   ___  ___| |_ _  ___  _ __  ___ \n");
    printf("| |\\/| |/ _` | | '_ \\   | | | '_ \\ \\ / / |/ __| __| |/ _ \\| '_ \\/ __|\n");
    printf("| |  | | (_| | | | | | _| |_| | | \\ V /| | (__| |_| | (_) | | | \\__ \\\n");
    printf("|_|  |_|\\__,_|_|_| |_||_____|_| |_|\\_/ |_|\\___|\\__|_|\\___/|_| |_|___/\n");
    printf("\n");

    // El resto de tu código main va aquí, por ejemplo:
    int choice;
    do {
        printf("Main Menu\n");
        printf("1. Start Game\n");
        printf("2. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                StartGame();
                break;
            case 2:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 2);

    return 0;
}
