#include "screen/clock_display_screen.hpp"

#include <cstdio>

void ClockDisplayScreen::update(const RenderContext& context) {
    local_time_ = context.local_time;
}

void ClockDisplayScreen::render(NanoCanvas<128, 64, 1>& canvas) {
    char hhmm[6];
    char seconds[3];
    char date[22];

    std::snprintf(
        hhmm, sizeof(hhmm),
        "%02d:%02d",
        local_time_.tm_hour,
        local_time_.tm_min
    );

    std::snprintf(
        seconds, sizeof(seconds),
        "%02d",
        local_time_.tm_sec
    );

    static const char* weekdays[] = {
        "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
    };

    static const char* months[] = {
        "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
        "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
    };

    const int weekday = local_time_.tm_wday;
    const int month = local_time_.tm_mon;

    std::snprintf(
        date, sizeof(date),
        "%s %02d %s %04d",
        weekdays[weekday >= 0 && weekday < 7 ? weekday : 0],
        local_time_.tm_mday,
        months[month >= 0 && month < 12 ? month : 0],
        local_time_.tm_year + 1900
    );

    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.printFixed(4, 0, "PI CLOCK", STYLE_NORMAL);
    canvas.printFixed(112, 0, seconds, STYLE_NORMAL);

    canvas.setFixedFont(comic_sans_font24x32_123);
    canvas.printFixed(4, 16, hhmm, STYLE_NORMAL);

    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.drawHLine(4, 50, 123);
    canvas.printFixed(17, 56, date, STYLE_NORMAL);
}