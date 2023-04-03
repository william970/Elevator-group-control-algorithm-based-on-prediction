#include "greedy.h"

//开启时加入记录信息
#define RECODE 0






void MallocRecodeMemory(FloorProbability(*& floorProbability)[MAX_RECORD_SIZE], unsigned int(*& probabilitySize))
{
	//分配内存
	if (floorProbability == NULL) {
		floorProbability = (FloorProbability(*)[MAX_RECORD_SIZE]) malloc(sizeof(FloorProbability)*MAX_FLOOR*MAX_RECORD_SIZE);
		memset(floorProbability, 0, sizeof(FloorProbability)*MAX_FLOOR*MAX_RECORD_SIZE);
	}
	if (probabilitySize == NULL) {
		probabilitySize = (unsigned int(*)) malloc(sizeof(unsigned int)*MAX_FLOOR);
		memset(probabilitySize, 0, sizeof(probabilitySize)*MAX_FLOOR);
	}
	return;
}


void elevatorRecode(unsigned int lowestwight, unsigned int heighstwight, unsigned int currentday, unsigned int currentTime, unsigned char curFloor, unsigned char dir, unsigned char(*innerCall)[INNER_CALL_SIZE], unsigned char(*outerCall)[2][OUTER_CALL_SIZE], unsigned char elevatorNum, unsigned char(*invalidFloor)[MAX_FLOOR], FloorProbability(*&floorProbability)[MAX_RECORD_SIZE], unsigned int(*&probabilitySize), unsigned int CurrentEno)
{
	//记录上车人数.假设60一个人
	int getinpassage = (heighstwight - lowestwight) / WEIGHT_PER_PASSAGE;

	//分配内存
	if (floorProbability == NULL) {
		floorProbability = (FloorProbability(*)[MAX_RECORD_SIZE]) malloc(sizeof(FloorProbability)*MAX_FLOOR*MAX_RECORD_SIZE);
		memset(floorProbability, 0, sizeof(FloorProbability)*MAX_FLOOR*MAX_RECORD_SIZE);
	}
	if (probabilitySize == NULL) {
		probabilitySize = (unsigned int(*)) malloc(sizeof(unsigned int)*MAX_FLOOR);
		memset(probabilitySize, 0, sizeof(probabilitySize)*MAX_FLOOR);
	}

	floorProbability[curFloor][probabilitySize[curFloor] % MAX_RECORD_SIZE].recodeday = currentday;
	floorProbability[curFloor][probabilitySize[curFloor] % MAX_RECORD_SIZE].recodetime = currentTime;
	floorProbability[curFloor][probabilitySize[curFloor] % MAX_RECORD_SIZE].fromfloor = curFloor;
	floorProbability[curFloor][probabilitySize[curFloor] % MAX_RECORD_SIZE].passageincount = getinpassage;

	//暂时只考虑内招
	if (dir == 1) {
		for (int i = 0; i < INNER_CALL_SIZE; i++) {
			if (innerCall[CurrentEno][i] == INVALID_NUM) {
				break;
			}
			//只计算当前楼层的向上的
			else if (innerCall[CurrentEno][i] > curFloor) {
				//加入记录中,记录概率
				//最多记录1000条数据，多了覆盖
				floorProbability[curFloor][probabilitySize[curFloor] % MAX_RECORD_SIZE].allProbability++;
				floorProbability[curFloor][probabilitySize[curFloor] % MAX_RECORD_SIZE].offProbability[innerCall[CurrentEno][i]]++;
			}
		}
	}
	else if (dir == 2) {
		for (int i = 0; i < INNER_CALL_SIZE; i++) {
			if (innerCall[CurrentEno][i] == INVALID_NUM) {
				break;
			}
			else if (innerCall[CurrentEno][i] < curFloor) {
				//加入记录中,记录概率
				//最多记录1000条数据，多了覆盖
				floorProbability[curFloor][probabilitySize[curFloor] % MAX_RECORD_SIZE].allProbability++;
				floorProbability[curFloor][probabilitySize[curFloor] % MAX_RECORD_SIZE].offProbability[innerCall[CurrentEno][i]]++;
			}
		}
	}

	probabilitySize[curFloor]++;
	return;
}


unsigned char getDirection(unsigned char startFloor,
	unsigned char endFloor) {
	/*******************
	Description://判断后一个楼层在前一个楼层的哪个方向上，上方为1，下方为2，同一层为0；
	Input://startFloor：前一个楼层; endFloor: 后一个楼层;
	Return://返回unsigned char型的数字0或1或2。
	*******************/
	unsigned char dir;
	//ADDED-2019-09-18
	if (startFloor == INVALID_NUM || endFloor == INVALID_NUM) { return 0; }
	if (endFloor == UP_NUM) {
		return 1;
	}
	else if (endFloor == DOWN_NUM) {
		return 2;
	}
	else {
		if (endFloor > startFloor) {
			dir = 1;
		}
		else if (endFloor < startFloor) {
			dir = 2;
		}
		else {
			dir = 0;
		}
		return dir;
	}
}


unsigned char judgeCase(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned char dir,
	unsigned char curFloor) {
	/*******************
	Description://判断呼梯信号与电梯位置的情景，总共7种情况。
	//0：电梯空闲
	//1：电梯方向向上，呼梯信号方向向上且在电梯之上；
	//2：电梯方向向上，呼梯信号方向向下；
	//3：电梯方向向上，呼梯信号方向向上且在电梯之下；
	//4：电梯方向向下，呼梯信号方向向下且在电梯之下；
	//5：电梯方向向下，呼梯信号方向向上；
	//6：电梯方向向下，呼梯信号方向向下且在电梯之上；
	Input://fromFloor：呼梯信号的初始楼层; aimFloor: 呼梯信号的目的楼层;

	Return://返回unsigned char型的数字0~6中间的一个数字表示对应情况。
	*******************/
	unsigned char prcs;
	unsigned char p_dir = getDirection(fromFloor, aimFloor);
	if (dir == 0) {
		prcs = 0;
	}
	else {
		if (p_dir != dir) {
			if (dir == 1) { prcs = 2; }
			else { prcs = 5; }
		}
		else {
			if (dir == 1 && (fromFloor >= curFloor)) { //fromFloor呼梯信号的所在楼层 curFloor电梯的所在楼层
				prcs = 1;
			}
			else if (dir == 1 && (fromFloor < curFloor)) {
				prcs = 3;
			}
			else if (dir == 2 && (fromFloor <= curFloor)) {
				prcs = 4;
			}
			else if (dir == 2 && (fromFloor > curFloor)) {
				prcs = 6;
			}
			else {
				return 0;
			}
		}
	}
	return prcs;
}

unsigned char
getHighestFloor(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned char *dest,
	unsigned char curFloor) {
	/*******************
	Description://计算电梯最远目的楼层和呼梯信号中的初始楼层和目的楼层中最高的楼层并返回。
	Input://fromFloor:初始楼层;aimFloor:目的楼层; *dest:长度为2的一维指针。
	Return://返回电梯最远目的楼层和呼梯信号中的初始楼层和目的楼层中最高的楼层。
	*******************/
	unsigned char f;
	unsigned char a;
	unsigned char b;
	if (aimFloor == UP_NUM) { a = MAX_FLOOR; }
	else if (aimFloor == DOWN_NUM) {
		a = fromFloor;
		b = 0;
	}
	else {
		a = fromFloor > aimFloor ? fromFloor : aimFloor;
	}
	b = dest[0] > dest[1] ? dest[0] : dest[1];
	f = a > b ? a : b;
	return f;
}

unsigned int
getDistCurToFrom(unsigned char caseNum,
	unsigned char fromFloor,
	unsigned char curFloor,
	unsigned char *dest) {
	/*******************
	Description://计算电梯到呼梯信号初始层需要移动的路程。
	Input://caseNum:情景的编号0~6;fromFloor:初始楼层;curFloor:电梯的当前楼层; *dest:长度为2的一维指针,dest[0]为最低层，dest[1]为最高层。
	Return://返回电梯到呼梯信号初始层需要移动的路程。
	*******************/
	unsigned int d;
	if (((caseNum == 0) || (caseNum == 1)) || (caseNum == 4)) {//顺向截梯
		d = fromFloor > curFloor ? (fromFloor - curFloor) : (curFloor - fromFloor);
	}
	else if ((caseNum == 2) || (caseNum == 5)) {
		d = (dest[0] > curFloor ? (dest[0] - curFloor) : (curFloor - dest[0])) +
			(dest[0] > fromFloor ? (dest[0] - fromFloor) : (fromFloor - dest[0]));
	}
	else {
		d = (dest[0] > curFloor ? (dest[0] - curFloor) : (curFloor - dest[0])) +
			(dest[0] > dest[1] ? (dest[0] - dest[1]) : (dest[1] - dest[0]))
			+ (fromFloor > dest[1] ? (fromFloor - dest[1]) : (dest[1] - fromFloor));
	}
	return d;
}

unsigned int getDistFromToAim(unsigned char fromFloor,
	unsigned char aimFloor) {
	if (aimFloor == UP_NUM || aimFloor == DOWN_NUM) {
		return 0;
	}
	else {
		return fromFloor > aimFloor ? (fromFloor - aimFloor) : (aimFloor - fromFloor);
	}
}

unsigned int getAddedDistance(unsigned char caseNum,
	unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned char *dest) {
	/*******************
	Description://计算电梯在某一层floor停梯会给已分配的信号增加多少距离
	Input://caseNum:情景的编号0~6;fromFloor:呼层梯信号的初始楼层;aimFloor:电呼层梯信号的目的楼层;dest:最远目的层;
	Return://返回呼梯信号增加的后面的人的距离。
	*******************/
	unsigned int d;
	if (aimFloor == UP_NUM || aimFloor == DOWN_NUM) {
		return 0;
	}
	else {
		switch (caseNum) {
		case 1: {
			d = aimFloor > dest[0] ? (aimFloor - dest[0]) : 0;
		}
				break;
		case 2: {
			d = (fromFloor > dest[0] ? (fromFloor - dest[0]) : 0) + (aimFloor < dest[1] ? (dest[1] - aimFloor) : 0);
		}
				break;
		case 3: {
			d = fromFloor < dest[1] ? (dest[1] - fromFloor) : 0;
		}
				break;
		case 4: {
			d = aimFloor < dest[0] ? (dest[0] - aimFloor) : 0;
		}
				break;
		case 5: {
			d = (fromFloor < dest[0] ? (dest[0] - fromFloor) : 0) + (aimFloor > dest[1] ? (aimFloor - dest[1]) : 0);
		}
				break;
		case 6: {
			d = fromFloor > dest[1] ? (fromFloor - dest[1]) : 0;
		}
				break;
		default: {
			d = 0;
		}
		}
		d = d * 2;
		return d;
	}
}


unsigned int
getStopTimes(unsigned char(*outerList)[OUTER_CALL_SIZE],
	unsigned char innerList[INNER_CALL_SIZE],
	unsigned int dir,
	unsigned char highestFloor,
	unsigned char pos) {
	/*******************
	Description: 计算某部电梯所有的停梯次数
	Input: (*outerList)[OUTER_CALL_SIZE] 外招的集合；
	innerList[INNER_CALL_SIZE] 内招的集合；
	dir 方向；
	highest 最远到达的楼层；
	pos 电梯当前位置；
	Return://返回某部电梯的所有的停梯次数。
	*******************/
	unsigned int count = 0;
	unsigned char stopTime[MAX_FLOOR];
	unsigned char upStopTime[MAX_FLOOR];
	unsigned char downStopTime[MAX_FLOOR];
	memset(upStopTime, 0, sizeof(upStopTime));
	memset(downStopTime, 0, sizeof(downStopTime));
	memset(stopTime, 0, sizeof(stopTime));
	if (dir == 1) {
		for (int i = 0; i < INNER_CALL_SIZE; i++) {
			if (innerList[i] == INVALID_NUM) {
				break;
			}
			else {
				if (pos != innerList[i]) { upStopTime[innerList[i]] = 1; }
			}
		}
	}
	else if (dir == 2) {
		for (int i = 0; i < INNER_CALL_SIZE; i++) {
			if (innerList[i] == INVALID_NUM) {
				break;
			}
			else {
				if (pos != innerList[i]) { downStopTime[innerList[i]] = 1; }
			}
		}
	}
	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		if (outerList[0][i] == INVALID_NUM) {
			break;
		}
		else {
			if (getDirection(outerList[0][i], outerList[1][i]) == 1) {
				if (pos != outerList[0][i]) { upStopTime[outerList[0][i]] = 1; }
				if (pos != outerList[1][i]) { upStopTime[outerList[1][i]] = 1; }

			}
			else if (getDirection(outerList[0][i], outerList[1][i] == 2)) {
				if (pos != outerList[0][i]) { downStopTime[outerList[0][i]] = 1; }
				if (pos != outerList[1][i]) { downStopTime[outerList[1][i]] = 1; }
			}
		}
	}
	for (int i = 0; i < highestFloor; i++) {
		stopTime[i] = upStopTime[i] + downStopTime[i];
		////printf("%d",stopTime[i]);
	}
	for (int i = 0; i < highestFloor; i++) {
		count = count + stopTime[i];
		////printf("stop time :%d",stopTime[i]);
	}
	return count;
}

////ADD by YJH
//double getAdditionalDowntime(
//	unsigned char aimFloor,
//	unsigned char curFloor,
//	unsigned char dir,
//	unsigned char innerList[INNER_CALL_SIZE],
//	unsigned char(*outerList)[OUTER_CALL_SIZE],
//	unsigned char highestFloor,
//	struct FloorProbability maxNearProbability)
//{
//	//原先就需要挺的楼层
//	unsigned char needStopFloor[MAX_FLOOR] = {0};
//
//	double ret = 0;
//	//计算对应的概率
//	//如果电梯当前向上
//	if (dir == 1) {
//		for (int i = 0; i < INNER_CALL_SIZE; i++) {
//			if (innerList[i] == INVALID_NUM) {
//				break;
//			}
//			else {
//				needStopFloor[innerList[i]] = 1;
//			}
//		}
//		for (int i = 0; i < OUTER_CALL_SIZE; i++) {
//			if (outerList[0][i] == INVALID_NUM) {
//				break;
//			}
//			//必须是相同方向的
//			if (getDirection(outerList[0][i], outerList[1][i]) == 1) {
//				needStopFloor[outerList[0][i]] = 1;
//				needStopFloor[outerList[1][i]] = 1;
//			}
//			//else {
//			//	if (outerList[0][i] > highestFloor) {
//			//		needStopFloor[outerList[0][i]] = 1;
//			//	}
//			//}
//		}
//
//		//如果是目的地派梯
//		if (aimFloor != UP_NUM || aimFloor != DOWN_NUM) {
//			if (!needStopFloor[aimFloor]) {
//				ret += 1.0;
//			}
//		}
//		else {
//			//计算概率分布
//			if (maxNearProbability.recodetime != 0) {
//				for (int f = curFloor; f < MAX_FLOOR; f++) {
//					//如果原本就需要停梯
//					if (needStopFloor[f]) {
//						continue;
//					}
//					//增加在f层停下的概率
//					double pro = 1.0 * maxNearProbability.offProbability[f] / maxNearProbability.allProbability;
//					ret += pro;
//				}
//			}
//		}
//	}
//	//如果电梯当前向下
//	else if(dir == 2){
//		for (int i = 0; i < INNER_CALL_SIZE; i++) {
//			if (innerList[i] == INVALID_NUM) {
//				break;
//			}
//			else {
//				needStopFloor[innerList[i]] = 1;
//			}
//		}
//		for (int i = 0; i < OUTER_CALL_SIZE; i++) {
//			if (outerList[0][i] == INVALID_NUM) {
//				break;
//			}
//			//必须是相同方向的
//			if (getDirection(outerList[0][i], outerList[1][i]) == 2) {
//				needStopFloor[outerList[0][i]] = 1;
//				needStopFloor[outerList[1][i]] = 1;
//			}
//		}
//
//		//如果是目的地派梯
//		if (aimFloor != UP_NUM || aimFloor != DOWN_NUM) {
//			if (!needStopFloor[aimFloor]) {
//				ret += 1.0;
//			}
//		}
//		else {
//			if (maxNearProbability.recodetime != 0) {
//				for (int f = 0; f < curFloor; f++) {
//					if (needStopFloor[f]) {
//						continue;
//					}
//					//增加在f层停下的概率
//					double pro = 1.0 * maxNearProbability.offProbability[f] / maxNearProbability.allProbability;
//					ret += pro;
//				}
//			}
//		}
//		
//	}
//	return ret;
//}


