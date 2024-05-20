#include <riv.h>
#include <math.h>
#include <stdlib.h>

// Game config enums
enum {
    SCREEN_SIZE = 256,
    MAX_FPS = 60,
    FONTMOJI_SIZE = 16,
    BIGSPRITE_SIZE = 64,
    GAME_TIME = 30,
};

// Fruit enums and structs
typedef enum {
    PEACH,
    APPLE,
    GRAPES,
    BANANAS
} FruitType;

typedef enum {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} Direction;

typedef struct {
    FruitType type;
    Direction* directions;
    int directions_count;
    int current_direction;
} Fruit;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} SpriteRect;

// Sound fx
riv_waveform_desc slash_fx = {
    .type = RIV_WAVEFORM_NOISE,
    .attack = 0.000,
    .decay = 0.150,
    .sustain = 0.100,
    .release = 0.075,
    .start_frequency = 355.000,
    .end_frequency = 520.000,
    .amplitude = 0.150,
    .sustain_level = 0.250,
    .duty_cycle = 0.650,
    .pan = 0.000
};

riv_waveform_desc miss_fx = {
    .type = RIV_WAVEFORM_SINE,
    .attack = 0.100,
    .decay = 0.050,
    .sustain = 0.150,
    .release = 0.050,
    .start_frequency = 330.000,
    .end_frequency = 250.000,
    .amplitude = 0.200,
    .sustain_level = 0.550,
    .duty_cycle = 0.550,
    .pan = 0.000
};

riv_waveform_desc kill_fx = {
    .type = RIV_WAVEFORM_TRIANGLE,
    .attack = 0.000,
    .decay = 0.100,
    .sustain = 0.120,
    .release = 0.120,
    .start_frequency = 750.000,
    .end_frequency = 1025.000,
    .amplitude = 0.250,
    .sustain_level = 0.300,
    .duty_cycle = 0.500,
    .pan = 0.000
};

// Misc vars
int countdown_timer = GAME_TIME * MAX_FPS;
int time_remaining;
int min_slashes = 2;
int max_slashes = 4;
int score;
bool started = false;
bool ended = false;
Fruit* current_fruit = NULL;
bool up_pressed = false;
bool down_pressed = false;
bool left_pressed = false;
bool right_pressed = false;
int fruit_image_id = 0;
int misses = 0;
int peaches_slashed = 0;
int apples_slashed = 0;
int grapes_slashed = 0;
int bananas_slashed = 0;
int total_slashed = 0;

void start_game() {
    started = true;
    spawn_fruit();
}

void end_game() {
    ended = true;
    riv->quit_frame = riv->frame + 3 * riv->target_fps;
}

const char* get_fruit_type_name(FruitType type) {
    switch (type) {
        case PEACH: return "PEACH";
        case APPLE: return "APPLE";
        case GRAPES: return "GRAPES";
        case BANANAS: return "BANANAS";
        default: return "UNKNOWN";
    }
}

const char* get_direction_name(Direction direction) {
    switch (direction) {
        case DIRECTION_UP:
            return "UP";
        case DIRECTION_DOWN:
            return "DOWN";
        case DIRECTION_LEFT:
            return "LEFT";
        case DIRECTION_RIGHT:
            return "RIGHT";
        default:
            return "UNKNOWN";
    }
}

void spawn_fruit() {
    current_fruit = (Fruit*)malloc(sizeof(Fruit));
    current_fruit->directions_count = min_slashes + rand() % max_slashes;
    current_fruit->directions = (Direction*)malloc(current_fruit->directions_count * sizeof(Direction));
    
    int random_index = rand() % 4; // Generate a random index between 0 and 3
    switch (random_index) {
        case 0: current_fruit->type = PEACH; break;
        case 1: current_fruit->type = APPLE; break;
        case 2: current_fruit->type = GRAPES; break;
        case 3: current_fruit->type = BANANAS; break;
    }

    current_fruit->directions[0] = rand() % 4;

    for (int i = 1; i < current_fruit->directions_count; i++) {
        do {
            current_fruit->directions[i] = rand() % 4;
        } while (current_fruit->directions[i] == current_fruit->directions[i - 1]);
    }

    current_fruit->current_direction = 0;
}

void slash_fruit(Direction direction) {
    if (current_fruit->directions[current_fruit->current_direction] == direction) {
        riv_waveform(&slash_fx);
        current_fruit->current_direction++;
        if (current_fruit->current_direction == current_fruit->directions_count) {
            // Add score, more time and more slashes, destroy image, outcard stuff
            riv_waveform(&kill_fx);
            score += 100;
            countdown_timer += MAX_FPS * 2;
            if(score % 500 == 0) {
                min_slashes += 1;
                max_slashes += 1;
            }
            riv_destroy_image(fruit_image_id);
            if(current_fruit->type==PEACH){
                peaches_slashed += 1;
            } else if(current_fruit->type==APPLE){
                apples_slashed += 1;
            } else if(current_fruit->type==GRAPES){
                grapes_slashed += 1;
            } else {
                bananas_slashed += 1;
            }
            free(current_fruit->directions);
            free(current_fruit);
            current_fruit = NULL;
        }
    } else {
        // On miss remove time, score and update misses
        riv_waveform(&miss_fx);
        countdown_timer -= MAX_FPS;
        score -= 25;
        misses += 1;
    }
}

