// Scores.h
#ifndef SCORES_H
#define SCORES_H

typedef struct {
    char name[50];
    int current_score;
    int best_score;
} Player;

void init_player(Player *player);
void update_score(Player *player, int score);
void save_best_score(Player *player);
void load_best_scores(Player players[], int size);
void display_top_scores(Player players[], int size);
int is_name_taken(const char *name, Player players[], int size);
void to_lowercase(char *str);

#endif // SCORES_H