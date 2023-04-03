#include "PassageManage.h"
#include "greedy.h"

Passage::Passage(int PassageID, int WorkFloor, int FromFloor, int AimFloor, double ComeTime) {
	iPassageID = PassageID;
	iWorkFloor = WorkFloor;
	iFromFloor = FromFloor;
	iAimFloor = AimFloor;
	iComeTime = ComeTime;
	iLandTime = 0.0;
	iOffTime = 0.0;
	eleno = -1;
	is_vip = 0;
	is_disabled = 0;
	//计算出乘客的方向
	dir = getDirection(iFromFloor, iAimFloor);

}





bool  PassageManage::addWaitPassage(Passage* p) {
	map_wait_passage[p->iFromFloor].insert(p);
	return true;
}


//上梯动作结束之后去删除已经派梯的乘客
bool PassageManage::delWaitPassageIn(int currentfloor) {
	map<int, set<Passage*>>::iterator passageItr2 = PassageManage::GetInstance()->map_wait_passage.find(currentfloor);
	if (passageItr2 != PassageManage::GetInstance()->map_wait_passage.end()) {
		set<Passage*>::iterator passage_itr = passageItr2->second.begin();
		for (; passage_itr != passageItr2->second.end(); ) {
			//如果已经上电梯了
			if ((*passage_itr)->eleno != -1) {
				passage_itr = passageItr2->second.erase(passage_itr);
			}
			else {
				passage_itr++;
			}
		}
		if (passageItr2->second.size() == 0) {
			PassageManage::GetInstance()->map_wait_passage.erase(currentfloor);
		}
	}
	return false;
}
