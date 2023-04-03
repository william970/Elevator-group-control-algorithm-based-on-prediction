//乘客
#pragma once
#include"config.h"
#include"greedy.h"

class Passage {
public:
	Passage() {}
	
	Passage(int PassageID,int WorkFloor, int FromFloor, int AimFloor, double ComeTime);

	int iPassageID;
	int iWorkFloor;
	int iFromFloor;
	int iAimFloor;
	//乘客方向
	int dir;


	double iComeTime;
	double iLandTime;//登上电梯的时间
	double iOffTime;//下电梯的时间
	int eleno; //指派的电梯号

	unsigned char is_vip;//是否是vip
	unsigned char is_disabled;//是否残疾
	
};





//乘客管理模块
class PassageManage
{
private:
	PassageManage() {}  //私有的构造函数
public:
	static PassageManage* GetInstance()
	{
		static PassageManage Instance;
		return &Instance;
	}

	//应载重无法登上电梯的乘客需要再一次派梯

	map<int, set<Passage*> > map_wait_passage;//<楼层，乘客指针>已经被分配，但是还没有接上的乘客

											  //添加已经到达的乘客
	
	bool addWaitPassage(Passage * p);

	bool delWaitPassageIn(int currentfloor);

};