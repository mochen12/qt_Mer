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
	void DoOnDeviceOfflineEvent(void* pUserParam); //掉线事件回调函数
public:
	CGXDevicePointer m_objDevicePtr; //设备指针，指向打开的相机
	CGXFeatureControlPointer m_objRemoteFeatureControlPtr; //（远端）设备属性控制器指针
	CGXFeatureControlPointer m_objFeatureControlPtr; //本地和流属性控制器
	CGXStreamPointer m_objStreamPtr; //流指针，指向相机打开的流
	GX_DEVICE_OFFLINE_CALLBACK_HANDLE m_hDeviceOffline; //掉线事件
	GX_DEVICE_CLASS_LIST objDeviceClass;//设备类型
	CGXDeviceInfo m_deviceInfo;//设备信息
	QString m_firmName;//厂商名称
	QString m_modelName;//设备型号
	QString m_serialNumName;//序列号
	QString m_ipName;//ip地址
	QString m_maskName;//掩码
	QString m_macName;//MAC地址
	int64_t m_width;
	int64_t m_height;
	QString m_bitDepth;
	cv::Mat capturedImg; //相机采集的图片
	//相机状态判断
	bool isOpenCam; //相机是否打开
	bool isOpenStream; //相机流是否打开
	bool isOffline; //设备是否掉线
	bool isTrigger; //相机是否触发
	int exposeTime; //曝光时间
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







