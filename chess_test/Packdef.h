#pragma once

#define GRID_NUM    15 //每一行(列)的棋盘交点数
#define GRID_COUNT  225//棋盘上交点总数
#define BLACK        0 //黑棋用0表示
#define WHITE        1 //白棋用1表示
#define NOSTONE     '+'  //没有棋子
//这组宏定义了用以代表几种棋型的数字
#define STWO        1  //眠二
#define STHREE        2  //眠三
#define SFOUR        3  //冲四
#define TWO        4  //活二
#define THREE        5  //活三
#define FOUR        6  //活四
#define FIVE        7  //五连
#define NOTYPE        11 //未定义
#define ANALSISED   255//已分析过的
#define TOBEANALSIS 0  //已分析过的
//这个宏用以检查某一坐标是否是棋盘上的有效落子点
#define IsValidPos(x,y) ((x>=0 && x<GRID_NUM) && (y>=0 && y<GRID_NUM)