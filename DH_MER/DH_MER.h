#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DH_MER.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "GalaxyIncludes.h"

using namespace cv;
using namespace std;

class DH_MER : public QMainWindow, public IDeviceOfflineEventHandler, public ICaptureEventHandler
{
	Q_OBJECT

public:
	DH_MER(QWidget* parent = Q_NULLPTR);

public:
	void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
	void DoOnDeviceOfflineEvent(void* pUserParam); //�����¼��ص�����
public:
	CGXDevicePointer m_objDevicePtr; //�豸ָ�룬ָ��򿪵����
	CGXFeatureControlPointer m_objRemoteFeatureControlPtr; //��Զ�ˣ��豸���Կ�����ָ��
	CGXFeatureControlPointer m_objFeatureControlPtr; //���غ������Կ�����
	CGXStreamPointer m_objStreamPtr; //��ָ�룬ָ������򿪵���
	GX_DEVICE_OFFLINE_CALLBACK_HANDLE m_hDeviceOffline; //�����¼�
	GX_DEVICE_CLASS_LIST objDeviceClass;//�豸����
	CGXDeviceInfo m_deviceInfo;//�豸��Ϣ
	QString m_firmName;//��������
	QString m_modelName;//�豸�ͺ�
	QString m_serialNumName;//���к�
	QString m_ipName;//ip��ַ
	QString m_maskName;//����
	QString m_macName;//MAC��ַ
	int64_t m_width;
	int64_t m_height;
	QString m_bitDepth;
	cv::Mat capturedImg; //����ɼ���ͼƬ
	//���״̬�ж�
	bool isOpenCam; //����Ƿ��
	bool isOpenStream; //������Ƿ��
	bool isOffline; //�豸�Ƿ����
	bool isTrigger; //����Ƿ񴥷�
	int exposeTime; //�ع�ʱ��
	QString m_triggerMode;
	QString m_triggerSource;

private:
	Ui::DH_MERClass ui;

public slots:
	void on_openDevice_clicked();
	void on_closeDevice_clicked();
	void on_startCapture_clicked();
	void on_stopCapture_clicked();
	void on_TriggerSoftware_clicked();
	void getExposeTime();
public:
	void showMatInLabel(Mat matImg);
	void updateUI();
};







