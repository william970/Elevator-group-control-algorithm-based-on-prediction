#ifndef C_EXAMPLE_GREEDY_H
#define C_EXAMPLE_GREEDY_H

#include<stdlib.h>
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <stdbool.h>
#include<math.h>

//Data Size
#define WEIGHT_PER_PASSAGE 60
#define MAX_FLOOR 0x40
#define MAX_ELEVATOR 0x0a
#define IN_PARAM_NUM 27
#define OUT_PARAM_NUM 8
#define AverageFloorTime 10
#define OUTER_CALL_SIZE 200
#define INNER_CALL_SIZE 30
#define MIN_DIFF 0.000001
//GrpInParam Index
#define ID_INDEX 0
#define VELOCITY_INDEX 1      //06D4  1748
#define ACCELERATION_INDEX 2  //0258  600
#define DECELERATION_INDEX 3  //01F4  500
#define OPEN_INDEX 4          //0014  20
#define CLOSE_INDEX 5         //0014  20
#define DOOR_TIME_INDEX 6     //001E  30
#define OUT_OF_GROUP_CONTROL_INDEX 7  //0000
#define DIRECTION_INDEX 8             //0000
#define POS_INDEX 9
#define CAPACITY_INDEX 10
#define ACCELERATE_TIME_INDEX 11      //000F  15
#define DECELERATE_TIME_INDEX 12      //000F  15
#define FLOOR_TIME_INDEX 13           //000F  15
#define FLOOR_NUM_INDEX 14            //0024  36
#define SAVING_CONTROL_INDEX 15
#define IS_DISABLED_INDEX 16
#define IS_FULL_INDEX 17
#define IS_MANUAL_ASSIGNING_INDEX 18
#define IS_AUTO_ASSIGNING_INDEX 19
#define IS_MOVING_INDEX 20
#define IS_VIP_MODE_INDEX 21
#define DO_INDEX 22
#define DISABLED_NUM_INDEX 23
#define DECELERATE_DISTANCE 24
#define CAN_NORMAL_ASSIGN 25
#define WEIGHT 26

//Passenger
//outerCall[n][]={(1,2,3,4,255}
#define MAX_RECORD_SIZE 1000 //记录的最大的概率
#define INVALID_NUM 255 //无效数字
#define UP_NUM 253
#define DOWN_NUM 254
//Mode Flag
#define IDLE_MODE_NONE  1//空闲时不做任何操作
#define IDLE_MODE_UNIFORM  2//空闲时分散待梯
#define IDLE_MODE_ASSIGN  3//空闲时指派楼层待梯
#define UP_PEAK_MODE 1  //上高峰模式
#define DOWN_PEAK_MODE 2 //下高峰模式
#define INTER_PEAK_MODE 3 //层间高峰模式
#define SAVING_MODE 4 //节能模式
#define NONE_MODE 5//无高峰、节能模式
#define MANUAL_MODE 1 //开启高峰、节能模式
#define NO_MODE 2 //关闭高峰、节能模式
#define ONE_PEAK_FLOOR_MODE 1//单层进入层间高峰
#define TWO_PEAK_FLOOR_MODE 2//两层进入层间高峰
#define INTER_PEAK_NO_MODE 3//未进入层间高峰
#define INTER_PEAK_OUT_MODE 4//mode置为这个的时候开始计时退出高峰
//Error num
#define ERROR_OUT_OF_CONTROL 250
#define ERROR_INVALID_FLOOR 251


struct FloorProbability {//昨天当前时间的目的地分布概率信息
	int recodeday;
	double recodetime;
	int fromfloor;
	unsigned int passageincount;//上电梯的人数
	unsigned int allProbability;//<当前楼层，概率分母>
	unsigned int offProbability[MAX_FLOOR];//<当前楼层，概率分布>
};




//各种脱离群控的信号都要分类,所有的信号只要有一个脱离群控，就脱离群控，全部为0才不脱离

struct AssignSetting {//只有指定电梯能响应普通外招的时间段结构体指针
    unsigned int startTime;//此模式的开始时间
    unsigned int endTime;//此模式的结束时间
};

struct IdleSetting {//空闲模式的结构体
    unsigned char mode;   //空闲的三种模式
    unsigned char *idleFloor;  //指定的楼层
    unsigned int *idleBuffer;  //记录所有电梯都空闲的时间
    unsigned int idleBufferLen;//空闲滞后时间
};
struct ManualSetting {  //高峰、节能手动指定时间段的设置参数
    unsigned int startTime;  //开始时间
    unsigned int endTime;    //结束时间
    unsigned char elevatorMode;   //上行、下行、节能的模式
    unsigned char upPeakFloor;    //上行高峰指定楼层
    unsigned char *downScatterFloor;  //下行高峰指定楼层
    unsigned char *savingControl; //1表示脱离群控,[1,0,0,0]表示0号梯脱离
};

struct AutoSetting {  //高峰、节能自动判定的设置参数
    unsigned char upPeakFloor;  //上行高峰楼层
    unsigned int upPeakCount;   //上行高峰的指令的数量
    unsigned int upPeakDelayTime;  //上行高峰延时时间
	
    unsigned int interPeakCount;  //下行高峰的指令数量
    unsigned int interPeakDelayTime; //下行高峰延时时间

