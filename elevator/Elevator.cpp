#include "Elevator.h"
#include"ElevatorManage.h"
#include"PassageManage.h"
#include<assert.h>

//map<unsigned char, int> Elevator::getouteraim(unsigned char from) {
//	map<unsigned char, int> ret;
//	if (outer_people[from].size() > 0) {
//		for (int i = 0; i < outer_people[from].size(); i++) {
//			ret[outer_people[from][i]->iAimFloor]++;
//		}
//	}
//	return ret;
//}

unsigned int Elevator::getinitdir() {
	unsigned int dir = 1;
	//初始方向为距离最近的外招
	int mindiff = INT_MAX;
	//优先内招
	for (auto& inner : inner_people) {
		if (inner.second.size() > 0) {
			if (mindiff > abs(currentpos - inner.first)) {
				mindiff = abs(currentpos - inner.first);
				//获取初始方向
				if (currentpos <= inner.first) {
					dir = 1;
				}
				else {
					dir = 2;
				}
			}
		}
	}
	if (mindiff != INT_MAX) {
		return dir;
	}
	//遍历外招,找到最近的外招,针对非目的地派梯
	for (auto outer : outer_people) {
		if (outer.second.size() > 0) {
			if (mindiff > abs(currentpos - outer.first)) {
				mindiff = abs(currentpos - outer.first);
				//获取初始方向
				if (currentpos <= outer.first) {
					//cout << "???" << endl;
					dir = 1;
				}
				else {
					//cout << "!!!" << endl;
					dir = 2;
				}
			}
		}
	}
	//针对非目的地派梯
	for (int i = 0; i < FLOOR_COUNT + 1; i++) {
		//如果有内外招
		if (outer_call[i][0] || outer_call[i][1]) {
			if (mindiff > abs(currentpos - i)) {
				mindiff = abs(currentpos - i);
				//获取初始方向
				if (currentpos <= i) {
					//cout << "???" << endl;
					dir = 1;
				}
				else {
					//cout << "!!!" << endl;
					dir = 2;
				}
			}
		}
	}

	return dir;
}

bool Elevator::hasoutcall() {
	//针对目的地派梯
	if (outer_people.size() > 0) {
		for (auto outer : outer_people) {
			if (outer.second.size() > 0) {
				return true;
			}
		}
	}
	
	//针对非目的地派梯
	for (int i = 0; i < FLOOR_COUNT + 1; i++) {
		if (outer_call[i][0] || outer_call[i][1]) {
			return true;
		}
	}
	return false;
}

bool Elevator::hasinncall() {
	for (auto& inner : inner_people) {
		if (inner.second.size() > 0) {
			return true;
		}
	}
	return false;
}

//从PassageManage中删除这个等待的人,加入到电梯的内招中
bool Elevator::taskIn(Passage* p,bool aim) {
	//在内招中加入该人
	inner_people[p->iAimFloor].push_back(p);
	p->iLandTime = currentTime;
	p->eleno = iEleID;
	return true;
}

//外招和当前方向相同
bool Elevator::hasSameDir() {
	if (dir == 1) {
		if (outer_call[currentpos][0]) {
			return true;
		}
	}
	else if (dir == 2) {
		if (outer_call[currentpos][1]) {
			return true;
		}
	}
	return true;
}

