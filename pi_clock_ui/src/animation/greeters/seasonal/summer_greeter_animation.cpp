#include "animation/greeters/seasonal/summer_greeter_animation.hpp"
#include "animation/greeters/seasonal/greeter_drawing.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

using namespace greeter_detail;

namespace {
constexpr double pi=std::numbers::pi;
constexpr double kRevealStart=2.55;
constexpr double kFlashStart=2.04;
constexpr int cx=64;
constexpr int sunY=24;

void drawHorizon(Canvas& c,double t) {
    const double appear=smooth((t-0.03)/0.35);
    if(appear<=0.0)return;
    const int l=iround(63*appear);
    line(c,cx-l,52,cx+l,52);
    for(int i=0;i<12;++i) {
        const double twinkle=smooth((t-0.45)/0.45);
        if(twinkle<=0.0)continue;
        const int x=3+iround(noise(i,5,1)*120.0);
        const int y=54+(i%3)*3;
        const int len=std::max(1,iround(twinkle*(3+(i%4)*2)));
        const int shift=iround(3*std::sin(t*(4+i%3)+i));
        if(x+shift-len<0||x+shift+len>127)continue;
        line(c,x+shift-len,y,x+shift+len,y);
    }
}

void drawSun(Canvas& c,double t) {
    const double rise=easeOut((t-0.26)/1.05);
    if(rise<=0.0)return;
    const int cy=iround(60.0-36.0*rise);
    const int radius=std::max(1,iround(11.0*smooth((t-0.26)/0.72)));
    c.drawCircle(cx,cy,radius);
    if(radius>=8)c.drawCircle(cx,cy,radius-3);
    dot(c,cx,cy);

    const double rays=smooth((t-0.73)/0.64);
    if(rays<=0.0)return;
    for(int i=0;i<12;++i) {
        const double angle=2*pi*i/12.0+0.10*std::sin(t*2.6);
        const Point a=polar(cx,cy,angle,radius+3);
        const Point b=polar(cx,cy,angle,radius+3+(5.0+3.0*std::sin(t*4.0+i))*rays);
        if(b.x<1||b.x>126||b.y<1||b.y>62)continue;
        line(c,a.x,a.y,b.x,b.y);
    }
}

void drawHeatWaves(Canvas& c,double t) {
    const double strength=smooth((t-0.88)/0.64)
                         *(1.0-smooth((t-2.02)/0.44));
    if(strength<=0.0)return;
    for(int row=0;row<5;++row) {
        const int y=38+row*5;
        for(int i=0;i<5;++i) {
            const double shifted=14+i*26+5*std::sin(t*(3.5+row*.3)+i);
            const int x=iround(shifted);
            const int len=iround((3+(i+row)%5)*strength);
            if(len<=0||x<1||x+len>127)continue;
            line(c,x,y,x+len/2,y-1);
            line(c,x+len/2,y-1,x+len,y);
        }
    }
}

void drawSolarHalo(Canvas& c,double t) {
    const double burst=smooth((t-1.34)/0.74);
    if(burst<=0.0)return;
    const double dissolve=1.0-smooth((t-2.03)/0.40);
    const double intensity=burst*dissolve;
    if(intensity<=0.0)return;

    for(int ring=0;ring<2;++ring) {
        const double radius=18+ring*10+burst*(ring?11:14);
        for(int i=0;i<16;++i) {
            const double angle=i*2*pi/16.0+t*0.13+ring*0.17;
            const Point a=polar(cx,sunY,angle,radius);
            const Point b=polar(cx,sunY,angle,radius+2+4*intensity);
            if(a.x<1||a.x>126||a.y<1||a.y>62||b.x<1||b.x>126||b.y<1||b.y>62)continue;
            if(i%4!=ring)line(c,a.x,a.y,b.x,b.y);
        }
    }
    for(int i=0;i<9;++i) {
        const double a=i*2*pi/9.0-t*.25;
        const Point p=polar(cx,sunY,a,17+iround(18*burst));
        if(i%3==0)sparkle(c,p.x,p.y,1);
    }
}

void whiteFlash(Canvas& c,double t) {
    const double p=smooth((t-kFlashStart)/(kRevealStart-kFlashStart));
    if(p<=0.0)return;
    c.setColor(0xFFFF);
    // Expand the sun's disc into a full-screen flash before reveal.
    const double radius=85.0*p;
    for(int y=0;y<64;++y) {
        const double dy=y-sunY;
        if(std::abs(dy)>radius)continue;
        const int dx=iround(std::sqrt(radius*radius-dy*dy));
        c.fillRect(std::max(0,cx-dx),y,std::min(127,cx+dx),y);
    }
}
} // namespace

void SummerGreeterAnimation::drawScene(Canvas& c) const {
    drawHorizon(c,elapsed_);
    drawSun(c,elapsed_);
    drawHeatWaves(c,elapsed_);
    drawSolarHalo(c,elapsed_);
    whiteFlash(c,elapsed_);
}

double SummerGreeterAnimation::revealThreshold(int col,int row) const {
    const double dx=(col-7.5)/7.5;
    const double dy=(row-3.5)/3.5;
    return clamp01(0.04+0.83*std::min(1.0,std::hypot(dx,dy)/1.45)
                   +0.13*noise(col,row,13));
}
