#include <stdio.h>
#include "../../includes/UI.h"

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
//Ship Design
const char shipDesign[][20] = {
    "     _    ",
    "    /_\\   ",
    "   |( )|  ",
    "   | _ |  ",
    "   ||-||  ",
    "  / |_| \\ ",
    " (MAT_COM)",
    "  (') (') "
};
const char *blankDesign[] = {
    "          ",
    "          ",
    "          ",
    "          ",
    "          ",
    "          ",
    "          ",
    "          "
};
const int shipWidth = 9, shipHeight = 8;
// Tipo 1
const char* alien1_sprite[] = {
    " /\\ ",
    "/  \\",
    "\\  /",
    " \\/ "
};
const int alien1_width = 4, alien1_height = 4;

// Tipo 2
const char* alien2_sprite[] = {
    " /\\ ",
    "/||\\",
    "\\  /",
    " \\/ "
};
const int alien2_width = 4, alien2_height = 4;

// Tipo 3
const char* alien3_sprite[] = {
    " /\\/\\ ",
    "/|  |\\",
    "\\|  |/",
    " \\/\\/ "
};
const int alien3_width = 6, alien3_height = 4;
