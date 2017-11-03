/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team��		SSL-ZJUNlict											*/
/* HomePage:	http://www.nlict.zju.edu.cn/ssl/WelcomePage.html		*/
/************************************************************************/
/* File:	  DefPos1G2D.h												*/
/* Func:	  1���Ž���2���������Ϸ���									*/
/* Author:	  ��Ⱥ 2012-08-18											*/
/* Refer:	  ###														*/
/* E-mail:	  wangqun1234@zju.edu.cn									*/
/* Version:	  0.0.1														*/
/************************************************************************/

#ifndef _DEFPOS_1G_2D_H_
#define _DEFPOS_1G_2D_H_

#include "AtomPos.h"
#include <singleton.h>
#include "DefendUtils.h"




/** 
@brief  �Ž�վλ��ͺ���վλ��Ľṹ�� */
typedef struct  
{
	CGeoPoint leftD;	///<�����վλ��
	CGeoPoint rightD;	///<�Һ���վλ��
	CGeoPoint middleD;	///<�Ž�վλ��
	/** 
	@brief  �����վλ��Ķ���μ��ӿ� */
	CGeoPoint getLeftPos(){return leftD;}
	/** 
	@brief  �Һ���վλ��Ķ���μ��ӿ� */
	CGeoPoint getRightPos(){return rightD;}
	/** 
	@brief  �Ž�վλ��Ķ���μ��ӿ� */
	CGeoPoint getGoaliePos(){return middleD;}
} defend3;

class CVisionModule;

/**
@brief    1���Ž���2���������Ϸ���
@details  ����ļ��㺯���У��ǽ����е�λ����Ϣȡ�����ټ����
@note	  ע�⣡�ڵ��ñ�վλ����ʱ��Ҫע�ᳵ�ţ���ͬʱ����setLeftDefender��setRightDefender����*/
class CDefPos1G2D:public CAtomPos
{
public:
	CDefPos1G2D();
	~CDefPos1G2D();	

	/** 
	@brief  ����վλ��ĺ������ڲ�ʹ�� */
	virtual CGeoPoint generatePos(const CVisionModule* pVision);

	/** 
	@brief  ����ʵ�������ߣ����Ҹ��·���Ŀ���ͷ��س��� */
	//CGeoLine getDefenceTargetAndLine(const CVisionModule* pVision);
	/** 
	@brief  �ҳ��з����п������ŵĶ�Ա */
	//int DefendUtils::getEnemyShooter(const CVisionModule* pVision);
	///** 
	//@brief  ��������Ա��վλ�� */
	//CGeoPoint calcGoaliePoint(const CVisionModule* pVision,const CGeoPoint Rtarget,const double Rdir,const int mode = 0);
	/////** 
	//@brief  ���������վλ�� */
	//CGeoPoint DefendUtils::calcDefenderPoint(const CVisionModule* pVision,const CGeoPoint Rtarget,const double Rdir,const posSide Rside);

	/** 
	@brief  �����ҷ�ĳһ��Ա ��Ŀ������ҷ����ŵ����ŽǶȵ� �谭�� */
	/*double DefendUtils::calcBlockAngle(const CGeoPoint& target,const CGeoPoint& player);*/
	/** 
	@brief  ����ĳһ���������ֱ������ϵ����ԭ��ĶԳƵ� */
	//CGeoPoint reversePoint(const CGeoPoint& p){return CGeoPoint(-1*p.x(),-1*p.y());}
	/** 
	@brief  �������Բ�ϵĵ��Ƿ��ڽ��� */
	//bool DefendUtils::leftCirValid(const CGeoPoint& p);
	/** 
	@brief  �����Ұ�Բ�ϵĵ��Ƿ��ڽ��� */
	//bool DefendUtils::rightCirValid(const CGeoPoint& p);

	/** 
	@brief �ⲿȡ��ӿ� */
	defend3 getDefPos1G2D(const CVisionModule* pVision);
	/** 
	@brief  �ⲿ���ñ���ʱ��ע����������ŵĽӿ�
	@param  num �ҷ�������ĳ���
	@param	cycle ��֡�µ�֡��*/
	void setLeftDefender(const int num,const int cycle){_leftDefender = num;_leftDefRecord = cycle;}
	/** 
	@brief  �ⲿ���ñ���ʱ��ע���Һ������ŵĽӿ� 
	@param  num �ҷ��Һ����ĳ���
	@param	cycle ��֡�µ�֡��*/
	void setRightDefender(const int num,const int cycle){_rightDefender = num;_rightDefRecord = cycle;}

	/** 
	@brief  ���º�����Ϣ�������ʱ��û�е��ñ��������������Ա����Ϊ0�� */
	void updateDefenders(const CVisionModule* pVision);

	//Ϊ���������㷨��ļ̳У��˴�ʹ��protect
protected:
	defend3 _defendPoints;	
	defend3 _lastPoints;
	//����Ŀ������ŵķ������ݣ�ע���R(reverse)��ͷ�Ķ��Ƿ����Ժ�ı���
	CGeoPoint _RdefendTarget;//����Ŀ���
	double	  _RdefendDir;//���س���	
	double _RleftgoalDir;
	double _RrightgoalDir;
	double _RmiddlegoalDir;
	
	double _RgoalieLeftDir;
	double _RgoalieRightDir;
	double _RleftmostDir;
	double _RrightmostDir;
	
	int _lastCycle;	

private:
	int _leftDefender;
	int _rightDefender;
	int _leftDefRecord;
	int _rightDefRecord;
};

typedef NormalSingleton< CDefPos1G2D > DefPos1G2D;

#endif //_DEFPOS_1G_2D_H_