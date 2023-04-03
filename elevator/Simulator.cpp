#include "greedy.h"
#include"config.h"
#include"PassageManage.h"
#include"ElevatorManage.h"

using namespace std;
//简单的模拟器，早高峰数据测试


//全局变量
int currentday = 0;
//当前时间
double currentTime = 0.0;
//当前时间
unsigned char currentMode = SAVING_MODE;
//上车的乘客
int landpassage = 0;
//下车的乘客
int offpassage = 0;
//模拟生成的乘客数据
vector<Passage> vec_passages;//第一天

//分割
std::vector<std::string> stringSplit(const std::string& str, char delim) {
	std::size_t previous = 0;
	std::size_t current = str.find(delim);
	std::vector<std::string> elems;
	while (current != std::string::npos) {
		if (current > previous) {
			string temp = str.substr(previous, current - previous);
			elems.push_back(temp);
		}
		previous = current + 1;
		current = str.find(delim, previous);
	}
	if (previous != str.size()) {
		elems.push_back(str.substr(previous));
	}
	return elems;
}


int ReadPassage(string strline,Passage& p) {
	//乘客信息
	vector<string> vi;
	vi = stringSplit(strline, ',');
	int PassageID;
	int WorkFloor;
	int FromFloor;
	int AimFloor;
	double ComeTime;
	int day = stoi(vi[0]);
	PassageID = stoi(vi[1]);
	WorkFloor = stoi(vi[2]);
	FromFloor = stoi(vi[3]);
	AimFloor = stoi(vi[4]);
	ComeTime = stod(vi[5]);
	p = Passage(PassageID, WorkFloor, FromFloor, AimFloor, ComeTime);
	return day;
}

void resetDay(int d) {
	//更新全局变量
	currentday = d;
	currentTime = 0.0;
	landpassage = 0;
	offpassage = 0;
	vec_passages.clear();
}

//开始仿真
int Run() {
	int iPassageCount = vec_passages.size();
	//当前还未派梯的乘客
	int currentIndex = 0;

	

	while (true) {
		if (currentIndex >= vec_passages.size()) {
			break;
		}

		//分配完所有到了的乘客
		if (vec_passages[currentIndex].iComeTime < currentTime) {
			//给该乘客分配电梯
			PassageManage::GetInstance()->addWaitPassage(&vec_passages[currentIndex]);
			int eleno = ElevatorManage::GetInstance()->dispatchPassage(&vec_passages[currentIndex]);
			currentIndex++;
		}
		else {
			//这边已经处理完了所有的乘客,更新电梯状态并触发,一秒钟触发一次模式监控
			if (abs(round(currentTime) - currentTime) < MIN_DIFF) {
				ElevatorManage::GetInstance()->ModeMonitor();
			}
			currentTime += TIME_INTERVAL;
			//更新电梯状态
			ElevatorManage::GetInstance()->Update();
		}
	}
	//全部分配完成，等待电梯调度
	while (true) {
		//全部乘客已经到达
		if (offpassage >= iPassageCount) {
			break;
		}
		//每秒钟检测一次
		if (abs(round(currentTime) - currentTime) < MIN_DIFF) {
			ElevatorManage::GetInstance()->ModeMonitor();
		}
		//时间加0.1秒
		currentTime += TIME_INTERVAL;
		//更新电梯状态
		ElevatorManage::GetInstance()->Update();
	}
	return true;
}

double sum1 = 0;
double sum2 = 0;

int Result() {
	double avg_arrive_time = 0.0;
	double avg_wait_time = 0.0;
	for (auto passage_ptr : vec_passages) {
		avg_wait_time += passage_ptr.iLandTime - passage_ptr.iComeTime;
		avg_arrive_time += passage_ptr.iOffTime - passage_ptr.iComeTime;
	}
	avg_arrive_time /= vec_passages.size();
	avg_wait_time /= vec_passages.size();
	cout << "currentday: "<< currentday << endl;
	cout << "avg_wait_time:" << avg_wait_time << endl;
	cout << "avg_arrive_time:" << avg_arrive_time << endl;

	sum1 += avg_wait_time;
	sum2 += avg_arrive_time;

	return 0;
}


//模拟器
int main(){
	
	unsigned char eleNum = ELE_COUNT;
	//初始化电梯的参数模块
	ElevatorManage::GetInstance()->Init(eleNum);


	//通过gmock_main.cc生成模拟乘客数据，上行高峰
	//读入乘客信息
	ifstream inFile;	//定义ofstream对象outFile
	inFile.open("./10day_up.csv");	//打开文件
	string strline;
	while (getline(inFile, strline))
	{
		Passage p;
		int day = ReadPassage(strline, p);
		if (day == currentday) {
			vec_passages.push_back(p);
		}
		//如果是下一天,仿真完当天之后再重新更新全局变量
		else {
			Run();
			Result();
			cout << "reset:" << day << endl;
			resetDay(day);
			vec_passages.push_back(p);
		}
		
	}
	inFile.close();
	
	cout << "wait_time:" << sum1 << endl;
	cout << "arrive_time:" << sum2 << endl;
	return 0;
}