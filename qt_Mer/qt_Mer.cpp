#pragma once
#include "qt_Mer.h"
#include <map>
#include <math.h>

#include <QLabel>
#include <QVBoxLayout>
#include <qtimer.h>
#include <qdebug.h>
#include <opencv2\imgproc\types_c.h>
#include <filesystem>
#include <fstream>
#include <io.h>

namespace fs = std::filesystem;
using namespace cv;
using namespace std;

bool isInit;
bool isRunning;
gxdeviceinfo_vector vectorDeviceInfo;

qt_Mer::qt_Mer(gxstring SN, QWidget* parent)
    : QWidget(parent)
{
    m_SN = SN;
    std::cout << "qt_Mer�̵߳�IDΪ: " << std::this_thread::get_id() << std::endl;


    this->resize(1120, 830);
    ui = new MyWidget(this);
    //ui.setupUi(this);
    isInstance = false;
    isOpenCam = false;
    isOpenStream = false;
    //isOffline = false;
    isSave = true;
    isTrigger = false;
    firstCollect = false;
    isShow = true;
    exposeTime = 1000;
    //m_hDeviceOffline = NULL;
    m_firmName = "";
    m_modelName = "";
    //m_serialNumName = "";
    //m_ipName = "";
    //m_maskName = "";
    m_macName = "";
    m_width = 0;
    m_height = 0;
    m_bitDepth = "";
    m_triggerMode = "";
    m_triggerSource = "";

    //չʾ��ʱ��
    timer = new QTimer(this);
    timer->start(200);

    //��������
    m_fps = 20;
    m_ExposureTime = 10000;  //�趨�ع�ʱ��
    //ģ�����
    m_GainAuto = "Continuous";	//�Զ�����ѡ�� Once Continuous Off
    m_Gain = 16;	//	offʱ����ֵ
    m_AutoGainMin = 0;
    m_AutoGainMax = 24;

    m_BalanceRatioSelector = "Red";		//��ƽ��ͨ��ѡ�� Red Green Blue
    m_BalanceRatio = 1;	//offʱ��ƽ��ϵ��
    m_BalanceWhiteAuto = "Continuous";	//�Զ���ƽ��ѡ�� Once Continuous Off

    // ����һ��QIntValidator���󣬲����÷�ΧΪ0-24
    QIntValidator* validator_Gain = new QIntValidator(0, 24, this);
    QIntValidator* validator_Fps = new QIntValidator(1, 210, this);
    QIntValidator* validator_BalanceRatio = new QIntValidator(1, 4000, this);
    // ��QIntValidator��������ΪQLineEdit����֤��
    ui->gainValue->setValidator(validator_Gain);
    ui->gainValueMin->setValidator(validator_Gain);
    ui->gainValueMax->setValidator(validator_Gain);
    ui->fps->setValidator(validator_Fps);
    ui->balanceRatio->setValidator(validator_BalanceRatio);

    connect(ui->openDevice, &QPushButton::clicked, this, &qt_Mer::on_openDevice_clicked);
    connect(ui->closeDevice, &QPushButton::clicked, this, &qt_Mer::on_closeDevice_clicked);
    connect(ui->startCapture, &QPushButton::clicked, this, &qt_Mer::on_startCapture_clicked);
    connect(ui->stopCapture, &QPushButton::clicked, this, &qt_Mer::on_stopCapture_clicked);

    connect(ui->gainAuto, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &qt_Mer::updateGainState);  //�����
    connect(ui->balanceWhiteAuto, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &qt_Mer::updateBalanceState);   //��ƽ���
    //connect(ui->openDevice, &QPushButton::clicked, this, &qt_Mer::updateButtonOn);    //�ɼ���ť��
    connect(ui->closeDevice, &QPushButton::clicked, this, &qt_Mer::updateButtonOff);    //�ɼ���ť��
    connect(ui->gainValueMin, &QLineEdit::textChanged, this, &qt_Mer::validateGainminInput);    //���������޶Ա�
    connect(ui->gainValueMax, &QLineEdit::textChanged, this, &qt_Mer::validateGainmaxInput);    //���������޶Ա�
    connect(ui->gainValue, &QLineEdit::textChanged, this, &qt_Mer::validateGainInput);    //����ֵ����
    connect(ui->balanceRatio, &QLineEdit::textChanged, this, &qt_Mer::validateBalanceRatioInput);    //��ƽ��ϵ������
    connect(ui->fps, &QLineEdit::textChanged, this, &qt_Mer::validateFpsInput);    //fps����
    connect(ui->exposeTime, &QLineEdit::textChanged, this, &qt_Mer::validateExposeTimeInput);    //�ع�ʱ�����
    connect(ui->saveSettings, &QPushButton::clicked, this, &qt_Mer::writeSettings);    //��������
    connect(ui->loadSettings, &QPushButton::clicked, this, &qt_Mer::loadSettings);    //��ȡ����
    connect(ui->showButton, &QPushButton::clicked, this, &qt_Mer::transShow);    //�Ƿ�չʾ
    connect(timer, &QTimer::timeout, this, &qt_Mer::showMatInLabel);
    connect(this, &qt_Mer::frameIDUpdated, this, &qt_Mer::updateFrameID);

    if (ui->gainAuto->currentText() == "Continuous")
    {
        ui->gainValue->setEnabled(false);
        ui->gainValueMin->setEnabled(true);
        ui->gainValueMax->setEnabled(true);
    }
    else 
    {
        ui->gainValue->setEnabled(true);
        ui->gainValueMin->setEnabled(false);
        ui->gainValueMax->setEnabled(false);
    }
    if (ui->gainAuto->currentText() == "Continuous") {
        ui->balanceRatio->setEnabled(false);
    }
    else 
    {
        ui->balanceRatio->setEnabled(true);
    }

    ui->startCapture->setEnabled(false);
    ui->stopCapture->setEnabled(false);
    ui->groupBox_2->setEnabled(true);
    ui->groupBox_3->setEnabled(true);


    try
    {
        cout << "��ʼ��" << endl;
        IGXFactory::GetInstance().Init();//GxIAPICPPEx����ʹ��֮ǰ����ִ�г�ʼ��
    }
    catch (CGalaxyException& e)
    {
        cout << "������: " << e.GetErrorCode() << endl;
        cout << "����������Ϣ: " << e.what() << endl;
    }
    
    //ö���豸
    gxdeviceinfo_vector vectorDeviceInfo;
    IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);

    if (0 == vectorDeviceInfo.size())
    {
        ui->label_Img->setText("�޿����豸��");
        cout << "�޿����豸!" << endl;
        ui->startCapture->setEnabled(false);
        ui->stopCapture->setEnabled(false);
        return;
    }
    isInstance = true;
    m_saveThread = std::thread(&qt_Mer::saveThread, this);
}


