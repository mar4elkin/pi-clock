
#pragma once

#include <array>
#include <cstring>
#include "lcdgfx.h"
#include <cmath>

class TextLayout final {
public:
    static void printCentered(
        NanoCanvas<128, 64, 1>& canvas,
        int size_x,
        int size_y,
        int x,
        int y,
        const char* text
    ) {
        const int width =
            static_cast<int>(std::strlen(text)) * size_x;

        canvas.printFixed(
            x - width / 2,
            y - size_y / 2,
            text,
            STYLE_NORMAL
        );
    }

    template <typename... Args>
    static void printSpaceBetween(
        NanoCanvas<128, 64, 1>& canvas,
        int y,
        int padding,
        int char_width,
        Args... text
    ) {
        std::array<const char*, sizeof...(Args)> texts{text...};

        if (texts.empty()) {
            return;
        }

        constexpr int screen_width = 128;

        int total_width = 0;

        for (const char* str : texts) {
            total_width += std::strlen(str) * char_width;
        }

        const int free_space =
            screen_width - 2 * padding - total_width;

        if (free_space < 0) {
            return;
        }

        const int gap_count = static_cast<int>(texts.size()) - 1;

        int used_width = 0;

        for (std::size_t i = 0; i < texts.size(); ++i) {
            const int index = static_cast<int>(i);

            const int offset = gap_count > 0
                ? free_space * index / gap_count
                : 0;

            const int x = padding + used_width + offset;

            canvas.printFixed(x, y, texts[i]);

            used_width += std::strlen(texts[i]) * char_width;
        }
    }
};
