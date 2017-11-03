/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team��		SSL-ZJUNlict											*/
/* HomePage:	http://www.nlict.zju.edu.cn/ssl/WelcomePage.html		*/
/************************************************************************/
/* File:	  DefPos1G2D.cpp											*/
/* Func:	  1���Ž���2���������Ϸ���									*/
/* Author:	  ��Ⱥ 2012-08-18											*/
/* Refer:	  ###														*/
/* E-mail:	  wangqun1234@zju.edu.cn									*/
/************************************************************************/

#include "DefPos1G2D.h"
#include "param.h"
#include "utils.h"
#include "WorldModel.h"
#include "BestPlayer.h"
#include "GDebugEngine.h"
#include <math.h>
#include "BallSpeedModel.h"


namespace{
	bool debug = false;
	bool debug_target_line = false;
	bool debug_mainLine = false;
	bool debug_defenderPos = false;
	bool debug_extreme = false;
	bool bigbug = false;
	bool debug_goalie_self = false;
	bool debug_ballInPenalty = false;
	//����
	const double GoalBuffer = 2;
	const double PLAYERSIZE = Param::Vehicle::V2::PLAYER_SIZE - 1.0; //��ƪ���õ�player size  0.5
	CGeoPoint RGOAL_LEFT_POS;
	CGeoPoint RGOAL_RIGHT_POS;
	CGeoPoint RGOAL_CENTRE_POS;

	//�Ž��õ�3�������㡢���޵�
	CGeoPoint RDENFENCE_LEFT_POINT;
	CGeoPoint RDENFENCE_RIGHT_POINT;
	CGeoPoint RDENFENCE_MIDDLE_POINT;//ԭ250
	CGeoPoint RD_LEFT_POINT ;//ע�������������������������㲻ͬ��Ҫ�������Ͳ�Ҫ��ƴ�Ž���
	CGeoPoint RD_RIGHT_POINT;
	//�Ž��滮�����ڵ���
	CGeoLine BASE_LINE_LEFT;
	CGeoLine BASE_LINE_RIGHT;
	
	//һЩ�����õ��м��
	const double AVOID_PENALTY_BUFFER = 7;//ori:7
	double PEN_RADIUS;
	double PEN_DEPTH;
	//���ض�Ա վλ�� ��ǰ�߶����ߵ��������޵�
	CGeoPoint RCENTER_LEFT;
	CGeoPoint RCENTER_RIGHT;
	//�������˵��������޵�
	CGeoPoint RBOTTOM_LEFT;
	CGeoPoint RBOTTOM_RIGHT;
	//��ǰ�߶�ӳ�䵽���ߵ�������
	CGeoPoint RCENTER2BOTTOM_LEFT;
	CGeoPoint RCENTER2BOTTOM_RIGHT;
	
	////�������ڹ滮��
	//CGeoLine RD_CENTER_SEGMENT = CGeoLine(RCENTER_LEFT,RCENTER_RIGHT);
	//CGeoCirlce RD_CIR_LEFT = CGeoCirlce(RCENTER2BOTTOM_LEFT,PEN_RADIUS);
	//CGeoCirlce RD_CIR_RIGHT = CGeoCirlce(RCENTER2BOTTOM_RIGHT,PEN_RADIUS);

	//�ж����Ƿ��ڼ���λ�õ��о�
	const double RgudgeLeftDir = -Param::Math::PI/2 - atan(0.5);
	const double RgudgeRightDir = Param::Math::PI/2 + atan(0.5);
	const double gudgeBuffer = Param::Math::PI * 10 / 180;
	CGeoPoint RleftGudgePoint;
	CGeoPoint RrightGudgePoint ;

	const double BALL_SHOOT_DIR_BUFFER = Param::Math::PI * 5 / 180;		//�����жϵ�������ֻ�������ʱʹ��
	const double ENEMY_FACE_BUFFER = Param::Math::PI * 45 / 180;		//�жϵ����Ƿ�����ҷ����ŵ�����
	const double ENEMY_BALL_DIST_BUFFER = 35;                           //���س���ľ����о�
	const double PENALTY_BUFFER = 5;									//��������
	const double ENEMY_PASS_SPEED = 300;								//�жϵ��˴����ٶ���
	const double GOALIE_SELF_DIST = 25;									//�������������þ�������ʱ������Ա�¾���ս

};