qt_Mer::~qt_Mer()
{
    std::cout << "Performing cleanup before quitting..." << std::endl;
    isSave = false; 
    m_conditionVariable.notify_all();
    // �ȴ������߳̽���
    if (m_saveThread.joinable())
    {
        std::cout << "Waiting for save thread to finish..." << std::endl;
        m_saveThread.join();
    }

    // ��ն���
    std::queue<ImageDataWithFrameID> emptyQueue;
    std::swap(g_imageQueue, emptyQueue);

    std::cout << "Cleanup finished" << std::endl;
}

void qt_Mer::updateGainState(int index)
{
    if (ui->gainAuto->itemText(index) == "Continuous") {
        ui->gainValue->setEnabled(false);
        ui->gainValueMin->setEnabled(true);
        ui->gainValueMax->setEnabled(true);
    }
    else {
        ui->gainValue->setEnabled(true);
        ui->gainValueMin->setEnabled(false);
        ui->gainValueMax->setEnabled(false);
    }
}

void qt_Mer::updateBalanceState(int index)
{
    if (ui->gainAuto->itemText(index) == "Continuous") {
        ui->balanceRatio->setEnabled(false);
    }
    else {
        ui->balanceRatio->setEnabled(true);
    }
}

void qt_Mer::updateButtonOn() {
    ui->startCapture->setEnabled(true);
    ui->stopCapture->setEnabled(true);
}

