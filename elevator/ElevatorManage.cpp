#include "ElevatorManage.h"
#include "Elevator.h"

ElevatorManage* ElevatorManage::GetInstance()
{
	static ElevatorManage Instance;
	return &Instance;
}
/*
//bool ElevatorManage::ResetRecord()
//{
//	memcpy(records_Yesterday, records,sizeof(records));
//	memset(records, 0, sizeof(records));
//
//	memcpy(records_yesterday_count, records_count, sizeof(records_count));
//	memset(records_count, 0, sizeof(records_count));
//	return true;
//}

//bool ElevatorManage::AddRecord(Record temp_recode,int currentfloor)
//{
//	//加入到记忆模块
//	records[currentfloor][records_count[currentfloor]% MAX_RECORD_SIZE] = temp_recode;
//	records_count[currentfloor]++;
//	return true;
//}

////设置记忆信息,只针对非目的地派梯
//void ElevatorManage::SetRecode() {
//	//设置电梯的内外招
//	memset(recodeProbability, 0, sizeof(recodeProbability));
//	memset(probabilitySize, 0, sizeof(probabilitySize));
//	for (int f = 0; f < MAX_FLOOR; f++) {
//		for (int r = 0; r < MAX_RECORD_SIZE; r++) {
//			if (records_Yesterday[f][r].opendoorTime == 0) {
//				break;
//			}
//			recodeProbability[f][r].recodetime = records_Yesterday[f][r].opendoorTime;
//			recodeProbability[f][r].fromfloor = records_Yesterday[f][r].opendoorFloor;
//			recodeProbability[f][r].allProbability = records_Yesterday[f][r].inpassgeCount;
//			for (int i = 0; i < records_Yesterday[f][r].inpassgeCount; i++) {
//				for (int j = 0; j < MAX_FLOOR; j++) {
//					recodeProbability[f][r].offProbability[j] += 1.0 * records_Yesterday[f][r].p[i]->offProbability[j] / records_Yesterday[f][r].p[i]->allProbability;
//				}
//			}
//			probabilitySize[f]++;
//		}
//	}
//
//	return;
//}
*/