CDefPos1G2D::CDefPos1G2D()
{
	clearPos();
	RGOAL_LEFT_POS = CGeoPoint(Param::Field::PITCH_LENGTH / 2, -Param::Field::GOAL_WIDTH / 2 - GoalBuffer);
	RGOAL_RIGHT_POS = CGeoPoint(Param::Field::PITCH_LENGTH / 2, Param::Field::GOAL_WIDTH / 2 + GoalBuffer);
	RGOAL_CENTRE_POS = CGeoPoint(Param::Field::PITCH_LENGTH / 2,0);

	//�Ž��õ�3�������㡢���޵�
	RDENFENCE_LEFT_POINT = CGeoPoint(290*Param::Field::RATIO,-45*Param::Field::RATIO);
	RDENFENCE_RIGHT_POINT = CGeoPoint(290*Param::Field::RATIO,45*Param::Field::RATIO);
	RDENFENCE_MIDDLE_POINT = CGeoPoint(280*Param::Field::RATIO,0);//ԭ250
	RD_LEFT_POINT = CGeoPoint(285*Param::Field::RATIO,-30*Param::Field::RATIO);//ע�������������������������㲻ͬ��Ҫ�������Ͳ�Ҫ��ƴ�Ž���
	RD_RIGHT_POINT = CGeoPoint(285*Param::Field::RATIO,30*Param::Field::RATIO);
	//�Ž��滮�����ڵ���
	BASE_LINE_LEFT = CGeoLine(RDENFENCE_LEFT_POINT,RDENFENCE_MIDDLE_POINT);
	BASE_LINE_RIGHT = CGeoLine(RDENFENCE_RIGHT_POINT,RDENFENCE_MIDDLE_POINT);

	//һЩ�����õ��м��
	PEN_RADIUS = Param::Field::PENALTY_AREA_R + Param::Vehicle::V2::PLAYER_SIZE + AVOID_PENALTY_BUFFER;
	PEN_DEPTH = Param::Field::PENALTY_AREA_DEPTH + Param::Vehicle::V2::PLAYER_SIZE + AVOID_PENALTY_BUFFER;
	//���ض�Ա վλ�� ��ǰ�߶����ߵ��������޵�
	RCENTER_LEFT = CGeoPoint(Param::Field::PITCH_LENGTH/2 - PEN_DEPTH,-Param::Field::PENALTY_AREA_L/2);
    RCENTER_RIGHT = CGeoPoint(Param::Field::PITCH_LENGTH/2 - PEN_DEPTH,Param::Field::PENALTY_AREA_L/2);
	//�������˵��������޵�
	RBOTTOM_LEFT = CGeoPoint(Param::Field::PITCH_LENGTH/2,-PEN_RADIUS - Param::Field::PENALTY_AREA_L/2);
	RBOTTOM_RIGHT = CGeoPoint(Param::Field::PITCH_LENGTH/2,PEN_RADIUS + Param::Field::PENALTY_AREA_L/2);
	//��ǰ�߶�ӳ�䵽���ߵ�������
	RCENTER2BOTTOM_LEFT = CGeoPoint(Param::Field::PITCH_LENGTH/2,-Param::Field::PENALTY_AREA_L/2);
	RCENTER2BOTTOM_RIGHT = CGeoPoint(Param::Field::PITCH_LENGTH/2,Param::Field::PENALTY_AREA_L/2);
	RleftGudgePoint = CGeoPoint(200*Param::Field::RATIO,-200*Param::Field::RATIO);
	RrightGudgePoint = CGeoPoint(200*Param::Field::RATIO,200*Param::Field::RATIO);
	_defendPoints.leftD = CGeoPoint(-200*Param::Field::RATIO,-20*Param::Field::RATIO);
	_defendPoints.rightD = CGeoPoint(-200*Param::Field::RATIO,20*Param::Field::RATIO);
	_defendPoints.middleD = CGeoPoint(-270*Param::Field::RATIO,0);
	_lastPoints.leftD = CGeoPoint(-200*Param::Field::RATIO,-20*Param::Field::RATIO);
	_lastPoints.rightD = CGeoPoint(-200*Param::Field::RATIO,20*Param::Field::RATIO);
	_lastPoints.middleD = CGeoPoint(-270*Param::Field::RATIO,0);
	_RleftmostDir = 0;
	_RrightmostDir = 0;
	_RgoalieLeftDir = 0;
	_RgoalieRightDir = 0;
	_RleftgoalDir = 0;
	_RrightgoalDir = 0;
	_RmiddlegoalDir = 0.0;
	_RdefendDir = 0.0;
	_RdefendTarget = CGeoPoint(0,0);
	_lastCycle = 0;	
	_leftDefender = 0;
	_rightDefender = 0;
	_leftDefRecord = 0;
	_rightDefRecord = 0;
}

//����Ŀ�꣺����������!pVision->TheirPlayer(enemyNum).Valid(),
// ballshooted , ball.Valid() && RballVelMod < ENEMY_PASS_SPEED + 20 ������ֶ�������DefendUtils::getEnemyShooter()
//���س����ߣ���getDefenceLine()��ȡ��,ע����س�����һ��Ҫ�ͷ���Ŀ��ͳһ
CDefPos1G2D::~CDefPos1G2D()
{}

defend3 CDefPos1G2D::getDefPos1G2D(const CVisionModule* pVision)
{
	if (pVision->Cycle() == _lastCycle)
	{
		return _defendPoints;
	} else _lastCycle = pVision->Cycle();
	//���ɷ���3����
	generatePos(pVision);
	return _defendPoints;
}

void CDefPos1G2D::updateDefenders(const CVisionModule* pVision)
{
	if (pVision->Cycle() - _leftDefRecord > 5 || pVision->Cycle() - _rightDefRecord > 5)
	{
		_leftDefender = 0;
		_rightDefender = 0;
	}
}
 