void qt_Mer::updateButtonOff() {
    ui->startCapture->setEnabled(false);
    ui->stopCapture->setEnabled(false);
}

void qt_Mer::validateGainminInput()
{
    int value1 = ui->gainValueMin->text().toInt();
    int value2 = ui->gainValueMax->text().toInt();

    if (value1 >= value2)
    {
        // ���Min��ֵ���ڵ���Max��ֵ����Min��ֵ����ΪMax��ֵ��1
        ui->gainValueMin->setText(QString::number(value2 - 1));
    }
    if (value1 < 0)
    {
        ui->gainValueMin->setText(QString::number(0));
    }
}

void qt_Mer::validateGainmaxInput()
{
    int value1 = ui->gainValueMin->text().toInt();
    int value2 = ui->gainValueMax->text().toInt();

    if (value1 >= value2)
    {
        ui->gainValueMax->setText(QString::number(value1 + 1));
    }
    if (value2 > 24)
    {
        ui->gainValueMax->setText(QString::number(24));
    }
}

void qt_Mer::validateGainInput()
{
    int value1 = ui->gainValue->text().toInt();
    if (value1 > 24)
    {
        // ���ֵ����24������Ϊ24
        ui->gainValue->setText(QString::number(24));
    }
    if (value1 < 0)
    {
        // ���ֵС��0������Ϊ0
        ui->gainValue->setText(QString::number(0));
    }
}

void qt_Mer::validateBalanceRatioInput() {
    int value1 = ui->balanceRatio->text().toInt();
    if (value1 > 4000)
    {
        ui->balanceRatio->setText(QString::number(4000));
    }
    if (value1 < 1)
    {
        ui->balanceRatio->setText(QString::number(1));
    }
}
void qt_Mer::validateFpsInput() {
    int value1 = ui->fps->text().toInt();
    int exposeTime = ui->exposeTime->text().toInt();
    int max = floor(1000000 / exposeTime);
    if (value1 > max)
    {
        ui->fps->setText(QString::number(max));
    }
    if (value1 > 210)
    {
        ui->fps->setText(QString::number(210));
    }
    if (value1 < 1)
    {
        ui->fps->setText(QString::number(1));
    }
}

void qt_Mer::validateExposeTimeInput() {
    int value1 = ui->exposeTime->text().toInt();
    int fps = ui->fps->text().toInt();
    int max = floor(1000000/fps);
    if (value1 > max)
    {
        ui->exposeTime->setText(QString::number(max));
    }
    if (value1 < 1)
    {
        ui->exposeTime->setText(QString::number(1));
    }
}

void qt_Mer::updateInput()
{
    //��������
    m_fps = ui->fps->text().toInt();
    m_ExposureTime = ui->exposeTime->text().toInt();  //�趨�ع�ʱ��
    //ģ�����
    m_GainAuto = ui->gainAuto->currentText().toStdString().c_str();	//�Զ�����ѡ�� Once Continuous Off
    m_Gain = ui->gainValue->text().toInt();	//	offʱ����ֵ
    m_AutoGainMin = ui->gainValueMin->text().toInt();
    m_AutoGainMax = ui->gainValueMax->text().toInt();

    m_BalanceRatioSelector = ui->balanceRatioSelector->currentText().toStdString().c_str();		//��ƽ��ͨ��ѡ�� Red Green Blue
    m_BalanceRatio = ui->balanceRatio->text().toInt();	//offʱ��ƽ��ϵ��
    m_BalanceWhiteAuto = ui->balanceWhiteAuto->currentText().toStdString().c_str();	//�Զ���ƽ��ѡ�� Once Continuous Off
    cout << "�����������" << endl;
};

