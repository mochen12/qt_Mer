#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qt_Mer.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "GalaxyIncludes.h"
#include "String"
#include <vector>

//qt������Ƕ����
//�źŲ۱�����qt����

//#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
//# pragma execution_character_set("utf-8")    
//#endif

using namespace cv;
using namespace std;



class QtCaptureEventHandler : public ICaptureEventHandler, public QLabel
{
public:
	QtCaptureEventHandler(QWidget* parent = nullptr);
	//QtCaptureEventHandler(string outputPicFolder, gxstring SN);
	~QtCaptureEventHandler();
public:
	gxstring m_RunningSN;
	string m_OutputPicFolder;
	cv::Mat capturedImg; //����ɼ���ͼƬ
	void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
	void pictureSave(CImageDataPointer& objImageDataPointer, Mat capturedImg);  //����ͼƬ
};

class qt_Mer : public QMainWindow, public IDeviceOfflineEventHandler
{
    Q_OBJECT

public:
    qt_Mer(QWidget *parent = nullptr);
    ~qt_Mer();

public:
	//void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
	void DoOnDeviceOfflineEvent(void* pUserParam); //�����¼��ص�����
 
public:
	std::map<gxstring, int> snMap;
	int snIndex;
	int numSN;
	CGXDevicePointer m_objDevicePtr; //�豸ָ�룬ָ��򿪵����
	vector<CGXDevicePointer> m_objDevicePtrVector;
	CGXFeatureControlPointer m_objRemoteFeatureControlPtr; //��Զ�ˣ��豸���Կ�����ָ��
	vector<CGXFeatureControlPointer> m_objRemoteFeatureControlPtrVector;
	CGXFeatureControlPointer m_objFeatureControlPtr; //���غ������Կ�����
	vector<CGXFeatureControlPointer> m_objFeatureControlPtrVector;
	CGXStreamPointer m_objStreamPtr; //��ָ�룬ָ������򿪵���
	vector<CGXStreamPointer> m_objStreamPtrVector;
	
	//GX_DEVICE_OFFLINE_CALLBACK_HANDLE m_hDeviceOffline; //�����¼�
	GX_DEVICE_CLASS_LIST objDeviceClass;//�豸����
	CGXDeviceInfo m_deviceInfo;//�豸��Ϣ
	QString m_firmName;//��������
	QString m_modelName;//�豸�ͺ�
	//QString m_serialNumName;//���к�
	//QString m_ipName;//ip��ַ
	//QString m_maskName;//����
	QString m_macName;//MAC��ַ
	int64_t m_width;
	int64_t m_height;
	QString m_bitDepth;
	
	//���״̬�ж�
	bool isInstance; //����Ƿ��ʼ��
	bool isOpenCam; //����Ƿ��
	//bool isOpenStream; //������Ƿ��
	vector<char> isOpenStreamVector;
	bool isOffline; //�豸�Ƿ����
	bool isTrigger; //����Ƿ񴥷�
	int exposeTime; //�ع�ʱ��

	//��������
	int m_fps;
	int m_ExposureTime;  //�趨�ع�ʱ��
	//ģ�����
	gxstring m_GainAuto;	//�Զ�����ѡ�� Once Continuous Off
	int m_Gain;	//	offʱ����ֵ
	int m_AutoGainMin;
	int m_AutoGainMax;

	gxstring m_BalanceRatioSelector;		//��ƽ��ͨ��ѡ�� Red Green Blue
	int m_BalanceRatio;	//offʱ��ƽ��ϵ��
	gxstring m_BalanceWhiteAuto;	//�Զ���ƽ��ѡ�� Once Continuous Off

	string outputPicFolder;
	static vector<string> outputPicFolderVector;

	QString m_triggerMode;
	QString m_triggerSource;

public:
	Ui::qt_MerClass ui;

public slots:
	void on_openDevice_clicked();
	void on_closeDevice_clicked();
	void on_startCapture_clicked();
	void on_stopCapture_clicked();
	//void on_TriggerSoftware_clicked();
	//void getExposeTime();
	void updateGainState(int index);
	void updateBalanceState(int index);
	void updateButtonOn();
	void updateButtonOff();
	void validateGainminInput();	//���������޶Ա�
	void validateGainmaxInput();	//���������޶Ա�
	void validateGainInput();
	void validateBalanceRatioInput();
	void validateFpsInput();
	void validateExposeTimeInput();
	void writeSettings();

public:
	void updateUI();
 	void updateInput();
	void folderCreated(string SN);	// �����ļ���
	void loadSettings();
	void settingsToUI();
	gxstring getCurrentSN();
	void updateSNList(gxdeviceinfo_vector vectorDeviceInfo);
	void openAndSet(int index);
	void showMatInLabel(Mat matImg, QString m_FrameID, gxstring m_RunningSN);
	

};

//class QtCaptureEventHandler : public QObject, public ICaptureEventHandler
//{
//	Q_OBJECT
//public:
//	QtCaptureEventHandler(QObject* parent = nullptr);
//	~QtCaptureEventHandler();
//public:
//	string m_OutputPicFolder;
//	cv::Mat capturedImg; //����ɼ���ͼƬ
//	void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
//	void pictureSave(CImageDataPointer& objImageDataPointer, Mat capturedImg);  //����ͼƬ
//signals:
//	void transMat(Mat matImg, QString m_FrameID);
//};