CGeoPoint CDefPos1G2D::generatePos(const CVisionModule* pVision)
{
	CGeoPoint RgoaliePoint;
	CGeoPoint RleftPoint;
	CGeoPoint RrightPoint;
	//���·�����Աλ����Ϣ
	updateDefenders(pVision);
	const CGeoLine RrealDefendLine = DefendUtils::getDefenceTargetAndLine(_RdefendTarget,_RdefendDir);//��Ҫ���ݣ�ʵ�ʷ��س�����
	const CVector RleftgoalVector = RGOAL_LEFT_POS - _RdefendTarget;
	_RleftgoalDir = RleftgoalVector.dir();
	const CVector RrightgoalVector = RGOAL_RIGHT_POS - _RdefendTarget;
	_RrightgoalDir = RrightgoalVector.dir();
	const CVector RmiddlegoalVector = RGOAL_CENTRE_POS - _RdefendTarget;
	_RmiddlegoalDir = RmiddlegoalVector.dir();
	//�������ߵ����!!
	double RgudgeDir = Utils::Normalize(_RmiddlegoalDir + Param::Math::PI);
	static posSide RtargetSide = POS_SIDE_MIDDLE;
	if (POS_SIDE_MIDDLE == RtargetSide)
	{
		if (RgudgeDir < 0 && RgudgeDir > RgudgeLeftDir + gudgeBuffer)
		{
			RtargetSide = POS_SIDE_LEFT;
		} else if (RgudgeDir > 0 && RgudgeDir < RgudgeRightDir - gudgeBuffer)
		{
			RtargetSide = POS_SIDE_RIGHT;
		}
	} else if (POS_SIDE_LEFT == RtargetSide)
	{
		if (RgudgeDir < RgudgeLeftDir || RgudgeDir > 0)
		{
			RtargetSide = POS_SIDE_MIDDLE;
		}
	} else if (POS_SIDE_RIGHT == RtargetSide)
	{
		if (RgudgeDir > RgudgeRightDir || RgudgeDir < 0)
		{
			RtargetSide = POS_SIDE_MIDDLE;
		}
	}
	if (debug_extreme)
	{
		cout<<"targetSide is "<<RtargetSide<<endl;
	}
	
	if (Utils::InOurPenaltyArea(pVision->Ball().Pos(),PENALTY_BUFFER))
	{
		_defendPoints = _lastPoints;
		//��������ڵĺ�����Ϊ
		RgoaliePoint = DefendUtils::calcGoaliePoint(_RdefendTarget,_RdefendDir,0,_lastPoints.getGoaliePos(),_RdefendDir);
		if (POS_SIDE_RIGHT == RtargetSide)
		{
			RleftPoint = DefendUtils::calcDefenderPoint(RrightGudgePoint,CVector(RGOAL_CENTRE_POS - RrightGudgePoint).dir() - 0.05,POS_SIDE_LEFT);// - 0.05 ������������һ��
			RrightPoint = DefendUtils::calcDefenderPoint(RrightGudgePoint,CVector(RGOAL_CENTRE_POS - RrightGudgePoint).dir() + 0.05,POS_SIDE_RIGHT);
			if (debug_ballInPenalty)
			{
				GDebugEngine::Instance()->gui_debug_line(DefendUtils::reversePoint(_RdefendTarget),DefendUtils::reversePoint(_RdefendTarget)+Utils::Polar2Vector(1000,CVector(RGOAL_CENTRE_POS - RrightGudgePoint).dir() - 0.05 + Param::Math::PI),COLOR_GRAY);
				GDebugEngine::Instance()->gui_debug_line(DefendUtils::reversePoint(_RdefendTarget),DefendUtils::reversePoint(_RdefendTarget)+Utils::Polar2Vector(1000,CVector(RGOAL_CENTRE_POS - RrightGudgePoint).dir() + 0.05 + Param::Math::PI),COLOR_GRAY);
			}
		} else if (POS_SIDE_LEFT == RtargetSide)
		{
			RleftPoint = DefendUtils::calcDefenderPoint(RleftGudgePoint,CVector(RGOAL_CENTRE_POS - RleftGudgePoint).dir() - 0.05,POS_SIDE_LEFT);
			RrightPoint = DefendUtils::calcDefenderPoint(RleftGudgePoint,CVector(RGOAL_CENTRE_POS - RleftGudgePoint).dir() + 0.05,POS_SIDE_RIGHT);
			if (debug_ballInPenalty)
			{
				GDebugEngine::Instance()->gui_debug_line(DefendUtils::reversePoint(_RdefendTarget),DefendUtils::reversePoint(_RdefendTarget)+Utils::Polar2Vector(1000,CVector(RGOAL_CENTRE_POS - RleftGudgePoint).dir() - 0.05 + Param::Math::PI),COLOR_GRAY);
				GDebugEngine::Instance()->gui_debug_line(DefendUtils::reversePoint(_RdefendTarget),DefendUtils::reversePoint(_RdefendTarget)+Utils::Polar2Vector(1000,CVector(RGOAL_CENTRE_POS - RleftGudgePoint).dir() + 0.05 + Param::Math::PI),COLOR_GRAY);
			}
		} else {
			CGeoPoint ballPos = pVision->Ball().Pos();
			CGeoPoint ourGoalPos = CGeoPoint(-Param::Field::PITCH_LENGTH/2,0);
			CVector ourGoal2ballVec = ballPos - ourGoalPos;
			CGeoPoint defendTarPos = ballPos + Utils::Polar2Vector(150,ourGoal2ballVec.dir());
			RleftPoint = DefendUtils::calcDefenderPoint(DefendUtils::reversePoint(defendTarPos),ourGoal2ballVec.dir() - 0.05,POS_SIDE_LEFT);
			RrightPoint = DefendUtils::calcDefenderPoint(DefendUtils::reversePoint(defendTarPos),ourGoal2ballVec.dir() + 0.05,POS_SIDE_RIGHT);
			if (debug_ballInPenalty)
			{
				GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(RleftPoint),COLOR_CYAN);
				GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(RrightPoint),COLOR_CYAN);
			}
		}
		_defendPoints.middleD = DefendUtils::reversePoint(RgoaliePoint);
		_defendPoints.leftD = DefendUtils::reversePoint(RleftPoint);
		_defendPoints.rightD = DefendUtils::reversePoint(RrightPoint);
	} else{
		if (POS_SIDE_RIGHT == RtargetSide)
		{
			/************************************************************************/
			/* ��������ӽ�������С�ǶȽ���ʱ��η���              0.463648   */
			/************************************************************************/
			RgoaliePoint = DefendUtils::calcGoaliePoint(_RdefendTarget,_RdefendDir,0,_lastPoints.getGoaliePos(),_RdefendDir);
			RleftPoint = DefendUtils::calcDefenderPoint(RrightGudgePoint,CVector(RGOAL_CENTRE_POS - RrightGudgePoint).dir() - 0.05,POS_SIDE_LEFT);// - 0.05 ������������һ��
			RrightPoint = DefendUtils::calcDefenderPoint(RrightGudgePoint,CVector(RGOAL_CENTRE_POS - RrightGudgePoint).dir(),POS_SIDE_RIGHT);
		} else if(POS_SIDE_LEFT == RtargetSide)
		{
			RgoaliePoint = DefendUtils::calcGoaliePoint(_RdefendTarget,_RdefendDir,0,_lastPoints.getGoaliePos(),_RdefendDir);
			RleftPoint = DefendUtils::calcDefenderPoint(RleftGudgePoint,CVector(RGOAL_CENTRE_POS - RleftGudgePoint).dir() - 0.05,POS_SIDE_LEFT);
			RrightPoint = DefendUtils::calcDefenderPoint(RleftGudgePoint,CVector(RGOAL_CENTRE_POS - RleftGudgePoint).dir(),POS_SIDE_RIGHT);
			if (debug_extreme)
			{
				GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(_RdefendTarget),COLOR_RED);
				GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(RleftPoint),COLOR_RED);
				GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(RrightPoint),COLOR_RED);
			}
		} else {
			//��һ�μ����Ž�λ�ã�ע��Ӧ�õĳ����ǣ�_RmiddlegoalDir
			RgoaliePoint = DefendUtils::calcGoaliePoint(_RdefendTarget,_RmiddlegoalDir,0,_lastPoints.getGoaliePos(),_RdefendDir);
			_RgoalieLeftDir = _RmiddlegoalDir - DefendUtils::calcBlockAngle(_RdefendTarget,RgoaliePoint);
			_RgoalieRightDir = _RmiddlegoalDir + DefendUtils::calcBlockAngle(_RdefendTarget,RgoaliePoint);
			//������������������վλ��
			RleftPoint = DefendUtils::calcDefenderPoint(_RdefendTarget,_RgoalieLeftDir,POS_SIDE_LEFT);
			if (debug_mainLine)
			{
				GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(RleftPoint),COLOR_YELLOW);
			}
			RrightPoint = DefendUtils::calcDefenderPoint(_RdefendTarget,_RgoalieRightDir,POS_SIDE_RIGHT);
			//�������������������赲��
			const CVector Rtarget2leftDef = RleftPoint - _RdefendTarget;
			_RleftmostDir = Rtarget2leftDef.dir() - DefendUtils::calcBlockAngle(_RdefendTarget,RleftPoint);
			const CVector Rtarget2rightDef = RrightPoint - _RdefendTarget;
			_RrightmostDir = Rtarget2rightDef.dir() + DefendUtils::calcBlockAngle(_RdefendTarget,RrightPoint);
			//ʵ���赲�Ƕ��Ƿ��㹻
			bool leftBlockEnough = _RleftgoalDir >= _RleftmostDir;
			bool rightBlockEnough = _RrightgoalDir <= _RrightmostDir;
			if ( !leftBlockEnough || !rightBlockEnough)
			{
				//���ݳ����ٴ��ж�
				double leftAngleDiff = leftBlockEnough ? 0.0 : (fabs(_RleftmostDir - _RleftgoalDir));
				double rightAngleDiff = rightBlockEnough ? 0.0 : (fabs(_RrightmostDir - _RrightgoalDir));
				double actualDefendDir = 0.0;//����ʵ�ʷ��س���
				if (_RdefendDir < _RmiddlegoalDir)
				{
					//ʵ������ƫ
					actualDefendDir = fabs(_RmiddlegoalDir - _RdefendDir) <= leftAngleDiff ? _RdefendDir : (_RmiddlegoalDir - leftAngleDiff);
				} else {
					//ʵ���߲�ƫ������ƫ
					actualDefendDir = fabs(_RdefendDir - _RmiddlegoalDir) <= rightAngleDiff ? _RdefendDir : (_RmiddlegoalDir + rightAngleDiff);
				}
				//����ʵ�ʳ����жϣ��ڶ��μ�������վλ��
				RgoaliePoint = DefendUtils::calcGoaliePoint(_RdefendTarget,actualDefendDir,0,_lastPoints.getGoaliePos(),_RdefendDir);
				_RgoalieLeftDir = actualDefendDir - DefendUtils::calcBlockAngle(_RdefendTarget,RgoaliePoint);
				_RgoalieRightDir = actualDefendDir + DefendUtils::calcBlockAngle(_RdefendTarget,RgoaliePoint);
				//�ڶ��μ�����������������վλ��
				RleftPoint = DefendUtils::calcDefenderPoint(_RdefendTarget,_RgoalieLeftDir,POS_SIDE_LEFT);
				RrightPoint = DefendUtils::calcDefenderPoint(_RdefendTarget,_RgoalieRightDir,POS_SIDE_RIGHT);
			}		
		}
		//��ʵ��վλ�㴫��˽�б���
		_defendPoints.middleD = DefendUtils::reversePoint(RgoaliePoint);
		_defendPoints.leftD = DefendUtils::reversePoint(RleftPoint);
		_defendPoints.rightD = DefendUtils::reversePoint(RrightPoint);
		//����û��λʱ������Ա�¾���ս
		if (!pVision->OurPlayer(_leftDefender).Valid() ||
			pVision->OurPlayer(_leftDefender).Pos().dist(_defendPoints.leftD) > GOALIE_SELF_DIST ||
			!pVision->OurPlayer(_rightDefender).Valid() ||
			pVision->OurPlayer(_rightDefender).Pos().dist(_defendPoints.rightD) > GOALIE_SELF_DIST)
		{
			if (debug_goalie_self)
			{
				GDebugEngine::Instance()->gui_debug_msg(CGeoPoint(-100,100),"SELF!!!!!!!!!!!!!!!!!!",COLOR_WHITE);
			}
			RgoaliePoint = DefendUtils::calcGoaliePoint(_RdefendTarget,_RdefendDir,0,_lastPoints.getGoaliePos(),_RdefendDir);
			_defendPoints.middleD = DefendUtils::reversePoint(RgoaliePoint);
		}
	}
	//���������,��ʱ��д
	if (debug_mainLine)
	{
		GDebugEngine::Instance()->gui_debug_line(DefendUtils::reversePoint(_RdefendTarget),DefendUtils::reversePoint(_RdefendTarget)+Utils::Polar2Vector(1000,_RleftgoalDir + Param::Math::PI),COLOR_GRAY);
		GDebugEngine::Instance()->gui_debug_line(DefendUtils::reversePoint(_RdefendTarget),DefendUtils::reversePoint(_RdefendTarget)+Utils::Polar2Vector(1000,_RrightgoalDir + Param::Math::PI),COLOR_GRAY);
		GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(RCENTER_LEFT),COLOR_WHITE);
		GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(RCENTER_RIGHT),COLOR_WHITE);
		GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(RBOTTOM_LEFT),COLOR_WHITE);
		GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(RBOTTOM_RIGHT),COLOR_WHITE);
	}
	_lastPoints = _defendPoints;	
	return _defendPoints.middleD;
}