gxstring qt_Mer::getCurrentSN()
{
    return m_SN;
}

void qt_Mer::openAndSet(gxstring m_SN)
{
    cout << "���豸�����ò���" << endl;
    //�򿪵�ǰ�豸
    /*�豸��Ϣ��ȡ*/
    m_deviceInfo = m_objDevicePtr->GetDeviceInfo();
    m_firmName = m_deviceInfo.GetVendorName();
    m_modelName = m_deviceInfo.GetModelName();
    //m_serialNumName = m_deviceInfo.GetSN();
    //m_ipName = m_deviceInfo.GetIP();
    //m_maskName = m_deviceInfo.GetSubnetMask();
    m_macName = m_deviceInfo.GetMAC();
    m_width = m_objRemoteFeatureControlPtr->GetIntFeature("Width")->GetValue();
    m_height = m_objRemoteFeatureControlPtr->GetIntFeature("Height")->GetValue();
    m_bitDepth = m_objRemoteFeatureControlPtr->GetEnumFeature("PixelSize")->GetValue();

    /*�豸��ʼ����*/
    //�ɼ�֡�ʵ���
    m_objRemoteFeatureControlPtr->GetEnumFeature("AcquisitionFrameRateMode")->SetValue("On");
    m_objRemoteFeatureControlPtr->GetFloatFeature("AcquisitionFrameRate")->SetValue(m_fps);
    //�ع�ʱ��(us)
    m_objRemoteFeatureControlPtr->GetEnumFeature("ExposureMode")->SetValue("Timed");
    m_objRemoteFeatureControlPtr->GetFloatFeature("ExposureTime")->SetValue(m_ExposureTime);
    if (m_fps > 90 && (m_fps * m_ExposureTime < 1000000))
    {
        //�����������
        m_objRemoteFeatureControlPtr->GetEnumFeature("DeviceLinkThroughputLimitMode")->SetValue("Off");
    }
    //����
    m_objRemoteFeatureControlPtr->GetEnumFeature("GainSelector")->SetValue("AnalogAll");		//����ͨ��ѡ��
    m_objRemoteFeatureControlPtr->GetEnumFeature("GainAuto")->SetValue(m_GainAuto);			//�Զ�����
    if (m_GainAuto == "Off")
    {
        m_objRemoteFeatureControlPtr->GetFloatFeature("Gain")->SetValue(m_Gain);
    }
    else
    {
        m_objRemoteFeatureControlPtr->GetFloatFeature("AutoGainMin")->SetValue(m_AutoGainMin);
        m_objRemoteFeatureControlPtr->GetFloatFeature("AutoGainMax")->SetValue(m_AutoGainMax);
    }
    //��ƽ��ͨ��
    m_objRemoteFeatureControlPtr->GetEnumFeature("BalanceRatioSelector")->SetValue(m_BalanceRatioSelector);
    //�Զ���ƽ��
    m_objRemoteFeatureControlPtr->GetEnumFeature("BalanceWhiteAuto")->SetValue(m_BalanceWhiteAuto);
    if (m_BalanceWhiteAuto == "Off")
    {
        m_objRemoteFeatureControlPtr->GetFloatFeature("BalanceRatio")->SetValue(m_BalanceRatio);
    }
    //����ѡ��
    m_objRemoteFeatureControlPtr->GetEnumFeature("LineSelector")->SetValue("Line1");
    //���ŷ���
    m_objRemoteFeatureControlPtr->GetEnumFeature("LineMode")->SetValue("Output");
    //���ŵ�ƽ��ת
    m_objRemoteFeatureControlPtr->GetBoolFeature("LineInverter")->SetValue(true);
    //�������Դ
    m_objRemoteFeatureControlPtr->GetEnumFeature("LineSource")->SetValue("Strobe");

}

