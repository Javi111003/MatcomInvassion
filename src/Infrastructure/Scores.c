// Scores.c
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../../includes/Scores.h"

void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void init_player(Player *player) {
    char name[50];
    Player top_players[5];
    load_best_scores(top_players, 5);

    printf("Enter your name: ");
    scanf("%s", name);
    to_lowercase(name);

    while (is_name_taken(name, top_players, 5)) {
        printf("Name already taken. Enter a different name: ");
        scanf("%s", name);
        to_lowercase(name);
    }

    strcpy(player->name, name);
    player->current_score = 0;
    player->best_score = 0;
}

int is_name_taken(const char *name, Player players[], int size) {
    for (int i = 0; i < size; i++) {
        char existing_name[50];
        strcpy(existing_name, players[i].name);
        to_lowercase(existing_name);
        if (strcmp(name, existing_name) == 0) {
            return 1;
        }
    }
    return 0;
}

void update_score(Player *player, int score) {
    player->current_score += score;
    if (player->current_score > player->best_score) {
        player->best_score = player->current_score;
    }
}

void save_best_score(Player *player) {
    FILE *file = fopen("scores.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s %d\n", player->name, player->best_score);
        fclose(file);
    }
}

void load_best_scores(Player players[], int size) {
    FILE *file = fopen("scores.txt", "r");
    if (file != NULL) {
        for (int i = 0; i < size && !feof(file); i++) {
            fscanf(file, "%s %d", players[i].name, &players[i].best_score);
        }
        fclose(file);
    }
}

void display_top_scores(Player players[], int size) {
    printf("Top 5 Scores:\n");
    for (int i = 0; i < size; i++) {
        printf("%d. %s: %d\n", i + 1, players[i].name, players[i].best_score);
    }
}