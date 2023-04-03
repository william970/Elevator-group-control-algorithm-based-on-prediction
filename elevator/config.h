#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<random>
#include<string>
#include<time.h>
#include<algorithm>
#include<fstream>
#include<queue>
using namespace std;
//环境参数
#define CAPACITY 20		//载重量
#define FLOOR_COUNT 32	//楼层数量
#define ELE_COUNT 4		//电梯数量
#define FLOOR_HIGHT 3000 //	楼层高度
//模拟器参数
#define TIME_INTERVAL 0.1
#define DEBUG_ENO -1		//调式电梯编号


//电梯状态机
enum EleState {
	IDLE,//闲置
	STOP,//停止,暂时不用
	OPEN_DOOR,//开门
	OFF_PASSENAGE,//下乘客
	LAND_PASSENAGE,//上乘客
	CLOSE_DOOR,//关门
	MOVE,//停止
	TURN//转向
};

//全局变量
extern int currentday;
//当前时间
extern double currentTime;
//当前模式
extern unsigned char currentMode;
//目前登入上的乘客数量
extern int landpassage;
//目前已经到达的乘客数量
extern int offpassage;