#include <riv.h>
#include <math.h>
#include <stdlib.h>

enum {
    SCREEN_SIZE = 256,
    MAX_FPS = 60,
    FONTMOJI_SIZE = 16,
    BIGSPRITE_SIZE = 64,
    GAME_TIME = 30,
};

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
    current_fruit->type = rand() % 4;
    current_fruit->directions_count = min_slashes + rand() % max_slashes;
    current_fruit->directions = (Direction*)malloc(current_fruit->directions_count * sizeof(Direction));

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
        current_fruit->current_direction++;
        if (current_fruit->current_direction == current_fruit->directions_count) {
            // Fruit fully slashed
            free(current_fruit->directions);
            free(current_fruit);
            current_fruit = NULL;
            score += 100;
            countdown_timer += MAX_FPS;
            if(score % 500 == 0) {
                min_slashes += 1;
                max_slashes += 1;
            }
        }
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
}

void draw_game() {
    char buf[128];
    uint32_t final_seconds = (time_remaining <= 10) ? RIV_COLOR_RED : RIV_COLOR_WHITE;
    riv_snprintf(buf, sizeof(buf), "TIME LEFT: %d", time_remaining);
    riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_BOTTOMLEFT, 1, riv->height - 4, 1, final_seconds);
    riv_snprintf(buf, sizeof(buf), "SCORE: %d", score);
    riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_BOTTOMRIGHT, 254, riv->height - 4, 1, RIV_COLOR_WHITE);

    if (current_fruit) {
        riv_snprintf(buf, sizeof(buf), "CURRENT FRUIT: %s", get_fruit_type_name(current_fruit->type));
        riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_CENTER, 128, 10, 1, RIV_COLOR_WHITE);
        riv_snprintf(buf, sizeof(buf), "SLASHES LEFT: %d", current_fruit->directions_count - current_fruit->current_direction);
        riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_CENTER, 128, 20, 1, RIV_COLOR_WHITE);
        riv_snprintf(buf, sizeof(buf), "CURRENT DIRECTION: %s", get_direction_name(current_fruit->directions[current_fruit->current_direction]));
        riv_draw_text(buf, RIV_SPRITESHEET_FONT_3X5, RIV_BOTTOM, riv->width / 2, riv->height - 20, 1, RIV_COLOR_WHITE);
    }
}

void draw_start_screen() {
    riv_draw_text(
        "REAL NINJA",
        RIV_SPRITESHEET_FONT_5X7,
        RIV_CENTER,
        128,
        15,
        4,
        RIV_COLOR_ORANGE
    );
    riv_draw_text("CHALLENGE!",
        RIV_SPRITESHEET_FONT_5X7,
        RIV_CENTER,
        128,
        50,
        4,
        RIV_COLOR_ORANGE
    );
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
    uint32_t flash_end = (riv->frame % 60 < 30) ? RIV_COLOR_RED : RIV_COLOR_BLACK;
    riv_draw_text("GAME OVER", RIV_SPRITESHEET_FONT_5X7, RIV_CENTER, 128, 128, 4, flash_end);
}

void update() {
    if (!started) {
        if (riv->key_toggle_count > 0) {
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
