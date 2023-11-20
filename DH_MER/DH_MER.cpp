#pragma once

#include "DH_MER.h"
#include<QWidget>


DH_MER::DH_MER(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	isOpenCam = false;
	isOpenStream = false;
	isOffline = false;
	isTrigger = false;
	exposeTime = 30000;
	m_hDeviceOffline = NULL;
	m_firmName = "";
	m_modelName = "";
	m_serialNumName = "";
	m_ipName = "";
	m_maskName = "";
	m_macName = "";
	m_width = 0;
	m_height = 0;
	m_bitDepth = "";
	m_triggerMode = "";
	m_triggerSource = "";
	//曝光时间
	connect(ui.exposeTime,&QLineEdit::editingFinished, this, &DH_MER::getExposeTime);
	//初始化库
	try
	{
		IGXFactory::GetInstance().Init();//GxIAPICPPEx库在使用之前必须执行初始化
	}
	catch (CGalaxyException& e)
	{
		cout << "错误码: " << e.GetErrorCode() << endl;
		cout << "错误描述信息: " << e.what() << endl;
	}
	ui.TriggerMode->setEnabled(false);
	ui.TriggerSource->setEnabled(false);
	ui.TriggerSoftware->setEnabled(false);
	ui.comboBox_3->setEnabled(false);
}
void DH_MER::on_openDevice_clicked()
{
	try
	{
		if (!isOpenCam)
		{
			//初始化
			IGXFactory::GetInstance().Init();
			//枚举设备
			gxdeviceinfo_vector vectorDeviceInfo;
			IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
			if (0 == vectorDeviceInfo.size())
			{
				cout << "无可用设备!" << endl;
				return;
			}
			cout << vectorDeviceInfo.size() << endl;
			//打开第一台设备
			m_objDevicePtr = IGXFactory::GetInstance().OpenDeviceBySN(vectorDeviceInfo[0].GetSN(), GX_ACCESS_EXCLUSIVE);
			isOpenCam = true;
			m_objRemoteFeatureControlPtr = m_objDevicePtr->GetRemoteFeatureControl();
			m_objFeatureControlPtr = m_objDevicePtr->GetFeatureControl();
			/*设备信息获取*/
			m_deviceInfo = m_objDevicePtr->GetDeviceInfo();
			m_firmName = m_deviceInfo.GetVendorName();
			m_modelName = m_deviceInfo.GetModelName();
			m_serialNumName = m_deviceInfo.GetSN();
			m_ipName = m_deviceInfo.GetIP();
			m_maskName = m_deviceInfo.GetSubnetMask();
			m_macName = m_deviceInfo.GetMAC();
			m_width = m_objRemoteFeatureControlPtr->GetIntFeature("Width")->GetValue();
			m_height = m_objRemoteFeatureControlPtr->GetIntFeature("Height")->GetValue();
			m_bitDepth = m_objRemoteFeatureControlPtr->GetEnumFeature("PixelSize")->GetValue();
//#ifdef _DEBUG
//			//设置心跳超时时间 6s
//			m_objRemoteFeatureControlPtr->GetIntFeature("GevHeartbeatTimeout")->SetValue(6000);
//#endif
//			/*设备初始设置*/
//			//千兆网系列相机设置
//			objDeviceClass = m_objDevicePtr->GetDeviceInfo().GetDeviceClass();
//			if (objDeviceClass == GX_DEVICE_CLASS_GEV)
//			{
//				//提高网络相机的采集性
//				if (true == m_objFeatureControlPtr->IsImplemented("GevSCPSPacketSize"))		//判断设备是否支持流通道数据包功能
//				{
//					//获取当前网络环境的最优包长值
//					int nPacketSize = m_objStreamPtr->GetOptimalPacketSize();
//					//将最优包长值设置为当前设备的流通道包长值
//					m_objRemoteFeatureControlPtr->GetIntFeature("GevSCPSPacketSize")->SetValue(nPacketSize);
//				}
//				//注册掉线事件
//				m_hDeviceOffline = m_objDevicePtr->RegisterDeviceOfflineCallback(this, NULL);
//			}
			//曝光时间
			if (exposeTime > 0 && exposeTime < 200000)
				m_objRemoteFeatureControlPtr->GetFloatFeature("ExposureTime")->SetValue(exposeTime);
			else
				exposeTime = 30000;
			//采集模式
			m_objRemoteFeatureControlPtr->GetEnumFeature("AcquisitionMode")->SetValue("Continuous");
			//触发模式
			m_objRemoteFeatureControlPtr->GetEnumFeature("TriggerSelector")->SetValue("FrameStart");
			m_objRemoteFeatureControlPtr->GetEnumFeature("TriggerMode")->SetValue("Off");
			//自动曝光
			m_objRemoteFeatureControlPtr->GetEnumFeature("ExposureAuto")->SetValue("Off");

			ui.TriggerMode->setEnabled(true);
			ui.TriggerSource->setEnabled(true);
			ui.TriggerSoftware->setEnabled(true);
			ui.comboBox_3->setEnabled(true);
		}
	}
	catch (CGalaxyException& e)
	{
		cout << "错误码: " << e.GetErrorCode() << endl;
		cout << "错误描述信息: " << e.what() << endl;
	}
	catch (std::exception& e)
	{
		cout << "错误描述信息: " << e.what() << endl;
	}
}
void DH_MER::on_closeDevice_clicked()
{
	if (isOpenCam)
	{
		if (isOpenStream)
		{
			//发送停采命令
			m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
			m_objStreamPtr->StopGrab();
			ui.TriggerMode->setEnabled(true);
			ui.TriggerSource->setEnabled(true);
			ui.TriggerSoftware->setEnabled(true);
			ui.comboBox_3->setEnabled(true);
			m_objStreamPtr->UnregisterCaptureCallback();//注销采集回调
			m_objStreamPtr->Close(); //关闭相机流
			isOpenStream = false;
		}
		if (objDeviceClass == GX_DEVICE_CLASS_GEV)
		{
			//注销掉线回调事件
			m_objDevicePtr->UnregisterDeviceOfflineCallback(m_hDeviceOffline);
		}
		m_objDevicePtr->Close(); //关闭相机，释放相机资源
		IGXFactory::GetInstance().Uninit(); //释放GxIAPICPPEx申请的所有资源
		isOpenCam = false;
	}
}
void DH_MER::on_startCapture_clicked() 
{
	if (isOpenCam)
	{
		uint32_t nStreamNum = m_objDevicePtr->GetStreamCount();
		if ((nStreamNum > 0) && (!isOpenStream))
		{
			m_objStreamPtr = m_objDevicePtr->OpenStream(0);
			isOpenStream = true;
		}
		m_objStreamPtr->RegisterCaptureCallback(this, NULL);
		m_objStreamPtr->StartGrab();
		m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStart")->Execute();
	}
	ui.TriggerMode->setEnabled(false);
	ui.TriggerSource->setEnabled(false);
	ui.TriggerSoftware->setEnabled(false);
	ui.comboBox_3->setEnabled(false);
}
void DH_MER::on_stopCapture_clicked()
{
	if (isOpenStream)
	{
		//发送停采命令
		m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
		m_objStreamPtr->StopGrab();
		isOpenStream = false;
	}
	ui.TriggerMode->setEnabled(true);
	ui.TriggerSource->setEnabled(true);
	ui.TriggerSoftware->setEnabled(true);
	ui.comboBox_3->setEnabled(true);
}
void DH_MER::on_TriggerSoftware_clicked()
{
	try
	{
		do
		{
			m_triggerMode = ui.TriggerMode->currentText();
			m_triggerSource = ui.TriggerSource->currentText();
			if ((m_triggerMode =="on") && (m_triggerSource == "Software"))
			{
				uint32_t nStreamNum = m_objDevicePtr->GetStreamCount();
				if ((nStreamNum > 0) && (!isOpenStream))
				{
					m_objStreamPtr = m_objDevicePtr->OpenStream(0);
					isOpenStream = true;
				}
				//发送开采命令
				m_objStreamPtr->StartGrab();
				m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStart")->Execute();
				CImageDataPointer objImageDataPtr = m_objStreamPtr->GetImage(500);//超时时间使用500ms，用户可以自行设定
				if (objImageDataPtr->GetStatus() == GX_FRAME_STATUS_SUCCESS)
				{
					cout << "采集成功" << endl;
					capturedImg.create(objImageDataPtr->GetHeight(), objImageDataPtr->GetWidth(), CV_8UC3);
					void* pRGB24Buffer = NULL;
					//假设原始数据是BayerRG8图像
					pRGB24Buffer = objImageDataPtr->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, true);
					memcpy(capturedImg.data, pRGB24Buffer, (objImageDataPtr->GetHeight()) * (objImageDataPtr->GetWidth()) * 3);
					//resize(img, img, Size(), 0.1, 0.1, CV_INTER_AREA);
					showMatInLabel(capturedImg);
				}
				m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
				m_objStreamPtr->StopGrab();
			}
		} while (0);
	}
	catch (CGalaxyException& e)
	{
		cout << "错误码: " << e.GetErrorCode() << endl;
		cout << "错误描述信息: " << e.what() << endl;
	}
}
void DH_MER::showMatInLabel(Mat matImg)
{
	Mat src = matImg.clone();
	QImage imgForDisplay;
	if (matImg.channels() == 3)//RGB Img
	{
		cv::cvtColor(src, src, cv::COLOR_BGR2RGB);//颜色空间转换
		imgForDisplay = QImage((const uchar*)(src.data), src.cols, src.rows, src.cols * src.channels(), QImage::Format_RGB888);
	}
	else//Gray Img
	{
		imgForDisplay = QImage((const uchar*)(src.data), src.cols, src.rows, src.cols * src.channels(), QImage::Format_Indexed8);
	}
	ui.label_Img->setPixmap(QPixmap::fromImage(imgForDisplay));
}
void DH_MER::getExposeTime()
{
	try
	{
		exposeTime = ui.exposeTime->text().toInt();
		if (exposeTime >= 31)
		{
			m_objRemoteFeatureControlPtr->GetFloatFeature("ExposureTime")->SetValue(exposeTime);
		}
	}
	catch (CGalaxyException& e)
	{
		cout << "错误码: " << e.GetErrorCode() << endl;
		cout << "错误描述信息: " << e.what() << endl;
	}
	catch (std::exception& e)
	{
		cout << "错误描述信息: " << e.what() << endl;
	}
}
void DH_MER::updateUI()
{
	ui.width->setText(QString::number(m_width));
	ui.height->setText(QString::number(m_height));
	ui.factoryName->setText(m_firmName);
	ui.basicModel->setText(m_modelName);
}
void DH_MER::DoOnDeviceOfflineEvent(void* pUserParam)
{
	isOffline = true;
	IGXFactory::GetInstance().Uninit(); //释放GxIAPICPPEx申请的所有资源
	cout << "设备掉线，请检查并重新连接。" << endl;
}
void DH_MER::DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam)
{
	//判断图像是否为完整帧
	if (GX_FRAME_STATUS_SUCCESS == objImageDataPointer->GetStatus())
	{
		//图像获取为完整帧，可以读取图像宽、高、数据格式等
		uint64_t m_width = objImageDataPointer->GetWidth();
		uint64_t m_height = objImageDataPointer->GetHeight();
		//GX_PIXEL_FORMAT_ENTRY emPixelFormat =objImageDataPointer->GetPixelFormat();
		capturedImg.create(objImageDataPointer->GetHeight(), objImageDataPointer->GetWidth(), CV_8UC3);
		void* pRGB24Buffer = NULL;
		//原始数据是 Mono8 图像
		if (GX_PIXEL_FORMAT_MONO8 == objImageDataPointer->GetPixelFormat())
			pRGB24Buffer = objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, true);
		if (objImageDataPointer->GetPixelFormat() == GX_PIXEL_FORMAT_MONO10)
			pRGB24Buffer = objImageDataPointer->ConvertToRGB24(GX_BIT_2_9, GX_RAW2RGB_NEIGHBOUR, true);
		if (pRGB24Buffer != NULL)
		{
			memcpy(capturedImg.data, pRGB24Buffer, (objImageDataPointer->GetHeight()) * (objImageDataPointer->GetWidth()) * 3);
			double d = m_objRemoteFeatureControlPtr->GetFloatFeature("ExposureTime")->GetValue();
			exposeTime = int(d);
		}
		showMatInLabel(capturedImg);
		updateUI();
	}
}