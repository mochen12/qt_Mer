#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qt_Mer.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "GalaxyIncludes.h"
#include "String"
#include <vector>
#include "myWidget.h"

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

//qt������Ƕ����
//�źŲ۱�����qt����

//#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
//# pragma execution_character_set("utf-8")    
//#endif

using namespace cv;
using namespace std;

struct ImageDataWithFrameID
{
	cv::Mat imageData;
	string filePath;
};
//// �ɼ�ͼƬ
//class QtCaptureEventHandler : public ICaptureEventHandler, public QLabel
//{
//public:
//	QtCaptureEventHandler(QWidget* parent = nullptr);
//	//QtCaptureEventHandler(string outputPicFolder, gxstring SN);
//	~QtCaptureEventHandler();
//public:
//	gxstring m_RunningSN;
//	string m_OutputPicFolder;
//	cv::Mat capturedImg; //����ɼ���ͼƬ
//	void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
//	void pictureSave(CImageDataPointer& objImageDataPointer, Mat capturedImg);  //����ͼƬ
//};

// �������
class qt_Mer : public QWidget, public IDeviceOfflineEventHandler, public ICaptureEventHandler
{
    Q_OBJECT
public:
    qt_Mer(gxstring SN, QWidget* parent = nullptr);
    ~qt_Mer();

private:
	std::thread m_saveThread;
	std::condition_variable m_conditionVariable;
	std::mutex m_mutex;	//���л�����
	std::mutex matToShowMutex;	//չʾ������
	std::queue<ImageDataWithFrameID> g_imageQueue; // ͼ�����

public:
	//void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
	void DoOnDeviceOfflineEvent(void* pUserParam); //�����¼��ص�����
 
public:
	int snIndex;
	//int numSN;
	gxstring m_SN;
	CGXDevicePointer m_objDevicePtr; //�豸ָ�룬ָ��򿪵����
	//vector<CGXDevicePointer> m_objDevicePtrVector;
	CGXFeatureControlPointer m_objRemoteFeatureControlPtr; //��Զ�ˣ��豸���Կ�����ָ��
	//vector<CGXFeatureControlPointer> m_objRemoteFeatureControlPtrVector;
	CGXFeatureControlPointer m_objFeatureControlPtr; //���غ������Կ�����
	//vector<CGXFeatureControlPointer> m_objFeatureControlPtrVector;
	CGXStreamPointer m_objStreamPtr; //��ָ�룬ָ������򿪵���
	//vector<CGXStreamPointer> m_objStreamPtrVector;
	
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
	bool isOpenStream; //������Ƿ��
	bool isOffline; //�豸�Ƿ����
	bool isTrigger; //����Ƿ񴥷�
	bool isSave;
	bool firstCollect;
	bool isShow; //�Ƿ�չʾ
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
	cv::Mat capturedImg; //����ɼ���ͼƬ
	cv::Mat matToShow; //չʾͼƬ
	QTimer* timer; //չʾ��ʱ��

	//std::mutex g_mutex; // ������
	//std::condition_variable g_conditionVariable; // ��������
	string Path = "./test.txt";

	QString m_triggerMode;
	QString m_triggerSource;

public:
	MyWidget * ui;
//	Ui::qt_MerClass ui;

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
	void transShow();
	void showMatInLabel();

public:
	void updateUI();
 	void updateInput();
	void folderCreated(string SN);	// �����ļ���
	void loadSettings();
	void settingsToUI();
	gxstring getCurrentSN();
	void openAndSet(gxstring m_SN);
	void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
	void pictureSave(CImageDataPointer& objImageDataPointer, Mat capturedImg, QString m_FrameID);
	void pictureSaveBayer(CImageDataPointer& objImageDataPointer, void* pRGB24Buffer);  //����ͼƬ
	void saveThread();
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


