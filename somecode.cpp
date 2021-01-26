#define _AMD64_
#include <WinDef.h>
#include <cmath>
#include <comdef.h>
#include <gdiplus.h>
#include <iostream>
#include <pybind11/pybind11.h>
#include <string>
#include <vector>
#include <windows.h>
#pragma comment(lib, "gdiplus.lib")

namespace py = pybind11;

using namespace std;
using namespace Gdiplus;

struct block {
    double x1, y1, x2, y2;
    friend bool operator==(block a, block b)
    {
        return a.y2 == b.y2 && a.x2 == b.x2;
    }
    double distance(block a)
    {
        if (a.x1 <= x2 && x1 <= a.x2 && a.y2 >= y1 && y2 >= a.y1) {
            return 0;
        }
        int lenx, leny;
        double ans = 0x3f3f3f3f;
        lenx = a.x1 - x1;
        leny = a.y1 - y1;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x2 - x1;
        leny = a.y1 - y1;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x1 - x1;
        leny = a.y2 - y1;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x2 - x1;
        leny = a.y2 - y1;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x1 - x2;
        leny = a.y1 - y1;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x2 - x2;
        leny = a.y1 - y1;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x1 - x2;
        leny = a.y2 - y1;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x2 - x2;
        leny = a.y2 - y1;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x1 - x2;
        leny = a.y1 - y2;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x2 - x2;
        leny = a.y1 - y2;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x1 - x2;
        leny = a.y2 - y2;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x2 - x2;
        leny = a.y2 - y2;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x1 - x1;
        leny = a.y1 - y2;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x2 - x1;
        leny = a.y1 - y2;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x1 - x1;
        leny = a.y2 - y2;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        lenx = a.x2 - x1;
        leny = a.y2 - y2;
        ans = min(ans, sqrt(lenx * lenx + leny * leny * 1.0));
        return ans;
    }
    block(double _x1, double _y1, double _x2, double _y2)
    {
        x1 = _x1;
        y1 = _y1;
        x2 = _x2;
        y2 = _y2;
    }
    block() { }
};

vector<block> wall;
inline bool isblack(int r, int g, int b)
{
    return (r <= 20 && g <= 20 && b <= 20);
}
inline bool isred(int r, int g, int b)
{
    return (r <= 229 && g <= 10 && b <= 31) && !isblack(r, g, b);
}

block man;
int height, width;
const double vx = 75.97137;
const double cannotJumpTime = 0.8;

//表示起跳t秒后y的偏移量
inline double xy(double t)
{
    return 196.163265 * t * t - 387.28571 * t;
}

inline void readImg()
{
    GdiplusStartupInput gdiplusstartupinput;
    ULONG_PTR gdiplustoken;
    GdiplusStartup(&gdiplustoken, &gdiplusstartupinput, NULL);

    wstring infilename(L"temp.bmp");

    Bitmap* bmp = new Bitmap(infilename.c_str());
    height = bmp->GetHeight();
    width = bmp->GetWidth();
    //cout << "width " << width << ", height " << height << endl;

    Color color;
    int x = 0, y = 42;
    int cut = 0;
    wall.clear();
    while (y < height) {
        for (; y < height; y++) {
            if (x >= width) {
                x = 0;
                y++;
            }
            bool over = false;
            for (; x < width; x++) {
                cut++;
                bmp->GetPixel(x, y, &color);
                //cout << (int)color.GetRed() << (int)color.GetGreen() << (int)color.GetBlue() << endl;
                if (isblack((int)color.GetRed(), (int)color.GetGreen(), (int)color.GetBlue())) {
                    over = true;
                    break;
                }
            }
            if (over) {
                break;
            }
            //cout << y << endl;
        }
        bool isMan = false;
        int x2 = x, y2 = y;
        int igrone = 2;
        int it = 2;
        int minlarge = 800;
        for (; x2 < width; x2++) {
            bmp->GetPixel(x2, y, &color);
            if (isred((int)color.GetRed(), (int)color.GetGreen(), (int)color.GetBlue())) {
                isMan = true;
                continue;
            }
            if (!isblack((int)color.GetRed(), (int)color.GetGreen(), (int)color.GetBlue())) {
                igrone--;
                if (igrone <= 0)
                    break;
            } else {
                igrone = it;
            }
        }
        for (; y2 < height; y2++) {
            bmp->GetPixel(x, y2, &color);
            if (isred((int)color.GetRed(), (int)color.GetGreen(), (int)color.GetBlue())) {
                isMan = true;
                continue;
            }
            if (!isblack((int)color.GetRed(), (int)color.GetGreen(), (int)color.GetBlue())) {
                igrone--;
                if (igrone <= 0)
                    break;
            } else {
                igrone = it;
            }
        }
        if ((x2 - x) * (y2 - y) >= minlarge)
            if (isMan) {
                man = block { (double)x, (double)y, (double)x2, (double)y2 };
            } else {
                bool isVis = false;
                block bl = block { (double)x, (double)y, (double)x2, (double)y2 };
                for (int i = 0; i < wall.size(); i++) {
                    if (bl == wall[i]) {
                        isVis = true;
                    }
                }
                if (bl == man) {
                    isVis = true;
                }
                if (!isVis)
                    wall.push_back(bl);
            }
        x = x2 + 1;
    }
    //cout << cut << endl;
    delete bmp;
    GdiplusShutdown(gdiplustoken);
}