//CGeoLine CDefPos1G2D::getDefenceTargetAndLine(const CVisionModule* pVision)
//{
//	CGeoLine RdefenceLine = CGeoLine(CGeoPoint(0,0),0.0);
//	//ע�⣺��R(reverse)��ͷ�Ķ��Ƿ����ı���������
//	const int enemyNum = DefendUtils::getEnemyShooter(pVision);
//
//	//��
//	const MobileVisionT& ball = pVision->Ball();
//	CGeoPoint RballPos = DefendUtils::reversePoint(ball.Pos());
//	CVector RballVel = Utils::Polar2Vector(ball.Vel().mod(),Utils::Normalize(ball.Vel().dir() + Param::Math::PI));
//	double RballVelMod = ball.Vel().mod();
//	double RballVelDir = RballVel.dir();
//	//��������
//	CVector Rball2LeftGoal = RGOAL_LEFT_POS - RballPos;
//	CVector Rball2RightGoal = RGOAL_RIGHT_POS - RballPos;
//	CVector Rball2GoalCenter = RGOAL_CENTRE_POS - RballPos;
//	double Rball2LeftDir = Rball2LeftGoal.dir();
//	double Rball2RightDir = Rball2RightGoal.dir();
//	double Rball2GoalCenterDir = Rball2GoalCenter.dir();
//	double left2centreAngle = fabs(Utils::Normalize(Rball2LeftDir - Rball2GoalCenterDir));
//	double right2centreAngle = fabs(Utils::Normalize(Rball2RightDir - Rball2GoalCenterDir));
//
//	///�����ߣ��ж�2 ���Ƿ��Ѿ����	
//	bool ballSpeed = RballVelMod >= 50;
//	bool outOfShooter = !(WorldModel::Instance()->getEnemySituation().getBallKeeper().isBallTaked());
//	bool ballDirLimit = Utils::InBetween(RballVelDir,Rball2LeftDir - BALL_SHOOT_DIR_BUFFER,Rball2RightDir + BALL_SHOOT_DIR_BUFFER);
//	bool ballShooted = ballSpeed && outOfShooter && ballDirLimit;
//
//	if (pVision->TheirPlayer(enemyNum).Valid())
//	{
//		const PlayerVisionT& enemy = pVision->TheirPlayer(enemyNum);
//		CGeoPoint RenemyPos = DefendUtils::reversePoint(enemy.Pos());
//		double RenemyDir = Utils::Normalize(enemy.Dir() + Param::Math::PI);		
//
//		///�����ߣ��ж�1 �Ƿ������
//		bool defenceEnemy = false;	//���س�����о�
//		double defendRenemyDir = 0;     //���صĳ���
//
//		//�ж��Ƿ�Է�������:��ʱ�÷����Ժ���λ�ɾȥ***********!!!!!!!!!!!!!!!!
//		const int ourFastPlayer = BestPlayer::Instance()->ourFastestPlayerToBallList().at(0).num;
//		const PlayerVisionT& me = pVision->OurPlayer(ourFastPlayer);
//		bool enemyHasBall = enemy.Pos().dist(ball.Pos()) < me.Pos().dist(ball.Pos()) ? true : false;
//		//TODO �Ժ���������仰*****************************!!!!!!!!!!!!!!
//		//bool enemyHasBall = BestPlayer::Instance()->oppWithBall();
//
//		//�Է����ֺ�������
//		CVector Renemy2ball = RballPos - RenemyPos;
//		CVector Rball2enemy = RenemyPos - RballPos;
//		double Renemy2ballDir = Renemy2ball.dir();
//		bool Renemy2ballDirAdapt = Utils::InBetween(Renemy2ballDir,Rball2LeftDir - ENEMY_FACE_BUFFER,Rball2RightDir + ENEMY_FACE_BUFFER);
//		bool Renemy2ballDistAdapt = RballPos.dist(RenemyPos) < ENEMY_BALL_DIST_BUFFER;
//		defenceEnemy = enemyHasBall && Renemy2ballDirAdapt && Renemy2ballDistAdapt;
//		//�ж϶����Ƿ������
//		bool enemyPass = RballVelMod > ENEMY_PASS_SPEED &&
//			fabs(Utils::Normalize(RballVelDir - Rball2enemy.dir())) < Param::Math::PI / 9.0;
//		//�ܽ��Ƿ������
//		defenceEnemy = defenceEnemy || enemyPass;
//
//		if (defenceEnemy)//��̬��������
//		{
//			if (Utils::InBetween(RenemyDir,Rball2LeftDir - ENEMY_FACE_BUFFER,Rball2LeftDir))
//			{
//				double Renemy2leftEdge = fabs(Utils::Normalize(RenemyDir - Rball2LeftDir));
//				defendRenemyDir = Rball2GoalCenterDir + left2centreAngle * (Renemy2leftEdge / ENEMY_FACE_BUFFER - 1);
//			} else if (Utils::InBetween(RenemyDir,Rball2LeftDir,Rball2RightDir))
//			{
//				defendRenemyDir = RenemyDir;
//			} else if (Utils::InBetween(RenemyDir,Rball2RightDir,Rball2RightDir + ENEMY_FACE_BUFFER))
//			{
//				double Renemy2rightEdge = fabs(Utils::Normalize(RenemyDir - Rball2RightDir));
//				defendRenemyDir = Rball2GoalCenterDir - right2centreAngle * (Renemy2rightEdge / ENEMY_FACE_BUFFER - 1);
//			} else defendRenemyDir = Rball2GoalCenterDir;
//		}
//
//		//���ж�����ִ����ת
//		if (ballShooted)//�˴��� && ball.Valid()��ֻҪ����һ֡��Ҫ�ᶨ���߹�ȥ
//		{
//			RdefenceLine = CGeoLine(RballPos,RballVelDir); //��������
//			_RdefendTarget = RballPos;
//			_RdefendDir = RballVelDir;
//			if (bigbug)
//			{
//				cout<<"11  :"<<_RdefendDir<<endl;
//			}
//		} else if (defenceEnemy)
//		{
//			if (ball.Valid() && RballVelMod < ENEMY_PASS_SPEED + 20)
//			{
//				RdefenceLine = CGeoLine(RballPos,defendRenemyDir);      //����������Ϊ��������
//				_RdefendTarget = RballPos;
//				_RdefendDir = defendRenemyDir;
//				if (bigbug)
//				{
//					cout<<"12  :"<<_RdefendDir<<endl;
//				}
//				if (debug)
//				{
//					GDebugEngine::Instance()->gui_debug_line(RballPos,RballPos+Utils::Polar2Vector(30,defendRenemyDir),COLOR_RED);
//				}
//			} else {
//				RdefenceLine = CGeoLine(RenemyPos,defendRenemyDir);	//��������������ٹ����Գ�����Ϊ��
//				_RdefendTarget = RenemyPos;
//				_RdefendDir = defendRenemyDir;
//				if (bigbug)
//				{
//					cout<<"13  :"<<_RdefendDir<<endl;
//				}
//			}
//		} else
//		{
//			RdefenceLine = CGeoLine(RballPos,RGOAL_CENTRE_POS);//���򡪡�����������
//			_RdefendTarget = RballPos;
//			_RdefendDir = CVector(RGOAL_CENTRE_POS - RballPos).dir();
//			if (bigbug)
//			{
//				cout<<"14  :"<<_RdefendDir<<endl;
//			}
//		}
//	} else {
//		if (ballShooted)
//		{
//			RdefenceLine = CGeoLine(RballPos,RballVelDir);
//			_RdefendTarget = RballPos;
//			_RdefendDir = RballVelDir;
//			if (bigbug)
//			{
//				cout<<"15  :"<<_RdefendDir<<endl;
//			}
//		} else
//		{
//			RdefenceLine = CGeoLine(RballPos,RGOAL_CENTRE_POS);
//			_RdefendTarget = RballPos;
//			_RdefendDir = CVector(RGOAL_CENTRE_POS - RballPos).dir();
//			if (bigbug)
//			{
//				cout<<"16  :"<<_RdefendDir<<endl;
//			}
//		}
//	}
//	if (debug_target_line)
//	{
//		//cout<<WorldModel::Instance()->getEnemySituation().getBallKeeper().Num()<<endl;
//		//cout<<ballShooted<<ballSpeed<<outOfShooter<<ballDirLimit<<endl;
//		CGeoLineLineIntersection pointA = CGeoLineLineIntersection(RdefenceLine,CGeoLine(CGeoPoint(300,-200),CGeoPoint(300,0)));
//		CGeoPoint pointAA = pointA.IntersectPoint();
//		GDebugEngine::Instance()->gui_debug_line(ball.Pos(),ball.Pos()+Utils::Polar2Vector(1000,Rball2LeftDir + Param::Math::PI),COLOR_ORANGE);
//		GDebugEngine::Instance()->gui_debug_line(ball.Pos(),ball.Pos()+Utils::Polar2Vector(1000,Rball2RightDir + Param::Math::PI),COLOR_ORANGE);
//		GDebugEngine::Instance()->gui_debug_line(ball.Pos(),CGeoPoint(-1*pointAA.x(),-1*pointAA.y()),COLOR_GREEN);
//	}
//	return RdefenceLine;
//}