bool ElevatorManage::Init(unsigned char eleNum) {
	this->m_eleNum = eleNum;
	this->floorHight = FLOOR_HIGHT;
	for (int i = 0; i < m_eleNum; i++) {
		Elevator ele(i);
		vec_elevator.push_back(ele);
	}
	for (int i = 0; i < m_eleNum; i++) {
		inputParam[i][ID_INDEX] = i;
		inputParam[i][VELOCITY_INDEX] = 1500;//1.5m/s
		inputParam[i][ACCELERATION_INDEX] = 650;//加速度 0.6m/s
		inputParam[i][DECELERATION_INDEX] = 650;//加速度
		inputParam[i][OPEN_INDEX] = 30;//开门时间 3秒
		inputParam[i][CLOSE_INDEX] = 30;//关门时间 3秒
		inputParam[i][DOOR_TIME_INDEX] = 30;//乘客上下梯时间 3秒
		inputParam[i][OUT_OF_GROUP_CONTROL_INDEX] = 0;//可群控 TODO
		inputParam[i][CAPACITY_INDEX] = CAPACITY;	//超重人数
		inputParam[i][ACCELERATE_TIME_INDEX] = 13;//1.3秒
		inputParam[i][DECELERATE_TIME_INDEX] = 13;//1.3秒
		inputParam[i][FLOOR_TIME_INDEX] = 15;	//1.5秒
		inputParam[i][FLOOR_NUM_INDEX] = FLOOR_COUNT;//楼层数量
		inputParam[i][SAVING_CONTROL_INDEX] = 0;
		inputParam[i][IS_DISABLED_INDEX] = 0;
		inputParam[i][IS_MANUAL_ASSIGNING_INDEX] = 0;
		inputParam[i][IS_AUTO_ASSIGNING_INDEX] = 0;
		inputParam[i][IS_VIP_MODE_INDEX] = 0;
		inputParam[i][DO_INDEX] = 0;
		inputParam[i][DISABLED_NUM_INDEX] = 0;
		inputParam[i][DECELERATE_DISTANCE] = 0;
		inputParam[i][CAN_NORMAL_ASSIGN] = 0;
		//这几个参数是可变的
		inputParam[i][IS_FULL_INDEX] = 0;
		inputParam[i][DIRECTION_INDEX] = 0;//方向//后面去改
		inputParam[i][POS_INDEX] = 0;//位子//后面去改
		inputParam[i][IS_MOVING_INDEX] = 1;
	}
	::memset(innerCall, INVALID_NUM, sizeof(innerCall));
	::memset(outerCall, INVALID_NUM, sizeof(outerCall));
	
	autoSetting.upPeakFloor = 1;
	autoSetting.upPeakCount = 10;
	autoSetting.upPeakDelayTime = 500;
	autoSetting.interPeakCount = 10;
	autoSetting.interPeakDelayTime = 500;
	autoSetting.savingCount = 1;
	autoSetting.savingBufferLen = 100;
	autoSetting.interPeakTime = (unsigned int *)malloc(sizeof(unsigned int));//层间模式计数器
	::memset(autoSetting.interPeakTime, 0, sizeof(unsigned int));//自动调度之后的目的地
	autoSetting.upPeakTime = (unsigned int *)malloc(sizeof(unsigned int));//上行模式计数器
	::memset(autoSetting.upPeakTime, 0, sizeof(unsigned int));//自动调度之后的目的地
	autoSetting.savingBuffer = (unsigned int *)malloc(sizeof(unsigned int));//节能模式计数器
	::memset(autoSetting.savingBuffer, 0, sizeof(unsigned int));//自动调度之后的目的地
	autoSetting.savingControl = (unsigned char *)malloc(sizeof(unsigned char) * eleNum);
	::memset(autoSetting.savingControl, 0, sizeof(unsigned char) * eleNum);//全部没有脱离群控
	autoSetting.upPeakRecordDelayTime = 10;//上行高峰记录的延迟时间
																		   //输出参数
	grpOutParam.elevatorMode = NONE_MODE;
	grpOutParam.dest = (unsigned char *)malloc(sizeof(unsigned char) * eleNum);
	memset(grpOutParam.dest, INVALID_NUM, sizeof(unsigned char) * eleNum);//自动调度之后的目的地
	grpOutParam.savingControl = (unsigned char *)malloc(sizeof(unsigned char) * eleNum);
	memset(grpOutParam.savingControl, 0, sizeof(unsigned char) * eleNum);//是否脱离群控
	memset(outer_call_manage, 0, sizeof(outer_call_manage));

	//申请记忆信息
	//MallocRecodeMemory(floorProbability, probabilitySize);
	return true;
}



//设置内招外招
void ElevatorManage::SetInnerOuterCall() {
	//初始化
	memset(innerCall, INVALID_NUM, sizeof(unsigned char)* INNER_CALL_SIZE * 4);
	memset(outerCall, INVALID_NUM, sizeof(unsigned char)* OUTER_CALL_SIZE * 4 * 2);
	for (int e = 0; e < vec_elevator.size(); e++) {
		int c = 0;
		for (auto& inner : vec_elevator[e].inner_people) {
			if (inner.second.size() > 0) {
				innerCall[e][c] = inner.first;
			}
			c++;
			if (c >INNER_CALL_SIZE) {
				break;
			}
		}
	}
	for (int e = 0; e < vec_elevator.size(); e++) {
		int c = 0;
		if (vec_elevator[e].outer_people.size() > 0) {
			for (auto& outer : vec_elevator[e].outer_people) {
				//<目的地,多少次>
				if (outer.second.size() > 0) {
					for (auto passage : outer.second) {
						outerCall[e][0][c] = passage->iFromFloor;
						outerCall[e][1][c] = passage->iAimFloor;
						c++;
						if (c >OUTER_CALL_SIZE) {
							break;
						}
					}
				}
			}
		}
		
		
		for (int f = 0; f < FLOOR_COUNT + 1;f++) {
			//<目的地,多少次>
			if (vec_elevator[e].outer_call[f][0]) {
				outerCall[e][0][c] = f;
				outerCall[e][1][c] = UP_NUM;
				c++;
				if (c >OUTER_CALL_SIZE) {
					break;
				}
			}
			if (vec_elevator[e].outer_call[f][1]) {
				outerCall[e][0][c] = f;
				outerCall[e][1][c] = DOWN_NUM;
				c++;
				if (c >OUTER_CALL_SIZE) {
					break;
				}
			}
		}
	}
	return;
}