unsigned int
getCountNew(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned char curFloor,
	unsigned char dir,
	unsigned char innerList[INNER_CALL_SIZE],
	unsigned char(*outerList)[OUTER_CALL_SIZE],
	unsigned char highestFloor,
	unsigned char *dest) {
	/*******************
	Description://计算电梯到某一层的停梯次数
	Input://caseNum:情景的编号0~6;floor:需要统计的那一层;curFloor:电梯当前楼层;innerList[INNER_CALL_SIZE]:某台电梯的内招信号,传参形式为innerCall[n];
	//outerList[OUTER_CALL_SIZE]:外招信号,传参形式为outerCall[n];highestFloor:当前所有外招中最高的楼层。*dest:电梯的两端最远目的楼层;
	Return://返回电梯到呼梯信号初始层需要停下的次数。
	*******************/
	unsigned char count = 0;
	unsigned char firstFloor[MAX_FLOOR];
	unsigned char secondFloor[MAX_FLOOR];
	unsigned char thirdFloor[MAX_FLOOR];
	unsigned char stopTime[MAX_FLOOR];

	memset(firstFloor, 0, sizeof(firstFloor));
	memset(secondFloor, 0, sizeof(secondFloor));
	memset(thirdFloor, 0, sizeof(thirdFloor));
	memset(stopTime, 0, sizeof(stopTime));
	for (int i = 0; i < INNER_CALL_SIZE; i++) {
		if (innerList[i] == INVALID_NUM) {
			break;
		}
		else {
			firstFloor[innerList[i]] = 1;
		}
	}
	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		if (outerList[0][i] == INVALID_NUM) {
			break;
		}
		else if (getDirection(outerList[0][i], outerList[1][i]) == getDirection(fromFloor, aimFloor) &&
			fromFloor == outerList[0][i]) {
			//这步是为了在redispatch时，把计算的楼层去掉
			continue;
		}
		else {
			if (dir == 1) {
				if (getDirection(outerList[0][i], outerList[1][i]) == 1 && fromFloor >= curFloor) {
					if (outerList[1][i] != UP_NUM) {
						firstFloor[outerList[1][i]] = 1;
					}
					else { firstFloor[highestFloor - 1] = 1; }
					firstFloor[outerList[0][i]] = 1;
				}
				else if (getDirection(outerList[0][i], outerList[1][i]) == 1 && fromFloor < curFloor) {
					if (outerList[1][i] != UP_NUM) {
						thirdFloor[outerList[1][i]] = 1;
					}
					else { thirdFloor[highestFloor - 1] = 1; }
					thirdFloor[outerList[0][i]] = 1;
				}
				else if (getDirection(outerList[0][i], outerList[1][i]) == 2) {
					if (outerList[1][i] != DOWN_NUM) {
						secondFloor[outerList[1][i]] = 1;
					}
					else { secondFloor[0] = 1; }
					secondFloor[outerList[0][i]] = 1;
				}
			}
			else if (dir == 2) {
				if (getDirection(outerList[0][i], outerList[1][i]) == 2 && fromFloor <= curFloor) {
					if (outerList[1][i] != DOWN_NUM) {
						firstFloor[outerList[1][i]] = 1;
					}
					else { firstFloor[0] = 1; }
					firstFloor[outerList[0][i]] = 1;
				}
				else if (getDirection(outerList[0][i], outerList[1][i]) == 2 && fromFloor > curFloor) {
					if (outerList[1][i] != DOWN_NUM) {
						thirdFloor[outerList[1][i]] = 1;
					}
					else { secondFloor[0] = 1; }
					thirdFloor[outerList[0][i]] = 1;
				}
				else if (getDirection(outerList[0][i], outerList[1][i]) == 1) {
					if (outerList[1][i] != UP_NUM) {
						secondFloor[outerList[1][i]] = 1;
					}
					else { secondFloor[highestFloor - 1] = 1; }
					secondFloor[outerList[0][i]] = 1;
				}
			}
			else {
				if (aimFloor == UP_NUM) { stopTime[highestFloor - 1] = 1; }
				else if (aimFloor == DOWN_NUM) { stopTime[0] = 1; }
				else { stopTime[aimFloor] = 1; }
				stopTime[fromFloor] = 1;
			}
		}
	}
	for (int i = 0; i < highestFloor; i++) {
		stopTime[i] = firstFloor[i] + secondFloor[i] + thirdFloor[i] + stopTime[i];
	}
	if (stopTime[dest[0]] > 1) {
		//转变方向出最大只能为1
		stopTime[dest[0]] = 1;
	}
	if (stopTime[dest[1]] > 1) {
		stopTime[dest[1]] = 1;
	}
	stopTime[fromFloor] = 1;
	for (int i = 0; i < highestFloor; i++) {
		count = count + stopTime[i];
		////printf("%d",stopTime[i]);
	}
	return count;
}

unsigned int
getAddedCountNew(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned char curFloor,
	unsigned char dir,
	unsigned char innerList[INNER_CALL_SIZE],
	unsigned char(*outerList)[OUTER_CALL_SIZE]) {
	unsigned char count1 = 1;
	unsigned char count2 = 1;
	unsigned char firstFloor[MAX_FLOOR];
	unsigned char secondFloor[MAX_FLOOR];
	unsigned char thirdFloor[MAX_FLOOR];
	unsigned char stopTime[MAX_FLOOR];

	memset(firstFloor, 0, sizeof(firstFloor));
	memset(secondFloor, 0, sizeof(secondFloor));
	memset(thirdFloor, 0, sizeof(thirdFloor));
	memset(stopTime, 0, sizeof(stopTime));
	for (int i = 0; i < INNER_CALL_SIZE; i++) {
		if (innerList[i] == INVALID_NUM) {
			break;
		}
		else {
			firstFloor[innerList[i]] = 1;
		}
	}
	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		if (outerList[0][i] == INVALID_NUM) {
			break;
		}
		else {
			if (dir == 1) {
				if (getDirection(outerList[0][i], outerList[1][i]) == 1 && fromFloor >= curFloor) {
					if (outerList[1][i] != UP_NUM) {
						firstFloor[outerList[1][i]] = 1;
					}
					firstFloor[outerList[0][i]] = 1;
				}
				else if (getDirection(outerList[0][i], outerList[1][i]) == 1 && fromFloor < curFloor) {
					if (outerList[1][i] != UP_NUM) {
						thirdFloor[outerList[1][i]] = 1;
					}
					thirdFloor[outerList[0][i]] = 1;
				}
				else if (getDirection(outerList[0][i], outerList[1][i]) == 2) {
					if (outerList[1][i] != DOWN_NUM) {
						secondFloor[outerList[1][i]] = 1;
					}
					secondFloor[outerList[0][i]] = 1;
				}
			}
			else if (dir == 2) {
				if (getDirection(outerList[0][i], outerList[1][i]) == 2 && fromFloor <= curFloor) {
					if (outerList[1][i] != DOWN_NUM) {
						firstFloor[outerList[1][i]] = 1;
					}
					firstFloor[outerList[0][i]] = 1;
				}
				else if (getDirection(outerList[0][i], outerList[1][i]) == 2 && fromFloor > curFloor) {
					if (outerList[1][i] != DOWN_NUM) {
						thirdFloor[outerList[1][i]] = 1;
					}
					thirdFloor[outerList[0][i]] = 1;
				}
				else if (getDirection(outerList[0][i], outerList[1][i]) == 1) {
					if (outerList[1][i] != UP_NUM) {
						secondFloor[outerList[1][i]] = 1;
					}
					secondFloor[outerList[0][i]] = 1;
				}
			}
			else {
				if (aimFloor != UP_NUM || aimFloor != DOWN_NUM) { stopTime[aimFloor] = 1; }
				stopTime[fromFloor] = 1;
			}
		}
	}
	if (dir == 1) {
		if (getDirection(fromFloor, aimFloor) == 1 && fromFloor >= curFloor) {
			if (aimFloor != UP_NUM) {
				if (firstFloor[aimFloor] == 1) { count2 = 0; }
			}
			if (firstFloor[fromFloor] == 1) { count1 = 0; }
		}
		else if (getDirection(fromFloor, aimFloor) == 1 && fromFloor < curFloor) {
			if (aimFloor != UP_NUM) {
				if (thirdFloor[aimFloor] == 1) { count2 = 0; }
			}
			if (thirdFloor[fromFloor] == 1) { count1 = 0; }
		}
		else if (getDirection(fromFloor, aimFloor) == 2) {
			if (aimFloor != DOWN_NUM) {
				if (secondFloor[aimFloor] == 1) { count2 = 0; }
			}
			if (secondFloor[fromFloor] == 1) { count1 = 0; }
		}
	}
	else if (dir == 2) {
		if (getDirection(fromFloor, aimFloor) == 2 && fromFloor <= curFloor) {
			if (aimFloor != DOWN_NUM) {
				if (firstFloor[aimFloor] == 1) { count2 = 0; }
			}
			if (firstFloor[fromFloor] == 1) { count1 = 0; }
		}
		else if (getDirection(fromFloor, aimFloor) == 2 && fromFloor > curFloor) {
			if (aimFloor != DOWN_NUM) {
				if (thirdFloor[aimFloor] == 1) { count2 = 0; }
			}
			if (thirdFloor[fromFloor] == 1) { count1 = 0; }
		}
		else if (getDirection(fromFloor, aimFloor) == 1) {
			if (aimFloor != UP_NUM) {
				if (secondFloor[aimFloor] == 1) { count2 = 0; }
			}
			if (secondFloor[fromFloor] == 1) { count1 = 0; }
		}
	}
	else {
		count1 = 0;
		count2 = 0;
	}
	return count1 + count2;
}

unsigned int is_full(unsigned int full_signal,
	unsigned int capacity,
	unsigned char innerList[INNER_CALL_SIZE],
	unsigned char(*outerList)[OUTER_CALL_SIZE],
	unsigned char passengerNum,
	unsigned char disabledNum) {
	unsigned int count = 0;
	unsigned int isfull = 0;
	for (int i = 0; i < INNER_CALL_SIZE; i++) {
		if (innerList[i] == INVALID_NUM) { break; }
		else {
			count += 1;
		}
	}
	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		if (outerList[0][i] == INVALID_NUM) {
			break;
		}
		else {
			count += 1;
		}
	}
	if ((count + passengerNum + disabledNum * 5 <= capacity) && full_signal == 0) { isfull = 0; }
	else { isfull = 1; }
	return isfull;
}

void getDestination(unsigned char dir,
	unsigned char curFloor,
	unsigned char innerList[INNER_CALL_SIZE],
	unsigned char(*outerList)[OUTER_CALL_SIZE],
	unsigned char *dest,
	unsigned char highestFloor) {
	unsigned char dest1 = curFloor;
	unsigned char dest2;
	/*******************
	Description://计算电梯当前方向上的最远目的楼层和反方向上的最远目的楼层，将他们存入dest指针里。
	Input://dir:电梯方向; curFloor:电梯所在楼层; innerList[INNER_CALL_SIZE]:某台电梯的内招信号;
	// *(outerList)[OUTER_CALL_SIZE]:某台电梯的外招信号;*dest长度为2的一维指针,dest[0]为最低目的层，dest[1]为最高目的层
	Return://无返回值。
	*******************/
	////printf("dir:%d",dir);
	////printf("highestFloor:%d",highestFloor);
	if (dir == 1) {
		for (int i = 0; i < INNER_CALL_SIZE; i++) {
			if (innerList[i] == INVALID_NUM) {
				break;
			}
			else {
				dest1 = innerList[i] > dest1 ? innerList[i] : dest1;
			}
		}
		for (int j = 0; j < OUTER_CALL_SIZE; j++) {
			if (outerList[0][j] == INVALID_NUM) {
				//到无效位终止循环
				break;
			}
			else {
				if (getDirection(outerList[0][j], outerList[1][j]) == 1 && outerList[0][j] >= curFloor) {
					//第一段外招信号
					//如果乘客方向为上且在电梯上方
					//判断目的楼层是否未知,已知就用目的楼层比较，未知就用初始楼层来比较
					if (outerList[1][j] != UP_NUM) { dest1 = outerList[1][j] > dest1 ? outerList[1][j] : dest1; }
					else {
						dest1 = highestFloor - 1;
						break;
					}
				}
				else if (getDirection(outerList[0][j], outerList[1][j]) == 2) {
					//第二段
					//如果乘客方向向下
					dest1 = outerList[0][j] > dest1 ? outerList[0][j] : dest1;
				}
			}
		}
		dest2 = dest1;
		for (int j = 0; j < OUTER_CALL_SIZE; j++) {
			if (outerList[0][j] == INVALID_NUM) {
				//到无效位终止循环
				break;
			}
			else {
				if (getDirection(outerList[0][j], outerList[1][j]) == 1 && outerList[0][j] < curFloor) {
					//第三段外招信号
					dest2 = outerList[0][j] < dest2 ? outerList[0][j] : dest2;
				}
				else if (getDirection(outerList[0][j], outerList[1][j]) == 2) {
					if (outerList[1][j] != DOWN_NUM) { dest2 = outerList[1][j] < dest2 ? outerList[1][j] : dest2; }
					else {
						dest2 = 0;
						break;
					}
				}
			}
		}
	}
	else if (dir == 2) {
		for (int i = 0; i < INNER_CALL_SIZE; i++) {
			if (innerList[i] == INVALID_NUM) {
				break;
			}
			else {
				dest1 = innerList[i] < dest1 ? innerList[i] : dest1;
			}
		}
		for (int j = 0; j < OUTER_CALL_SIZE; j++) {
			if (outerList[0][j] == INVALID_NUM) {
				//到无效位终止循环
				break;
			}
			else {
				if (getDirection(outerList[0][j], outerList[1][j]) == 2 && outerList[0][j] <= curFloor) {
					//第一段外招信号
					if (outerList[1][j] != DOWN_NUM) { dest1 = outerList[1][j] < dest1 ? outerList[1][j] : dest1; }
					else {
						dest1 = 0;
						break;
					}
				}
				else if (getDirection(outerList[0][j], outerList[1][j]) == 1) {
					dest1 = outerList[0][j] < dest1 ? outerList[0][j] : dest1;
				}
			}
		}
		dest2 = dest1;
		for (int j = 0; j < OUTER_CALL_SIZE; j++) {
			if (outerList[0][j] == INVALID_NUM) {
				//到无效位终止循环
				break;
			}
			else {
				if (getDirection(outerList[0][j], outerList[1][j]) == 2 && outerList[0][j] > curFloor) {
					//第三段外招信号
					dest2 = outerList[0][j] > dest2 ? outerList[0][j] : dest2;
				}
				else if (getDirection(outerList[0][j], outerList[1][j]) == 1) {
					if (outerList[1][j] != UP_NUM) { dest2 = outerList[1][j] > dest2 ? outerList[1][j] : dest2; }
					else {
						dest2 = highestFloor - 1;
						break;
					}
				}
			}
		}
	}
	else {
		dest1 = curFloor;
		dest2 = curFloor;
	}
	dest[0] = dest1;
	dest[1] = dest2;
}

unsigned int hasSameFloor(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned char innerCall[INNER_CALL_SIZE],
	unsigned char(*outerCall)[OUTER_CALL_SIZE]) {
	unsigned int count = 0;
	unsigned int count1 = 0;
	unsigned int count2 = 0;

	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		if (outerCall[0][i] == fromFloor || outerCall[1][i] == fromFloor) {
			count2 = 0;
			break;
		}
		else { count1 += 1; }
	}
	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		if (outerCall[0][i] == aimFloor || outerCall[1][i] == aimFloor) {
			count2 = 0;
			break;
		}
		else { count2 += 1; }
	}
	count = count1 + count2;
	if (count == 0) return 1;
	else return 0;
}

unsigned int
isSameInOuterCall(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned char(*outerCall)[OUTER_CALL_SIZE]) {
	//判断外招中是否有和呼梯信号相同始末的信号
	////printf("isSameInOuterCall");
	unsigned int isFlag = 0;
	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		if (outerCall[0][i] == INVALID_NUM || outerCall[1][i] == INVALID_NUM) { break; }
		else if (outerCall[0][i] == fromFloor && outerCall[1][i] == aimFloor) {
			isFlag = 1;
			break;
		}
	}
	return isFlag;
}

unsigned int
isSameInInnerCall(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned char innerCall[INNER_CALL_SIZE],
	unsigned int elevatorFloor,
	unsigned int isMoving) {
	//判断内招中是否有和呼梯信号相同始末的信号
	unsigned int isFlag = 0;
	for (int i = 0; i < INNER_CALL_SIZE; i++) {
		if (innerCall[i] == INVALID_NUM) { break; }
		else if (elevatorFloor == fromFloor && innerCall[i] == aimFloor && isMoving == 0) {
			isFlag = 1;
			break;
		}
	}
	return isFlag;
}

//某一个电梯某一层是否为盲层
int
isInvalidFloor(unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned int floor,
	unsigned int elevatorIndex,
	unsigned int floorNum) {
	int invalid = 0;
	if (floor == UP_NUM || floor == DOWN_NUM) {
		invalid = 0;
	}
	else {
		invalid = invalidFloor[elevatorIndex][floor];
	}
	if (grpInParam[elevatorIndex][OUT_OF_GROUP_CONTROL_INDEX] == 1 ||
		grpInParam[elevatorIndex][SAVING_CONTROL_INDEX] == 1) { //脱离群控默认为盲层
		invalid = 1;
	}
	return invalid;
}

//某一层所有电梯是否为盲层
int
isAllInvalidFloor(unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned int floor,
	unsigned int elevatorNum,
	unsigned int floorNum) {
	int invalidNum = 0;
	for (int j = 0; j < elevatorNum; j++) {
		int invalid = isInvalidFloor(grpInParam, invalidFloor, floor, j, floorNum);
		if (invalid == 1) {
			invalidNum++;
		}
	}
	return invalidNum == elevatorNum;
}

unsigned char isDoorClose(unsigned char innerCall[INNER_CALL_SIZE],
	unsigned char outerCall[2][OUTER_CALL_SIZE],
	unsigned int doorSignal,
	unsigned int is_moving) {
	unsigned char flag = 0;
	unsigned char emptyFlag = 1;
	//空闲、门开着
	for (int i = 0; i < INNER_CALL_SIZE; i++) {
		if (innerCall[i] != INVALID_NUM) {
			emptyFlag = 0;
			break;
		}
	}
	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		if (outerCall[0][i] != INVALID_NUM) {
			emptyFlag = 0;
			break;
		}
	}
	//emptyFlag = 0说明空闲
	//1.它不在移动，在关门状态，但是它里面是有人的 关门状态，要么有人，要么没人，
	//现在是开门到关门之后就不分配了，也就是说doorSignal=0，is_moving = 0,但是空闲状态也是这样的标志，所以要排除空闲状态，
	//不是空闲emptyFlag = 0;
	if (doorSignal == 0 && is_moving == 0 && emptyFlag == 0) { flag = 1; }
	////printf("doorSignal:%d,is_moving:%d,emptyFlag:%d\n",doorSignal,is_moving,emptyFlag);
	return flag;
}

unsigned char cantStop(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned int elevatorPos,
	unsigned int is_moving,
	unsigned int dir,
	unsigned char innerCall[INNER_CALL_SIZE],
	unsigned char outerCall[2][OUTER_CALL_SIZE],
	unsigned int distanceFloor) {
	//1.is_moving的时候，电梯
	unsigned char cantStopFlag = 0;
	unsigned char innerCallFlag = 1; //1为不需要停
	unsigned char outerCallFlag = 1;
	unsigned char delta = 0;
	unsigned char c = 0;
	//电梯会不会停在触发层或出发层减一
	c = judgeCase(fromFloor, aimFloor, dir, elevatorPos);
	//0：电梯空闲
	//1：电梯方向向上，呼梯信号方向向上且在电梯之上；
	//2：电梯方向向上，呼梯信号方向向下；
	//3：电梯方向向上，呼梯信号方向向上且在电梯之下；
	//4：电梯方向向下，呼梯信号方向向下且在电梯之下；
	//5：电梯方向向下，呼梯信号方向向上；
	//6：电梯方向向下，呼梯信号方向向下且在电梯之上；
	if (c == 4) {
		for (int i = 0; i < INNER_CALL_SIZE; i++) {
			if (innerCall[i] == fromFloor || innerCall[i] - fromFloor == 1) {
				innerCallFlag = 1;
				break;
			}
			else { innerCallFlag = 0; }
		}
		for (int i = 0; i < OUTER_CALL_SIZE; i++) {
			if (outerCall[0][i] == fromFloor || outerCall[1][i] == fromFloor ||
				outerCall[0][i] - fromFloor == 1 || outerCall[1][i] - fromFloor == 1) {
				outerCallFlag = 1;
				break;
			}
			else { outerCallFlag = 0; }
		}
	}
	else if (c == 1) {
		for (int i = 0; i < INNER_CALL_SIZE; i++) {
			if (innerCall[i] == fromFloor || fromFloor - innerCall[i] == 1) {
				innerCallFlag = 1;
				break;
			}
			else { innerCallFlag = 0; }
		}
		for (int i = 0; i < OUTER_CALL_SIZE; i++) {
			if (outerCall[0][i] == fromFloor || outerCall[1][i] == fromFloor ||
				fromFloor - outerCall[0][i] == 1 || fromFloor - outerCall[1][i] == 1) {
				outerCallFlag = 1;
				break;
			}
			else { outerCallFlag = 0; }
		}
	}
	//出发层-电梯层的值
	delta = fromFloor > elevatorPos ? fromFloor - elevatorPos : elevatorPos - fromFloor;
	if (delta <= distanceFloor && is_moving == 1 && (innerCallFlag == 0 && outerCallFlag == 0)) { cantStopFlag = 1; }
	////printf("delta:%d,is_moving:%d,callFlag:%d\n",delta,is_moving,callFlag);
	return cantStopFlag;
}

