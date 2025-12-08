#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

/*
 * Minimal text-mode video state for v0.1.
 *
 * We do not emulate a full VGA yet. Instead, we model a simple 80x25
 * text buffer similar to classic PC text mode: each cell has a character
 * and an attribute byte.
 */

#define VIDEO_TEXT_COLS 80
#define VIDEO_TEXT_ROWS 25

typedef struct {
    /* Character codes for each cell (e.g., ASCII). */
    uint8_t chars[VIDEO_TEXT_ROWS][VIDEO_TEXT_COLS];

    /* Attribute bytes for each cell (foreground/background colors, etc.). */
    uint8_t attrs[VIDEO_TEXT_ROWS][VIDEO_TEXT_COLS];
} video_state_t;

#endif /* VIDEO_H */