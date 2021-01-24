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
    double vy, vx;
    friend bool operator==(block a, block b)
    {
        return a.y2 == b.y2 && a.x2 == b.x2;
    }
    double distance(block a)
    {
        if (a.x1 <= x2 && x1 <= a.x2 && a.y2 >= y1 && y2 >= a.y1) {
            return 0;
        }
        /*if (a.x1 <= x2 && x1 <= a.x2) {
            if (a.y2 < y1) {
                return y1 - a.y2;
            } else if (y2 < a.y1) {
                return a.y1 - y2;
            } else {
                return 0;
            }
        } else if (a.y1 <= y2 && y1 <= a.y2) {
            if (a.x2 < x1) {  
                return x1 - a.x2;
            } else if (x2 < a.x1) {
                return a.x1 - x2;
            } else {
                return 0;
            }
        } else {
            int lenx, leny;
            if (a.x1 < x1 && a.y1 < y1) {
                lenx = a.x2 - x1;
                lenx = a.y2 - x1;
            }
            return sqrt(lenx * lenx + leny * leny);
        }*/
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
const double vx = 75.97137, vy = -391.86, g = 466.5; //pixel/s pixel/s pixel/s^2 以向下（y增大）为正方向
const double startTime = 0.85; //s
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

inline block passtime(block pos, double s)
{
    block ans;
    //x=v0t+gt^2/2
    for (double ss = 0; ss <= s; ss += 0.05) {
        double xy = pos.vy * s + g * s * s / 2;
        ans = block {
            pos.x1 + s * pos.vx,
            pos.y1 + xy,
            pos.x2 + s * pos.vx,
            pos.y2 + xy,
            pos.vy + s * g,
            pos.vx
        };
        if (pos.x2 > width || pos.x1 < 0) {
            return block { -1 };
        }
        if (pos.y2 > height || pos.y1 < 0) {
            return block { -1 };
        }
        for (int i = 0; i < wall.size(); i++)
            if (ans.distance(wall[i]) <= 0) {
                //不能到达
                cout << "Not" << endl;
                cout << pos.x1 << " " << pos.y1 << " " << pos.x2 << " " << pos.y2 << endl;
                return block { -1 };
            }
    }
    return ans;
}

double delay;
//在起跳之后调用，左跳负，右跳正
double dfs(int depth, block pos)
{
    if (depth == 0) {
        return 0;
    }
    if (pos.x1 == -1) {
        return 0;
    }
    block next;
    double ans = 0;
    double maxs = 0;
    for (double s = startTime; s <= startTime * 2; s += 0.1) {

        next = passtime(pos, s);
        next.vx = vx;
        next.vy = vy;
        double ret = dfs(depth - 1, next);
        if (ans < ret) {
            ans = ret;
            maxs = s;
        }
    }
    for (double s = startTime; s < startTime * 2; s += 0.1) {
        next = passtime(pos, s);
        next.vx = -vx;
        next.vy = vy;
        double ret = dfs(depth - 1, next);
        if (ans < ret) {
            ans = ret;
            maxs = -s;
        }
    }
    delay = maxs;
    return ans;
}

//x数值表示距上次起跳经过的秒数，符号左跳负，右跳正(x)
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
    double xy = vy * x + g * x * x / 2;
    man = {
        man.x1 + (lastIsLeft ? vx * x : -vx * x),
        man.y1 - xy,
        man.x2 - vx * x,
        man.y2 - xy,
        vy - g * x,
        lastIsLeft ? -vx : vx,
    };
    cout << "man:" << man.x1 << " " << man.y1 << endl;
    dfs(3, man);
    cout << delay << endl;
    return delay;
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
