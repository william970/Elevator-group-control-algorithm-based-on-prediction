#pragma once
#include"config.h"
#include"greedy.h"
#include"PassageManage.h"
#include"Elevator.h"
//电梯管理模块




class ElevatorManage
{
private:
	ElevatorManage() {}  //私有的构造函数
public:
	//电梯数量
	unsigned char m_eleNum;
	//楼高
	unsigned int floorHight;
	//电梯实例
	vector<Elevator> vec_elevator;
	//表示非目的地派梯是否已分配电梯
	bool outer_call_manage[FLOOR_COUNT + 1][2] = { 0 };//<楼层号,是否有外招(0表示上,1表示下)>只针对非目的地派梯
													   //电梯参数
	unsigned int inputParam[4][IN_PARAM_NUM] = { 0 };
	//派梯输出参数
	unsigned int outputParam[4][OUT_PARAM_NUM] = { 0 };
	//派梯内外招信号
	unsigned char innerCall[4][INNER_CALL_SIZE] = { 0 };
	unsigned char outerCall[4][2][OUTER_CALL_SIZE] = { 0 };
	
	//电梯盲层配置
	unsigned char invalidFloor[4][MAX_FLOOR] = { 0 };
	//权衡候梯时间和乘梯时间的参数,默认为8
	unsigned int adjustParam = 8;
	//非VIP模式
	unsigned char vip_mode = 0;
	//排梯乘客数量
	unsigned char passengerNum = 1;
	//指定时间段 grpInParam[n][CAN_NORMAL_ASSIGN]=1的电梯可响应普通外招
	AssignSetting assignSetting = { 0 };
	//时间段外,都可以响应普通外招，所以设置为0
	unsigned char assignSize = 0;

	//模式监控参数
	unsigned char triggerMode = 1; //1表示触发，0表示不触发
	ManualSetting manualSettings = { 0 };
	unsigned char manualSize = 0;
	AutoSetting autoSetting = { 0 };

	GrpOutParam grpOutParam = { 0 };

	

	//记忆模块<楼层号，记忆序号>
	//Record records_Yesterday[MAX_FLOOR][MAX_RECORD_SIZE] = {0};
	//int records_yesterday_count[MAX_FLOOR] = { 0 };
	//今天的记录
	//Record records[MAX_FLOOR][MAX_RECORD_SIZE] = { 0 };
	//int records_count[MAX_FLOOR] = { 0 };


	static ElevatorManage * GetInstance();

	//每天的凌晨记录把今天的记录保存到昨晚，同时清空今天的记录
	//bool ResetRecord();

	//bool AddRecord(Record temp_recode, int currentfloor);

	bool Init(unsigned char eleNum);
	
	void SetInnerOuterCall();

	
	

	void SetEnv();

	int dispatch(Passage * p,bool aim);

	
	
	int dispatchPassage(Passage * p);
	
	int ModeMonitor();
	bool Update();

	//记忆信息
	FloorProbability(*floorProbability)[MAX_RECORD_SIZE];
	unsigned int(*probabilitySize);
	void AddElevatorRecode(int peoplecount, int curF, int dir, int e_id);
	bool FindBestRecord(FloorProbability & bestRecord, unsigned char fromFloor, double currentTime);
	//void SetRecode();
};