//设置电梯环境
void ElevatorManage::SetEnv() {
	//设置电梯的内外招
	SetInnerOuterCall();
	//设置电梯当前位置，电梯当前方向，电梯是否已满
	for (int e = 0; e < vec_elevator.size(); e++) {
		inputParam[e][POS_INDEX] = vec_elevator[e].currentpos;
		inputParam[e][DIRECTION_INDEX] = vec_elevator[e].dir;
		if (vec_elevator[e].PassageCount >= inputParam[e][CAPACITY_INDEX]) {
			inputParam[e][IS_FULL_INDEX] = 1;
		}
		else {
			inputParam[e][IS_FULL_INDEX] = 0;
		}
		//关门之后表示已经移动了，无法继续上乘客
		if (vec_elevator[e].state == CLOSE_DOOR || vec_elevator[e].state == TURN || vec_elevator[e].state == MOVE) {
			inputParam[e][IS_MOVING_INDEX] = 1;
		}
		else {
			inputParam[e][IS_MOVING_INDEX] = 0;
		}
		//模拟电梯当前重量
		//inputParam[e][WEIGHT] = vec_elevator[e].PassageCount * WEIGHT_PER_PASSAGE;
	}
	return;
}

//派梯接口
int ElevatorManage::dispatch(Passage *p,bool aim) {
	unsigned char fromFloor = p->iFromFloor;
	unsigned int is_vip = p->is_vip;
	unsigned int is_disabled = p->is_disabled;
	unsigned char aimFloor;
	if (!aim) {
		if (p->dir == 1) {
			aimFloor = UP_NUM;
		}
		else if (p->dir == 2) {
			aimFloor = DOWN_NUM;
		}
	}
	else {
		aimFloor = p->iAimFloor;
	}
	
	//设置电梯环境参数
	SetEnv();
	int eleno = compassInterface(fromFloor, aimFloor, is_vip, is_disabled, m_eleNum, floorHight, inputParam, outputParam, innerCall, outerCall, invalidFloor, adjustParam, vip_mode, passengerNum, &assignSetting, assignSize, (unsigned int)currentTime);
	//int eleno = 0;
	return eleno;
}


bool ElevatorManage::FindBestRecord(FloorProbability& bestRecord, unsigned char fromFloor, double currentTime) {
	//ADD by YJH
	//判断是否有无找到历史信息
	bool find = false;

	if (probabilitySize == NULL) {
		return find;
	}
	//找到最近的记录信息,相隔时间不超过5分钟
	double MinDiffTime = 300;
	if (probabilitySize[fromFloor] > 20) {
		for (int r = 0; r < min(int(probabilitySize[fromFloor]), MAX_RECORD_SIZE); r++) {
			//不是当前天的才有意义
			if (currentday == floorProbability[fromFloor][r].recodeday) {
				continue;
			}
			double recodetime = floorProbability[fromFloor][r].recodetime;
			double diff = fabs(recodetime - currentTime);
			if (MinDiffTime>diff) {
				find = true;
				MinDiffTime = diff;
				bestRecord = floorProbability[fromFloor][r];
			}
		}
	}
	return find;
}


