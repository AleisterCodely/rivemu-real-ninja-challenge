#include <riv.h>
#include <math.h>

enum {
  SCREEN_SIZE = 256,
  MAX_FPS = 60,
  FONTMOJI_SIZE = 16,
  BIGSPRITE_SIZE = 64,
  GAME_TIME = 30,
};


// How do I fps math?
int countdown_timer = GAME_TIME * MAX_FPS;
int time_remaining;
int score;

bool started = false;
bool ended = false;
bool fruit_present = false;

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
        if(riv->keys[RIV_GAMEPAD_UP].down){
            slash_fruit();
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
    riv_printf("Spawning fruit\n");
    fruit_present = true;
}

void slash_fruit() {
    riv_printf("Slashed fruit\n");
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
