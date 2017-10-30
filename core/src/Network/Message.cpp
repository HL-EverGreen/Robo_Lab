#include "./Message.h"
#include "tinyxml/ParamReader.h"
#include "PlayInterface.h"

namespace{
	// ע��: ��������ҷ������ǶԷ�������ɫû�й�ϵ,����Ĭ�ϵ�λ���ҷ���������,���Դ浽��λȥ;
	// �����ҷ����Ķӱ���ʲô, ֻ���ڷ�������Ϣʱ�����жϸ���Ϣ�Ƿ�����Լ���
	bool ENEMY_INVERT = false;
	bool VERBOSE_MODE = false;			// ���������Ϣ
	float ANLGE_CALIBRATION[12]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
}

Message::Message()
{
	DECLARE_PARAM_READER_BEGIN(General)
	READ_PARAM(ENEMY_INVERT)
	DECLARE_PARAM_READER_END
}

void Message::message2VisionInfo(CServerInterface::VisualInfo& info)
{
	// ball
	info.ball.x = msgX2InfoX(this->_ballx);
	info.ball.y = msgY2InfoY(this->_bally);
	info.ball.valid = this->_ballFound;
	info.imageBall.valid= this->_CameraID;
	info.imageBall.x = this->_ballImageX;
	info.imageBall.y = this->_ballImageY;
	info.cycle = this->_cycle;

	// player
	//PlayInterface::Instance()->clearRealIndex();
	for (int i = 0; i < Param::Field::MAX_PLAYER*2; ++i ) {
		// ��λ(0-5)��Զ����ҷ���������Ϣ
		if (i < Param::Field::MAX_PLAYER) {
			if (! ENEMY_INVERT) {
				// û��ENEMY_INVERT, ֱ��ͼ����Ϣ���ҷ�����Ϣ�����ҷ�����Ϣ, �з������ǵз���
				info.player[i].angle = msgAngle2InfoAngle(RobotRotation[0][i])+ANLGE_CALIBRATION[RobotINDEX[0][i]-1];
				info.player[i].pos.valid = RobotFound[0][i];
				info.player[i].pos.x = msgX2InfoX(RobotPosX[0][i]);
				info.player[i].pos.y = msgY2InfoY(RobotPosY[0][i]);
				info.ourRobotIndex[i] = RobotINDEX[0][i];
			//	PlayInterface::Instance()->setRealIndex(i+1,RobotINDEX[0][i]);
			} else {
				// ����ENEMY_INVERT, ͼ����Ϣ������ҷ�����Ϣǡ�ǵз�����Ϣ, �з�����ǡ���ҷ���
				info.player[i].angle = msgAngle2InfoAngle(RobotRotation[1][i])+ANLGE_CALIBRATION[RobotINDEX[1][i]-1];
				info.player[i].pos.valid = RobotFound[1][i];
				info.player[i].pos.x = msgX2InfoX(RobotPosX[1][i]);
				info.player[i].pos.y = msgY2InfoY(RobotPosY[1][i]);
				info.ourRobotIndex[i] = RobotINDEX[1][i];
			//	PlayInterface::Instance()->setRealIndex(i+1, RobotINDEX[1][i]);
			}
		// ��λ(6-11)��Զ��ŵз���������Ϣ
		} else {										
			if (! ENEMY_INVERT) {
				// û��invert,ֱ��ͼ����Ϣ���ҷ�����Ϣ�����ҷ�����Ϣ, �з������ǵз���
				info.player[i].angle = msgAngle2InfoAngle(RobotRotation[1][i-Param::Field::MAX_PLAYER]);
				info.player[i].pos.valid = RobotFound[1][i-Param::Field::MAX_PLAYER];
				info.player[i].pos.x = msgX2InfoX(RobotPosX[1][i-Param::Field::MAX_PLAYER]);
				info.player[i].pos.y = msgY2InfoY(RobotPosY[1][i-Param::Field::MAX_PLAYER]);
				info.theirRobotIndex[i-Param::Field::MAX_PLAYER] = RobotINDEX[1][i-Param::Field::MAX_PLAYER];
			}else{
				// ����ENEMY_INVERT, ͼ����Ϣ������ҷ�����Ϣǡ�ǵз�����Ϣ, �з�����ǡ���ҷ���
				info.player[i].angle = msgAngle2InfoAngle(RobotRotation[0][i-Param::Field::MAX_PLAYER]);
				info.player[i].pos.valid = RobotFound[0][i-Param::Field::MAX_PLAYER];
				info.player[i].pos.x = msgX2InfoX(RobotPosX[0][i-Param::Field::MAX_PLAYER]);
				info.player[i].pos.y = msgY2InfoY(RobotPosY[0][i-Param::Field::MAX_PLAYER]);
				info.theirRobotIndex[i-Param::Field::MAX_PLAYER] = RobotINDEX[0][i-Param::Field::MAX_PLAYER];
			}
		}
	}

	return ;
}