//int CDefPos1G2D::DefendUtils::getEnemyShooter(const CVisionModule* pVision)
//{
//	int shooter;
//	if (pVision->Ball().Vel().mod() < ENEMY_PASS_SPEED)
//	{
//		shooter = WorldModel::Instance()->getEnemySituation().getBallKeeper().Num();
//	} else {
//		const MobileVisionT ball = pVision->Ball();
//		const double ball_speed_dir = ball.Vel().dir();
//		double min_dist = Param::Field::PITCH_LENGTH, dist, angle_diff;
//		int min_player = 0;
//		const double PassAngleDiff = Param::Math::PI / 6;
//		for (int i = 1; i <= Param::Field::MAX_PLAYER; i ++)
//		{
//			if (pVision->TheirPlayer(i).Valid() == true)
//			{
//				angle_diff = fabs(Utils::Normalize(Utils::Normalize(CVector(pVision->TheirPlayer(i).Pos()
//					- ball.Pos()).dir()) - ball_speed_dir));
//				dist = pVision->TheirPlayer(i).Pos().dist(ball.Pos());
//				if (angle_diff < PassAngleDiff && dist < min_dist)
//				{
//					min_player = i;
//					min_dist = dist;
//				}
//			}
//		}
//		if (min_player == 0)
//		{
//			shooter = WorldModel::Instance()->getEnemySituation().getBallKeeper().Num();
//		}
//		else
//		{
//			shooter = min_player;
//		}
//	}
//	return shooter;
//}