unsigned char
onlyOneFloorSignal(unsigned char signalFloor,
	unsigned char signalDir,
	unsigned char(*outerList)[OUTER_CALL_SIZE]) {
	//内招的目的层只有这一个楼层，外招的出发层也只有这一个楼层且该层信号方向单一
	//signalDir传入的时候是UP_NUM和DOWN_NUM，因此要转化成1和2表示上或者下
	unsigned char flag = 1;
	unsigned char dir = 0;
	if (signalDir == UP_NUM) { dir = 1; }
	else if (signalDir == DOWN_NUM) { dir = 2; }
	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		//printf("signalFloor:%d\n",signalFloor);
		//printf("outerList[0][i]%d\n",outerList[0][i]);
		if (outerList[0][i] == INVALID_NUM) { break; }
		//需要重新分配的信号楼层与该电梯所有的外招信号比较，如果有多个楼层信号，或者有同层却方向不同的信号，那么将flag置为0
		else if (signalFloor != outerList[0][i] || dir != getDirection(outerList[0][i], outerList[1][i])) {
			flag = 0;
			break;
		}
	}
	if (outerList[0][0] == INVALID_NUM) { flag = 0; }//没有信号的电梯也要排除
	return flag;
}

unsigned char
idOfEleStopInFloor(unsigned char signalFloor,
	unsigned char signalDir,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char elevatorNum) {
	//判断是否已经有电梯停在该层且符合装载信号的条件，如果满足条件，返回电梯的梯号。
	unsigned int dir = 0;
	unsigned char e_id = INVALID_NUM;
	if (signalDir == UP_NUM) { dir = 1; }
	else if (signalDir == DOWN_NUM) { dir = 2; }
	for (int n = 0; n < elevatorNum; n++) {
		//如果电梯信号
		//1.电梯所处的位置与信号相同，且方向与信号方向相反或者电梯的方向为0
		//2.电梯没有在移动，且不处于脱离群控的状态
		//则返回
		if (((grpInParam[n][DIRECTION_INDEX] == dir || grpInParam[n][DIRECTION_INDEX] == 0) &&
			grpInParam[n][POS_INDEX] == signalFloor)
			&& grpInParam[n][IS_MOVING_INDEX] == 0 && grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX] == 0 &&
			grpInParam[n][IS_FULL_INDEX] == 0 && grpInParam[n][IS_VIP_MODE_INDEX] == 0) {
			e_id = n;
		}
	}
	return e_id;
}

unsigned char isCallEmpty(unsigned char innerCall[INNER_CALL_SIZE],
	unsigned char outerCall[2][OUTER_CALL_SIZE],
	unsigned int doorSignal) {
	unsigned char flag = 1;
	//空闲、门开着
	if (innerCall[0] != INVALID_NUM) { flag = 0; }
	if (outerCall[0][0] != INVALID_NUM) { flag = 0; }
	//关着门且不在移动，而且不是空闲状态（里面有人）
	if (doorSignal == 1) { flag = 0; }
	//if(is_moving == 1){flag = 0;}
	return flag;
}

unsigned char mostCloseEle(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned originIndex,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char elevatorNum,
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned char(*innerCall)[INNER_CALL_SIZE]) {
	//找出目前与信号最近的电梯
	//如果是 最近、且该层只有一个方向的电梯信号（）
	unsigned char min_index = INVALID_NUM;
	unsigned char value = INVALID_NUM;
	min_index = originIndex;
	value = grpInParam[originIndex][POS_INDEX] > fromFloor ? (grpInParam[originIndex][POS_INDEX] - fromFloor) : (
		fromFloor - grpInParam[originIndex][POS_INDEX]);
	if (grpInParam[min_index][OUT_OF_GROUP_CONTROL_INDEX] == 1) { value = INVALID_NUM; }
	//存在所有电梯都是out_of_group_control_index的情况以及没有顺向电梯的情况
	for (int n = 0; n < elevatorNum; n++) {
		if (grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX] == 1 || grpInParam[n][IS_VIP_MODE_INDEX] == 1 ||
			grpInParam[n][IS_FULL_INDEX] == 1) {
			continue;
		}
		else {
			//如果这台电梯处于p1或者p4，而且它没有超过减速点，则更改。
			unsigned char p = judgeCase(fromFloor, aimFloor, grpInParam[n][DIRECTION_INDEX], grpInParam[n][POS_INDEX]);
			////printf("p:%d\n",p);
			unsigned char temp =
				grpInParam[n][POS_INDEX] > fromFloor ? (grpInParam[n][POS_INDEX] - fromFloor) : (fromFloor -
					grpInParam[n][POS_INDEX]);
			if (isCallEmpty(innerCall[n], outerCall[n], grpInParam[n][DO_INDEX]) == 1) { p = 0; } //刚到某一层，还没开门的时候很可能出错
			if ((isCallEmpty(innerCall[n], outerCall[n], grpInParam[n][DO_INDEX]) == 1 &&
				grpInParam[n][IS_MOVING_INDEX] == 0 && grpInParam[n][DIRECTION_INDEX] == 0) ||
				grpInParam[n][IS_MOVING_INDEX] == 1) {
				if (p == 1 || p == 4 || p == 0) {
					//如果是顺向的且第一个外招信号与这个信号同向(或者没有)，才改变。
					if (outerCall[n][0][0] == INVALID_NUM ||
						getDirection(fromFloor, aimFloor) == getDirection(outerCall[n][0][0], outerCall[n][1][0])) {
						if (temp < value && grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX] == 0) {
							min_index = n;
							value = temp;
						}
					}
				}
			}
		}
	}
	if (grpInParam[min_index][OUT_OF_GROUP_CONTROL_INDEX] == 1) {
		//存在所有电梯都是out_of_group_control_index的情况以及没有顺向电梯的情况
		for (int n = 0; n < elevatorNum; n++) {
			if (grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX] == 1 || grpInParam[n][IS_VIP_MODE_INDEX] == 1 ||
				grpInParam[n][IS_FULL_INDEX] == 1) {
				continue;
			}
			else { value = n; }
		}
	}
	return min_index;
}

unsigned char
cantPartNormalAssign(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned char *priority,
	unsigned char elevatorNum,
	struct AssignSetting *assignSetting,
	unsigned char assignSize,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned int currentTime) {
	//可以开启这个模式返回1，不能开启这个模式返回0
	//判断是否可以开启所有电梯都响应普通外招模式
	unsigned int i = 0;
	unsigned char partAssignFlag = 0;
	unsigned char cantNormalAssignFlag = 1;
	while (i < assignSize) {
		//while中执行完所有的内容 如果超过这个数组内容
		unsigned int startTime = assignSetting[i].startTime;
		unsigned int endTime = assignSetting[i].endTime;
		if (currentTime >= startTime && currentTime < endTime) {
			partAssignFlag = 1;
			break;
		}
		else { i++; }
	}
	if (partAssignFlag == 1 && (aimFloor == UP_NUM || aimFloor == DOWN_NUM)) {
		//在时间段内，如果存在可以响应普通外招的电梯，则将不能普通分配的信号置为1
		//printf("partAssignFlag%d\n",partAssignFlag);
		for (int n = 0; n < elevatorNum; n++) {
			if (grpInParam[n][CAN_NORMAL_ASSIGN] == 0) { continue; }
			else {
				if (priority[n] != 2) {
					cantNormalAssignFlag = 0;
					break;
				}
				else { cantNormalAssignFlag = 1; }
			}
		}
	}
	else { cantNormalAssignFlag = 1; }
	//printf("cantNormalAssignFlag:%d\n",cantNormalAssignFlag);
	return cantNormalAssignFlag;
}

unsigned char isDoorSignalRemoved(unsigned char innerList[INNER_CALL_SIZE],
	unsigned char(*outerList)[OUTER_CALL_SIZE],
	unsigned char fromFloor,
	unsigned char aimFloor,
	int elevatorPos,
	int doorSignal,
	int is_moving) {
	//表示开门信号是否刚刚撤除
	if (aimFloor == 1) { aimFloor = UP_NUM; }
	else if (aimFloor == 2) { aimFloor = DOWN_NUM; }
	unsigned char out_flag = 0;
	unsigned char inner_flag = 0;
	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		if (outerList[0][i] == INVALID_NUM) { break; }
		else if (outerList[0][i] == fromFloor && outerList[1][i] == aimFloor) {
			//表示还没有消掉该层外招信号
			out_flag = 1;
		}
	}
	for (int i = 0; i < OUTER_CALL_SIZE; i++) {
		if (outerList[0][i] == INVALID_NUM) { break; }
		else if (innerList[i] == elevatorPos) {
			//表示还没有消掉该层内招信号
			inner_flag = 1;
		}
	}
	if (out_flag == 0 && inner_flag == 0 && doorSignal == 0 && is_moving == 0 && fromFloor == elevatorPos) { return 1; }
	else { return 0; }
}