    unsigned int savingCount;    //节能的数量
    unsigned int savingBufferLen;//节能触发时间
    unsigned int *upPeakTime;  //上行高峰的延时计时器
    unsigned int *interPeakTime;  //下行高峰延时计时器

    unsigned int *savingBuffer;   //节能模式存储时间
    unsigned char *savingControl;//1表示脱离群控,[1,0,0,0]表示0号梯脱离

	unsigned int upPeakRecordDelayTime;  //上行高峰时记录延时时间

};

struct GrpOutParam {              //输出参数，供外部（硬件）调用
    unsigned char elevatorMode; //电梯的模式
    unsigned char interPeakMode; //层间高峰模式，初始为INTER_PEAK_NO_MODE
    unsigned char *dest;        //电梯的目的地 无指令时存的是INVALID_NUM
    unsigned char *savingControl;  //节能脱离群控的电梯
};

struct ChangeSignal {         //动态分配函数输出的参数，用于无选层器楼层信号的动态分配
    unsigned char idBefore; //更改之前的电梯编号
    unsigned char idAfter; //更改之后的电梯编号
    unsigned char floor;  //更改的外招信号的楼层
    unsigned char dir;   //方向
};

void MallocRecodeMemory(FloorProbability(*&floorProbability)[MAX_RECORD_SIZE], unsigned int(*&probabilitySize));

void elevatorRecode(unsigned int lowestwight, unsigned int heighstwight, unsigned int currentday, unsigned int currentTime, unsigned char curFloor, unsigned char dir,
	unsigned char(*innerCall)[INNER_CALL_SIZE], unsigned char(*outerCall)[2][OUTER_CALL_SIZE], unsigned char elevatorNum, unsigned char(*invalidFloor)[MAX_FLOOR],
	FloorProbability(*&floorProbability)[MAX_RECORD_SIZE], unsigned int(*&probabilitySize), unsigned int RecordDelayTime);


unsigned char getDirection(unsigned char startFloor, unsigned char endFloor);

unsigned char isVipOutOfControl(unsigned char is_vip,
                                unsigned int (*grpInParam)[IN_PARAM_NUM],
                                unsigned char elevatorNum);

unsigned char compassInterface(unsigned char fromFloor,
                               unsigned char aimFloor,
                               unsigned int is_vip,
                               unsigned int is_disabled,
                               unsigned char elevatorNum,
                               unsigned int floorHigh,
                               unsigned int (*grpInParam)[IN_PARAM_NUM],
                               unsigned int (*grpOutParam)[OUT_PARAM_NUM],
                               unsigned char (*innerCall)[INNER_CALL_SIZE],
                               unsigned char (*outerCall)[2][OUTER_CALL_SIZE],
                               unsigned char (*invalidFloor)[MAX_FLOOR],
                               unsigned int adjustParam,
                               unsigned char vip_mode,
                               unsigned char passengerNum,
                               struct AssignSetting *assignSetting,
							   unsigned char assignSize,
                               unsigned int currentTime);

void idleScatter(struct IdleSetting idleSetting,
                 unsigned int currentTime,
                 unsigned char (*innerCall)[INNER_CALL_SIZE],
                 unsigned char (*outerCall)[2][OUTER_CALL_SIZE],
                 unsigned int (*grpInParam)[IN_PARAM_NUM],
                 unsigned char (*invalidFloor)[MAX_FLOOR],
                 unsigned char elevatorNum, unsigned char *dest,
                 unsigned char isIdling,
                 struct ManualSetting *manualSetting,
                 unsigned char manualSize,
                 unsigned char is_base_mode,
                 unsigned char baseFloor,
                 struct GrpOutParam *grpOutParam);

void elevatorModeMonitor(unsigned char triggerMode,
                         struct ManualSetting *manualSettings,
                         unsigned char manualSize,
                         struct AutoSetting autoSetting, unsigned int currentTime,
                         unsigned char (*innerCall)[INNER_CALL_SIZE],
                         unsigned char (*outerCall)[2][OUTER_CALL_SIZE],
                         unsigned int (*grpInParam)[IN_PARAM_NUM],
                         unsigned char elevatorNum,
                         struct GrpOutParam *grpOutParam,
                         unsigned char (*invalidFloor)[MAX_FLOOR]);

void dynamicAssigner(struct ChangeSignal *changeSignal,
                     unsigned char elevatorNum,
                     unsigned int floorHigh,
                     unsigned int (*grpInParam)[IN_PARAM_NUM],
                     unsigned int (*grpOutParam)[OUT_PARAM_NUM],
                     unsigned char (*innerCall)[INNER_CALL_SIZE],
                     unsigned char (*outerCall)[2][OUTER_CALL_SIZE],
                     unsigned char (*invalidFloor)[MAX_FLOOR],
                     unsigned int adjustParam,
                     struct AssignSetting *assignSetting,
                     unsigned char assignSize,
                     unsigned int currentTime);

unsigned char backToBase(unsigned char isBaseMode,
                         unsigned char baseFloor,
                         unsigned char elevatorNum,
                         unsigned int (*grpInParam)[IN_PARAM_NUM],
                         unsigned char (*innerCall)[INNER_CALL_SIZE],
                         unsigned char (*outerCall)[2][OUTER_CALL_SIZE],
                         struct GrpOutParam *grpOutParam,
                         struct IdleSetting idleSetting,
                         unsigned char isIdling);

#endif
