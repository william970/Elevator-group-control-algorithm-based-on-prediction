#pragma once
#include"config.h"
#include"greedy.h"
#include"PassageManage.h"

//记录模块
struct Record
{
	//开门时间
	double opendoorTime;
	//电梯编号
	int eleNo;
	//开门楼层
	int opendoorFloor;
	//上乘客人数
	int inpassgeCount;
	//对应的乘客指针,里面记录对应的概率信息
	Passage* p[CAPACITY];
};

//电梯模拟器
class Elevator {
public:
	Elevator() {}
	Elevator(int id) {
		iEleID = id;
		dir = 0;
		currentpos = 1;
		currentheight = 0;
		state = IDLE;
		_t_controller = 0;
	}
	int iEleID;//电梯编号
	unsigned int dir;//方向
	int currentpos;//位置
	double velocity = 1.5;//电梯运行速度
	double currentheight;//当前高度

	//int peopleNeedOff[FLOOR_COUNT + 1] = { 0 };	//正对目的地派梯需要在该层下的人数，其余下梯的人是非目的地派梯上的电梯

	map<int, vector<Passage*> >  inner_people;//当前在电梯上的人，<目的地，内招的乘客>
	map<int, vector<Passage*> > outer_people;//当前在电梯外的人，<起始地，外招的乘客>,只针对目的地派梯
	bool outer_call[FLOOR_COUNT + 1][2] = { 0 };//<楼层号,是否有外招(0表示上,1表示下)>针对非目的地派梯
	
	
	
	EleState state;//状态
	
	int PassageCount = 0;//当前的乘客数量

	double _t_controller = 0;//动作持续时间
	double opendoor = 3.0;//开门的持续时间
	double closedoor = 3.0;//关门的持续时间
	double turn = 1.0;//转向的持续时间
	double passagetakeoff = 3.0;//下乘客的持续时间
	double passagetakein = 3.0;//上乘客的持续时间

	//记录当前是否需要上乘客或者下乘客
	bool m_needoffpassage = false;
	bool m_needonpassage = false;
	
	
	

	//map<unsigned char, int> getouteraim(unsigned char from);

	unsigned int getinitdir();

	bool hasoutcall();

	bool hasinncall();

	bool taskIn(Passage * p,bool aim);

	bool hasSameDir();

	bool passageIn();


	bool passageOFF();

	bool needturn();

	bool needopendoor();


	void update(unsigned char dest);

};
