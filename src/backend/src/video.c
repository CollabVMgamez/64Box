#include "video.h"
#include <string.h>

/*
 * Very simple text-mode video helper functions.
 *
 * For now we only support a single 80x25 text page stored in the
 * video_state_t struct. Higher-level code (emulator, UI, or future
 * GPU/device emulation) can update this buffer to reflect what should
 * be shown on screen.
 */

/* Clear the entire text buffer (characters and attributes). */
void video_clear(video_state_t* video)
{
    if (!video) {
        return;
    }

    memset(video->chars, 0, sizeof(video->chars));
    memset(video->attrs, 0, sizeof(video->attrs));
}

/* Initialize video state to a known default. */
void video_init(video_state_t* video)
{
    video_clear(video);
}

/*
 * Set a character and attribute at the given text-mode position.
 * Out-of-range coordinates are ignored.
 */
void video_put_char(video_state_t* video,
                    uint32_t col,
                    uint32_t row,
                    uint8_t ch,
                    uint8_t attr)
{
    if (!video) {
        return;
    }

    if (col >= VIDEO_TEXT_COLS || row >= VIDEO_TEXT_ROWS) {
        return;
    }

    video->chars[row][col] = ch;
    video->attrs[row][col] = attr;
}