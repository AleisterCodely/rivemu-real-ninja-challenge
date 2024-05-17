#include <riv.h>
#include <math.h>

enum {
  SCREEN_SIZE = 256,
  MAX_FPS = 15,
  FONTMOJI_SIZE = 16,
  BIGSPRITE_SIZE = 64  
};

bool started;
bool ended;

void start_game() {
    started = true;
}

void end_game() {
    ended = true;
    riv->quit_frame = riv->frame + 3*riv->target_fps;
}

void update_game() {
    end_game();
}

void draw_game() {
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
    uint32_t flash_start = (riv->frame % 10 == 0) ? RIV_COLOR_WHITE : RIV_COLOR_BLACK;
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
    uint32_t flash_end = (riv->frame % 10 == 0) ? RIV_COLOR_RED : RIV_COLOR_BLACK;
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
    } while(riv_present());
    return 0;
}