//派梯接口
int ElevatorManage::dispatchPassage(Passage *p) {
	int eleno = -1;
	unsigned char fromFloor = p->iFromFloor;
	unsigned char aimFloor = p->iAimFloor;

	//这边是非目的地派梯
	if (fromFloor != 0) {
		unsigned char dir = p->dir;
		//如果还没有分配电梯的话
		if (dir == 1) {
			//如果已经该层已经有人分配了电梯，则不做处理
			if (!outer_call_manage[fromFloor][0]) {
				//这边如果当前是上行高峰的话去生成一系列的预测数据，循环派梯
				if ((int)currentMode == UP_PEAK_MODE) {
					FloorProbability bestRecord = {0};
					bool find = FindBestRecord(bestRecord, fromFloor ,currentTime);
					//bool find = false;
					if (find) {
						//cout << "find!!" << endl;
						for (int f = fromFloor + 1; f < MAX_FLOOR; f++) {
							//如果历史数据该层有人要去,变成目的地派梯
							if (bestRecord.offProbability[f]) {
								//模拟客流，预测生成虚拟的乘客，id为-1表示不是真实的
								Passage passage(-1, f, fromFloor, f, currentTime);
								//转换为目的地派梯
								eleno = dispatch(&passage, true);
								//给电梯加入外招
								vec_elevator[eleno].outer_call[fromFloor][0] = true;
							}
						}
					}
					else {
						//printf("s:%d", (int)currentMode == UP_PEAK_MODE);
						eleno = dispatch(p, false);
						//给电梯加入外招
						vec_elevator[eleno].outer_call[fromFloor][0] = true;
					}
				}
				else {
					
					eleno = dispatch(p, false);
					//给电梯加入外招
					vec_elevator[eleno].outer_call[fromFloor][0] = true;
				}
				outer_call_manage[fromFloor][0] = true;
				
				
			}
		}
		else if (dir == 2) {
			if (!outer_call_manage[fromFloor][1]) {
				eleno = dispatch(p, false);
				outer_call_manage[fromFloor][1] = true;
				//给电梯加入外招
				vec_elevator[eleno].outer_call[fromFloor][1] = true;
			}
		}
	}
	//这边是目的地派梯
	else {
		eleno = dispatch(p,true);
		//cout << "ele:" << eleno << endl;
		//给电梯加入外招
		vec_elevator[eleno].outer_people[p->iFromFloor].push_back(p);
	}
	return eleno;
}


//添加记忆信息
void ElevatorManage::AddElevatorRecode(int peoplecount, int curF, int dir, int e_id) {
	SetEnv();
	elevatorRecode(0, peoplecount*WEIGHT_PER_PASSAGE, currentday, currentTime, curF, dir, innerCall, outerCall, m_eleNum, invalidFloor, floorProbability, probabilitySize, e_id);
}

int ElevatorManage::ModeMonitor() {
	//设置电梯环境参数
	SetEnv();
	//模式监控
	elevatorModeMonitor(triggerMode, &manualSettings, manualSize, autoSetting, currentTime, innerCall, outerCall, inputParam, m_eleNum, &grpOutParam, invalidFloor);
	//模式转变

	if (currentMode != grpOutParam.elevatorMode) {
		currentMode = grpOutParam.elevatorMode;
		switch (currentMode)
		{
			case(UP_PEAK_MODE): {
				//如果是上行高峰的话。每过一段时间记录下所有的内招
				cout << " currentTime:" << currentTime << " 进入上行高峰" << endl;
				break;
			}
			case(DOWN_PEAK_MODE): {
				cout << " currentTime:" << currentTime << " 进入下行高峰" << endl;
				break;
			}
			case(INTER_PEAK_MODE): {
				cout << " currentTime:" << currentTime << " 进入层间高峰高峰" << endl;
				break;
			}
			case(SAVING_MODE): {
				cout << " currentTime:" << currentTime << " 进入节能模式" << endl;
				break;
			}
			case(NONE_MODE): {
				cout << " currentTime:" << currentTime << " 进入无高峰" << endl;
				break;
			}
		}
	}
	
	return true;
}

//更新电梯状态
bool ElevatorManage::Update() {
	for (int e = 0; e < vec_elevator.size(); e++) {
		vec_elevator[e].update(INVALID_NUM);
		//vec_elevator[e].update(grpOutParam.dest[e]);
	}
	return true;
}