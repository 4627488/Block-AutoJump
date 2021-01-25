# Block-AutoJump
对于微信小游戏跃动方块的辅助

## 编译 & 运行
按py文件的import下好模块

下载pybind11的include，把cpp文件下面pybind的include和python的include路径改成自己的

打开微信电脑版跃动方块，开游戏先死一次把提示略过，然后放在最前端

运行main.py，cppimport会编译cpp源代码（需要vs2015以上环境），然后就会自己跳，把鼠标移出窗口就会结束程序

## 别的什么
它使用bfs来找路径，而预测路径似乎在不同的机器上有不同的表现，所以可以自行更改cpp文件里的vx,vy,g常量，使用test.py可以测出方块位置，再把数据分析一下就可以得出了