inline double getWallDis(block pos)
{
    vector<block>::iterator it = wall.begin();
    double mindis = 0x3f3f3f3f;
    for (; it != wall.end(); ++it) {
        mindis = min(mindis, (*it).distance(pos));
    }
    return mindis;
}

//做一次跳跃经t秒后的位置
double dojump_mindis = 0;
inline block dojump(block pos, bool isLeft, double t, bool ignoreLimitInUp = false)
{
    double w = pos.x2 - pos.x1, h = pos.y2 - pos.y1;
    double x, y;
    dojump_mindis = 0x3f3f3f3f;
    //以一定频率检测碰撞
    for (double tt = 0; tt <= t; tt += 0.05) {
        if (tt > cannotJumpTime) {
            ignoreLimitInUp = false;
        }
        x = pos.x1, y = pos.y1;
        if (isLeft) {
            x -= vx * tt;
        } else {
            x += vx * tt;
        }
        y += xy(tt);
        if (y > height - h && (!ignoreLimitInUp)) {
            dojump_mindis = 0;
        }
        if (x < 0 && (!ignoreLimitInUp)) {
            x = 0;
        }
        if (x > width - 30 && (!ignoreLimitInUp)) {
            x = width - 30;
        }
        dojump_mindis = min(dojump_mindis, getWallDis(block(x, y, x + w, y + h)));
    }
    return block(x, y, x + w, y + h);
}

double dfs_delay;
bool dfs_isLeft;
//在起跳之后调用，左跳负，右跳正
//返回值为最从此处路径走的最大和墙距离
double dfs(int depth, block pos, bool lastIsLeft, bool ignoreLimit = false)
{
    if (depth <= 0) {
        return 0x3f3f3f3f;
    }
    double maxdis = 0;
    double w = pos.x2 - pos.x1, h = pos.y2 - pos.y1;
    //左跳
    for (double s = cannotJumpTime; s <= cannotJumpTime * 2; s += 0.05) {
        block next = dojump(pos, true, s, ignoreLimit);
        if (dojump_mindis <= 0) {
            break;
        }
        double this_mindis = dojump_mindis + 2 * (pos.y1 - next.y1);
        double dis = dfs(depth - 1, next, true);
        if (maxdis < min(this_mindis, dis)) {
            maxdis = min(this_mindis, dis);
            dfs_delay = s;
            dfs_isLeft = true;
        }
    }
    //right
    for (double s = cannotJumpTime; s <= cannotJumpTime * 2; s += 0.05) {
        block next = dojump(pos, false, s, ignoreLimit);
        if (dojump_mindis <= 0) {
            break;
        }
        double this_mindis = dojump_mindis + 2 * (pos.y1 - next.y1);
        double dis = dfs(depth - 1, next, false);
        if (maxdis < min(this_mindis, dis)) {
            maxdis = min(this_mindis, dis);
            dfs_delay = s;
            dfs_isLeft = false;
        }
    }
    return maxdis;
}

//x数值表示距上次起跳经过的秒数，符号左跳负，右跳正(x)
//返回的值为距上次起跳应延迟跳的秒数+1，符号左跳负，右跳正
double exec(double x)
{
    //读图
    readImg();
    for (int i = 0; i < wall.size(); i++) {
        cout << wall[i].x1 << " " << wall[i].y1 << " " << wall[i].x2 << " " << wall[i].y2 << endl;
    }
    //根据给出的x算出上次人在当前图的位置
    double maxs = 0;
    bool lastIsLeft = (x < 0);
    x = abs(x);
    cout << "man:" << man.x1 << " " << man.y1 << endl;
    man.y1 -= xy(x), man.y2 -= xy(x);
    if (lastIsLeft)
        man.x1 += vx * x, man.x2 += vx * x;
    else
        man.x1 -= vx * x, man.x2 -= vx * x;
    cout << "man:" << man.x1 << " " << man.y1 << endl;
    double ans = 0;
    double maxdis = 0;
    /*if (LastIsLeft) {
        for (double t = cannotJumpTime; t < cannotJumpTime * 2; t += 0.05) {
            block next = dojump(man, true, t, true);
            if (dojump_mindis <= 0) {
                break;
            }
            double ret = dfs(3, next, true);
            if (min(dojump_mindis, ret) > maxdis) {
                maxdis = min(dojump_mindis, ret);
                ans = t;
                isLeft = true;
            }
        }
    } else {
        for (double t = cannotJumpTime; t < cannotJumpTime * 2; t += 0.05) {
            block next = dojump(man, false, t, true);
            if (dojump_mindis <= 0) {
                break;
            }
            double ret = dfs(3, next, false);
            if (min(dojump_mindis, ret) > maxdis) {
                maxdis = min(dojump_mindis, ret);
                ans = t;
                isLeft = false;
            }
        }
    }*/
    bool isLeft;
    dfs(3, man, lastIsLeft, true);
    ans = dfs_delay;
    isLeft = dfs_isLeft;
    cout << ans << endl;
    cout << isLeft << endl;
    ans += 1;
    return isLeft ? -ans : ans;
}

void test()
{
    readImg();
    cout << "man:" << man.x1 << " " << man.y1 << " ";
}

PYBIND11_MODULE(somecode, m)
{
    m.def("exec", &exec);
    m.def("test", &test);
}

/*
<%
cfg['compiler_args'] = ['/O2']
cfg['include_dirs'] = ['C:\\Users\\4627488\\Code\\jump\\pybind11\\include\\', 'C:\\Users\\4627488\\AppData\\Local\\Programs\\Python\\Python39\\include']
setup_pybind11(cfg)
%>
*/