//处理所有需要上的乘客
bool Elevator::passageIn() {
	map<int, vector<Passage*>>::iterator passageItr = outer_people.find(currentpos);
	//看看目的地派梯是否有需要上的乘客
	if (passageItr != outer_people.end()) {
		//把外招换成内招
		assert(currentpos == 1);
		//更新目的地派梯应下人数
		//memset(peopleNeedOff, 0, sizeof(peopleNeedOff));
		
		for (Passage* passage_ptr : passageItr->second) {
			//乘客的数量不能大于载重量
			if (PassageCount + 1 > CAPACITY) {
				//再分配一台电梯，放到另一台电梯的外招中
				ElevatorManage::GetInstance()->dispatchPassage(passage_ptr);
			}
			else {
				taskIn(passage_ptr,true);
				//计算目的地派梯应下人数
				//peopleNeedOff[passage_ptr->iAimFloor]++;
				//当前全局正在电梯上的乘客数量
				landpassage++;
				PassageCount++;
			}
		}


		outer_people.erase(passageItr);
		PassageManage::GetInstance()->delWaitPassageIn(currentpos);

		return true;
	}
	//针对非目的地派梯
	else {
		//如果运行方向和外招一致的话
		if (hasSameDir() || needturn()) {
			map<int, set<Passage*>>::iterator passageItr2 = PassageManage::GetInstance()->map_wait_passage.find(currentpos);
			if (passageItr2 != PassageManage::GetInstance()->map_wait_passage.end()) {
				//上
				if (dir == 1) {
					outer_call[currentpos][0] = false;
					ElevatorManage::GetInstance()->outer_call_manage[currentpos][0] = false;
				}
				//下
				else if (dir == 2) {
					outer_call[currentpos][1] = false;
					ElevatorManage::GetInstance()->outer_call_manage[currentpos][1] = false;
				}
				//上电梯的乘客数量
				int passageIncount = 0;
				
				for (Passage* passage_ptr : passageItr2->second) {
					//如果乘客方向和
					if (passage_ptr->dir == (unsigned char)dir) {
						if (PassageCount + 1 > CAPACITY) {
							//再分配一台电梯
							ElevatorManage::GetInstance()->dispatchPassage(passage_ptr);
							ElevatorManage::GetInstance()->outer_call_manage[currentpos][0] = false;
						}
						else {
							taskIn(passage_ptr, false);
							//当前全局正在电梯上的乘客数量
							landpassage++;
							//当前电梯的乘客数量
							PassageCount++;

							passageIncount++;
						}
					}
				}


				//从等待队列中删除
				PassageManage::GetInstance()->delWaitPassageIn(currentpos);

				//如果是上行高峰的话,记录电梯上行历史数据
				if (currentMode == UP_PEAK_MODE ) {
					//如果上电梯的人数不为0的话
					if (passageIncount != 0) {
						//记录模块
						ElevatorManage::GetInstance()->AddElevatorRecode(passageIncount, currentpos, dir, iEleID);
					}
				}

				return true;
			}
		}
		return false;
	}
	return false;
}




//处理所有需要下的乘客
bool Elevator::passageOFF() {
	map<int, vector<Passage*>>::iterator passageItr = inner_people.find(currentpos);
	//返回是否有需要下的乘客
	if (passageItr != inner_people.end()) {
		for (Passage* passage_ptr : passageItr->second) {
			//当前全局正在电梯上的乘客数量
			landpassage--;
			//全局已经下电梯的乘客数量
			offpassage++;
			//电梯上的乘客数量
			PassageCount--;
			passage_ptr->iOffTime = currentTime;
		}
	}
	passageItr = inner_people.find(currentpos);
	if (passageItr != inner_people.end()) {
		inner_people.erase(passageItr);
		return true;
	}
	return false;
}

//判断是否需要转向
bool Elevator::needturn() {
	//如果当前是向上
	if (dir == 1) {
		//如果还有乘客是需要往上的
		for (auto& vec_pepple : inner_people) {
			if (vec_pepple.first > currentpos) {
				return false;
			}
		}
		//针对目的地派梯
		for (auto vec_pepple : outer_people) {
			if (vec_pepple.first > currentpos) {
				return false;
			}
			else if (vec_pepple.first == currentpos) {
				for (auto people : vec_pepple.second) {
					if (people->iAimFloor > currentpos) {
						return false;
					}
				}
			}
			
		}
		//针对非目的地派梯，如果还有需要往上的
		for (int f = currentpos + 1; f < FLOOR_COUNT + 1; f++) {
			if (outer_call[f][0] || outer_call[f][1]) {
				return false;
			}
		}
		if (outer_call[currentpos][0]) {
			return false;
		}

		return true;
	}
	else if (dir == 2) {
		//如果还有乘客是需要往下的
		for (auto& vec_pepple : inner_people) {
			if (vec_pepple.first < currentpos) {
				return false;
			}
		}
		for (auto vec_pepple : outer_people) {
			if (vec_pepple.first < currentpos) {
				return false;
			}
			else if (vec_pepple.first == currentpos) {
				for (auto people : vec_pepple.second) {
					if (people->iAimFloor < currentpos) {
						return false;
					}
				}
			}
		}
		//针对非目的地派梯,如果还有需要往下的
		for (int f = 1; f < currentpos; f++) {
			if (outer_call[f][0] || outer_call[f][1]) {
				return false;
			}
		}
		if (outer_call[currentpos][1]) {
			return false;
		}
		return true;
	}
	return false;
}


