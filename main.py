import time
import win32gui
import win32ui
import win32con
import win32api
from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
import cv2
import cppimport
import pyautogui

zoom = 1  # 显示器放大倍数
img = None


if __name__ == "__main__":
    somecode = cppimport.imp("somecode")
    hwnd = win32gui.FindWindow(None, '跃动方块')  # 窗口的编号，0号表示当前活跃窗口
    # 根据窗口句柄获取窗口的设备上下文DC（Divice Context）
    hwndDC = win32gui.GetWindowDC(hwnd)
    # 根据窗口的DC获取mfcDC
    mfcDC = win32ui.CreateDCFromHandle(hwndDC)
    # mfcDC创建可兼容的DC
    saveDC = mfcDC.CreateCompatibleDC()
    # 创建bigmap准备保存图片
    saveBitMap = win32ui.CreateBitmap()
    # 获取监控器信息
    left, top, right, bottom = win32gui.GetWindowRect(hwnd)
    w = int((right-left) * zoom)
    h = int((bottom-top) * zoom)
    # print("得到窗口大小:", w, h)  # 图片大小
    # 为bitmap开辟空间
    saveBitMap.CreateCompatibleBitmap(mfcDC, w, h)
    jumpTime = 100
    # while True:
    # pyautogui.click(left+334, top+626)
    last = time.time()
    isleft = False
    pyautogui.click(left+334, top+626)
    time.sleep(0.85)
    while True:
        # 截图
        saveDC.SelectObject(saveBitMap)
        saveDC.BitBlt((0, 0), (w, h), mfcDC, (0, 0), win32con.SRCCOPY)
        saveBitMap.SaveBitmapFile(saveDC, "temp.bmp")
        # 求解
        if isleft:
            ret = somecode.exec(-(time.time()-last))
        else:
            ret = somecode.exec((time.time()-last))
        this = time.time()
        delay = last+ret-this
        if delay > 0:
            time.sleep(delay)
        if ret > 0:
            # right
            isleft = False
            pyautogui.click(left+334, top+626)
        else:
            # left
            isleft = True
            pyautogui.click(left+134, top+626)
        time.sleep(0.85)
        last = time.time()
        x, y = pyautogui.position()
        if x < left or x > right or y < top or y > bottom:
            print('光标不在窗口，退出')
            exit(0)
