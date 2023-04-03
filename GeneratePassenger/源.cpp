#include<vector>
#include<iostream>
#include<random>
#include<string>
#include<time.h>
#include<map>
#include<algorithm>
#include<fstream>
using namespace std;
//乘客

//时间
#define M 60
#define H 60*60


struct RecordTime
{
public:
	int iFrom;
	int iTo;
	double iComeTime;//来的时间
	double iLandTime;//登上电梯的时间
	double iOffTime;//下电梯的时间
};

class Passage {
public:
	Passage() {}
	Passage(int PassageID, int WorkFloor) {
		iPassageID = PassageID;
		iWorkFloor = WorkFloor;
	}
	int iPassageID;
	int iWorkFloor;
	map<int, vector<RecordTime> > recored;//<第几天，记录的时间>
};

int main() {
	//泊松分布模拟客流
	std::minstd_rand engine(time(NULL));
	int uptime = 5;
	std::poisson_distribution<int> distribution(uptime);
	//均匀分布概率，乘客的数据是1小时内泊松分布,5分钟类均匀分布
	std::uniform_real_distribution<double> dis(0.0, 1.0);

	vector<Passage> vec_passages;
	//3到32层直接生成均匀分布的乘客数量，2楼则为食堂
	int personID = 1;
	for (int i = 3; i <= 32; i++) {
		//假设每个楼层都有10到30个人，因为是相同楼层所以都相互认识的概率为0.8，更倾向能一起走
		int personcount = rand() % 20 + 10;
		for (int p = 0; p < personcount; p++) {
			Passage passage(personID, i);
			vec_passages.push_back(passage);
			personID++;
		}
	}
	ofstream outFile;
	outFile.open("10day_up.csv");	//打开文件
	for (auto& passage : vec_passages) {
		
		//outFile << passage.iPassageID << "," << passage.iWorkFloor << "\n";
		

		for (int d = 0; d < 10; d++) {
			//一共一个小时，分为12段，每5分钟之间均匀分布
			//每5分钟乘客的数量
			//早上去工位上班
			double iComeTime = 8 * H +  5 * M * distribution(engine) + dis(engine) * 5 * M;
			RecordTime r1;
			r1.iFrom = 1;
			r1.iTo = passage.iWorkFloor;
			r1.iComeTime = iComeTime;
			r1.iLandTime = 0;
			r1.iOffTime = 0;
			passage.recored[d].push_back(r1);

			//outFile << d << "," << r1.iFrom <<"," << r1.iTo <<","<< r1.iComeTime << "\n";

			////中午去食堂吃饭，触发层间高峰
			//RecordTime r2;
			//iComeTime = 11 * H + 5 * M * distribution(engine) + dis(engine) * 5 * M;
			//r2.iFrom = passage.iWorkFloor;
			//r2.iTo = 2;
			//r2.iComeTime = iComeTime;
			//r2.iLandTime = 0;
			//r2.iOffTime = 0;
			//passage.recored[d].push_back(r2);

			////outFile << d << "," << r2.iFrom << "," << r2.iTo << "," << r2.iComeTime << "\n";

			////去食堂吃完，回工位睡觉，触发层间高峰
			//RecordTime r3;
			//iComeTime = 12 * H + 5 * M * distribution(engine) + dis(engine) * 5 * M;
			//r3.iFrom = 2;
			//r3.iTo = passage.iWorkFloor;
			//r3.iComeTime = iComeTime;
			//r3.iLandTime = 0;
			//r3.iOffTime = 0;
			//passage.recored[d].push_back(r3);

			////outFile << d << "," << r3.iFrom << "," << r3.iTo << "," << r3.iComeTime << "\n";

			////18点下班，触发下行高峰
			//RecordTime r4;
			//iComeTime = 18 * H + 5 * M * distribution(engine) + dis(engine) * 5 * M;
			//r4.iFrom = passage.iWorkFloor;
			//r4.iTo = 1;
			//r4.iComeTime = iComeTime;
			//r4.iLandTime = 0;
			//r4.iOffTime = 0;
			//passage.recored[d].push_back(r4);

			//outFile << d << "," << r4.iFrom << "," << r4.iTo << "," << r4.iComeTime << "\n";
			
		}
	}
	


	for (int d = 0; d < 10; d++) {
		for (int j = 0; j < 1; j++) {
			//按照来的顺序排序
			sort(vec_passages.begin(), vec_passages.end(), [=](Passage a, Passage b) {
				return a.recored[d][j].iComeTime < b.recored[d][j].iComeTime;
			});
			//保存数据

			//天，id，工作楼层，起始楼层，目的楼层，时间
			for (auto& p : vec_passages) {
				outFile << d << "," << p.iPassageID<<"," << p.iWorkFloor << "," << p.recored[d][j].iFrom << "," << p.recored[d][j].iTo << "," << p.recored[d][j].iComeTime << "\n";
			}
		}
		
	}
	outFile.close();	//关闭文件
	return 0;
}