void qt_Mer::on_openDevice_clicked()
{
    if (!isOpenCam)
    {
        cout << "���豸" << endl;
        updateInput();
        ui->label_Img->setText("");
        ui->startCapture->setEnabled(true);
        ui->stopCapture->setEnabled(true);
        try
        {
            if (!isInstance)
            {
                cout << "��ʼ��" << endl;
                //��ʼ��
                IGXFactory::GetInstance().Init();
                cout << "isinstance" << endl;
                //ö���豸
                gxdeviceinfo_vector vectorDeviceInfo;
                IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
                if (0 == vectorDeviceInfo.size())
                {
                    ui->label_Img->setText("�޿����豸��");
                    cout << "�޿����豸!" << endl;
                    ui->startCapture->setEnabled(false);
                    ui->stopCapture->setEnabled(false);
                    return;
                }
                isInstance = true;
            }
            //�������ָ��
            m_objDevicePtr = IGXFactory::GetInstance().OpenDeviceBySN(m_SN, GX_ACCESS_EXCLUSIVE);
            m_objRemoteFeatureControlPtr = m_objDevicePtr->GetRemoteFeatureControl();
            m_objFeatureControlPtr = m_objDevicePtr->GetFeatureControl();
            uint32_t nStreamNum = m_objDevicePtr->GetStreamCount();
            if ((nStreamNum > 0))//���ڷ���!
            {
                m_objStreamPtr = m_objDevicePtr->OpenStream(0);
            }
            cout << "���ָ��ע�����" << endl;

            ui->groupBox_2->setEnabled(true);
            ui->groupBox_3->setEnabled(true);
            updateUI();
            isOpenCam = true;

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
}

void qt_Mer::on_startCapture_clicked()
{
    if (isOpenCam)
    {
        if (!isOpenStream)
        {
            //�������룬�����������
            updateInput();
            openAndSet(m_SN);
            if (!isOpenStream)
            {
                if (isShow)
                {
                    if (!timer->isActive())
                    {
                        timer->start(200); // �����ʱ��δ���У�������ʱ��
                    }
                }
                cout << "��ʼ�ɼ�" << endl;
                gxstring SN = getCurrentSN();
                folderCreated(static_cast<string>(getCurrentSN()));

                m_objStreamPtr->RegisterCaptureCallback(this, NULL);
                isOpenStream = true;
                ui->startCapture->setEnabled(false);

                m_objStreamPtr->StartGrab();
                m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStart")->Execute();
            }
        }
    }
    ui->groupBox_2->setEnabled(false);
    ui->groupBox_3->setEnabled(false);
}
void qt_Mer::on_closeDevice_clicked()
{
    if (isOpenCam)
    {
        if (isOpenStream)
        {
            //����ͣ������
            cout << "ֹͣ�ɼ�" << endl;
            m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
            m_objStreamPtr->StopGrab();
            m_objStreamPtr->UnregisterCaptureCallback();//ע���ɼ��ص�
            cout << "Unregister" << endl;
            m_objStreamPtr->Close(); //�ر������
            cout << "CloseStream" << endl;
            isOpenStream = false;
            ui->startCapture->setEnabled(true);
        }
        m_objDevicePtr->Close(); //�ر�������ͷ������Դ
        cout << "CloseDevice" << endl;
        
        //cout << "�ͷ���Դ" << endl;
        //IGXFactory::GetInstance().Uninit(); //�ͷ�GxIAPICPPEx�����������Դ

        ui->groupBox_2->setEnabled(true);
        ui->groupBox_3->setEnabled(true);

        isOpenCam = false;
        
    }
    isInstance = false;
}
void qt_Mer::on_stopCapture_clicked()
{
    if (isOpenStream)
    {
        cout << "ֹͣ�ɼ�" << endl;
        //����ͣ������
        m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
        m_objStreamPtr->StopGrab();
        isOpenStream = false;
        ui->startCapture->setEnabled(true);
    }
    ui->groupBox_2->setEnabled(true);
    ui->groupBox_3->setEnabled(true);
}


void qt_Mer::updateUI()
{
    ui->width->setText(QString::number(m_width));
    ui->height->setText(QString::number(m_height));
    ui->factoryName->setText(m_firmName);
    ui->basicModel->setText(m_modelName);
}
void qt_Mer::DoOnDeviceOfflineEvent(void* pUserParam)
{
    isOffline = true;
    IGXFactory::GetInstance().Uninit(); //�ͷ�GxIAPICPPEx�����������Դ
    cout << "�豸���ߣ����鲢�������ӡ�" << endl;
}

void qt_Mer::updateFrameID(const QString& newFrameID)
{
    ui->frameID->setText(newFrameID);
}

void qt_Mer::DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam)
{
    std::cout << "DoOnImageCaptured�̵߳�IDΪ: " << std::this_thread::get_id() << std::endl;
    firstCollect = true;
    //GX_PIXEL_FORMAT_ENTRY emPixelFormat =objImageDataPointer->GetPixelFormat();
    QString m_FrameID = QString::number(objImageDataPointer->GetFrameID());
    emit frameIDUpdated(m_FrameID);

    string filePath = outputPicFolder + m_FrameID.toStdString() + ".tiff";

    void* pBuffer = objImageDataPointer->GetBuffer();

    // �������󲢽���ָ����������
    cv::Mat matImg = cv::Mat::zeros(cv::Size(m_width, m_height), CV_8UC1);
    memcpy(matImg.data, pBuffer, m_width * m_height);
    ImageDataWithFrameID* imageDataWithFrameIDPtr = new ImageDataWithFrameID();
    imageDataWithFrameIDPtr->imageData = matImg;
    imageDataWithFrameIDPtr->filePath = filePath;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        g_imageQueue.push(*imageDataWithFrameIDPtr);
    }//����飬����ʱ���٣����������ļ����ͽ���
    delete imageDataWithFrameIDPtr;
    // �����������߳�
    m_conditionVariable.notify_all();

    //ImageDataWithFrameID* getPtr = new ImageDataWithFrameID();
    //getPtr = g_imageQueue.front();
    //g_imageQueue.pop();
    //string filePath = outputPicFolder + getPtr->frameID.toStdString() + ".tiff";

    //string filePath = outputPicFolder + to_string(objImageDataPointer->GetFrameID()) + ".tiff";
    //cv::Mat bayerImage(m_height, m_width, CV_8UC1, pBuffer); // ����pBayerData��ָ��Bayer��ʽ���ݵ�ָ��

    //cv::imwrite(filePath, getPtr->imageData);
    //delete getPtr;


    //ofstream ofs(Path, ios::app);
    //ofs << m_FrameID.toStdString() << endl;
    //ofs.close();

    //pictureSaveBayer(objImageDataPointer, pBuffer);
    
    //����ԭʼ������BayerRG8ͼ��Bayer תRGB24
    //void* pRGB24Buffer = NULL;
    //pRGB24Buffer = objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, true);
    //capturedImg = cv::Mat::zeros(cv::Size(m_width, m_height), CV_8UC3);
    //memcpy(capturedImg.data, pRGB24Buffer, m_width * m_height * 3);
    //cv::flip(capturedImg, capturedImg, 0);	//ͼ��ת
    //pictureSave(objImageDataPointer, capturedImg, m_FrameID);
        
    //cv::imshow(static_cast<string>(m_RunningSN), capturedImg);
    //cv::waitKey(1);
    // 
    //cout << "mainQt->showMatInLabel" << endl;
    //showMatInLabel(capturedImg, m_FrameID, m_SN);
    // 
    //emit transMat(capturedImg, m_FrameID);
    // 
    //updateUI();
    
}

