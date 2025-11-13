#ifndef __MECANUM_H
#define __MECANUM_H

extern PID_PositionInitTypedef Mecanum_SpeedPID[];//底盘四个电机的转速PID
extern float Mecanum_Power;//底盘功率

void Mecanum_Init(void);//麦轮初始化
void Mecanum_CleanPID(void);//麦轮PID清理
void Mecanum_ControlSpeed(int16_t LeftFrontSpeed,int16_t RightFrontSpeed,int16_t LeftRearSpeed,int16_t RightRearSpeed);//麦轮速度控制
void Mecanum_InverseMotionControl(float v_x,float v_y,float w);//麦轮逆运动解算
void Mecanum_MoveControl(void);//麦轮运动控制
void Mecanum_PowerMoveControl(void);//麦轮功率控制

#endif