//判断是否需要开门
bool Elevator::needopendoor() {
	//针对内招信号
	//如果有乘客到达目的地
	if (inner_people.find(currentpos) != inner_people.end()) {
		m_needoffpassage = true;
	}
	
	//针对外招信号
	//如果需要转向,则无论外招的方向，只要有乘客就需要开门
	if (needturn()) {
		//针对目的地派梯
		if (outer_people.find(currentpos) != outer_people.end()) {
			m_needonpassage = true;
		}
		//针对非目的地派梯
		if (outer_call[currentpos][0] || outer_call[currentpos][1]) {
			m_needonpassage = true;;
		}
	}

	//如果当前是向上
	if (dir == 1) {
		//针对目的地派梯
		map<int, vector<Passage*> >::iterator it = outer_people.find(currentpos);
		if (it != outer_people.end()) {
			for (Passage* p : it->second) {
				if (p->dir == 1) {
					m_needonpassage = true;
				}
			}
		}
		//针对非目的地派梯
		if (outer_call[currentpos][0]) {
			m_needonpassage = true;
		}
	}
	//如果当前是向下
	else if (dir == 2) {
		//针对目的地派梯
		map<int, vector<Passage*> >::iterator it = outer_people.find(currentpos);
		if (it != outer_people.end()) {
			for (Passage* p : it->second) {
				if (p->dir == 2) {
					m_needonpassage = true;
				}
			}
		}
		//针对非目的地派梯
		if (outer_call[currentpos][1]) {
			m_needonpassage = true;
		}
	}
	return m_needoffpassage || m_needonpassage;
}