unsigned char
compassInterface(unsigned char fromFloor,
	unsigned char aimFloor,
	unsigned int is_vip,
	unsigned int is_disabled,
	unsigned char elevatorNum,
	unsigned int floorHigh,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned int(*grpOutParam)[OUT_PARAM_NUM],
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned int adjustParam,
	unsigned char vip_mode,
	unsigned char passengerNum,
	struct AssignSetting *assignSetting,
	unsigned char assignSize,
	unsigned int currentTime
	) {
	/*******************
	Description://分配的函数，返回电梯编号
	Input:
	1.fromFloor,aimFloor,is_vip,is_disabled表示了一个乘客的信息
	2.elevatorNum:电梯的台数；
	3.floorHigh:表示的是楼层平均高度（已经不用了）
	4.(*grpInParam)[IN_PARAM_NUM]:电梯的参数和状态信息。
	5。(*innerCall)[INNER_CALL_SIZE]：内招。
	6.(*outerCall)[2][OUTER_CALL_SIZE]:外招。
	7.(*invalidFloor)[MAX_FLOOR]:盲层表。
	8.adjustParam:算分的权重
	9。passengerNum：目的层选层器呼叫的人数（群组模式）仅目的层选层器
	10.*assignSetting:只有指定电梯能响应普通外招的时间段结构体指针，grpInParam[n][CAN_NORMAL_ASSIGN]=1的电梯可响应
	//如果脱离了这个时间段，所有的电梯都可以去响应，不管grpInParam[n][CAN_NORMAL_ASSIGN]=1还是0
	assignSize：时间段的长度
	currentTime：用来判断当前时间是否在时间段里；
	caseNum:判断呼梯信号与电梯位置的情景，总共7种情况。
	fromFloor:呼梯信号的初始楼层。
	aimFloor:呼梯信号的目的楼层。
	*******************/
	unsigned int caseNum;
	unsigned int highestFloor;
	unsigned int valueWait;
	unsigned int totalCount = 0;
	unsigned int floorNum = elevatorNum > 0 ? grpInParam[0][FLOOR_NUM_INDEX] : 0;
	unsigned int isClose = 0;
	//判断空间是否存在 是否申请到
	unsigned int addedValue;

	//分配内存
	unsigned int *isfull = (unsigned int *)malloc(elevatorNum * sizeof(unsigned int));
	unsigned int *distCurToFrom = (unsigned int *)malloc(elevatorNum * sizeof(unsigned int));
	double *value = (double *)malloc(elevatorNum * sizeof(double));
	unsigned char *dest = (unsigned char *)malloc(2 * sizeof(unsigned char));  //最远目的层的两个楼层
	unsigned int *costTimeOnce = (unsigned int *)malloc(elevatorNum * sizeof(unsigned int));
	unsigned char *priority = (unsigned char *)malloc(elevatorNum * sizeof(unsigned char));
	//初始化变量
	memset(isfull,0 , elevatorNum * sizeof(unsigned int));
	memset(distCurToFrom, 0, elevatorNum * sizeof(unsigned int));
	memset(value, 0, elevatorNum * sizeof(double));
	memset(dest, 0, 2 * sizeof(unsigned char));
	memset(costTimeOnce, 0, elevatorNum * sizeof(unsigned int));
	memset(priority, 0, elevatorNum * sizeof(unsigned char));
	//float *value = (float *) malloc(elevatorNum * sizeof(float));
	//unsigned int *value = (unsigned int *)malloc(elevatorNum * sizeof(unsigned int));
	
	//inGroupValue表示分配的优先级 2表示脱离群控、盲层、节能模式脱离群控，不指派；
	//1表示vip模式，只指派vip乘客
	//0表示没有前面两者的情况，为最优先
	
	unsigned char cantStopFlag = 0;
	//    unsigned char value[MAX_FLOOR];
	//    unsigned char dest[2];
	//    unsigned char costTimeOnce[MAX_FLOOR];
	unsigned int fullWeight = 9999999;
	unsigned int p_weight = 99999;
	unsigned int door_weight = 999999;
	unsigned int min_index = 0;
	unsigned int stopTimes = 0;
	

	//float adj = 0.0;
	//adj=((float)(adjustParam/10.0));
	//printf("%f",adj);
	dest[0] = 0;
	dest[1] = 0;
	unsigned int tag = 0;
	for (unsigned char n = 0; n < elevatorNum; n++) {
		tag += ((grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX] + grpInParam[n][SAVING_CONTROL_INDEX]) == 0) ? 0 : 1;
		//printf("tag:%d\n",tag);
		//printf("n:%d,grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX]:%d\n",n,grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX]);
		//printf("n:%d,grpInParam[n][SAVING_CONTROL_INDEX]:%d\n",n,grpInParam[n][SAVING_CONTROL_INDEX]);
		//脱离群控
		if (tag == elevatorNum) {
			free(dest);
			free(value);
			free(costTimeOnce);
			free(priority);
			free(distCurToFrom);
			free(isfull);
			return ERROR_OUT_OF_CONTROL;
		}
	}
	if (isAllInvalidFloor(grpInParam, invalidFloor, fromFloor, elevatorNum, floorNum) == 1 ||
		isAllInvalidFloor(grpInParam, invalidFloor, aimFloor, elevatorNum, floorNum) == 1) {
		//如果全部都是盲层或者的情况，则返回一个错误的值
		free(dest);
		free(value);
		free(costTimeOnce);
		free(priority);
		free(distCurToFrom);
		free(isfull);
		return ERROR_INVALID_FLOOR;
	}
	for (int n = 0; n < elevatorNum; n++) {
		//盲层、脱离群控或者是节能模式，则将优先级设置为最低，也就是2，vip模式设置为1，其他情况设置为0，根据分数来判断。
		if (isInvalidFloor(grpInParam, invalidFloor, fromFloor, n, floorNum) == 1 ||
			isInvalidFloor(grpInParam, invalidFloor, aimFloor, n, floorNum) == 1) {
			priority[n] = 2;
		}
		else if (grpInParam[n][IS_VIP_MODE_INDEX] == 1) { priority[n] = 1; }
		else priority[n] = 0;
	}

	

	for (int n = 0; n < elevatorNum; n++) {
		//adjustParam用来校准模拟跟实际时间的差异
		if (priority[n] == 2) { value[n] = 999999999; }
		else {
			costTimeOnce[n] = (grpInParam[n][ACCELERATE_TIME_INDEX] + grpInParam[n][DECELERATE_TIME_INDEX] +grpInParam[n][DOOR_TIME_INDEX] +grpInParam[n][OPEN_INDEX] + grpInParam[n][CLOSE_INDEX]);
			caseNum = judgeCase(fromFloor, aimFloor, grpInParam[n][DIRECTION_INDEX],
				grpInParam[n][POS_INDEX]); //判断信号处于哪个阶段
			getDestination(grpInParam[n][DIRECTION_INDEX], grpInParam[n][POS_INDEX], innerCall[n], outerCall[n], dest,
				grpInParam[n][FLOOR_NUM_INDEX]);//返回最远目的层
			highestFloor = getHighestFloor(fromFloor, aimFloor, dest,
				grpInParam[n][POS_INDEX]); //dest、fromFloor、aimFloor里的最高楼层
			distCurToFrom[n] = getDistCurToFrom(caseNum, fromFloor, grpInParam[n][POS_INDEX], dest);//电梯接信号移动的距离（楼层数）
			totalCount = getCountNew(fromFloor, aimFloor, grpInParam[n][POS_INDEX], grpInParam[n][DIRECTION_INDEX],
				innerCall[n], outerCall[n], grpInParam[n][FLOOR_NUM_INDEX], dest);//电梯接信号过程中需要停梯的次数


			if (grpInParam[n][POS_INDEX] == fromFloor && grpInParam[n][IS_MOVING_INDEX] == 0 &&
				(grpInParam[n][DIRECTION_INDEX] == getDirection(fromFloor, aimFloor) ||
					grpInParam[n][DIRECTION_INDEX] == 0)) {
				valueWait = (grpInParam[n][FLOOR_TIME_INDEX] * distCurToFrom[n] + grpInParam[n][DOOR_TIME_INDEX] +
					grpInParam[n][OPEN_INDEX] + grpInParam[n][CLOSE_INDEX]) * adjustParam;
			}
			else {
				valueWait = (grpInParam[n][FLOOR_TIME_INDEX] * distCurToFrom[n] + costTimeOnce[n] * totalCount) *
					adjustParam;
			}
			//printf("valueWait%f",valueWait);
			isfull[n] = is_full(grpInParam[n][IS_FULL_INDEX], grpInParam[n][CAPACITY_INDEX], innerCall[n], outerCall[n],
				passengerNum, grpInParam[n][DISABLED_NUM_INDEX]);
			//addedValue = getAddedCountNew(fromFloor,aimFloor,grpInParam[n][POS_INDEX],grpInParam[n][DIRECTION_INDEX],innerCall[n], outerCall[n]);
			isClose = isDoorClose(innerCall[n], outerCall[n], grpInParam[n][DO_INDEX], grpInParam[n][IS_MOVING_INDEX]);
			stopTimes = getStopTimes(outerCall[n], innerCall[n], grpInParam[n][DIRECTION_INDEX], highestFloor,
				grpInParam[n][POS_INDEX]);
			cantStopFlag = cantStop(fromFloor, aimFloor, grpInParam[n][POS_INDEX], grpInParam[n][IS_MOVING_INDEX],
				grpInParam[n][DIRECTION_INDEX], innerCall[n], outerCall[n],
				grpInParam[n][DECELERATE_DISTANCE]);
			value[n] = valueWait + isfull[n] * fullWeight + 1.0* stopTimes * costTimeOnce[n] + isClose * door_weight + cantStopFlag * door_weight;
			//grpOutParam[n][0] = isClose;
		}
		//printf("value:%f",value[n]);
		//grpOutParam[n][0] = adjustParam;
		//grpOutParam[n][1] = valueWait;
		//grpOutParam[n][2] = value[n];
		//grpOutParam[0][4] = highestFloor;
		//grpOutParam[n][5] = caseNum;
		//grpOutParam[n][6] = value[n];
		//printf("%d号电梯：距离：%d,totalCount:%d,valueWait:%d,value[n]:%d,isClose:%d,cantStopFlag:%d,grpInParam[n][IS_MOVING]:%d,stopTimes:%d\n\n",n,distCurToFrom[n],totalCount,valueWait,value[n],isClose,cantStopFlag,grpInParam[n][IS_MOVING_INDEX],stopTimes);
		////printf("is_moving:%d,doorSignal:%d,cantStop:%d",grpInParam[n][IS_MOVING_INDEX],grpInParam[n][DO_INDEX],cantStopFlag);
	}
	//printf("min_index0:%d ",min_index);
	//先将min_index置为一个优先级最高的电梯编号
	//只在特定时间段内开启部分分配模式，只派遣特定的电梯去响应普通外招，也就是有几台不会去响应普通外招

	for (int n = 0; n < elevatorNum; n++) {
		//初始化电梯分值最小的编号
		if (priority[n] == 0) {
			//判断开不开启仅部分电梯响应普通外招的模式，=0表示开启
			if (cantPartNormalAssign(fromFloor, aimFloor, priority, elevatorNum, assignSetting, assignSize, grpInParam,
				currentTime) == 0) {
				//如果这台电梯可以响应普通外招，就先将最小的电梯编号设置为这台电梯
				if (grpInParam[n][CAN_NORMAL_ASSIGN] == 1 && isfull[n] != 1) {
					min_index = n;
					break;
				}
				else { continue; }
			}
			else {
				min_index = n;
				break;
			}
		}
		else { continue; }
	}
	unsigned char same_dir_flag = 0;
	if (aimFloor == UP_NUM || aimFloor == DOWN_NUM) {
		//优先返回增加停梯次数最少的，其次返回距离最近的
		unsigned int distance = MAX_FLOOR * 3;

		for (int i = 0; i < elevatorNum; i++) {
			//如果方向相同
			if ((outerCall[i][0][0] == INVALID_NUM || getDirection(outerCall[i][0][0], outerCall[i][1][0]) == getDirection(fromFloor, aimFloor)) && isfull[i] == 0) {
				
				if (priority[i] != 2 && priority[i] != 1) {
					//printf("i:%d,fromFloor:%d,aimFloor:%d,priority:%d,elevatorNum:%d,currentTime:%d\n",i, fromFloor, aimFloor, priority[i], elevatorNum, currentTime);
					if ((cantPartNormalAssign(fromFloor, aimFloor, priority, elevatorNum, assignSetting, assignSize, grpInParam, currentTime) == 0 && grpInParam[i][CAN_NORMAL_ASSIGN] == 1)) {

						if (distCurToFrom[i] < distance) {
							distance = distCurToFrom[i];
							min_index = i;
							same_dir_flag = 1;
						}

					}
					else if (cantPartNormalAssign(fromFloor, aimFloor, priority, elevatorNum, assignSetting, assignSize, grpInParam,currentTime) == 1) {
						if (distCurToFrom[i] < distance) {
							distance = distCurToFrom[i];
							min_index = i;
							same_dir_flag = 1;
						}
					}

					

					////ADD by YJH
					////如果有历史数据参考,则按照停梯次数最少的来
					//if (RECODE && probabilitySize != NULL && maxNearProbability.recodetime > MIN_DIFF) {
					//	/*if (distCurToFrom[i] < distance) {
					//		distance = distCurToFrom[i];
					//		min_index = i;
					//		same_dir_flag = 1;
					//	}*/
					//	if ( additionalDowntime[i] < StopCount) {
					//		StopCount = additionalDowntime[i];
					//		min_index = i;
					//		same_dir_flag = 1;
					//	}
					//	else if (additionalDowntime[i] == StopCount) {
					//		if (distCurToFrom[i] < distance) {
					//			distance = distCurToFrom[i];
					//			min_index = i;
					//			same_dir_flag = 1;
					//		}
					//	}
					//}
					//else {
					//	if (distCurToFrom[i] < distance) {
					//		distance = distCurToFrom[i];
					//		min_index = i;
					//		same_dir_flag = 1;
					//	}
					//}
				}
			}
		}
		if (same_dir_flag == 0) {
			for (int i = 0; i < elevatorNum; i++) {
				if ((cantPartNormalAssign(fromFloor, aimFloor, priority, elevatorNum, assignSetting, assignSize,
					grpInParam, currentTime) == 0 && grpInParam[i][CAN_NORMAL_ASSIGN] == 1)
					|| cantPartNormalAssign(fromFloor, aimFloor, priority, elevatorNum, assignSetting, assignSize,
						grpInParam, currentTime) == 1) {
					if (distCurToFrom[i] < distance && priority[i] != 2 && priority[i] != 1 && isfull[i] == 0) {
						distance = distCurToFrom[i];
						min_index = i;
						same_dir_flag = 1;
					}
				}
			}
		}

		//printf("value:%f\n", value[0]);
		free(dest);
		free(value);
		free(costTimeOnce);
		free(priority);
		free(distCurToFrom);
		free(isfull);
		return min_index;
	}
	//printf("priotiyt:%d",priority[min_index]);
	//printf("min_index1:%d",min_index);
	//残疾人除了残障梯脱离群控的情况以外，其他情况都派梯给残障梯
	if (is_disabled == 1) {
		//如果乘客是残疾人，把它派给残障梯
		for (int n = 0; n < elevatorNum; n++) {
			if (grpInParam[n][IS_DISABLED_INDEX] == 1 && priority[n] != 2 && priority[n] != 1) {
				min_index = n;
			}
		}
		if (grpInParam[min_index][IS_DISABLED_INDEX] == 1 && priority[min_index] != 2) {
			free(dest);
			free(value);
			free(costTimeOnce);
			free(priority);
			free(distCurToFrom);
			free(isfull);
			//printf("disabled\n");
			return min_index;
		}
	}
	//printf("min_index2:%d",min_index);
	for (int n = 0; n < elevatorNum; n++) {
		//优先级为2，则不派此梯
		if (priority[n] == 2) { continue; }
		if (is_vip == 0) {
			////printf("vip=0");
			//普通乘客
			//开启部分电梯响应普通信号功能时
			//不能响应普通信号的就跳过
			if (priority[n] == 1) { continue; }
			else if (value[n] + priority[n] * p_weight <
				value[min_index] + priority[min_index] * p_weight) {
				min_index = n;
			}
		}
		else {
			//vip乘客直接比较value，不用考虑优先级
			if (priority[n] == 1) {
				if (value[n] + priority[n] * p_weight <
					value[min_index] + priority[min_index] * p_weight) {
					min_index = n;
				}
			}
			else {
				if (value[n] < value[min_index]) {
					min_index = n;
				}
			}
		}
	}
	//printf("min_index3:%d",min_index);
	if ((vip_mode == 1 && is_vip == 1) || (passengerNum > 1)) {
		//指派空轿厢给vip
		//最近的那个电梯的编号
		unsigned char distance = MAX_FLOOR;
		unsigned char temp = 0;
		unsigned char min_index = INVALID_NUM;
		for (int n = 0; n < elevatorNum; n++) {
			if (priority[n] == 2) { continue; }
			//innerCall[n][0]== INVALID_NUM && outerCall[n][0][0] ==INVALID_NUM表示是空轿厢
			if (innerCall[n][0] == INVALID_NUM && outerCall[n][0][0] == INVALID_NUM &&
				(isInvalidFloor(grpInParam, invalidFloor, fromFloor, n, floorNum) == 0 &&
					isInvalidFloor(grpInParam, invalidFloor, aimFloor, n, floorNum) == 0)) {
				temp = grpInParam[n][POS_INDEX] > fromFloor ? grpInParam[n][POS_INDEX] - fromFloor : fromFloor -
					grpInParam[n][POS_INDEX];
				if (distance > temp && isfull[n] == 0) {
					min_index = n;
					distance = temp;
				}
			}
		}
		if (min_index != INVALID_NUM && priority[min_index] != 2 && isfull[min_index] == 0 &&
			(innerCall[min_index][0] == INVALID_NUM && outerCall[min_index][0][0] == INVALID_NUM)) {
			free(dest);
			free(value);
			free(costTimeOnce);
			free(priority);
			free(distCurToFrom);
			free(isfull);
			//printf("multiple\n");
			return min_index;
		}
	}
	//printf("min_index4:%d",min_index);
	for (unsigned char n = 0; n < elevatorNum; n++) {
		//判断是否有想同出发层和目的层的信号
		unsigned int innerSameFlag = 0;
		unsigned int outerSameFlag = 0;
		innerSameFlag = isSameInInnerCall(fromFloor, aimFloor, innerCall[n], grpInParam[n][POS_INDEX],
			grpInParam[n][IS_MOVING_INDEX]);
		outerSameFlag = isSameInOuterCall(fromFloor, aimFloor, outerCall[n]);
		if ((innerSameFlag == 1 || outerSameFlag == 1) && isfull[n] == 0 &&
			(isInvalidFloor(grpInParam, invalidFloor, fromFloor, n, floorNum) == 0 &&
				isInvalidFloor(grpInParam, invalidFloor, aimFloor, n, floorNum) == 0) &&
				((grpInParam[n][IS_VIP_MODE_INDEX] == 0) || (grpInParam[n][IS_VIP_MODE_INDEX] == 1 && is_vip == 1))) {
			free(dest);
			free(value);
			free(costTimeOnce);
			free(priority);
			free(distCurToFrom);
			free(isfull);
			//printf("same\n");
			return n;
		}
		////printf("id:%d,fromFloor%d,aimFloor%d,innerCall[n]%s,grpInParam[n][POS_INDEX]%d",n,fromFloor,aimFloor,innerCall[n],grpInParam[n][POS_INDEX]);
	}
	free(dest);
	free(value);
	free(costTimeOnce);
	free(priority);
	free(distCurToFrom);
	free(isfull);
	//printf("normal\n");
	return min_index;
}

unsigned char
isVipOutOfControl(unsigned char is_vip,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char elevatorNum) {
	unsigned char count = 0;
	unsigned int flag = 0;
	if (is_vip == 1) {
		for (int n = 0; n < elevatorNum; n++) {
			if (grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX] == 1 || grpInParam[n][IS_VIP_MODE_INDEX] == 1 ||
				grpInParam[n][SAVING_CONTROL_INDEX] == 1) {
				count = count + 1;
			}
		}
		unsigned char e = (elevatorNum >> 1);
		if (count >= e) flag = 0;
		else flag = 1;
	}
	return flag;
}

//判断是否连续空招
unsigned char isSustainCallEmpty(unsigned int *emptyCallBuffer,
	unsigned int bufferLen) {
	unsigned char flag = 0;
	if (*emptyCallBuffer >= bufferLen) { flag = 1; }
	return flag;
}

//判断当前内外招是否全为空
unsigned char
isAllCallEmpty(unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned char elevatorNum,
	unsigned int(*grpInParam)[IN_PARAM_NUM]) {
	unsigned char innerCallEmpty = 1;//内招是否为空
	unsigned char outerCallEmpty = 1;//外招是否为空
	unsigned char doorSignal = 0;
	unsigned char hasMovingEle = 0;
	for (int i = 0; i < elevatorNum; i++) {
		for (int j = 0; j < INNER_CALL_SIZE; j++) {
			if (innerCall[i][j] != INVALID_NUM) {
				innerCallEmpty = 0;
				break;
			}
		}
		if (innerCallEmpty == 0) {
			break;
		}
	}
	for (int i = 0; i < elevatorNum; i++) {
		if (grpInParam[i][OUT_OF_GROUP_CONTROL_INDEX] == 1 || grpInParam[i][SAVING_CONTROL_INDEX] == 1) { continue; }
		for (int j = 0; j < OUTER_CALL_SIZE; j++) {
			if (outerCall[i][0][j] != INVALID_NUM && outerCall[i][1][j] != INVALID_NUM) {
				outerCallEmpty = 0;
				break;
			}
		}
		if (outerCallEmpty == 0) {
			break;
		}
	}
	for (int i = 0; i < elevatorNum; i++) {
		if (grpInParam[i][DO_INDEX] == 1) { doorSignal = 1; }
	}
	for (int i = 0; i < elevatorNum; i++) {
		if (grpInParam[i][IS_MOVING_INDEX] == 1 && grpInParam[i][OUT_OF_GROUP_CONTROL_INDEX] != 0 &&
			grpInParam[i][SAVING_CONTROL_INDEX] != 0) {
			hasMovingEle = 1;
			break;
		}
	}
	return innerCallEmpty == 1 && outerCallEmpty == 1 && doorSignal == 0 && hasMovingEle == 0;
}

int inGroupControl(unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char elevatorNum) {
	int inGroupControlNum = 0;
	//求出电梯距离最近的电梯编号
	for (int i = 0; i < elevatorNum; i++) {
		if (grpInParam[i][OUT_OF_GROUP_CONTROL_INDEX] == 0 && grpInParam[i][SAVING_CONTROL_INDEX] == 0) { //脱离群控默认为盲层
			inGroupControlNum++;
		}
	}
	return inGroupControlNum;
}

struct FloorSection {
	unsigned int startFloor;
	unsigned int endFloor;
	unsigned int validFloor;//有效的楼层数
};

//判断一个电梯在区间内
int hasElevatorInSection(unsigned int(*grpInParam)[IN_PARAM_NUM],
	struct FloorSection floorSection,
	unsigned int *currentFloors,
	unsigned char elevatorNum) {
	int hasFlag = 0;
	for (int i = 0; i < elevatorNum; i++) {
		if (grpInParam[i][OUT_OF_GROUP_CONTROL_INDEX] == 1 || grpInParam[i][SAVING_CONTROL_INDEX] == 1) {
			continue;
		}
		int currentFloor = currentFloors[i];
		if (currentFloor <= floorSection.endFloor && currentFloor >= floorSection.startFloor) {
			hasFlag = 1;
			break;
		}
	}
	return hasFlag;
}

//是否所有电梯都在区间内
int
hasAllElevatorInSection(unsigned int(*grpInParam)[IN_PARAM_NUM],
	struct FloorSection *floorSections,
	unsigned int *currentFloors,
	unsigned char elevatorNum,
	int inGroupControlNum,
	int idleMode) {
	int hasAllFlag = 1;
	int destFloorSum = 0;
	for (int i = 0; i < inGroupControlNum; i++) {
		struct FloorSection floorSection = floorSections[i];
		destFloorSum += floorSection.startFloor;
		if (hasElevatorInSection(grpInParam, floorSection, currentFloors, elevatorNum) == 0) {
			hasAllFlag = 0;
			break;
		}
	}
	if (idleMode == IDLE_MODE_ASSIGN) {
		int currentFloorSum = 0;
		for (int i = 0; i < elevatorNum; i++) {
			currentFloorSum += currentFloors[i];
		}
		return (destFloorSum == currentFloorSum) && hasAllFlag;
	}
	else {
		return hasAllFlag;
	}
}

//某台电梯某个区间是否可达
int isSectionAccessible(unsigned int(*grpInParam)[IN_PARAM_NUM],
	struct FloorSection floorSection,
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned int elevatorIndex,
	unsigned int floorNum) {
	unsigned int startFloor = floorSection.startFloor;
	unsigned int endFloor = floorSection.endFloor;
	int accessibleFlag = 0;
	for (unsigned int i = startFloor; i <= endFloor; i++) {
		if (isInvalidFloor(grpInParam, invalidFloor, i, elevatorIndex, floorNum) == 0) {
			accessibleFlag = 1;
		}
	}
	return accessibleFlag;
}

void scatter(unsigned int(*grpInParam)[IN_PARAM_NUM],
	struct FloorSection *floorSections,
	unsigned int *currentFloors,
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned char elevatorNum,
	unsigned int floorNum,
	unsigned int *scatterIndex,
	int inGroupControlNum) {
	unsigned int originIndex[MAX_ELEVATOR];
	for (int i = 0; i < elevatorNum; i++) {
		originIndex[i] = i;
	}
	for (int i = 0; i < elevatorNum - 1; i++) {
		for (int j = 0; j < elevatorNum - 1 - i; j++) {
			if (currentFloors[j] > currentFloors[j + 1]) {
				unsigned int temp = currentFloors[j];
				currentFloors[j] = currentFloors[j + 1];
				currentFloors[j + 1] = temp;
				unsigned int temp2 = originIndex[j];
				originIndex[j] = originIndex[j + 1];
				originIndex[j + 1] = temp2;
			}
		}
	}
	for (int i = 0; i < inGroupControlNum; i++) {
		struct FloorSection floorSection = floorSections[i];
		for (int j = 0; j < elevatorNum; j++) {
			int elevatorIndex = originIndex[j];
			int visitedFlag = 0;
			for (int k = 0; k <= j; k++) {
				if (elevatorIndex == scatterIndex[k]) {
					visitedFlag = 1;
					break;
				}
			}
			if (visitedFlag == 0 &&
				isSectionAccessible(grpInParam, floorSections[i], invalidFloor, elevatorIndex, floorNum)) {
				scatterIndex[i] = elevatorIndex;
				break;
			}
		}
	}
}

