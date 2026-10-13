#include "timer_screen.hpp"
#include "screen/text_layout.hpp"

void TimerScreen::update(const RenderContext &context)
{
}

void TimerScreen::render(NanoCanvas<128, 64, 1> &canvas)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    TextLayout::printSpaceBetween(canvas, 2, 5, 6, "TIMER", "IDLE");

    canvas.setFixedFont(comic_sans_font24x32_123);
    TextLayout::printCentered(canvas, 24, 32, center_x, center_y, "00:00");

    canvas.drawRect(5, 50, 122, 55);
    
    canvas.fillRect(5, 50, 122 / 2, 55);
}

bool TimerScreen::handleInput(InputAction action)
{
    return false;
}
