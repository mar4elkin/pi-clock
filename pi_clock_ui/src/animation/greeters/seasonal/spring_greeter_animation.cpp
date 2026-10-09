#include "animation/greeters/seasonal/spring_greeter_animation.hpp"
#include "animation/greeters/seasonal/greeter_drawing.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

using namespace greeter_detail;

namespace {
constexpr double pi = std::numbers::pi;
constexpr double kRevealStart = 2.55;
constexpr double kFlashStart = 2.05;
constexpr int cx = 64;
constexpr int bloomY = 24;

void drawDrop(Canvas& c, int x, int y, int size) {
    if (x < 1 || x > 126 || y < 2 || y > 62) return;
    line(c, x, y - size, x, y);
    if (size > 2) dot(c, x - 1, y);
}

void springRain(Canvas& c, double t) {
    const double fade = 1.0 - smooth((t - 1.32) / 0.65);
    if (fade <= 0.0) return;
    const int count = 5 + iround(fade * 13.0);
    for (int i=0;i<count;++i) {
        const double speed = 15.0 + noise(i, 5, 9) * 25.0;
        const double yy = std::fmod(64.0*noise(i,2,4) + t*speed, 76.0) - 5.0;
        const double xx = 2.0 + noise(i,8,3)*123.0 + std::sin(t*2.0+i)*3.0;
        drawDrop(c, iround(xx), iround(yy), i%3==0?3:1);
    }
}

void drawPlant(Canvas& c, double t) {
    const double grow = easeOut((t - 0.32) / 0.92);
    if (grow<=0) return;
    const int top = iround(62.0-(62.0-bloomY)*grow);
    line(c,cx,63,cx,top);
    for(int i=0;i<3;++i) {
        const int y=54-i*10;
        const double leafGrowth = easeOut((grow-(0.23+i*0.20))/0.20);
        if (leafGrowth<=0.0 || y < top) continue;
        const int dir=(i%2==0)?-1:1;
        const int ex=cx+dir*iround(14*leafGrowth);
        const int ey=y-iround(10*leafGrowth);
        line(c,cx,y,ex,ey);
        const int mx=cx+dir*iround(7*leafGrowth);
        const int my=y-iround(7*leafGrowth);
        line(c,ex,ey,mx,my-2);
        line(c,mx,my-2,cx,y);
    }

    const double bloom=easeOut((t-1.16)/0.75);
    if(bloom<=0)return;
    // Five petals grow from a single center instead of popping in.
    for(int i=0;i<5;++i) {
        const double a = -pi/2 + (2*pi*i)/5;
        const Point middle=polar(cx,bloomY,a,iround(5*bloom));
        const Point tip=polar(cx,bloomY,a,iround(11*bloom));
        const Point side1=polar(middle.x,middle.y,a+pi/2,iround(4*bloom));
        const Point side2=polar(middle.x,middle.y,a-pi/2,iround(4*bloom));
        line(c,cx,bloomY,side1.x,side1.y);
        line(c,side1.x,side1.y,tip.x,tip.y);
        line(c,tip.x,tip.y,side2.x,side2.y);
        line(c,side2.x,side2.y,cx,bloomY);
    }
    c.drawCircle(cx,bloomY,std::max(1,iround(2*bloom)));
}

void floatingPollen(Canvas& c, double t) {
    const double bloom = smooth((t-1.40)/0.57);
    const double fade = 1.0-smooth((t-2.03)/0.42);
    if(bloom<=0.0 || fade<=0.0)return;
    const int n = iround(bloom*28.0);
    for(int i=0;i<n;++i) {
        const double a=(2*pi*i/28.0)+noise(i,1,7)*0.24;
        const double radial=(8.0+(i%5)*3.3)*bloom+(t-1.4)*(4.0+i%3*3.0);
        const int x=iround(cx+std::cos(a+0.08*t)*radial);
        const int y=iround(bloomY+std::sin(a+0.08*t)*radial*0.68);
        dot(c,x,y);
        if(i%7==0 && fade>0.38)sparkle(c,x,y,1);
    }
}

void whiteBloom(Canvas& c, double t) {
    const double p=smooth((t-kFlashStart)/(kRevealStart-kFlashStart));
    if(p<=0.0)return;
    // Expanded iris: at reveal_start the canvas is already completely white.
    const double radius=85.0*p;
    c.setColor(0xFFFF);
    for(int y=0;y<64;++y) {
        const double dy=y-bloomY;
        if(std::abs(dy)>radius)continue;
        const int dx=iround(std::sqrt(radius*radius-dy*dy));
        const int left=std::max(0,cx-dx);
        const int right=std::min(127,cx+dx);
        c.fillRect(left,y,right,y);
    }
}
} // namespace

void SpringGreeterAnimation::drawScene(Canvas& c) const {
    springRain(c,elapsed_);
    drawPlant(c,elapsed_);
    floatingPollen(c,elapsed_);
    whiteBloom(c,elapsed_);
}

double SpringGreeterAnimation::revealThreshold(int col,int row) const {
    // Rising from the bottom center, like the original growing stem.
    const double vertical=(7.0-row)/7.0;
    const double outward=std::abs(col-7.5)/7.5;
    return clamp01(0.03 + 0.64*vertical + 0.26*outward +
                   0.07*noise(col,row,11));
}