unsigned char
closeToBaseEle(unsigned char baseFloor,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char elevatorNum) {
	//返回与电梯最近的电梯编号
	unsigned char min_index = 0;
	unsigned char value = INVALID_NUM;
	//存在所有电梯都是out_of_group_control_index的情况以及没有顺向电梯的情况
	for (int n = 0; n < elevatorNum; n++) {
		if (grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX] == 1 || grpInParam[n][IS_VIP_MODE_INDEX] == 1 ||
			grpInParam[n][IS_FULL_INDEX] == 1) {
			continue;
		}
		else {
			unsigned char temp =
				grpInParam[n][POS_INDEX] > baseFloor ? grpInParam[n][POS_INDEX] - baseFloor : baseFloor -
				grpInParam[n][POS_INDEX];
			if (temp < value) {
				min_index = n;
				value = temp;
			}
		}
	}
	return min_index;
}

void
doScatter(unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned int *currentFloors,
	struct FloorSection *floorSections,
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned int *scatterIndex,
	unsigned char *dest,
	unsigned char elevatorNum,
	unsigned int floorNum,
	int inGroupControlNum,
	unsigned char isBaseMode,
	unsigned char baseFloor) {
	unsigned char min_index = closeToBaseEle(baseFloor, grpInParam, elevatorNum);
	for (int i = 0; i < inGroupControlNum; i++) {
		struct FloorSection floorSection = floorSections[i];
		int startFloor = floorSection.startFloor;
		int endFloor = floorSection.endFloor;
		//20200611增加
		for (int j = startFloor; j <= endFloor; j++) {
			if (isInvalidFloor(grpInParam, invalidFloor, j, scatterIndex[i], floorNum) == 0) {
				dest[scatterIndex[i]] = j;
				break;
			}
		}
	}
	//20200611增加
	if (isBaseMode == 1) { dest[min_index] = baseFloor; }
	for (int i = 0; i < elevatorNum; i++) {
		if (dest[i] == grpInParam[i][POS_INDEX]) {
			dest[i] = INVALID_NUM;
		}
	}
}

void uniformScatter(unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned char elevatorNum,
	unsigned char *dest,
	unsigned char isBaseMode,
	unsigned char baseFloor) {
	unsigned int floorNum = elevatorNum > 0 ? grpInParam[0][FLOOR_NUM_INDEX] : 0;//几层楼
	unsigned int currentFloors[MAX_ELEVATOR];
	for (int i = 0; i < elevatorNum; i++) {
		currentFloors[i] = grpInParam[i][POS_INDEX];
	}
	int allInvalidFloor = 0;
	for (int i = 0; i < floorNum; i++) {
		if (isAllInvalidFloor(grpInParam, invalidFloor, i, elevatorNum, floorNum) == 1) {
			allInvalidFloor++;
		}
	}
	int inGroupControlNum = inGroupControl(grpInParam, elevatorNum);
	if (inGroupControlNum == 0) {//所有电梯都不可群控
		return;
	}
	//开始分区
	struct FloorSection floorSections[MAX_ELEVATOR];//楼层区间
	unsigned int scatterIndex[MAX_ELEVATOR];//分配区间的索引
	for (int i = 0; i < MAX_ELEVATOR; i++) {
		scatterIndex[i] = INVALID_NUM;
	}
	int avgFloor = (floorNum - allInvalidFloor) / inGroupControlNum;
	int remainderFloor = (floorNum - allInvalidFloor) % inGroupControlNum;
	int startFloor = 0;
	int endFloor = 0;
	for (int i = 0; i < inGroupControlNum; i++) {
		unsigned int validFloor = i < remainderFloor ? avgFloor + 1 : avgFloor;
		floorSections[i].validFloor = validFloor;
		int j = 0;
		while (j < validFloor) {
			if (isAllInvalidFloor(grpInParam, invalidFloor, endFloor, elevatorNum, floorNum)) {

			}
			else {
				j++;
			}
			endFloor++;
		}
		floorSections[i].startFloor = startFloor;
		floorSections[i].endFloor = endFloor - 1;
		startFloor = endFloor;
	}
	//分区结束
	if (hasAllElevatorInSection(grpInParam, floorSections, currentFloors, elevatorNum, inGroupControlNum,
		IDLE_MODE_UNIFORM) == 1) {//所有电梯都在区间内就不用调度
		return;
	}
	scatter(grpInParam, floorSections, currentFloors, invalidFloor, elevatorNum, floorNum, scatterIndex,
		inGroupControlNum);
	doScatter(grpInParam, currentFloors, floorSections, invalidFloor, scatterIndex, dest, elevatorNum, floorNum,
		inGroupControlNum, isBaseMode, baseFloor);
}


void assignScatter(struct IdleSetting idleSetting,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char elevatorNum,
	unsigned char *dest,
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned char isBaseMode,
	unsigned char baseFloor) {
	int inGroupControlNum = inGroupControl(grpInParam, elevatorNum);
	unsigned int floorNum = elevatorNum > 0 ? grpInParam[0][FLOOR_NUM_INDEX] : 0;//几层楼
	if (inGroupControlNum == 0) {//所有电梯都不可群控
		return;
	}
	unsigned int currentFloors[MAX_ELEVATOR];
	for (int i = 0; i < elevatorNum; i++) {
		currentFloors[i] = grpInParam[i][POS_INDEX];
	}
	//开始分区
	struct FloorSection floorSections[MAX_ELEVATOR];//楼层区间
	unsigned int scatterIndex[MAX_ELEVATOR];//分配区间的索引
	for (int i = 0; i < MAX_ELEVATOR; i++) {
		scatterIndex[i] = INVALID_NUM;
	}
	//对楼层排序
	for (int i = 0; i < elevatorNum - 1; i++) {
		for (int j = 0; j < elevatorNum - 1 - i; j++) {
			if (idleSetting.idleFloor[j] > idleSetting.idleFloor[j + 1]) {
				unsigned int temp = idleSetting.idleFloor[j];
				idleSetting.idleFloor[j] = idleSetting.idleFloor[j + 1];
				idleSetting.idleFloor[j + 1] = temp;
			}
		}
	}
	for (int i = 0; i < inGroupControlNum; i++) {
		floorSections[i].startFloor = idleSetting.idleFloor[i];
		floorSections[i].endFloor = idleSetting.idleFloor[i];
		floorSections[i].validFloor =
			isAllInvalidFloor(grpInParam, invalidFloor, idleSetting.idleFloor[i], elevatorNum, floorNum) == 0;
		if (floorSections[i].validFloor == 0) {
			return;
		}
	}
	//代码运行到这表示分区成功
	scatter(grpInParam, floorSections, currentFloors, invalidFloor, elevatorNum, floorNum, scatterIndex,
		inGroupControlNum);
	doScatter(grpInParam, currentFloors, floorSections, invalidFloor, scatterIndex, dest, elevatorNum, floorNum,
		inGroupControlNum, isBaseMode, baseFloor);
}

//空闲分散待梯
void idleScatter(struct IdleSetting idleSetting,
	unsigned int currentTime,
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned char elevatorNum,
	unsigned char *dest,
	unsigned char isIdling,
	struct ManualSetting *manualSetting,
	unsigned char manualSize,
	unsigned char isBaseMode,
	unsigned char baseFloor,
	struct GrpOutParam *grpOutParam) {
	memset(dest, INVALID_NUM, sizeof(*dest) * elevatorNum);
	unsigned char mode = idleSetting.mode;
	unsigned char idleBufferLen = idleSetting.idleBufferLen;
	unsigned char allEmpty = isAllCallEmpty(innerCall, outerCall, elevatorNum, grpInParam);
	unsigned int *idleBuffer = idleSetting.idleBuffer;
	unsigned char elevatorMode = grpOutParam->elevatorMode;
	unsigned char *destGrpOutParam = grpOutParam->dest;
	if (idleBufferLen <= 0) { return; }
	for (int i = 0; i < manualSize; i++) {
		//while中执行完所有的内容 如果超过这个数组内容
		unsigned int startTime = manualSetting[i].startTime;
		unsigned int endTime = manualSetting[i].endTime;
		if (currentTime >= startTime && currentTime <= endTime) {
			//在手动指定时间的这段时间不用空闲分配
			//节能模式的时候需要空闲分配
			if (manualSetting[i].elevatorMode == UP_PEAK_MODE || manualSetting[i].elevatorMode == DOWN_PEAK_MODE) {
				*idleBuffer = 0;
				return;
			}
		}
	}
	if (allEmpty == 1) { *idleBuffer += 1; }
	else { *idleBuffer = 0; }
	if (elevatorMode == UP_PEAK_MODE || elevatorMode == DOWN_PEAK_MODE || elevatorMode == INTER_PEAK_MODE) {
		*idleBuffer = 0;
		return;
	}
	if (isSustainCallEmpty(idleBuffer, idleBufferLen) == 0 || isIdling == 1) {
		return;
	}
	switch (mode) {
	case IDLE_MODE_NONE:
		break;
	case IDLE_MODE_UNIFORM:
		uniformScatter(grpInParam, invalidFloor, elevatorNum, dest, isBaseMode, baseFloor);
		break;
	case IDLE_MODE_ASSIGN:
		assignScatter(idleSetting, grpInParam, elevatorNum, dest, invalidFloor, isBaseMode, baseFloor);
		break;
	default:
		break;
	}
}

//分配任务
struct InterAssignment {
	int peakFloor;//高峰楼层
	int requiredElevatorNum;//需要分配的电梯数
	int allocatedElevatorNum;//已分配的电梯数
};

unsigned int inInterPeak(unsigned int *interCount, unsigned int floorNum, unsigned int interPeakCount) {
	//return：有多少层进入层间高峰
	int greaterPeakCount = 0;
	for (int i = 0; i < floorNum; i++) {
		if (interCount[i] > interPeakCount && interPeakCount > 0) {
			greaterPeakCount++;
		}
	}
	if (greaterPeakCount>2) {
		greaterPeakCount = 2;
	}
	return greaterPeakCount;
}

void getPeakFloor(unsigned int *interCount, unsigned int floorNum, unsigned int interPeakCount,
	unsigned int *interPeakFloor) {
	//将进入高峰模式的层的Index塞入interPeakFloor
	int index = 0;
	for (int i = 0; i < floorNum; i++) {
		if (interCount[i] > interPeakCount && interPeakCount > 0) {
			interPeakFloor[index] = i;
			index++;
		}
	}
}

void oneFloorInterAssignmentBuilder(
	struct InterAssignment *assignments,
	unsigned int greaterPeakCount,
	unsigned int *interPeakFloor,
	struct AutoSetting autoSetting,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	struct GrpOutParam *grpOutParam,
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned char elevatorNum) {
	int idleElevatorNum = 0;
	int upPeakFloor = interPeakFloor[0];
	//统计需要分配多少电梯
	for (int i = 0; i < elevatorNum; i++) {
		if (isCallEmpty(innerCall[i], outerCall[i], grpInParam[i][DO_INDEX]) == 1) {
			//空闲
			idleElevatorNum++;
		}
	}
	int requiredElevatorNum = idleElevatorNum * 2 / 3;
	assignments[0].requiredElevatorNum = requiredElevatorNum;
}

void twoFloorInterAssignmentBuilder(
	struct InterAssignment *assignments,
	unsigned int greaterPeakCount,
	unsigned int *interPeakFloor,
	struct AutoSetting autoSetting,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	struct GrpOutParam *grpOutParam,
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned char elevatorNum) {
	int idleElevatorNum = 0;
	int upPeakFloor = interPeakFloor[0];
	//统计需要分配多少电梯
	for (int i = 0; i < elevatorNum; i++) {
		if (isCallEmpty(innerCall[i], outerCall[i], grpInParam[i][DO_INDEX]) == 1) {
			//空闲
			idleElevatorNum++;
		}
	}
	int assignmentIndex = 0;
	while (idleElevatorNum > 0) {
		assignments[assignmentIndex].requiredElevatorNum++;
		idleElevatorNum--;
		assignmentIndex++;
		assignmentIndex = assignmentIndex % greaterPeakCount;
	}
}

void doInterAssign(unsigned int greaterPeakCount,
	struct InterAssignment *assignments,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	struct GrpOutParam *grpOutParam,
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned char elevatorNum) {
	unsigned char *dest = grpOutParam->dest;
	for (int i = 0; i < greaterPeakCount; i++) {
		int peakFloor = assignments[i].peakFloor;
		//开始分配
		while (assignments[i].requiredElevatorNum > assignments[i].allocatedElevatorNum) {
			for (int j = 0; j < elevatorNum; j++) {
				if (isCallEmpty(innerCall[j], outerCall[j], grpInParam[j][DO_INDEX]) == 1 && dest[j] == INVALID_NUM) {
					dest[j] = peakFloor;
					assignments[i].allocatedElevatorNum++;
					break;
				}
			}
		}
	}
}

void interAssigner(
	unsigned int greaterPeakCount,
	unsigned int *interPeakFloor,
	struct AutoSetting autoSetting,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	struct GrpOutParam *grpOutParam,
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned char elevatorNum) {
	//    printf("interAssigner-----------------------------------------------\n");
	//    printf("OriginDest:");
	//    for(int i=0;i<elevatorNum;i++){
	//        printf("%d ",grpOutParam->dest[i]);
	//    }
	//    printf("\n");
	//    printf("interPeakFloor:");
	//    for(int i=0;i<greaterPeakCount;i++){
	//        printf("%d ",interPeakFloor[i]);
	//    }
	//    printf("\n");
	//    printf("interPeakMode:%d\n", grpOutParam->interPeakMode);
	unsigned int *interPeakTime = autoSetting.interPeakTime;
	unsigned int interPeakDelayTime = autoSetting.interPeakDelayTime;
	//创建任务，并初始化
	struct InterAssignment assignments[2];
	for (int i = 0; i < greaterPeakCount; i++) {
		assignments[i].peakFloor = interPeakFloor[i];
		assignments[i].requiredElevatorNum = 0;
		assignments[i].allocatedElevatorNum = 0;
	}
	//初始化数据
	unsigned char *dest = grpOutParam->dest;
	unsigned int *savingBuffer = autoSetting.savingBuffer;
	grpOutParam->elevatorMode = INTER_PEAK_MODE;
	*savingBuffer = 0;
	for (int j = 0; j < elevatorNum; j++) {
		dest[j] = INVALID_NUM;
		grpOutParam->savingControl[j] = 0;
	}
	//判断当前模式
	if (grpOutParam->interPeakMode == INTER_PEAK_NO_MODE) {//初次进入高峰
		if (greaterPeakCount == 1) {//初次进入层间高峰，必定只有一层
									//            printf("单层进入\n");
			grpOutParam->interPeakMode = ONE_PEAK_FLOOR_MODE;
			oneFloorInterAssignmentBuilder(assignments, greaterPeakCount, interPeakFloor, autoSetting, grpInParam,
				grpOutParam, innerCall, outerCall, elevatorNum);
			doInterAssign(greaterPeakCount,
				assignments,
				grpInParam,
				grpOutParam,
				innerCall,
				outerCall,
				elevatorNum);
		}
		else if (greaterPeakCount == 0) {
			grpOutParam->interPeakMode = INTER_PEAK_NO_MODE;
		}
	}
	else if (grpOutParam->interPeakMode == ONE_PEAK_FLOOR_MODE) {//之前为一层进入
		if (greaterPeakCount == 1) {//再次进入单层高峰
									//            printf("二次单层进入\n");
			grpOutParam->interPeakMode = ONE_PEAK_FLOOR_MODE;
		}
		else if (greaterPeakCount >= 1) {
			//            printf("多层进入\n");
			grpOutParam->interPeakMode = TWO_PEAK_FLOOR_MODE;
			twoFloorInterAssignmentBuilder(assignments,
				greaterPeakCount,
				interPeakFloor,
				autoSetting,
				grpInParam,
				grpOutParam,
				innerCall,
				outerCall,
				elevatorNum);
			doInterAssign(greaterPeakCount,
				assignments,
				grpInParam,
				grpOutParam,
				innerCall,
				outerCall,
				elevatorNum);
		}
		else if (greaterPeakCount == 0) {
			//            printf("单层开始退出\n");
			grpOutParam->interPeakMode = INTER_PEAK_OUT_MODE;
		}
	}
	else if (grpOutParam->interPeakMode == TWO_PEAK_FLOOR_MODE) {
		if (greaterPeakCount <= 1) {
			//            printf("多层开始退出\n");
			*interPeakTime = 0;
			grpOutParam->interPeakMode = INTER_PEAK_OUT_MODE;
		}
		else {
			//            printf("二次多层\n");
			grpOutParam->interPeakMode = TWO_PEAK_FLOOR_MODE;
		}
	}
	else if (grpOutParam->interPeakMode == INTER_PEAK_OUT_MODE) {

	}
	else {
		grpOutParam->interPeakMode = INTER_PEAK_NO_MODE;
	}

	//打印任务
	//    printf("assignments:");
	//    for (int i = 0; i < greaterPeakCount; i++) {
	//        printf("|");
	//        printf("%d ", assignments[i].peakFloor);
	//        printf("%d ", assignments[i].requiredElevatorNum);
	//        printf("%d ", assignments[i].allocatedElevatorNum);
	//        printf("|");
	//    }
	//    printf("\n");
	//    //按任务分配
	//
	//    printf("AllocatedDest:");
	//    for (int i = 0; i < elevatorNum; i++) {
	//        printf("%d ", grpOutParam->dest[i]);
	//    }
	//    printf("\n");
}

