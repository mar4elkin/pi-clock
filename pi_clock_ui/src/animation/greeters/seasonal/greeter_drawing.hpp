#pragma once

#include "lcdgfx.h"

namespace greeter_detail {
using Canvas = NanoCanvas<128, 64, 1>;
struct Point { int x; int y; };

double noise(int x, int y, int salt = 0);
double clamp01(double t);
double easeOut(double t);
double smooth(double t);
int iround(double v);
Point polar(int cx, int cy, double radians, double radius);
void dot(Canvas& c, int x, int y);
void line(Canvas& c, int x1, int y1, int x2, int y2);
void centered(Canvas& c, const char* str, int y);
void sparkle(Canvas& c, int cx, int cy, int r);
void fullWhite(Canvas& c, const char* heading, const char* subheading);
} // namespace greeter_detail