void qt_Mer::saveThread() {
        std::cout << "saveThread�̵߳�IDΪ: " << std::this_thread::get_id() << std::endl;
    while (isSave)
    {
        ImageDataWithFrameID* getPtr = new ImageDataWithFrameID();
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            std::cout << "wait lock " << std::endl;
            // �ȴ�ͼ����в�Ϊ��
            m_conditionVariable.wait(lock, [this]() { return (!g_imageQueue.empty() || !isSave); });
            if (!isSave)
            {
                cout<< "nosave" << endl;
                return;
            }
            // ȡ������ͷ����ͼ��
            *getPtr = g_imageQueue.front();
            g_imageQueue.pop();
        }
        ui->frameRemain->setText(QString::number(g_imageQueue.size()));
        cout << "pop"<< g_imageQueue.size() << "save" << endl;
        // ����ͼ��
        cv::imwrite(getPtr->filePath, getPtr->imageData);

        // չʾͼ��
        //cv::Mat bayer2rgb = cv::Mat::zeros(cv::Size(m_width, m_height), CV_8UC3);
        //cvtColor(getPtr->imageData, bayer2rgb, CV_BayerRG2BGR);
        //cv::flip(bayer2rgb, bayer2rgb, 0);	//ͼ��ת
        {
            std::lock_guard<std::mutex> lock(matToShowMutex);
            matToShow = getPtr->imageData;
        }
        // �ͷ�ImageData������ڴ�
        delete getPtr;
        //if (g_imageQueue.empty() && !isOpenStream)
        //{
        //    isSave = false;
        //}
    }
}