void autoAssigner(struct AutoSetting autoSetting,
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char elevatorNum,
	struct GrpOutParam *grpOutParam,
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned int currentTime) {
	//自动分配
	//printf("最初的elevatorMode:%d\n",grpOutParam->elevatorMode);
	unsigned int floorNum = elevatorNum > 0 ? grpInParam[0][FLOOR_NUM_INDEX] : 0;
	unsigned char upPeakFloor = autoSetting.upPeakFloor;
	unsigned int upPeakCount = autoSetting.upPeakCount;
	unsigned int *upPeakTime = autoSetting.upPeakTime;
	unsigned int upPeakDelayTime = autoSetting.upPeakDelayTime;
	unsigned int upPeakRecordDelayTime = autoSetting.upPeakRecordDelayTime;
	unsigned int interPeakCount = autoSetting.interPeakCount;
	unsigned int *interPeakTime = autoSetting.interPeakTime;
	unsigned int interPeakDelayTime = autoSetting.interPeakDelayTime;
	unsigned int elevatorMode = grpOutParam->elevatorMode;
	unsigned int savingCount = autoSetting.savingCount;
	unsigned int *savingBuffer = autoSetting.savingBuffer;
	unsigned int savingBufferLen = autoSetting.savingBufferLen;//>0
	unsigned char *savingControl = autoSetting.savingControl;
	unsigned int upCount = 0;
	unsigned int saveCount = 0;
	unsigned int interCount[MAX_FLOOR];
	for (int i = 0; i < floorNum; i++) {
		interCount[i] = 0;
	}
	//遍历电梯
	for (int i = 0; i < elevatorNum; i++) {
		//遍历内招
		for (int j = 0; j < INNER_CALL_SIZE; j++) {
			if (innerCall[i][j] == INVALID_NUM) { break; }
			else {
				if (innerCall[i][j] != INVALID_NUM) {
					saveCount++;
				}
			}
		}
		//遍历外招
		for (int k = 0; k < OUTER_CALL_SIZE; k++) {
			if (outerCall[i][0][k] == INVALID_NUM) { break; }
			else {
				if (outerCall[i][0][k] == upPeakFloor && getDirection(outerCall[i][0][k], outerCall[i][1][k]) == 1) {
					upCount++;
				}
				if (outerCall[i][0][k] != INVALID_NUM && outerCall[i][1][k] != INVALID_NUM) {
					saveCount++;
					interCount[outerCall[i][0][k]]++;
				}
			}
		}
		if (grpInParam[i][POS_INDEX] == upPeakFloor && grpInParam[i][DIRECTION_INDEX] == 1) {
			for (int k = 0; k < INNER_CALL_SIZE; k++) {
				if (innerCall[i][k] != INVALID_NUM && innerCall[i][k] > upPeakFloor) {
					upCount++;
				}
			}
		}
	}
	//add by yjh
	bool isUp_peak = false;
	int ss = -1;
	for (int i = 0; i < elevatorNum; i++) {
		//如果电梯正处于上行，且刚好在一楼
		if (grpInParam[i][IS_MOVING_INDEX] == 1 && grpInParam[i][POS_INDEX] == upPeakFloor && grpInParam[i][DIRECTION_INDEX] == 1) {
			if (grpInParam[i][IS_FULL_INDEX] || grpInParam[i][WEIGHT] > grpInParam[i][CAPACITY_INDEX] * WEIGHT_PER_PASSAGE * 0.9) {
				ss = i;
				isUp_peak = true;
			}
		}
	}
	if (isUp_peak) {
		//printf("currentTime: %d,%d\n", currentTime, ss);
	}
	
	//计算有多少层进入层间高峰
	unsigned int greaterPeakCount = inInterPeak(interCount, floorNum, interPeakCount);
	unsigned int interPeakFloor[MAX_FLOOR];
	//返回层间高峰的楼层
	getPeakFloor(interCount, floorNum, interPeakCount, interPeakFloor);
	//    printf("--------------------------------------------------------\n");
	//    printf("interPeakFloor：");
	//    for (int i = 0; i < greaterPeakCount; i++) {
	//        printf("%d ", interPeakFloor[i]);
	//    }
	//    printf("\n");
	//    printf("elevatorMode:%d\n", elevatorMode);
	//    printf("--------------------------------------------------------\n");
	switch (elevatorMode) {
	case NONE_MODE: //刚触发的情况
		if ((upCount > upPeakCount || isUp_peak) && upPeakCount != 0) {  //如果触发，则计数清零
														   //printf("进入...NONE_MODE1\n");
														   //upPeakCount=0时，表示不启动
														   //只要进入该模式，就有延后时间  判定进入上行高峰的方法是只要触发了，就进入。
														   //进入之后就开始计数延时时间upPeakTime,如果时间没有到延时时间，则继续上高峰模式
			*upPeakTime = 0; //从0计时
			grpOutParam->elevatorMode = UP_PEAK_MODE;
			for (int j = 0; j < elevatorNum; j++) {
				if (grpInParam[j][IS_AUTO_ASSIGNING_INDEX] == 1) { continue; }
				grpOutParam->savingControl[j] = 0;
				if (isCallEmpty(innerCall[j], outerCall[j], grpInParam[j][DO_INDEX]) == 1 &&
					isInvalidFloor(grpInParam, invalidFloor, upPeakFloor, j, floorNum) == 0 &&
					grpInParam[j][POS_INDEX] != upPeakFloor) {
					//判断为空闲状态
					grpOutParam->dest[j] = upPeakFloor;
				}
				else {
					grpOutParam->dest[j] = INVALID_NUM;
				}
			}
			*savingBuffer = 0;
		}
		else if (greaterPeakCount > 0) {
			interAssigner(greaterPeakCount, interPeakFloor, autoSetting, grpInParam, grpOutParam, innerCall,
				outerCall, elevatorNum);
		}
		else if (saveCount < savingCount) {
			//数量少于设定的阈值时，就会让计时器的值加1，否则让值直接变0
			//printf("进入...NONE_MODE3\n");
			if (*savingBuffer < savingBufferLen) { *savingBuffer += 1; }
			if (*savingBuffer >= savingBufferLen && savingBufferLen != 0) {
				//printf("进入节能模式\n");
				grpOutParam->elevatorMode = SAVING_MODE;
				for (int j = 0; j < elevatorNum; j++) {
					//if(grpInParam[j][IS_AUTO_ASSIGNING_INDEX] == 1){continue;}
					grpOutParam->savingControl[j] = savingControl[j];
					grpOutParam->dest[j] = INVALID_NUM;
				}
			}
		}
		else {
			//printf("进入...NONE_MODE4\n");
			*savingBuffer = 0;
			for (int j = 0; j < elevatorNum; j++) {
				grpOutParam->dest[j] = INVALID_NUM;
				grpOutParam->savingControl[j] = 0;
			}
			grpOutParam->elevatorMode = NONE_MODE;
		}
		break;
	case UP_PEAK_MODE:
		if ( (upCount > upPeakCount || isUp_peak)&& upPeakCount != 0 ) {  //如果触发，则计数清零
														  //printf("进入...UP_PEAK_MODE1\n");
														  //upPeakCount=0时，表示不启动
														  //只要进入该模式，就有延后时间  判定进入上行高峰的方法是只要触发了，就进入。
														  //进入之后就开始计数延时时间upPeakTime,如果时间没有到延时时间，则继续上高峰模式
			

			*upPeakTime = 0; //从0计时
			grpOutParam->elevatorMode = UP_PEAK_MODE;
			for (int j = 0; j < elevatorNum; j++) {
				//if(grpInParam[j][IS_AUTO_ASSIGNING_INDEX] == 1){continue;}
				grpOutParam->savingControl[j] = 0;
				if (isCallEmpty(innerCall[j], outerCall[j], grpInParam[j][DO_INDEX]) == 1 &&
					isInvalidFloor(grpInParam, invalidFloor, upPeakFloor, j, floorNum) == 0 &&
					grpInParam[j][POS_INDEX] != upPeakFloor) {
					//判断为空闲状态
					grpOutParam->dest[j] = upPeakFloor;
				}
				else {
					grpOutParam->dest[j] = INVALID_NUM;
				}
			}
			*savingBuffer = 0;
		}
		else {
			*upPeakTime += 1;
			if (*upPeakTime > upPeakDelayTime) {
				//printf("进入...UP_PEAK_MODE2\n");
				*savingBuffer = 0;
				for (int j = 0; j < elevatorNum; j++) {
					grpOutParam->dest[j] = INVALID_NUM;
					grpOutParam->savingControl[j] = 0;
				}
				grpOutParam->elevatorMode = NONE_MODE;
				*upPeakTime = 0;
			}
			else {
				//printf("进入...UP_PEAK_MODE3\n");
				grpOutParam->elevatorMode = UP_PEAK_MODE;
				for (int j = 0; j < elevatorNum; j++) {
					//if(grpInParam[j][IS_AUTO_ASSIGNING_INDEX] == 1){continue;}
					grpOutParam->savingControl[j] = 0;
					if (isCallEmpty(innerCall[j], outerCall[j], grpInParam[j][DO_INDEX]) == 1 &&
						isInvalidFloor(grpInParam, invalidFloor, upPeakFloor, j, floorNum) == 0 &&
						grpInParam[j][POS_INDEX] != upPeakFloor) {
						//判断为空闲状态
						grpOutParam->dest[j] = upPeakFloor;
					}
					else {
						grpOutParam->dest[j] = INVALID_NUM;
					}
				}
				*savingBuffer = 0;
			}
		}
		break;
	case INTER_PEAK_MODE:
		if (grpOutParam->interPeakMode != INTER_PEAK_OUT_MODE) {
			*interPeakTime = 0;
			interAssigner(greaterPeakCount, interPeakFloor, autoSetting, grpInParam, grpOutParam, innerCall,
				outerCall, elevatorNum);
		}
		else {
			if (greaterPeakCount == 0) {
				*interPeakTime += 1;
				if (*interPeakTime > interPeakDelayTime) {//退出高峰成功
														  //                        printf("退出成功\n");
					*savingBuffer = 0;
					for (int j = 0; j < elevatorNum; j++) {
						grpOutParam->dest[j] = INVALID_NUM;
						grpOutParam->savingControl[j] = 0;
					}
					grpOutParam->elevatorMode = NONE_MODE;
					grpOutParam->interPeakMode = INTER_PEAK_NO_MODE;
					*interPeakTime = 0;
				}
			}
			else {
				*interPeakTime = 0;
			}
		}
		break;
	case SAVING_MODE:
		if (saveCount > savingCount) {
			//printf("进入...SAVING_MODE1\n");
			*savingBuffer = 0;
			for (int j = 0; j < elevatorNum; j++) {
				grpOutParam->dest[j] = INVALID_NUM;
				grpOutParam->savingControl[j] = 0;
			}
			grpOutParam->elevatorMode = NONE_MODE;
		}
		else {
			//printf("进入...SAVING_MODE2\n");
			grpOutParam->elevatorMode = SAVING_MODE;
			for (int j = 0; j < elevatorNum; j++) {
				//if(grpInParam[j][IS_AUTO_ASSIGNING_INDEX] == 1){continue;}
				grpOutParam->savingControl[j] = savingControl[j];
				grpOutParam->dest[j] = INVALID_NUM;
			}
		}
		break;
	}
	//printf("最终的elevatorMode:%d\n\n",grpOutParam->elevatorMode);
}

void manualAssigner(struct ManualSetting *manualSetting,
	unsigned char manualSize, unsigned int currentTime,
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char elevatorNum,
	struct GrpOutParam *grpOutParam,
	unsigned char(*invalidFloor)[MAX_FLOOR],
	struct AutoSetting autoSetting/*,
	FloorProbability(*&floorProbability)[MAX_RECORD_SIZE],
	unsigned int(*&probabilitySize),
	unsigned int(*&lastRecordTime)*/) {
	unsigned int floorNum = elevatorNum > 0 ? grpInParam[0][FLOOR_NUM_INDEX] : 0;
	unsigned int i = 0;
	//printf("manualSize:%d",manualSize);
	//手动设置进去的模式
	while (i < manualSize) {
		//while中执行完所有的内容 如果超过这个数组内容
		unsigned int startTime = manualSetting[i].startTime;
		unsigned int endTime = manualSetting[i].endTime;
		unsigned char elevatorMode = manualSetting[i].elevatorMode;
		unsigned char upPeakFloor = manualSetting[i].upPeakFloor;
		unsigned char *downScatterFloor = manualSetting[i].downScatterFloor;
		unsigned char *savingControl = manualSetting[i].savingControl; //节能模式脱离群控的电梯号码
																	   //if(isManualAssigning == 1){return;}
		if (currentTime >= startTime && currentTime < endTime) {
			switch (elevatorMode) {
			case UP_PEAK_MODE:
				for (int j = 0; j < elevatorNum; j++) {
					if (grpInParam[j][IS_MANUAL_ASSIGNING_INDEX] == 1) { continue; }
					grpOutParam->savingControl[j] = 0;
					if (isCallEmpty(innerCall[j], outerCall[j], grpInParam[j][DO_INDEX]) == 1 &&
						isInvalidFloor(grpInParam, invalidFloor, upPeakFloor, j, floorNum) == 0 &&
						grpInParam[j][POS_INDEX] != upPeakFloor) {
						//判断为空闲状态
						grpOutParam->dest[j] = upPeakFloor;
					}
					else {
						grpOutParam->dest[j] = INVALID_NUM;
					}
					grpOutParam->elevatorMode = UP_PEAK_MODE;
				}
				*(autoSetting.upPeakTime) = autoSetting.upPeakDelayTime;
				*(autoSetting.interPeakTime) = autoSetting.interPeakDelayTime;
				//printf("upPeakTime:%d",*(autoSetting.upPeakTime));
				if (currentTime + 1 >= endTime) { grpOutParam->elevatorMode = NONE_MODE; }
				break;
			case DOWN_PEAK_MODE:
				for (int j = 0; j < elevatorNum; j++) {
					if (grpInParam[j][IS_MANUAL_ASSIGNING_INDEX] == 1) { continue; }
					grpOutParam->savingControl[j] = 0;
					if (isCallEmpty(innerCall[j], outerCall[j], grpInParam[j][DO_INDEX]) == 1 &&
						isInvalidFloor(grpInParam, invalidFloor, downScatterFloor[j], j, floorNum) == 0 &&
						grpInParam[j][POS_INDEX] != downScatterFloor[j]) {
						//判断为空闲状态
						grpOutParam->dest[j] = downScatterFloor[j];
					}
					else {
						grpOutParam->dest[j] = INVALID_NUM;
					}
					grpOutParam->elevatorMode = DOWN_PEAK_MODE;
				}
				*(autoSetting.upPeakTime) = autoSetting.upPeakDelayTime;
				*(autoSetting.interPeakTime) = autoSetting.interPeakDelayTime;
				if (currentTime + 1 >= endTime) { grpOutParam->elevatorMode = NONE_MODE; }
				break;
			case SAVING_MODE:
				for (int j = 0; j < elevatorNum; j++) {
					if (grpInParam[j][IS_MANUAL_ASSIGNING_INDEX] == 1) { continue; }
					grpOutParam->savingControl[j] = savingControl[j];
					grpOutParam->dest[j] = INVALID_NUM;
				}
				grpOutParam->elevatorMode = SAVING_MODE;
				*(autoSetting.upPeakTime) = autoSetting.upPeakDelayTime;
				*(autoSetting.interPeakTime) = autoSetting.interPeakDelayTime;
				if (currentTime + 1 >= endTime) { grpOutParam->elevatorMode = NONE_MODE; }
				break;
			default:
				for (int j = 0; j < elevatorNum; j++) {
					//if(grpInParam[j][IS_MANUAL_ASSIGNING_INDEX] == 1){continue;}
					grpOutParam->dest[j] = INVALID_NUM;
					grpOutParam->savingControl[j] = 0;
				}
				grpOutParam->elevatorMode = NONE_MODE;
				*(autoSetting.upPeakTime) = autoSetting.upPeakDelayTime;
				*(autoSetting.interPeakTime) = autoSetting.interPeakDelayTime;
				////printf("进入手动模式");
				if (currentTime + 1 >= endTime) { grpOutParam->elevatorMode = NONE_MODE; }
				break;
			}
			break;
		}
		else {
			i++;
		}
	}
	if (i >= manualSize) {
		//不在时间段内的情况
		for (int j = 0; j < elevatorNum; j++) {
			grpOutParam->dest[j] = INVALID_NUM;
			grpOutParam->savingControl[j] = 0;
		}
		//这边会去自动分配
		autoAssigner(autoSetting, innerCall, outerCall, grpInParam, elevatorNum, grpOutParam, invalidFloor, currentTime);
	}
}

void elevatorModeMonitor(unsigned char triggerMode,
	struct ManualSetting *manualSettings,
	unsigned char manualSize,
	struct AutoSetting autoSetting,
	unsigned int currentTime,
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char elevatorNum,
	struct GrpOutParam *grpOutParam,
	unsigned char(*invalidFloor)[MAX_FLOOR]) {
	if (triggerMode == NO_MODE) {
		for (int j = 0; j < elevatorNum; j++) {
			grpOutParam->dest[j] = INVALID_NUM;
			grpOutParam->savingControl[j] = 0;
			grpOutParam->elevatorMode = NONE_MODE;
		}
	}
	else if (triggerMode == MANUAL_MODE) {
		manualAssigner(manualSettings, manualSize, currentTime, innerCall, outerCall, grpInParam, elevatorNum,
			grpOutParam, invalidFloor, autoSetting);
	}
}


unsigned char hasIdleElevator(unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned char elevatorNum) {
	//判断是否有电梯是空闲的
	unsigned char flag = 0;
	//空闲、门开着
	for (int n = 0; n < elevatorNum; n++) {
		for (int i = 0; i < INNER_CALL_SIZE; i++) {
			if (innerCall[n][i] != INVALID_NUM) {
				flag = 1;
				return flag;
			}
			else { break; }
		}
		for (int i = 0; i < OUTER_CALL_SIZE; i++) {
			if (outerCall[n][0][i] != INVALID_NUM) {
				flag = 1;
				return flag;
			}
			else { break; }
		}
	}
	//关着门且不在移动，而且不是空闲状态（里面有人）
	//if(is_moving == 1){flag = 0;}
	return flag;
}

unsigned char cantPartAssign(unsigned char *priority,
	unsigned char elevatorNum,
	struct AssignSetting *assignSetting,
	unsigned char assignSize,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned int currentTime) {
	//可以开启这个模式返回1，不能开启这个模式返回0
	//判断是否可以开启所有电梯都响应普通外招模式
	unsigned int i = 0;
	unsigned char partAssignFlag = 0;
	unsigned char cantNormalAssignFlag = 1;
	while (i < assignSize) {
		//while中执行完所有的内容 如果超过这个数组内容
		unsigned int startTime = assignSetting[i].startTime;
		unsigned int endTime = assignSetting[i].endTime;
		if (currentTime >= startTime && currentTime < endTime) {
			partAssignFlag = 1;
			break;
		}
		else { i++; }
	}
	if (partAssignFlag == 1) {
		//在时间段内，如果存在可以响应普通外招的电梯，则将不能普通分配的信号置为1
		//printf("partAssignFlag%d\n",partAssignFlag);
		for (int n = 0; n < elevatorNum; n++) {
			if (grpInParam[n][CAN_NORMAL_ASSIGN] == 0) { continue; }
			else {
				if (priority[n] != 2) {
					cantNormalAssignFlag = 0;
					break;
				}
				else { cantNormalAssignFlag = 1; }
			}
		}
	}
	else { cantNormalAssignFlag = 1; }
	//printf("cantNormalAssignFlag:%d\n",cantNormalAssignFlag);
	return cantNormalAssignFlag;
}