//double CDefPos1G2D::DefendUtils::calcBlockAngle(const CGeoPoint& target,const CGeoPoint& player)
//{
//	double dist_target2player = target.dist(player) <= PLAYERSIZE ? (PLAYERSIZE + 0.1) : target.dist(player);
//	return fabs(asin(PLAYERSIZE / dist_target2player));
//}

//CGeoPoint CDefPos1G2D::calcGoaliePoint(const CVisionModule* pVision,const CGeoPoint Rtarget,const double Rdir,const int mode)
//{
//	double xFront,xBack,xExtreme,y,yBack;
//	//static bool comeFront = true;
//	bool ballVel2Goal = pVision->Ball().Vel().mod() > 50 &&
//		fabs(Utils::Normalize(pVision->Ball().Vel().dir() + Param::Math::PI - _RdefendDir)) < Param::Math::PI * 2 / 180;
//
//	if ((Rtarget.x() < 150 || (ballVel2Goal && Rtarget.x() < 230)) && 0 == mode)
//	{
//		xFront = 230;
//		xBack = 250;
//		xExtreme = 235;
//		y = 60;
//		yBack = 25;
//		//��ǰ�Ĳ���
//	} else{
//		xFront = 280;
//		xBack = 290;
//		xExtreme = 285;
//		y = 45;
//		yBack = 30;
//	}
//	//�Ž��õ�3�������㡢���޵�
//	const CGeoPoint RDENFENCE_LEFT_POINT = CGeoPoint(xBack,-1*y);//(290,-45)
//	const CGeoPoint RDENFENCE_RIGHT_POINT = CGeoPoint(xBack,y);//(290,45)
//	const CGeoPoint RDENFENCE_MIDDLE_POINT = CGeoPoint(xFront,0);//(280,0)
//	const CGeoPoint RD_LEFT_POINT = CGeoPoint(xExtreme,-1 * yBack);//ע�������������������������㲻ͬ��Ҫ�������Ͳ�Ҫ��ƴ�Ž���
//	const CGeoPoint RD_RIGHT_POINT = CGeoPoint(xExtreme,yBack);
//	//�Ž��滮�����ڵ���
//	const CGeoLine BASE_LINE_LEFT = CGeoLine(RDENFENCE_LEFT_POINT,RDENFENCE_MIDDLE_POINT);
//	const CGeoLine BASE_LINE_RIGHT = CGeoLine(RDENFENCE_RIGHT_POINT,RDENFENCE_MIDDLE_POINT);
//
//	CGeoPoint RGoaliePoint = CGeoPoint(280,0); //�������Ž���
//	CGeoLine ReverseDefenceLine = CGeoLine(Rtarget,Rdir);
//	const MobileVisionT& ball = pVision->Ball();
//	CGeoPoint RballPos = DefendUtils::reversePoint(ball.Pos());
//	posSide RBSide = RballPos.y() > 0 ? POS_SIDE_RIGHT : POS_SIDE_LEFT;//����� �����ı�
//	CGeoLine baseLine;
//	if (POS_SIDE_LEFT == RBSide)//�������������
//	{
//		baseLine = BASE_LINE_LEFT;
//		CGeoLineLineIntersection defencePoint = CGeoLineLineIntersection(ReverseDefenceLine,baseLine);
//		if (defencePoint.Intersectant())
//		{
//			CGeoPoint tempPoint = defencePoint.IntersectPoint();
//			if (tempPoint.y() < RDENFENCE_LEFT_POINT.y())
//			{
//				RGoaliePoint = RDENFENCE_LEFT_POINT;
//			} else if (tempPoint.y() > RDENFENCE_MIDDLE_POINT.y())
//			{
//				baseLine = BASE_LINE_RIGHT;
//				defencePoint = CGeoLineLineIntersection(ReverseDefenceLine,baseLine);
//				tempPoint = defencePoint.IntersectPoint();
//				if (tempPoint.y() > RD_RIGHT_POINT.y())
//				{
//					RGoaliePoint = RD_RIGHT_POINT;
//				} else RGoaliePoint = tempPoint;
//			} else RGoaliePoint = tempPoint;
//		} 
//	} else if(POS_SIDE_RIGHT == RBSide)//�������ұ�����
//	{
//		baseLine = BASE_LINE_RIGHT;
//		CGeoLineLineIntersection defencePoint = CGeoLineLineIntersection(ReverseDefenceLine,baseLine);
//		if (defencePoint.Intersectant())
//		{
//			CGeoPoint tempPoint = defencePoint.IntersectPoint();
//			if (tempPoint.y() > RDENFENCE_RIGHT_POINT.y())
//			{
//				RGoaliePoint = RDENFENCE_RIGHT_POINT;
//			} else if (tempPoint.y() < RDENFENCE_MIDDLE_POINT.y())
//			{
//				baseLine = BASE_LINE_LEFT;
//				defencePoint = CGeoLineLineIntersection(ReverseDefenceLine,baseLine);
//				tempPoint = defencePoint.IntersectPoint();
//				if (tempPoint.y() < RD_LEFT_POINT.y())
//				{
//					RGoaliePoint = RD_LEFT_POINT;
//				} else RGoaliePoint = tempPoint;
//			} else RGoaliePoint = tempPoint;
//		}
//	}
//	int preTime = (int)ball.Vel().mod();
//	CGeoPoint ballPre = BallSpeedModel::Instance()->posForTime(preTime,pVision);
//	//������������
//	if (ball.Vel().mod() < 120 && Utils::InOurPenaltyArea(ball.Pos(),PENALTY_BUFFER))
//	{
//		if (!Utils::OutOfField(ballPre,0))
//		{
//			RGoaliePoint = DefendUtils::reversePoint(ballPre);
//		} else RGoaliePoint = DefendUtils::reversePoint(_lastPoints.getGoaliePos());
//	}
//	return RGoaliePoint;
//}