//电梯的状态机迁移函数
void Elevator::update(unsigned char dest) {
	if (state == IDLE) {
		//如果电梯有一个外招或者内招
		if (hasoutcall() || hasinncall()) {
			//TODO找到最近的一个外招,并改变方向
			//获取初始方向
			dir = getinitdir();
			if (iEleID == DEBUG_ENO) {
				cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:IDLE=>MOVE" << endl;
			}
			state = MOVE;
		}
		else {
			//如果是空闲状态触发了高峰模式的话，电梯在空闲状态自动移动到对应的位置，但仍然是空闲状态
			if (dest != INVALID_NUM) {
				unsigned char dir = getDirection((unsigned char)currentpos, dest);
				if (dir == 1) {
					currentheight += velocity * TIME_INTERVAL;//升高0.3m
					currentpos = currentheight / 3 + 1;//更新当前楼层
				}
				if (dir == 2) {
					currentheight -= velocity * TIME_INTERVAL;//降低0.3m
					currentpos = currentheight / 3 + 1;//更新当前楼层
				}
				if (iEleID == DEBUG_ENO) {
					cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << (int)dir << " c:" << PassageCount << "  STATE:IDLE=>IDLE" << endl;
				}
			}
		}
	}
	else if (state == OPEN_DOOR) {
		//如果持续动作结束
		if (_t_controller > opendoor) {
			if (iEleID == DEBUG_ENO) {
				cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:OPEN_DOOR=>OFF_PASSENAGE" << endl;
			}
			state = OFF_PASSENAGE;
			_t_controller = 0;
		}
	}
	else if (state == OFF_PASSENAGE) {
		//处理下的乘客
		if (!m_needoffpassage) {
			if (iEleID == DEBUG_ENO) {
				cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:OFF_PASSENAGE=>LAND_PASSENAGE" << endl;
			}
			state = LAND_PASSENAGE;
			_t_controller = 0;
		}
		else {
			//如果持续动作结束
			if (_t_controller > passagetakeoff) {
				passageOFF();
				//下乘客完毕
				m_needoffpassage = false;
				if (iEleID == DEBUG_ENO) {
					cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:OFF_PASSENAGE=>LAND_PASSENAGE" << endl;
				}
				state = LAND_PASSENAGE;
				_t_controller = 0;
				
			}
		}
	}
	else if (state == LAND_PASSENAGE) {
		//如果持续动作结束
		//处理上的乘客
		if (!m_needonpassage) {
			if (iEleID == DEBUG_ENO) {
				cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:LAND_PASSENAGE=>CLOSE_DOOR" << endl;
			}
			state = CLOSE_DOOR;
			_t_controller = 0;
		}
		else {
			if (_t_controller > passagetakeoff) {
				passageIn();
				//上乘客完毕
				m_needonpassage = false;
				//取消外招
				if (dir == 1) {
					outer_call[currentpos][0] = false;
				}
				else if (dir == 2) {
					outer_call[currentpos][1] = false;
				}
				if (iEleID == DEBUG_ENO) {
					cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:LAND_PASSENAGE=>CLOSE_DOOR" << endl;
				}
				state = CLOSE_DOOR;
				_t_controller = 0;
			}
		}
	}
	else if (state == CLOSE_DOOR) {
		//如果持续动作结束
		if (_t_controller > closedoor) {
			//有无内外招
			if (hasoutcall() || hasinncall()) {
				//判断是否需要转向
				if (needturn()) {
					if (iEleID == DEBUG_ENO) {
						cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:CLOSE_DOOR=>TURN" << endl;
					}

					state = TURN;
					_t_controller = 0;
				}
				else {
					if (iEleID == DEBUG_ENO) {
						cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:CLOSE_DOOR=>MOVE" << endl;
					}
					state = MOVE;
				}

			}
			else {
				if (iEleID == DEBUG_ENO) {
					cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:CLOSE_DOOR=>IDLE" << endl;
				}
				state = IDLE;
			}
		}
	}
	else if (state == TURN) {
		//如果持续动作结束
		if (_t_controller > turn) {

			if (dir == 1) {
				dir = 2;
			}
			else if (dir == 2) {
				dir = 1;
			}
			if (iEleID == DEBUG_ENO) {
				cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:TURN=>MOVE" << endl;
			}

			state = MOVE;
		}
	}

	else if (state == MOVE) {
		//判断是否需要开门
		if (needopendoor()) {
			//有的话更新状态
			if (iEleID == DEBUG_ENO) {
				cout << "t:" << currentTime << " eno: " << iEleID << " p:" << currentpos << " d:" << dir << " c:" << PassageCount << "  STATE:MOVE=>OPEN_DOOR" << endl;
			}

			state = OPEN_DOOR;
			_t_controller = 0;
		}
		else {
			//不需要的话更新当前位置
			if (dir == 1) {
				currentheight += velocity * TIME_INTERVAL;//升高0.3m
			}
			if (dir == 2) {
				currentheight -= velocity * TIME_INTERVAL;//降低0.3m
			}
			//当前楼层
			currentpos = currentheight / 3 + 1;//更新当前楼层
		}
	}

	//不会进
	else if (state == STOP) {
		//TODO判断是否有内外招在当前楼层
		//有的话上下乘客，内招下乘客，外招上乘客，并删除内外招
		//上乘客的时候去记录<开门时间，上客数量，上客后乘客按了内招按钮之后，该层的内招分布情况
		//有无别的楼层的内外招
		//无的话状态改成IDLE
		//有的话状态改成MOVE
	}
	//cout << "eleno:" << this->iEleID << "  currentpos:" << this->currentpos << "  STATE:" << state << endl;
	_t_controller += TIME_INTERVAL;
}