void dynamicAssigner(struct ChangeSignal *changeSignal,
	unsigned char elevatorNum,
	unsigned int floorHigh,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned int(*grpOutParam)[OUT_PARAM_NUM],
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned char(*invalidFloor)[MAX_FLOOR],
	unsigned int adjustParam,
	struct AssignSetting *assignSetting,
	unsigned char assignSize,
	unsigned int currentTime) {
	//unsigned char changeId; //改变之后的电梯id
	//AssignSetting所有电梯都能响应普通外招
	//
	changeSignal->idBefore = INVALID_NUM; //初始化
	changeSignal->idAfter = INVALID_NUM;
	changeSignal->floor = INVALID_NUM;
	changeSignal->dir = INVALID_NUM;
	unsigned char is_vip = 0;
	unsigned char is_disabled = 0;
	unsigned char vip_mode = 0;
	unsigned char passengerNum = 1;
	unsigned char *priority = (unsigned char *)malloc(elevatorNum * sizeof(unsigned char));
	unsigned int floorNum = elevatorNum > 0 ? grpInParam[0][FLOOR_NUM_INDEX] : 0;
	unsigned char cantFlag = 0;

	for (int n = 0; n < elevatorNum; n++) {
		if (grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX] == 1) {
			for (int i = 0; i < OUTER_CALL_SIZE; i++) {
				//先遍历电梯，找出检修的电梯。
				if (outerCall[n][1][i] == INVALID_NUM) { break; }//没有信号，跳出
				else {
					//如果有停在该梯楼层的电梯先分配给该电梯，没有的话给最近的电梯
					for (int k = 0; k < elevatorNum; k++) {
						if (isInvalidFloor(grpInParam, invalidFloor, outerCall[n][0][i], k, floorNum) == 1 ||
							isInvalidFloor(grpInParam, invalidFloor, outerCall[n][1][i], k, floorNum) ==
							1) {
							priority[k] = 2;
						}
						else if (grpInParam[k][IS_VIP_MODE_INDEX] == 1) { priority[k] = 1; }
						else { priority[k] = 0; }
					}
					//最优先的梯号为
					unsigned char re_id = idOfEleStopInFloor(outerCall[n][0][i], outerCall[n][1][i], grpInParam,
						elevatorNum);
					unsigned char cantFlag = cantPartAssign(priority, elevatorNum, assignSetting, assignSize,
						grpInParam, currentTime);
					if ((re_id != INVALID_NUM && n != re_id) &&
						(cantFlag == 1 || (cantFlag == 0 && grpInParam[re_id][CAN_NORMAL_ASSIGN] == 1))) {
						//空闲且在自己的楼层
						changeSignal->idBefore = n;
						changeSignal->idAfter = re_id;
						changeSignal->floor = outerCall[n][0][i];
						changeSignal->dir = outerCall[n][1][i];
						free(priority);
						return;
					}
					else {
						//空闲且在自己的楼层，且要转向了，没有内招。
						re_id = mostCloseEle(outerCall[n][0][i], outerCall[n][1][i], n, grpInParam, elevatorNum,
							outerCall, innerCall);
						if ((re_id != INVALID_NUM && n != re_id) &&
							(cantFlag == 1 || (cantFlag == 0 && grpInParam[re_id][CAN_NORMAL_ASSIGN] == 1))) {
							changeSignal->idBefore = n;
							changeSignal->idAfter = re_id;
							changeSignal->floor = outerCall[n][0][i];
							changeSignal->dir = outerCall[n][1][i];
							free(priority);
							return;
						}
						else {
							for (int j = 0; j < elevatorNum; j++) {
								if (grpInParam[j][OUT_OF_GROUP_CONTROL_INDEX] == 0 &&
									grpInParam[j][IS_VIP_MODE_INDEX] == 0 && grpInParam[j][IS_FULL_INDEX] == 0
									&& (cantFlag == 1 || (cantFlag == 0 && grpInParam[j][CAN_NORMAL_ASSIGN] == 1))) {
									changeSignal->idBefore = n;
									changeSignal->idAfter = j;
									changeSignal->floor = outerCall[n][0][i];
									changeSignal->dir = outerCall[n][1][i];
									free(priority);
									return;
								}
							}
						}
					}
				}
			}
		}
	}

	//没有检修的电梯派遣方法。
	//1、首先，根据电梯的位置来计算。（先把检修电梯舍弃掉）第一、有停下来的电梯（空闲或者处于停止状态is_moving=0），那么遍历除他以外的所有电梯，看是否有信号处于它所在的楼层，而且是可以接的。
	//可以接的准则：
	//1.电梯空闲，isCallEmpty==1，is_moving=0，pos=fromFloor，door_signal==0
	//2.电梯停止，阶段1为刚停下到开门的阶段，dir!=0,is_door_close== 0
	//3.电梯开门阶段，（outerCall=0,innerCall=0(乘客进去了为0，乘客没进去就是1),dir!=0）可以忽略，只判断is_door_open==1
	//4.电梯do信号撤除，关门完毕阶段。（一种是继续跑，dir!=0，is_door_close==0。另一种是空闲了，该层信号消完了，空闲了回到第一种情况）

	for (int n = 0; n < elevatorNum; n++) {
		cantFlag = cantPartAssign(priority, elevatorNum, assignSetting, assignSize, grpInParam, currentTime);
		for (int i = 0; i < OUTER_CALL_SIZE; i++) {
			if (outerCall[n][0][i] == INVALID_NUM && grpInParam[n][CAPACITY_INDEX] == 1) { break; }
			if (outerCall[n][0][i] == grpInParam[n][POS_INDEX] && grpInParam[n][IS_MOVING_INDEX] == 0) { continue; }
			else {
				for (int j = 0; j < elevatorNum; j++) {
					if (grpInParam[j][IS_MOVING_INDEX] == 0 && grpInParam[j][DIRECTION_INDEX] == 0 &&
						grpInParam[j][POS_INDEX] == outerCall[n][0][i] &&
						isCallEmpty(innerCall[j], outerCall[j], grpInParam[j][DO_INDEX]) == 1) {
						if (grpInParam[j][IS_FULL_INDEX] != 1 && grpInParam[j][IS_VIP_MODE_INDEX] != 1 &&
							grpInParam[j][SAVING_CONTROL_INDEX] != 1 && grpInParam[j][OUT_OF_GROUP_CONTROL_INDEX] == 0
							&& (cantFlag == 1 || (cantFlag == 0 && grpInParam[j][CAN_NORMAL_ASSIGN] == 1))) {
							//printf("changeSignal%d",changeSignal->idBefore);
							//有电梯停在该层且同放向，则分配给该梯
							if (n != j) {
								changeSignal->idBefore = n;
								changeSignal->idAfter = j;
								changeSignal->floor = outerCall[n][0][i];
								changeSignal->dir = outerCall[n][1][i];
								free(priority);
								return;
							}
						}
					}
				}
			}
		}
	}
	for (int n = 0; n < elevatorNum; n++) {
		cantFlag = cantPartAssign(priority, elevatorNum, assignSetting, assignSize, grpInParam, currentTime);
		if (onlyOneFloorSignal(outerCall[n][0][0], outerCall[n][0][1], outerCall[n]) == 1 &&
			grpInParam[n][IS_MOVING_INDEX] == 1) {
			continue;
		}
		for (int i = 0; i < OUTER_CALL_SIZE; i++) {
			//unsigned char pos_to_call = grpInParam[n][POS_INDEX] > outerCall[n][0][i] ? (grpInParam[n][POS_INDEX] - outerCall[n][0][i]):(outerCall[n][0][i]-grpInParam[n][POS_INDEX]);
			if (outerCall[n][1][i] == INVALID_NUM) { break; }//没有信号，跳出
			else if (outerCall[n][1][i] == UP_NUM || outerCall[n][1][i] == DOWN_NUM) {
				for (int k = 0; k < elevatorNum; k++) {
					if (isInvalidFloor(grpInParam, invalidFloor, outerCall[n][0][i], k, floorNum) == 1 ||
						isInvalidFloor(grpInParam, invalidFloor, outerCall[n][1][i], k, floorNum) ==
						1) {
						priority[k] = 2;
					}
					else if (grpInParam[k][IS_VIP_MODE_INDEX] == 1) { priority[k] = 1; }
					else { priority[k] = 0; }
				}
				if (grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX] == 0 && grpInParam[n][SAVING_CONTROL_INDEX] == 0) {
					unsigned char ele_id = mostCloseEle(outerCall[n][0][i], outerCall[n][1][i], n, grpInParam,
						elevatorNum, outerCall, innerCall);
					//if((grpInParam[n][POS_INDEX] > outerCall[n][0][i] ? (grpInParam[n][POS_INDEX] - outerCall[n][0][i]):(outerCall[n][0][i]-grpInParam[n][POS_INDEX]))< grpInParam[n][DECELERATE_DISTANCE]){continue;}
					if (ele_id != INVALID_NUM
						&& grpInParam[ele_id][IS_FULL_INDEX] != 1 && grpInParam[ele_id][IS_VIP_MODE_INDEX] != 1 &&
						grpInParam[ele_id][SAVING_CONTROL_INDEX] != 1 &&
						grpInParam[ele_id][OUT_OF_GROUP_CONTROL_INDEX] == 0
						&& (cantFlag == 1 || (cantFlag == 0 && grpInParam[ele_id][CAN_NORMAL_ASSIGN] == 1))) {
						////printf("进入5\n\n");
						if (n != ele_id &&
							(((grpInParam[ele_id][POS_INDEX] > outerCall[n][0][i] ? (grpInParam[ele_id][POS_INDEX] -
								outerCall[n][0][i]) : (
									outerCall[n][0][i] - grpInParam[ele_id][POS_INDEX])) >=
								grpInParam[ele_id][DECELERATE_DISTANCE]
								&& grpInParam[ele_id][IS_MOVING_INDEX] == 1) ||
								grpInParam[ele_id][IS_MOVING_INDEX] == 0)) {
							changeSignal->idBefore = n;
							changeSignal->idAfter = ele_id;
							changeSignal->floor = outerCall[n][0][i];
							changeSignal->dir = outerCall[n][1][i];
							free(priority);
							return;
						}
						else { continue; }
					}
				}
			}
		}
	}
	free(priority);
}


unsigned char isInPeakOrIdle(struct GrpOutParam *grpOutParam,
	struct IdleSetting idleSetting,
	unsigned char elevatorNum,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	unsigned char isIdling) {
	//判断电梯是否在上行、下行、或节能模式中。
	unsigned char flag = 0;//0表示不是
	unsigned char moving_flag = 0;
	unsigned char allCallEmpty = isAllCallEmpty(innerCall, outerCall, elevatorNum, grpInParam);
	for (int i = 0; i < elevatorNum; i++) {
		if (grpInParam[i][IS_MOVING_INDEX] == 0) { moving_flag = 0; }
		else {
			moving_flag = 1;
			break;
		}
	}
	if (isIdling == 0) { flag = 0; }
	/*if((idleSetting.mode != IDLE_MODE_NONE && (allCallEmpty ==0  || (allCallEmpty == 1 && moving_flag==0))) ||idleSetting.mode == IDLE_MODE_NONE){
	//没开启空闲模式，或者开启了空闲模式，但是不是所有电梯都空闲。或者开启了空闲模式，且电梯都是空闲的，且不在移动
	flag =0;
	}*/
	else {
		flag = 1;
		return flag;
	}

	if (grpOutParam->elevatorMode == NONE_MODE) {
		flag = 0;
	}
	else { flag = 1; }
	return flag;
}


unsigned char backToBase(unsigned char isBaseMode,
	unsigned char baseFloor,
	unsigned char elevatorNum,
	unsigned int(*grpInParam)[IN_PARAM_NUM],
	unsigned char(*innerCall)[INNER_CALL_SIZE],
	unsigned char(*outerCall)[2][OUTER_CALL_SIZE],
	struct GrpOutParam *grpOutParam,
	struct IdleSetting idleSetting,
	unsigned char isIdling) {
	//返基站功能
	/*******************
	Description: 电梯的返基站函数。
	Input:
	isBaseMode:  是否开启返基站模式，1为开启，0为关闭；
	baseFloor:  基站的楼层;
	elevatorNum:  电梯台数;
	(*grpInParam)[IN_PARAM_NUM]: 电梯的基本参数;
	(*innerCall)[INNER_CALL_SIZE]: 电梯内招;
	(*outerCall)[2][OUTER_CALL_SIZE]: 电梯外招;
	Return: 返回基站的电梯编号，INVALIDNUM为没有开启这个模式或者没有需要返回的。
	*******************/
	if (isBaseMode == 0) { return INVALID_NUM; }
	unsigned char flag = 0;         //flag=0,表示
	unsigned char inPeakOrIdle = isInPeakOrIdle(grpOutParam, idleSetting, elevatorNum, grpInParam, innerCall, outerCall,
		isIdling);
	if (inPeakOrIdle == 0) {
		//只有在没有进入上行、下行、节能、空闲分散待梯等模式时触发返基站模式
		for (int i = 0; i < elevatorNum; i++) {
			if (grpInParam[i][OUT_OF_GROUP_CONTROL_INDEX] == 1 ||
				grpInParam[i][SAVING_CONTROL_INDEX] == 1) {
				continue;
			}
			for (int j = 0; j < INNER_CALL_SIZE; j++) {
				//如果已经有去往1层的内招，则将不再派遣电梯去往1层，即将flag置为1。
				if (innerCall[i][j] == INVALID_NUM) { break; }
				else if (innerCall[i][j] == baseFloor) {
					flag = 1;
					break;
				}
			}
			for (int j = 0; j < OUTER_CALL_SIZE; j++) {
				//如果已经有去往1层的外招,则将不再派遣电梯去往1层，即将flag置为1。
				if (outerCall[i][1][j] == INVALID_NUM || outerCall[i][0][j] == INVALID_NUM) { break; }//循环到位，跳出
				else if (outerCall[i][1][j] == baseFloor || outerCall[i][0][j] == baseFloor) {
					flag = 1;
					break;
				}
			}
			//1层已经有电梯了，则将flag置为1。
			if (grpInParam[i][POS_INDEX] == baseFloor) {
				flag = 1;
				break;
			}
			if (innerCall[i][0] == INVALID_NUM && outerCall[i][0][0] == INVALID_NUM && grpInParam[i][DO_INDEX] == 0
				&& getDirection(grpInParam[i][POS_INDEX], baseFloor) == grpInParam[i][DIRECTION_INDEX]
				&& (grpInParam[i][OUT_OF_GROUP_CONTROL_INDEX] == 0 && grpInParam[i][SAVING_CONTROL_INDEX] == 0)) {
				//如果有电梯在返基站了，1.空闲、在移动、
				flag = 1;
				break;
			}
		}
		if (flag == 0) {
			//flag为0的情况，将最近的电梯派遣到
			unsigned char distance = MAX_FLOOR;
			unsigned char temp = 0;
			unsigned char min_index = INVALID_NUM;
			for (int n = 0; n < elevatorNum; n++) {
				if (innerCall[n][0] == INVALID_NUM && outerCall[n][0][0] == INVALID_NUM &&
					grpInParam[n][DO_INDEX] == 0 &&
					(grpInParam[n][OUT_OF_GROUP_CONTROL_INDEX] == 0 && grpInParam[n][SAVING_CONTROL_INDEX] == 0)) {
					//空闲状态的电梯
					temp = grpInParam[n][POS_INDEX] > baseFloor ? grpInParam[n][POS_INDEX] - baseFloor : baseFloor -
						grpInParam[n][POS_INDEX];
					if (distance > temp) {
						min_index = n;
						distance = temp; 
					}
				}
			}
			return min_index;
		}
	}
	return INVALID_NUM;
}

void flush() {
	/*******************
	Description: 清空printf的缓冲区，解决Windows下printf无效的bug，每次循环都会调用
	*******************/
	fflush(stdout);
}