void qt_Mer::pictureSave(CImageDataPointer& objImageDataPointer, Mat capturedImg, QString m_FrameID)	// ����ͼƬ
{
    string filePath = outputPicFolder + to_string(objImageDataPointer->GetFrameID()) + ".jpg";
    cv::imwrite(filePath, capturedImg);
    cout << this->m_SN << "ͼƬ����ɹ�" << m_FrameID.toStdString() << std::endl;
}

void qt_Mer::pictureSaveBayer(CImageDataPointer& objImageDataPointer, void* pRGB24Buffer)	// ����ͼƬ
{
    string filePath = outputPicFolder + to_string(objImageDataPointer->GetFrameID()) + ".tiff";
    cv::Mat bayerImage(m_height, m_width, CV_8UC1, pRGB24Buffer); // ����pBayerData��ָ��Bayer��ʽ���ݵ�ָ��
    cv::imwrite(filePath, bayerImage);
}

void qt_Mer::showMatInLabel()
{
    if (firstCollect)
    {
        if (isShow)
        {
            std::cout << "showMatInLabel���̵߳�IDΪ: " << std::this_thread::get_id() << std::endl;
            cv::Mat img = cv::Mat::zeros(cv::Size(m_width, m_height), CV_8UC1);
            cout << "show" << endl;
            {
                std::lock_guard<std::mutex> lock(matToShowMutex);
                img = matToShow;
            }
            QImage imgForDisplay;
            if (img.channels() == 3)//RGB Img
            {
                cv::cvtColor(img, img, cv::COLOR_BGR2RGB);//��ɫ�ռ�ת��
                imgForDisplay = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
            }
            else//Gray Img
            {
                imgForDisplay = QImage((const unsigned char*)(img.data), img.cols, img.rows, img.cols * img.channels(), QImage::Format_Indexed8);
            }
            QPixmap Qimg = QPixmap::fromImage(imgForDisplay);
            ui->label_Img->setPixmap(Qimg);

            if (!isOpenStream && g_imageQueue.empty())  //���ر���ͼ����п�
            {
                if (timer->isActive()) {
                    timer->stop(); // �����ʱ���������У�ֹͣ��ʱ��
                }
            }
        }
    }
}

