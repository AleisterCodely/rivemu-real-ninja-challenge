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

enum {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
} Direction;

enum {
    PEACH,
    APPLE,
    GRAPES,
    BANANAS
} FruitType;

struct {
    FruitType type;
    Direction* directions;
    int directions_count;
    int current_direction;
} Fruit;

// How do I fps math?
int countdown_timer = GAME_TIME * MAX_FPS;
int time_remaining, score;
bool started, ended, fruit_present, up_pressed, down_pressed, left_pressed, right_pressed = false;
Fruit* current_fruit = NULL;

void start_game() {
    started = true;
    spawn_fruit();
}

void end_game() {
    ended = true;
    riv->quit_frame = riv->frame + 3*riv->target_fps;
}

void update_game() {
    if (countdown_timer > 0) {
        countdown_timer--;
    }
    time_remaining = countdown_timer / MAX_FPS;
    if(countdown_timer <= 0) {
        end_game();
    }
    if(!fruit_present){
        spawn_fruit();
    } else {
        if(riv->keys[RIV_GAMEPAD_UP].down && !up_pressed){
            slash_fruit(DIRECTION_UP);
            up_pressed = true;
        } else if (!riv->keys[RIV_GAMEPAD_UP].down) {
            up_pressed = false;
        }
        if(riv->keys[RIV_GAMEPAD_DOWN].down && !down_pressed){
            slash_fruit(DIRECTION_DOWN);
            down_pressed = true;
        } else if (!riv->keys[RIV_GAMEPAD_DOWN].down) {
            down_pressed = false;
        }
        if(riv->keys[RIV_GAMEPAD_LEFT].down && !left_pressed){
            slash_fruit(DIRECTION_LEFT);
            left_pressed = true;
        } else if (!riv->keys[RIV_GAMEPAD_LEFT].down) {
            left_pressed = false;
        }
        if(riv->keys[RIV_GAMEPAD_RIGHT].down && !right_pressed){
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
    uint32_t flash_start = (riv->frame % 60 == 0) ? RIV_COLOR_WHITE : RIV_COLOR_BLACK;
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
    uint32_t flash_end = (riv->frame % 60 == 0) ? RIV_COLOR_RED : RIV_COLOR_BLACK;
    riv_draw_text("GAME OVER", RIV_SPRITESHEET_FONT_5X7, RIV_CENTER, 128, 128, 4, flash_end);
}

void spawn_fruit() {
    current_fruit = (Fruit*)malloc(sizeof(Fruit));
    current_fruit->type = rand() % 4;
    current_fruit->directions_count = 2 + rand() % 3;
    current_fruit->directions = (Direction*)malloc(current_fruit->directions_count * sizeof(Direction));
    for (int i = 0; i < current_fruit->directions_count; i++) {
        current_fruit->directions[i] = rand() % 4; // Random direction
    }
    current_fruit->current_direction = 0;
    riv_printf("Spawning fruit\n");
    fruit_present = true;
}

void slash_fruit(Direction dir) {
    switch(dir) {
        case DIRECTION_UP:
            riv_printf("Slashed fruit upwards!\n");
            break;
        case DIRECTION_DOWN:
            riv_printf("Slashed fruit downwards!\n");
            break;
        case DIRECTION_LEFT:
            riv_printf("Slashed fruit leftways!\n");
            break;
        case DIRECTION_RIGHT:
            riv_printf("Slashed fruit rightways!\n");
            break;
    }
    fruit_present = false;
    score += 100;
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
    } while(riv_present());
    return 0;
}