/*

//2.顺向阶梯	A 6->11	B 11->17 C D 17->1	呼叫在7楼[7,] [8,] [9,]往上		2.A脱离群控	派梯
TEST(DownwardLadder, CASE1) {
	unsigned char fromFloor = 6;
	unsigned char aimFloor = UP_NUM;
	unsigned char is_vip = 0;
	unsigned char is_disabled = 0;
	unsigned char eleNum = 4;
	unsigned char floorHight = 3000;
	unsigned int inputParam[4][IN_PARAM_NUM] = { 0 };
	for (int i = 0; i < eleNum; i++) {
		inputParam[i][ID_INDEX] = i;
		inputParam[i][VELOCITY_INDEX] = 1500;//1.5m/s
		inputParam[i][ACCELERATION_INDEX] = 650;//加速度 0.6m/s
		inputParam[i][DECELERATION_INDEX] = 650;//加速度
		inputParam[i][OPEN_INDEX] = 30;//开门时间 3秒
		inputParam[i][CLOSE_INDEX] = 30;//关门时间 3秒
		inputParam[i][DOOR_TIME_INDEX] = 30;//乘客上下梯时间 3秒
		inputParam[i][OUT_OF_GROUP_CONTROL_INDEX] = 0;//可群控 TODO
		
		inputParam[i][DIRECTION_INDEX] = 0;//方向//后面去改
		inputParam[i][POS_INDEX] = 0;//位子//后面去改
		
		inputParam[i][CAPACITY_INDEX] = 20;
		inputParam[i][ACCELERATE_TIME_INDEX] = 13;//1.3秒
		inputParam[i][DECELERATE_TIME_INDEX] = 13;//1.3秒

		inputParam[i][FLOOR_TIME_INDEX] = 15;	//1.5秒
		inputParam[i][FLOOR_NUM_INDEX] = 32;//楼层数量
		inputParam[i][SAVING_CONTROL_INDEX] = 0;
		
		inputParam[i][IS_DISABLED_INDEX] = 0;
		inputParam[i][IS_FULL_INDEX] = 0;
		inputParam[i][IS_MANUAL_ASSIGNING_INDEX] = 0;
		inputParam[i][IS_AUTO_ASSIGNING_INDEX] = 0;
		inputParam[i][IS_MOVING_INDEX] = 1;
		inputParam[i][IS_VIP_MODE_INDEX] = 0;
		inputParam[i][DO_INDEX] = 0;
		inputParam[i][DISABLED_NUM_INDEX] = 0;
		inputParam[i][DECELERATE_DISTANCE] = 0;
		inputParam[i][CAN_NORMAL_ASSIGN] = 0;
	}
	//方向
	inputParam[0][DIRECTION_INDEX] = 1;
	inputParam[1][DIRECTION_INDEX] = 1;
	inputParam[2][DIRECTION_INDEX] = 2;
	inputParam[3][DIRECTION_INDEX] = 2;
	//位置
	inputParam[0][POS_INDEX] = 6;
	inputParam[1][POS_INDEX] = 11;
	inputParam[2][POS_INDEX] = 17;
	inputParam[3][POS_INDEX] = 17;
	//用不上
	unsigned int outputParam[4][OUT_PARAM_NUM] = {0};
	//设置内外招信号
	unsigned char innerCall[4][INNER_CALL_SIZE] = { 0 };//当前没有内招
	memset(innerCall, INVALID_NUM, sizeof(innerCall));
	unsigned char outerCall[4][2][OUTER_CALL_SIZE] = { 0 };//当前没有外招
	memset(outerCall, INVALID_NUM, sizeof(outerCall));
	//设置内招
	innerCall[0][11] = 1;//[6,11]
	innerCall[1][17] = 1;//[11,17]
	innerCall[2][1] = 1;//[17,1]
	innerCall[3][1] = 1;//[17,1]

	unsigned char invalidFloor[4][MAX_FLOOR] = {0};//没有盲层
	unsigned int adjustParam = 8;//权衡候梯时间和乘梯时间的参数,默认为8
	unsigned char vip_mode = 0;//非VIP模式
	unsigned char passengerNum = 1;//乘客数量
	AssignSetting assignSetting;//指定时间段 grpInParam[n][CAN_NORMAL_ASSIGN]=1的电梯可响应普通外招
	unsigned char assignSize = 0;//时间段外,都可以响应普通外招，所以设置为0
	unsigned int currentTime = 0;//和assignSetting相关所以设置为0
	EXPECT_EQ(compassInterface(fromFloor, aimFloor, is_vip, is_disabled, eleNum, floorHight, inputParam, outputParam, innerCall, outerCall, invalidFloor, adjustParam, vip_mode, passengerNum, &assignSetting, assignSize, currentTime), 0);
}

//3.目的地派梯	[1,17] A [6,1] B [10,1] C [2,21]	D [3,11]
TEST(DestinationLadder, CASE1) {
	unsigned char fromFloor = 0;
	unsigned char aimFloor = 16;
	unsigned char is_vip = 0;
	unsigned char is_disabled = 0;
	unsigned char eleNum = 4;
	unsigned char floorHight = 3000;
	unsigned int inputParam[4][IN_PARAM_NUM] = { 0 };
	for (int i = 0; i < eleNum; i++) {
		inputParam[i][ID_INDEX] = i;
		inputParam[i][VELOCITY_INDEX] = 1500;//1.5m/s
		inputParam[i][ACCELERATION_INDEX] = 650;//加速度 0.6m/s
		inputParam[i][DECELERATION_INDEX] = 650;//加速度
		inputParam[i][OPEN_INDEX] = 30;//开门时间 3秒
		inputParam[i][CLOSE_INDEX] = 30;//关门时间 3秒
		inputParam[i][DOOR_TIME_INDEX] = 30;//乘客上下梯时间 3秒
		inputParam[i][OUT_OF_GROUP_CONTROL_INDEX] = 0;//可群控 TODO

		inputParam[i][DIRECTION_INDEX] = 0;//方向//后面去改
		inputParam[i][POS_INDEX] = 0;//位子//后面去改

		inputParam[i][CAPACITY_INDEX] = 20;
		inputParam[i][ACCELERATE_TIME_INDEX] = 13;//1.3秒
		inputParam[i][DECELERATE_TIME_INDEX] = 13;//1.3秒

		inputParam[i][FLOOR_TIME_INDEX] = 15;	//1.5秒
		inputParam[i][FLOOR_NUM_INDEX] = 32;//楼层数量
		inputParam[i][SAVING_CONTROL_INDEX] = 0;

		inputParam[i][IS_DISABLED_INDEX] = 0;
		inputParam[i][IS_FULL_INDEX] = 0;
		inputParam[i][IS_MANUAL_ASSIGNING_INDEX] = 0;
		inputParam[i][IS_AUTO_ASSIGNING_INDEX] = 0;
		inputParam[i][IS_MOVING_INDEX] = 1;
		inputParam[i][IS_VIP_MODE_INDEX] = 0;
		inputParam[i][DO_INDEX] = 0;
		inputParam[i][DISABLED_NUM_INDEX] = 0;
		inputParam[i][DECELERATE_DISTANCE] = 0;
		inputParam[i][CAN_NORMAL_ASSIGN] = 0;
	}
	//方向
	inputParam[0][DIRECTION_INDEX] = 2;
	inputParam[1][DIRECTION_INDEX] = 2;
	inputParam[2][DIRECTION_INDEX] = 1;
	inputParam[3][DIRECTION_INDEX] = 1;

	//[1, 17] A[6, 1] B[10, 1] C[2, 21]	D[3, 11]
	//位置
	inputParam[0][POS_INDEX] = 1;
	inputParam[1][POS_INDEX] = 6;
	inputParam[2][POS_INDEX] = 10;
	inputParam[3][POS_INDEX] = 2;
	//用不上
	unsigned int outputParam[4][OUT_PARAM_NUM] = { 0 };

	//设置内外招信号
	unsigned char innerCall[4][INNER_CALL_SIZE] = { 0 };//当前没有内招
	memset(innerCall, INVALID_NUM, sizeof(innerCall));
	unsigned char outerCall[4][2][OUTER_CALL_SIZE] = { 0 };//当前没有外招
	memset(outerCall, INVALID_NUM, sizeof(outerCall));

	//设置内招 A [6,1] B [10,1] C 2 [2,21]	D [2,10]
	innerCall[0][1] = 1;//[6,11]
	innerCall[1][1] = 1;//[11,17]
	innerCall[2][21] = 1;//[17,1]
	innerCall[3][11] = 1;//[17,1]
	//没有外招

	unsigned char invalidFloor[4][MAX_FLOOR] = { 0 };//没有盲层
	unsigned int adjustParam = 8;//权衡候梯时间和乘梯时间的参数,默认为8
	unsigned char vip_mode = 0;//非VIP模式
	unsigned char passengerNum = 1;//乘客数量
	AssignSetting assignSetting;//指定时间段 grpInParam[n][CAN_NORMAL_ASSIGN]=1的电梯可响应普通外招
	unsigned char assignSize = 0;//时间段外,都可以响应普通外招，所以设置为0
	unsigned int currentTime = 0;//和assignSetting相关所以设置为0
	EXPECT_EQ(compassInterface(fromFloor, aimFloor, is_vip, is_disabled, eleNum, floorHight, inputParam, outputParam, innerCall, outerCall, invalidFloor, adjustParam, vip_mode, passengerNum, &assignSetting, assignSize, currentTime), 0);
}

//4.模式监控 能不能触发早高峰	//15个人	A 17往上 B 18往上 C 19往上 D 20 往上 没有内招 ?什么意思
TEST(ElevatorModeMonitor, CASE1) {
	unsigned char fromFloor = 0;
	unsigned char aimFloor = 16;
	unsigned char is_vip = 0;
	unsigned char is_disabled = 0;
	unsigned char eleNum = 4;
	unsigned char floorHight = 3000;
	unsigned int inputParam[4][IN_PARAM_NUM] = { 0 };
	for (int i = 0; i < eleNum; i++) {
		inputParam[i][ID_INDEX] = i;
		inputParam[i][VELOCITY_INDEX] = 1500;//1.5m/s
		inputParam[i][ACCELERATION_INDEX] = 650;//加速度 0.6m/s
		inputParam[i][DECELERATION_INDEX] = 650;//加速度
		inputParam[i][OPEN_INDEX] = 30;//开门时间 3秒
		inputParam[i][CLOSE_INDEX] = 30;//关门时间 3秒
		inputParam[i][DOOR_TIME_INDEX] = 30;//乘客上下梯时间 3秒
		inputParam[i][OUT_OF_GROUP_CONTROL_INDEX] = 0;//可群控 TODO

		inputParam[i][DIRECTION_INDEX] = 0;//方向//后面去改
		inputParam[i][POS_INDEX] = 0;//位子//后面去改

		inputParam[i][CAPACITY_INDEX] = 20;
		inputParam[i][ACCELERATE_TIME_INDEX] = 13;//1.3秒
		inputParam[i][DECELERATE_TIME_INDEX] = 13;//1.3秒

		inputParam[i][FLOOR_TIME_INDEX] = 15;	//1.5秒
		inputParam[i][FLOOR_NUM_INDEX] = 32;//楼层数量
		inputParam[i][SAVING_CONTROL_INDEX] = 0;

		inputParam[i][IS_DISABLED_INDEX] = 0;
		inputParam[i][IS_FULL_INDEX] = 0;
		inputParam[i][IS_MANUAL_ASSIGNING_INDEX] = 0;
		inputParam[i][IS_AUTO_ASSIGNING_INDEX] = 0;
		inputParam[i][IS_MOVING_INDEX] = 1;
		inputParam[i][IS_VIP_MODE_INDEX] = 0;
		inputParam[i][DO_INDEX] = 0;
		inputParam[i][DISABLED_NUM_INDEX] = 0;
		inputParam[i][DECELERATE_DISTANCE] = 0;
		inputParam[i][CAN_NORMAL_ASSIGN] = 0;
	}
	//方向
	inputParam[0][DIRECTION_INDEX] = 2;
	inputParam[1][DIRECTION_INDEX] = 2;
	inputParam[2][DIRECTION_INDEX] = 1;
	inputParam[3][DIRECTION_INDEX] = 1;
	//位置
	inputParam[0][POS_INDEX] = 5;
	inputParam[1][POS_INDEX] = 9;
	inputParam[2][POS_INDEX] = 1;
	inputParam[3][POS_INDEX] = 2;
	
	unsigned int outputParam[4][OUT_PARAM_NUM] = { 0 };
	//设置内外招信号
	unsigned char innerCall[4][INNER_CALL_SIZE] = { 0 };//当前没有内招
	memset(innerCall, INVALID_NUM, sizeof(innerCall));
	unsigned char outerCall[4][2][OUTER_CALL_SIZE] = { 0 };
	memset(outerCall, INVALID_NUM, sizeof(outerCall));
	//设置内招 A [6,1] B [10,1] C 2 [2,21]	D [2,10]
	//innerCall[0][10] = 1;//[6,11]
	//innerCall[1][16] = 1;//[11,17]
	//innerCall[2][20] = 1;//[17,19]
	//innerCall[3][10] = 1;//[17,9]
	//当前外招全部在1楼
	for (int i = 0; i < 4; i++) {
		for (int k = 0; k < 10; k++) {
			//外招的出发层和目的层,往上行，分别从第1层到[2-12]层
			outerCall[i][0][k] = 1;
			outerCall[i][1][k] = k+2;
		}
	}
	

	unsigned char invalidFloor[4][MAX_FLOOR] = { 0 };//没有盲层
	AssignSetting assignSetting;//指定时间段 grpInParam[n][CAN_NORMAL_ASSIGN]=1的电梯可响应普通外招
	unsigned char assignSize = 0;//时间段外,都可以响应普通外招，所以设置为0
	unsigned int currentTime = 0;//和assignSetting相关所以设置为0

	unsigned char triggerMode = 1; //1表示触发，0表示不触发
	ManualSetting manualSettings = {0};
	unsigned char manualSize = 0;
	AutoSetting autoSetting = { 0 };
	autoSetting.upPeakFloor = 1;
	autoSetting.upPeakCount = 10;
	autoSetting.upPeakTime = (unsigned int *)malloc(sizeof(unsigned int));
	memset(autoSetting.upPeakTime, 0, sizeof(unsigned int));//自动调度之后的目的地

	autoSetting.upPeakDelayTime = 100;

	autoSetting.interPeakCount = 10;
	autoSetting.interPeakDelayTime = 100;

	autoSetting.savingCount = 1;
	autoSetting.savingBufferLen = 100;

	autoSetting.savingBuffer = (unsigned int *)malloc(sizeof(unsigned int));
	memset(autoSetting.savingBuffer, 0, sizeof(unsigned int));//自动调度之后的目的地
	

	GrpOutParam grpOutParam = { 0 };
	grpOutParam.elevatorMode = NONE_MODE;
	grpOutParam.dest = (unsigned char *)malloc(sizeof(unsigned char)*4);
	memset(grpOutParam.dest, INVALID_NUM, sizeof(unsigned char) * 4);//自动调度之后的目的地
	grpOutParam.savingControl = (unsigned char *)malloc(sizeof(unsigned char) * 4);
	memset(grpOutParam.savingControl, 0, sizeof(unsigned char) * 4);//是否脱离群控
	elevatorModeMonitor(triggerMode, &manualSettings, manualSize, autoSetting, currentTime, innerCall, outerCall, inputParam, eleNum, &grpOutParam, invalidFloor);

	EXPECT_EQ(grpOutParam.elevatorMode, UP_PEAK_MODE);
}

//1.分散待梯	能不能触发分散待梯
TEST(IdleScatter, CASE1) {
	unsigned char fromFloor = 0;
	unsigned char aimFloor = 16;
	unsigned char is_vip = 0;
	unsigned char is_disabled = 0;
	unsigned char eleNum = 4;
	unsigned char floorHight = 3000;
	unsigned int inputParam[4][IN_PARAM_NUM] = { 0 };
	for (int i = 0; i < eleNum; i++) {
		inputParam[i][ID_INDEX] = i;
		inputParam[i][VELOCITY_INDEX] = 1500;//1.5m/s
		inputParam[i][ACCELERATION_INDEX] = 650;//加速度 0.6m/s
		inputParam[i][DECELERATION_INDEX] = 650;//加速度
		inputParam[i][OPEN_INDEX] = 30;//开门时间 3秒
		inputParam[i][CLOSE_INDEX] = 30;//关门时间 3秒
		inputParam[i][DOOR_TIME_INDEX] = 30;//乘客上下梯时间 3秒
		inputParam[i][OUT_OF_GROUP_CONTROL_INDEX] = 0;//可群控 TODO

		inputParam[i][DIRECTION_INDEX] = 0;//方向//后面去改
		inputParam[i][POS_INDEX] = 0;//位子//后面去改

		inputParam[i][CAPACITY_INDEX] = 20;
		inputParam[i][ACCELERATE_TIME_INDEX] = 13;//1.3秒
		inputParam[i][DECELERATE_TIME_INDEX] = 13;//1.3秒

		inputParam[i][FLOOR_TIME_INDEX] = 15;	//1.5秒
		inputParam[i][FLOOR_NUM_INDEX] = 32;//楼层数量
		inputParam[i][SAVING_CONTROL_INDEX] = 0;

		inputParam[i][IS_DISABLED_INDEX] = 0;
		inputParam[i][IS_FULL_INDEX] = 0;
		inputParam[i][IS_MANUAL_ASSIGNING_INDEX] = 0;
		inputParam[i][IS_AUTO_ASSIGNING_INDEX] = 0;
		inputParam[i][IS_MOVING_INDEX] = 1;
		inputParam[i][IS_VIP_MODE_INDEX] = 0;
		inputParam[i][DO_INDEX] = 0;
		inputParam[i][DISABLED_NUM_INDEX] = 0;
		inputParam[i][DECELERATE_DISTANCE] = 0;
		inputParam[i][CAN_NORMAL_ASSIGN] = 0;
	}
	//方向
	inputParam[0][DIRECTION_INDEX] = 0;
	inputParam[1][DIRECTION_INDEX] = 0;
	inputParam[2][DIRECTION_INDEX] = 0;
	inputParam[3][DIRECTION_INDEX] = 0;
	//位置
	inputParam[0][POS_INDEX] = 5;
	inputParam[1][POS_INDEX] = 9;
	inputParam[2][POS_INDEX] = 1;
	inputParam[3][POS_INDEX] = 2;
	//用不上
	unsigned int outputParam[4][OUT_PARAM_NUM] = { 0 };
	//设置内外招信号
	unsigned char innerCall[4][INNER_CALL_SIZE] = { 0 };//当前没有内招
	memset(innerCall,INVALID_NUM , sizeof(innerCall));
	unsigned char outerCall[4][2][OUTER_CALL_SIZE] = { 0 };//当前没有外招
	memset(outerCall, INVALID_NUM, sizeof(outerCall));

	unsigned char invalidFloor[4][MAX_FLOOR] = { 0 };//没有盲层
	AssignSetting assignSetting;//指定时间段 grpInParam[n][CAN_NORMAL_ASSIGN]=1的电梯可响应普通外招
	unsigned char assignSize = 0;//时间段外,都可以响应普通外招，所以设置为0
	unsigned int currentTime = 0;//当前时间
	unsigned char dest[4] = { 0 };
	unsigned char isIdling = 0;
	ManualSetting manualSetting;
	unsigned char manualSize = 0;
	unsigned char isBaseMode = 0;
	unsigned char baseFloor = 1;
	IdleSetting idleSetting = { 0 };
	idleSetting.mode = IDLE_MODE_UNIFORM;
	idleSetting.idleBuffer = (unsigned int *)malloc(sizeof(unsigned int));
	memset(idleSetting.idleBuffer, 2, sizeof(unsigned int));//自动调度之后的目的地
	idleSetting.idleBufferLen = 60;

	GrpOutParam grpOutParam = { 0 };
	grpOutParam.elevatorMode = NONE_MODE;
	grpOutParam.dest = (unsigned char *)malloc(sizeof(unsigned char) * 4);
	memset(grpOutParam.dest, INVALID_NUM, sizeof(unsigned char) * 4);//自动调度之后的目的地
	grpOutParam.savingControl = (unsigned char *)malloc(sizeof(unsigned char) * 4);
	memset(grpOutParam.savingControl, 0, sizeof(unsigned char) * 4);//是否脱离群控

	idleScatter(idleSetting, currentTime, innerCall, outerCall, inputParam, invalidFloor, eleNum, dest, isIdling, &manualSetting, manualSize, isBaseMode, baseFloor, &grpOutParam);
	
	unsigned char ret[4] = { 16,24,0,8 };

	for (int i = 0; i < eleNum; i++) {
		EXPECT_NE(dest[i], INVALID_NUM);
	}
	
	
}
*/



