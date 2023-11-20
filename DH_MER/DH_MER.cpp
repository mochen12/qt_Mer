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
	//�ع�ʱ��
	connect(ui.exposeTime,&QLineEdit::editingFinished, this, &DH_MER::getExposeTime);
	//��ʼ����
	try
	{
		IGXFactory::GetInstance().Init();//GxIAPICPPEx����ʹ��֮ǰ����ִ�г�ʼ��
	}
	catch (CGalaxyException& e)
	{
		cout << "������: " << e.GetErrorCode() << endl;
		cout << "����������Ϣ: " << e.what() << endl;
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
			//��ʼ��
			IGXFactory::GetInstance().Init();
			//ö���豸
			gxdeviceinfo_vector vectorDeviceInfo;
			IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
			if (0 == vectorDeviceInfo.size())
			{
				cout << "�޿����豸!" << endl;
				return;
			}
			cout << vectorDeviceInfo.size() << endl;
			//�򿪵�һ̨�豸
			m_objDevicePtr = IGXFactory::GetInstance().OpenDeviceBySN(vectorDeviceInfo[0].GetSN(), GX_ACCESS_EXCLUSIVE);
			isOpenCam = true;
			m_objRemoteFeatureControlPtr = m_objDevicePtr->GetRemoteFeatureControl();
			m_objFeatureControlPtr = m_objDevicePtr->GetFeatureControl();
			/*�豸��Ϣ��ȡ*/
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
//			//����������ʱʱ�� 6s
//			m_objRemoteFeatureControlPtr->GetIntFeature("GevHeartbeatTimeout")->SetValue(6000);
//#endif
//			/*�豸��ʼ����*/
//			//ǧ����ϵ���������
//			objDeviceClass = m_objDevicePtr->GetDeviceInfo().GetDeviceClass();
//			if (objDeviceClass == GX_DEVICE_CLASS_GEV)
//			{
//				//�����������Ĳɼ���
//				if (true == m_objFeatureControlPtr->IsImplemented("GevSCPSPacketSize"))		//�ж��豸�Ƿ�֧����ͨ�����ݰ�����
//				{
//					//��ȡ��ǰ���绷�������Ű���ֵ
//					int nPacketSize = m_objStreamPtr->GetOptimalPacketSize();
//					//�����Ű���ֵ����Ϊ��ǰ�豸����ͨ������ֵ
//					m_objRemoteFeatureControlPtr->GetIntFeature("GevSCPSPacketSize")->SetValue(nPacketSize);
//				}
//				//ע������¼�
//				m_hDeviceOffline = m_objDevicePtr->RegisterDeviceOfflineCallback(this, NULL);
//			}
			//�ع�ʱ��
			if (exposeTime > 0 && exposeTime < 200000)
				m_objRemoteFeatureControlPtr->GetFloatFeature("ExposureTime")->SetValue(exposeTime);
			else
				exposeTime = 30000;
			//�ɼ�ģʽ
			m_objRemoteFeatureControlPtr->GetEnumFeature("AcquisitionMode")->SetValue("Continuous");
			//����ģʽ
			m_objRemoteFeatureControlPtr->GetEnumFeature("TriggerSelector")->SetValue("FrameStart");
			m_objRemoteFeatureControlPtr->GetEnumFeature("TriggerMode")->SetValue("Off");
			//�Զ��ع�
			m_objRemoteFeatureControlPtr->GetEnumFeature("ExposureAuto")->SetValue("Off");

			ui.TriggerMode->setEnabled(true);
			ui.TriggerSource->setEnabled(true);
			ui.TriggerSoftware->setEnabled(true);
			ui.comboBox_3->setEnabled(true);
		}
	}
	catch (CGalaxyException& e)
	{
		cout << "������: " << e.GetErrorCode() << endl;
		cout << "����������Ϣ: " << e.what() << endl;
	}
	catch (std::exception& e)
	{
		cout << "����������Ϣ: " << e.what() << endl;
	}
}
void DH_MER::on_closeDevice_clicked()
{
	if (isOpenCam)
	{
		if (isOpenStream)
		{
			//����ͣ������
			m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
			m_objStreamPtr->StopGrab();
			ui.TriggerMode->setEnabled(true);
			ui.TriggerSource->setEnabled(true);
			ui.TriggerSoftware->setEnabled(true);
			ui.comboBox_3->setEnabled(true);
			m_objStreamPtr->UnregisterCaptureCallback();//ע���ɼ��ص�
			m_objStreamPtr->Close(); //�ر������
			isOpenStream = false;
		}
		if (objDeviceClass == GX_DEVICE_CLASS_GEV)
		{
			//ע�����߻ص��¼�
			m_objDevicePtr->UnregisterDeviceOfflineCallback(m_hDeviceOffline);
		}
		m_objDevicePtr->Close(); //�ر�������ͷ������Դ
		IGXFactory::GetInstance().Uninit(); //�ͷ�GxIAPICPPEx�����������Դ
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
		//����ͣ������
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
				//���Ϳ�������
				m_objStreamPtr->StartGrab();
				m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStart")->Execute();
				CImageDataPointer objImageDataPtr = m_objStreamPtr->GetImage(500);//��ʱʱ��ʹ��500ms���û����������趨
				if (objImageDataPtr->GetStatus() == GX_FRAME_STATUS_SUCCESS)
				{
					cout << "�ɼ��ɹ�" << endl;
					capturedImg.create(objImageDataPtr->GetHeight(), objImageDataPtr->GetWidth(), CV_8UC3);
					void* pRGB24Buffer = NULL;
					//����ԭʼ������BayerRG8ͼ��
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
		cout << "������: " << e.GetErrorCode() << endl;
		cout << "����������Ϣ: " << e.what() << endl;
	}
}
void DH_MER::showMatInLabel(Mat matImg)
{
	Mat src = matImg.clone();
	QImage imgForDisplay;
	if (matImg.channels() == 3)//RGB Img
	{
		cv::cvtColor(src, src, cv::COLOR_BGR2RGB);//��ɫ�ռ�ת��
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
		cout << "������: " << e.GetErrorCode() << endl;
		cout << "����������Ϣ: " << e.what() << endl;
	}
	catch (std::exception& e)
	{
		cout << "����������Ϣ: " << e.what() << endl;
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
	IGXFactory::GetInstance().Uninit(); //�ͷ�GxIAPICPPEx�����������Դ
	cout << "�豸���ߣ����鲢�������ӡ�" << endl;
}
void DH_MER::DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam)
{
	//�ж�ͼ���Ƿ�Ϊ����֡
	if (GX_FRAME_STATUS_SUCCESS == objImageDataPointer->GetStatus())
	{
		//ͼ���ȡΪ����֡�����Զ�ȡͼ����ߡ����ݸ�ʽ��
		uint64_t m_width = objImageDataPointer->GetWidth();
		uint64_t m_height = objImageDataPointer->GetHeight();
		//GX_PIXEL_FORMAT_ENTRY emPixelFormat =objImageDataPointer->GetPixelFormat();
		capturedImg.create(objImageDataPointer->GetHeight(), objImageDataPointer->GetWidth(), CV_8UC3);
		void* pRGB24Buffer = NULL;
		//ԭʼ������ Mono8 ͼ��
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