//CGeoPoint CDefPos1G2D::DefendUtils::calcDefenderPoint(const CVisionModule* pVision,const CGeoPoint Rtarget,const double Rdir,const posSide Rside)
//{
//	//�����һ������ĺ���վλ��
//	CGeoPoint RdefenderPoint;
//	int sideFactor;
//	if (POS_SIDE_LEFT == Rside)
//	{
//		sideFactor = -1;
//	} else if (POS_SIDE_RIGHT == Rside)
//	{
//		sideFactor = 1;
//	} else if (POS_SIDE_MIDDLE == Rside)
//	{
//		sideFactor = 0;
//	}
//	CVector transVector = Utils::Polar2Vector(PLAYERSIZE,Utils::Normalize(Rdir + sideFactor * Param::Math::PI / 2));
//	CGeoPoint transPoint = Rtarget + transVector;
//	CGeoLine targetLine = CGeoLine(transPoint,Rdir);//���ڼ��㽻���ֱ�ߣ�Ϊԭ����ֱ�߰���Ҫ��ƽ��PLAYERSIZE���ֱ��
//	
//	//���ض�Ա վλ�� ��ǰ�߶����ߵ��������޵�
//	// RCENTER_LEFT
//	// RCENTER_RIGHT
//	//�������˵��������޵�
//	// RBOTTOM_LEFT
//	// RBOTTOM_RIGHT
//	//��ǰ�߶�ӳ�䵽���ߵ�������
//	// RCENTER2BOTTOM_LEFT
//	// RCENTER2BOTTOM_RIGHT
//
//	//�������ڹ滮��
//	// CGeoLine RD_CENTER_SEGMENT
//	// CGeoCirlce RD_CIR_LEFT
//	// CGeoCirlce RD_CIR_RIGHT
//
//	// transPoint �����߻��㣬Rdir �����߷���defendLine ������
//	bool Up;
//	posSide tpSide;
//	if (transPoint.x() >= RCENTER_LEFT.x())
//	{
//		Up = true;
//	} else Up = false;
//	if (transPoint.y() > 0)
//	{
//		tpSide = POS_SIDE_RIGHT;
//	} else tpSide = POS_SIDE_LEFT;
//
//	CGeoPoint temp[3];//������ʱ��,��Ϊ���������������
//	if (Up && POS_SIDE_RIGHT == tpSide)
//	{
//		int pointCount = 0;
//		CGeoLineCircleIntersection intersect = CGeoLineCircleIntersection(targetLine,RD_CIR_RIGHT);
//		if (intersect.intersectant())
//		{
//			if (DefendUtils::rightCirValid(intersect.point1()))
//			{
//				temp[pointCount] = intersect.point1();
//				pointCount++;
//			}
//			if (DefendUtils::rightCirValid(intersect.point2()))
//			{
//				temp[pointCount] = intersect.point2();
//				pointCount++;
//			}
//			if (0 == pointCount)
//			{
//				if (Rdir < -Param::Math::PI/2)
//				{
//					RdefenderPoint = CGeoPoint(RBOTTOM_RIGHT.x() - PLAYERSIZE,RBOTTOM_RIGHT.y());
//				} else RdefenderPoint = RCENTER_RIGHT;
//			} else if (1 == pointCount)
//			{
//				RdefenderPoint = temp[0];
//			} else { //2 == pointCount
//				if (Rdir > 0)
//				{
//					if (temp[0].y() > temp[1].y())
//					{
//						RdefenderPoint = temp[1];
//					} else RdefenderPoint = temp[0];
//				} else {
//					if (temp[0].y() > temp[1].y())
//					{
//						RdefenderPoint = temp[0];
//					} else RdefenderPoint = temp[1];
//				}
//			} 
//		} else {//û����
//			if (Rdir < -Param::Math::PI/2)
//			{
//				RdefenderPoint = CGeoPoint(RBOTTOM_RIGHT.x() - PLAYERSIZE,RBOTTOM_RIGHT.y());
//			} else RdefenderPoint = RCENTER_RIGHT;
//		}
//	} 
//	else if (Up && POS_SIDE_LEFT == tpSide)
//	{
//		int pointCount = 0;
//		CGeoLineCircleIntersection intersect = CGeoLineCircleIntersection(targetLine,RD_CIR_LEFT);
//		if (intersect.intersectant())
//		{
//			if (DefendUtils::leftCirValid(intersect.point1()))
//			{
//				temp[pointCount] = intersect.point1();
//				pointCount++;
//			}
//			if (DefendUtils::leftCirValid(intersect.point2()))
//			{
//				temp[pointCount] = intersect.point2();
//				pointCount++;
//			}
//			if (0 == pointCount)
//			{
//				if (Rdir > Param::Math::PI/2)
//				{
//					RdefenderPoint = CGeoPoint(RBOTTOM_LEFT.x() - PLAYERSIZE,RBOTTOM_LEFT.y());
//				} else RdefenderPoint = RCENTER_LEFT;
//			} else if (1 == pointCount)
//			{
//				RdefenderPoint = temp[0];
//			} else { //2 == pointCount
//				if (Rdir > 0)
//				{
//					if (temp[0].y() > temp[1].y())
//					{
//						RdefenderPoint = temp[1];
//					} else RdefenderPoint = temp[0];
//				} else {
//					if (temp[0].y() > temp[1].y())
//					{
//						RdefenderPoint = temp[0];
//					} else RdefenderPoint = temp[1];
//				}
//			} 
//		} else {//û����
//			if (Rdir > Param::Math::PI/2)
//			{
//				RdefenderPoint = CGeoPoint(RBOTTOM_LEFT.x() - PLAYERSIZE,RBOTTOM_LEFT.y());
//			} else RdefenderPoint = RCENTER_LEFT;
//		}
//	} 
//	else if (!Up && POS_SIDE_RIGHT == tpSide)
//	{
//		int pointCount = 0;
//		CGeoLineCircleIntersection intersect = CGeoLineCircleIntersection(targetLine,RD_CIR_RIGHT);
//		if (intersect.intersectant())
//		{
//			if (DefendUtils::rightCirValid(intersect.point1()))
//			{
//				temp[pointCount] = intersect.point1();
//				pointCount++;
//			}
//			if (DefendUtils::rightCirValid(intersect.point2()))
//			{
//				temp[pointCount] = intersect.point2();
//				pointCount++;
//			}
//		}
//		CGeoLineLineIntersection intersect1 = CGeoLineLineIntersection(targetLine,RD_CENTER_SEGMENT);
//		if (intersect1.Intersectant())
//		{
//			CGeoPoint interP = intersect1.IntersectPoint();
//			if (interP.y() <= RCENTER_RIGHT.y() && interP.y() >= RCENTER_LEFT.y())
//			{
//				temp[pointCount] = interP;
//				pointCount++;
//			}			
//		}
//		double compareDir = CVector(RCENTER_RIGHT - transPoint).dir();
//		if (debug_defenderPos)
//		{
//			GDebugEngine::Instance()->gui_debug_x(DefendUtils::reversePoint(transPoint),COLOR_YELLOW);
//			GDebugEngine::Instance()->gui_debug_line(DefendUtils::reversePoint(transPoint),DefendUtils::reversePoint(transPoint)+Utils::Polar2Vector(1000,Utils::Normalize(compareDir+Param::Math::PI)),COLOR_RED);
//			GDebugEngine::Instance()->gui_debug_line(DefendUtils::reversePoint(transPoint),DefendUtils::reversePoint(transPoint)+Utils::Polar2Vector(1000,Utils::Normalize(Rdir+Param::Math::PI)),COLOR_YELLOW);
//		}
//		if (0 == pointCount)
//		{
//			if (Rdir < compareDir)
//			{
//				RdefenderPoint = RCENTER_LEFT;
//			} else RdefenderPoint = CGeoPoint(RBOTTOM_RIGHT.x() - PLAYERSIZE,RBOTTOM_RIGHT.y());
//		} else if (1 == pointCount)
//		{
//			RdefenderPoint = temp[0];
//		} else {// pointCount >= 2
//			if (temp[0].y() <= temp[1].y())
//			{
//				RdefenderPoint = temp[0];
//			} else RdefenderPoint = temp[1];
//		}
//	} 
//	else ///  !Up && POS_SIDE_LEFT == tpSide
//	{
//		int pointCount = 0;
//		CGeoLineCircleIntersection intersect = CGeoLineCircleIntersection(targetLine,RD_CIR_LEFT);
//		if (intersect.intersectant())
//		{
//			if (DefendUtils::leftCirValid(intersect.point1()))
//			{
//				temp[pointCount] = intersect.point1();
//				pointCount++;
//			}
//			if (DefendUtils::leftCirValid(intersect.point2()))
//			{
//				temp[pointCount] = intersect.point2();
//				pointCount++;
//			}
//		}
//		CGeoLineLineIntersection intersect1 = CGeoLineLineIntersection(targetLine,RD_CENTER_SEGMENT);
//		if (intersect1.Intersectant())
//		{
//			CGeoPoint interP = intersect1.IntersectPoint();
//			if (interP.y() <= RCENTER_RIGHT.y() && interP.y() >= RCENTER_LEFT.y())
//			{
//				temp[pointCount] = interP;
//				pointCount++;
//			}			
//		}
//		double compareDir = CVector(RCENTER_LEFT - transPoint).dir();
//		if (0 == pointCount)
//		{
//			if (Rdir > compareDir)
//			{
//				RdefenderPoint = RCENTER_RIGHT;
//			} else RdefenderPoint = CGeoPoint(RBOTTOM_LEFT.x() - PLAYERSIZE,RBOTTOM_LEFT.y());
//		} else if (1 == pointCount)
//		{
//			RdefenderPoint = temp[0];
//		} else {// pointCount >= 2
//			if (temp[0].y() >= temp[1].y())
//			{
//				RdefenderPoint = temp[0];
//			} else RdefenderPoint = temp[1];
//		}
//	}
//	return RdefenderPoint;
//}

//bool CDefPos1G2D::DefendUtils::leftCirValid(const CGeoPoint& p)
//{
//	return (p.x() < RBOTTOM_LEFT.x()) && (p.x() > RCENTER_LEFT.x())
//		&& (p.y() < RCENTER_LEFT.y());
//}

//bool CDefPos1G2D::DefendUtils::rightCirValid(const CGeoPoint& p)
//{
//	return (p.x() < RBOTTOM_RIGHT.x()) && (p.x() > RCENTER_RIGHT.x())
//		&& (p.y() > RCENTER_RIGHT.y());
//}