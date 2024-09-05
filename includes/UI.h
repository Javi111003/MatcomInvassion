//UI.h
#ifndef UI_H
#define UI_H

void printObservatory();
void printMainMenu();
void printInstructions();

extern const char shipDesign[][20];
extern const char *blankDesign[];
extern const int shipWidth, shipHeight;

extern const char* alien1_sprite[];
extern const int alien1_width, alien1_height;

extern const char* alien2_sprite[];
extern const int alien2_width, alien2_height;

extern const char* alien3_sprite[];
extern const int alien3_width, alien3_height;

#endif //UI_H