void update_game() {
    if (countdown_timer > 0) {
        countdown_timer--;
    }
    time_remaining = countdown_timer / MAX_FPS;
    if (countdown_timer <= 0) {
        end_game();
    }
    if (!current_fruit) {
        spawn_fruit();
    } else {
        if (riv->keys[RIV_GAMEPAD_UP].down && !up_pressed) {
            slash_fruit(DIRECTION_UP);
            up_pressed = true;
        } else if (!riv->keys[RIV_GAMEPAD_UP].down) {
            up_pressed = false;
        }

        if (riv->keys[RIV_GAMEPAD_DOWN].down && !down_pressed) {
            slash_fruit(DIRECTION_DOWN);
            down_pressed = true;
        } else if (!riv->keys[RIV_GAMEPAD_DOWN].down) {
            down_pressed = false;
        }

        if (riv->keys[RIV_GAMEPAD_LEFT].down && !left_pressed) {
            slash_fruit(DIRECTION_LEFT);
            left_pressed = true;
        } else if (!riv->keys[RIV_GAMEPAD_LEFT].down) {
            left_pressed = false;
        }

        if (riv->keys[RIV_GAMEPAD_RIGHT].down && !right_pressed) {
            slash_fruit(DIRECTION_RIGHT);
            right_pressed = true;
        } else if (!riv->keys[RIV_GAMEPAD_RIGHT].down) {
            right_pressed = false;
        }
    }
    update_score();
}

void update_score() {
    riv->outcard_len = riv_snprintf((char*)riv->outcard, RIV_SIZE_OUTCARD, "JSON{\"score\":%d,\"peaches\":%d,\"apples\":%d,\"grapes\":%d,\"bananas\":%d,\"misses\":%d}", score, peaches_slashed, apples_slashed, grapes_slashed, bananas_slashed, misses);
}

void draw_game() {
    char buf[128];
    uint32_t final_seconds = (time_remaining <= 10) ? RIV_COLOR_RED : RIV_COLOR_WHITE;
    riv_snprintf(buf, sizeof(buf), "TIME LEFT: %d", time_remaining);
    riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_BOTTOMLEFT, 1, riv->height - 4, 1, final_seconds);
    riv_snprintf(buf, sizeof(buf), "SCORE: %d", score);
    riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_BOTTOMRIGHT, 254, riv->height - 4, 1, RIV_COLOR_WHITE);

    if (current_fruit) {
        char *fruit_name = get_fruit_type_name(current_fruit->type);
        riv_snprintf(buf, sizeof(buf), "CURRENT FRUIT: %s", fruit_name);
        riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_CENTER, 128, 10, 1, RIV_COLOR_WHITE);
        char fruit_image_path[50];
        snprintf(fruit_image_path, sizeof(fruit_image_path), "%s.png", fruit_name);
        fruit_image_id = riv_make_image(fruit_image_path, RIV_COLOR_BLACK);
        riv_draw_image_rect(fruit_image_id, 112, 112, 32, 32, 0, 0, 1, 1);
        riv_snprintf(buf, sizeof(buf), "SLASHES LEFT: %d", current_fruit->directions_count - current_fruit->current_direction);
        riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_CENTER, 128, 20, 1, RIV_COLOR_WHITE);
        riv_snprintf(buf, sizeof(buf), "CURRENT DIRECTION: %s", get_direction_name(current_fruit->directions[current_fruit->current_direction]));
        riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_BOTTOM, riv->width / 2, riv->height - 20, 1, RIV_COLOR_WHITE);
    }
}

void draw_start_screen() {
    int splash_screen = riv_make_image("splash.png", RIV_COLOR_BLACK);
    riv_draw_image_rect(splash_screen, 0, 10, 256, 256, 0, 0, 1, 1);
    uint32_t flash_start = (riv->frame % 60 < 30) ? RIV_COLOR_WHITE : RIV_COLOR_BLACK;
    riv_draw_text("PRESS START!",
        RIV_SPRITESHEET_FONT_5X7,
        RIV_CENTER,
        128,
        230,
        2,
        flash_start
    );
}

void draw_end_screen() {
    draw_game();
    riv_clear(RIV_COLOR_BLUE);
    uint32_t flash_end = (riv->frame % 60 < 30) ? RIV_COLOR_RED : RIV_COLOR_BLACK;
    riv_draw_text("GAME OVER", RIV_SPRITESHEET_FONT_5X7, RIV_CENTER, 128, 128, 4, flash_end);
}

void update() {
    if (!started) {
        if (riv->keys[RIV_GAMEPAD_START].press||riv->keys[RIV_GAMEPAD_A1].press) {
            start_game();
        }
    } else if (!ended) {
        update_game();
    }
}

void draw() {
    riv_clear(RIV_COLOR_BLUE);
    if (!started) {
        draw_start_screen();
    } else if (!ended) {
        draw_game();
    } else {
        draw_end_screen();
    }
}

int main() {
    // Params
    riv->width = SCREEN_SIZE;
    riv->height = SCREEN_SIZE;
    riv->target_fps = MAX_FPS;
    do {
        update();
        draw();
    } while (riv_present());
    return 0;
}
