#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qt_Mer.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include "GalaxyIncludes.h"
#include "String"
#include <vector>

//qt不接受嵌套类
//信号槽必须有qt父类

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
	cv::Mat capturedImg; //相机采集的图片
	void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
	void pictureSave(CImageDataPointer& objImageDataPointer, Mat capturedImg);  //保存图片
};

class qt_Mer : public QMainWindow, public IDeviceOfflineEventHandler
{
    Q_OBJECT

public:
    qt_Mer(QWidget *parent = nullptr);
    ~qt_Mer();

public:
	//void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
	void DoOnDeviceOfflineEvent(void* pUserParam); //掉线事件回调函数
 
public:
	std::map<gxstring, int> snMap;
	int snIndex;
	int numSN;
	CGXDevicePointer m_objDevicePtr; //设备指针，指向打开的相机
	vector<CGXDevicePointer> m_objDevicePtrVector;
	CGXFeatureControlPointer m_objRemoteFeatureControlPtr; //（远端）设备属性控制器指针
	vector<CGXFeatureControlPointer> m_objRemoteFeatureControlPtrVector;
	CGXFeatureControlPointer m_objFeatureControlPtr; //本地和流属性控制器
	vector<CGXFeatureControlPointer> m_objFeatureControlPtrVector;
	CGXStreamPointer m_objStreamPtr; //流指针，指向相机打开的流
	vector<CGXStreamPointer> m_objStreamPtrVector;
	
	//GX_DEVICE_OFFLINE_CALLBACK_HANDLE m_hDeviceOffline; //掉线事件
	GX_DEVICE_CLASS_LIST objDeviceClass;//设备类型
	CGXDeviceInfo m_deviceInfo;//设备信息
	QString m_firmName;//厂商名称
	QString m_modelName;//设备型号
	//QString m_serialNumName;//序列号
	//QString m_ipName;//ip地址
	//QString m_maskName;//掩码
	QString m_macName;//MAC地址
	int64_t m_width;
	int64_t m_height;
	QString m_bitDepth;
	
	//相机状态判断
	bool isInstance; //相机是否初始化
	bool isOpenCam; //相机是否打开
	//bool isOpenStream; //相机流是否打开
	vector<char> isOpenStreamVector;
	bool isOffline; //设备是否掉线
	bool isTrigger; //相机是否触发
	int exposeTime; //曝光时间

	//基本参数
	int m_fps;
	int m_ExposureTime;  //设定曝光时间
	//模拟控制
	gxstring m_GainAuto;	//自动增益选择 Once Continuous Off
	int m_Gain;	//	off时增益值
	int m_AutoGainMin;
	int m_AutoGainMax;

	gxstring m_BalanceRatioSelector;		//白平衡通道选择 Red Green Blue
	int m_BalanceRatio;	//off时白平衡系数
	gxstring m_BalanceWhiteAuto;	//自动白平衡选择 Once Continuous Off

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
	void validateGainminInput();	//增益上下限对比
	void validateGainmaxInput();	//增益上下限对比
	void validateGainInput();
	void validateBalanceRatioInput();
	void validateFpsInput();
	void validateExposeTimeInput();
	void writeSettings();

public:
	void updateUI();
 	void updateInput();
	void folderCreated(string SN);	// 创建文件夹
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
//	cv::Mat capturedImg; //相机采集的图片
//	void DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam);
//	void pictureSave(CImageDataPointer& objImageDataPointer, Mat capturedImg);  //保存图片
//signals:
//	void transMat(Mat matImg, QString m_FrameID);
//};