void qt_Mer::transShow()
{
    if (isShow == true)
    {
        isShow = false;
        ui->label_Img->clear();
    }
    else
    {
        isShow = true;
    }
    if (timer->isActive()) {
        timer->stop(); // �����ʱ���������У�ֹͣ��ʱ��
    }
    else {
        timer->start(200); // �����ʱ��δ���У�������ʱ��
    }
}

void qt_Mer::folderCreated(string SN)	// �����ļ���
{
    outputPicFolder = "./picture/";
    fs::path pathToCreate = fs::path(outputPicFolder);
    if (fs::create_directory(pathToCreate));
    struct tm newtime;
    time_t time_seconds = time(0);
    localtime_s(&newtime, &time_seconds);

    SN += "/";
    outputPicFolder+= SN;
    pathToCreate = fs::path(outputPicFolder);
    fs::create_directory(pathToCreate);

    std::stringstream ss;
    ss << "y" << newtime.tm_year + 1900 << " m" << newtime.tm_mon + 1 << " d" << newtime.tm_mday
        << " h" << newtime.tm_hour << " min" << newtime.tm_min << " s" << newtime.tm_sec << "/";

    outputPicFolder += ss.str();

    pathToCreate = fs::path(outputPicFolder);
    if (fs::create_directory(pathToCreate))
    {
        cout << outputPicFolder << "ͼƬ���·�������ɹ�" << endl;
    }
    else {
        cout << "ͼƬ���·������ʧ��" << endl;
    }

}



void qt_Mer::writeSettings()
{
    updateInput();
    string outputTxtPath = "./settings.txt";
    ofstream ofs(outputTxtPath, ios::out);

    ofs << m_fps << endl << m_ExposureTime
        << endl << m_GainAuto << endl << m_Gain << endl << m_AutoGainMin
        << endl << m_AutoGainMax << endl << m_BalanceRatioSelector
        << endl << m_BalanceWhiteAuto << endl << m_BalanceRatio << endl;

    cout << "д��ɹ�" << endl;

    ofs.close();
}

void qt_Mer::loadSettings()
{
    if (fs::exists("./settings.txt")) 
    {
        ifstream ifs;
        ifs.open("./settings.txt", ios::in); // ���ļ�

        ifs >> m_fps;
        ifs >> m_ExposureTime;

        ifs >> m_GainAuto;
        ifs >> m_Gain;
        ifs >> m_AutoGainMin;
        ifs >> m_AutoGainMax;

        ifs >> m_BalanceRatioSelector;
        ifs >> m_BalanceWhiteAuto;
        ifs >> m_BalanceRatio;

        cout << "��ȡ�ɹ�" << endl;
        settingsToUI();
    }
    else 
    {
        std::cout << "File does not exist" << std::endl;
    }

}

void qt_Mer::settingsToUI()
{
    //��������
    ui->fps->setText(QString::number(m_fps));
    ui->exposeTime->setText(QString::number(m_ExposureTime));  //�趨�ع�ʱ��
    //ģ�����
    ui->gainAuto->setCurrentText(static_cast<QString>(m_GainAuto));	//�Զ�����ѡ�� Once Continuous Off
    ui->gainValue->setText(QString::number(m_Gain));	//	offʱ����ֵ
    ui->gainValueMin->setText(QString::number(m_AutoGainMin));
    ui->gainValueMax->setText(QString::number(m_AutoGainMax));

    ui->balanceRatioSelector->setCurrentText(static_cast<QString>(m_BalanceRatioSelector));		//��ƽ��ͨ��ѡ�� Red Green Blue
    ui->balanceRatio->setText(QString::number(m_BalanceRatio));	//offʱ��ƽ��ϵ��
    ui->balanceWhiteAuto->setCurrentText(static_cast<QString>(m_BalanceWhiteAuto));	//�Զ���ƽ��ѡ�� Once Continuous Off
    cout << "�����������" << endl;
};


