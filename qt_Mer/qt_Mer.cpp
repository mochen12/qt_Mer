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
    std::cout << "qt_Mer线程的ID为: " << std::this_thread::get_id() << std::endl;


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

    //展示定时器
    timer = new QTimer(this);
    timer->start(200);

    //基本参数
    m_fps = 20;
    m_ExposureTime = 10000;  //设定曝光时间
    //模拟控制
    m_GainAuto = "Continuous";	//自动增益选择 Once Continuous Off
    m_Gain = 16;	//	off时增益值
    m_AutoGainMin = 0;
    m_AutoGainMax = 24;

    m_BalanceRatioSelector = "Red";		//白平衡通道选择 Red Green Blue
    m_BalanceRatio = 1;	//off时白平衡系数
    m_BalanceWhiteAuto = "Continuous";	//自动白平衡选择 Once Continuous Off

    // 创建一个QIntValidator对象，并设置范围为0-24
    QIntValidator* validator_Gain = new QIntValidator(0, 24, this);
    QIntValidator* validator_Fps = new QIntValidator(1, 210, this);
    QIntValidator* validator_BalanceRatio = new QIntValidator(1, 4000, this);
    // 将QIntValidator对象设置为QLineEdit的验证器
    ui->gainValue->setValidator(validator_Gain);
    ui->gainValueMin->setValidator(validator_Gain);
    ui->gainValueMax->setValidator(validator_Gain);
    ui->fps->setValidator(validator_Fps);
    ui->balanceRatio->setValidator(validator_BalanceRatio);

    connect(ui->openDevice, &QPushButton::clicked, this, &qt_Mer::on_openDevice_clicked);
    connect(ui->closeDevice, &QPushButton::clicked, this, &qt_Mer::on_closeDevice_clicked);
    connect(ui->startCapture, &QPushButton::clicked, this, &qt_Mer::on_startCapture_clicked);
    connect(ui->stopCapture, &QPushButton::clicked, this, &qt_Mer::on_stopCapture_clicked);

    connect(ui->gainAuto, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &qt_Mer::updateGainState);  //增益框
    connect(ui->balanceWhiteAuto, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &qt_Mer::updateBalanceState);   //白平衡框
    //connect(ui->openDevice, &QPushButton::clicked, this, &qt_Mer::updateButtonOn);    //采集按钮亮
    connect(ui->closeDevice, &QPushButton::clicked, this, &qt_Mer::updateButtonOff);    //采集按钮暗
    connect(ui->gainValueMin, &QLineEdit::textChanged, this, &qt_Mer::validateGainminInput);    //增益上下限对比
    connect(ui->gainValueMax, &QLineEdit::textChanged, this, &qt_Mer::validateGainmaxInput);    //增益上下限对比
    connect(ui->gainValue, &QLineEdit::textChanged, this, &qt_Mer::validateGainInput);    //增益值检验
    connect(ui->balanceRatio, &QLineEdit::textChanged, this, &qt_Mer::validateBalanceRatioInput);    //白平衡系数检验
    connect(ui->fps, &QLineEdit::textChanged, this, &qt_Mer::validateFpsInput);    //fps检验
    connect(ui->exposeTime, &QLineEdit::textChanged, this, &qt_Mer::validateExposeTimeInput);    //曝光时间检验
    connect(ui->saveSettings, &QPushButton::clicked, this, &qt_Mer::writeSettings);    //保存配置
    connect(ui->loadSettings, &QPushButton::clicked, this, &qt_Mer::loadSettings);    //读取配置
    connect(ui->showButton, &QPushButton::clicked, this, &qt_Mer::transShow);    //是否展示
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
        cout << "初始化" << endl;
        IGXFactory::GetInstance().Init();//GxIAPICPPEx库在使用之前必须执行初始化
    }
    catch (CGalaxyException& e)
    {
        cout << "错误码: " << e.GetErrorCode() << endl;
        cout << "错误描述信息: " << e.what() << endl;
    }
    
    //枚举设备
    gxdeviceinfo_vector vectorDeviceInfo;
    IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);

    if (0 == vectorDeviceInfo.size())
    {
        ui->label_Img->setText("无可用设备！");
        cout << "无可用设备!" << endl;
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
    // 等待保存线程结束
    if (m_saveThread.joinable())
    {
        std::cout << "Waiting for save thread to finish..." << std::endl;
        m_saveThread.join();
    }

    // 清空队列
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
        // 如果Min的值大于等于Max的值，将Min的值设置为Max的值减1
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
        // 如果值大于24，则设为24
        ui->gainValue->setText(QString::number(24));
    }
    if (value1 < 0)
    {
        // 如果值小于0，则设为0
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
    //基本参数
    m_fps = ui->fps->text().toInt();
    m_ExposureTime = ui->exposeTime->text().toInt();  //设定曝光时间
    //模拟控制
    m_GainAuto = ui->gainAuto->currentText().toStdString().c_str();	//自动增益选择 Once Continuous Off
    m_Gain = ui->gainValue->text().toInt();	//	off时增益值
    m_AutoGainMin = ui->gainValueMin->text().toInt();
    m_AutoGainMax = ui->gainValueMax->text().toInt();

    m_BalanceRatioSelector = ui->balanceRatioSelector->currentText().toStdString().c_str();		//白平衡通道选择 Red Green Blue
    m_BalanceRatio = ui->balanceRatio->text().toInt();	//off时白平衡系数
    m_BalanceWhiteAuto = ui->balanceWhiteAuto->currentText().toStdString().c_str();	//自动白平衡选择 Once Continuous Off
    cout << "读入面板数据" << endl;
};

gxstring qt_Mer::getCurrentSN()
{
    return m_SN;
}

void qt_Mer::openAndSet(gxstring m_SN)
{
    cout << "打开设备，设置参数" << endl;
    //打开当前设备
    /*设备信息获取*/
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

    /*设备初始设置*/
    //采集帧率调节
    m_objRemoteFeatureControlPtr->GetEnumFeature("AcquisitionFrameRateMode")->SetValue("On");
    m_objRemoteFeatureControlPtr->GetFloatFeature("AcquisitionFrameRate")->SetValue(m_fps);
    //曝光时间(us)
    m_objRemoteFeatureControlPtr->GetEnumFeature("ExposureMode")->SetValue("Timed");
    m_objRemoteFeatureControlPtr->GetFloatFeature("ExposureTime")->SetValue(m_ExposureTime);
    if (m_fps > 90 && (m_fps * m_ExposureTime < 1000000))
    {
        //解除带宽限制
        m_objRemoteFeatureControlPtr->GetEnumFeature("DeviceLinkThroughputLimitMode")->SetValue("Off");
    }
    //增益
    m_objRemoteFeatureControlPtr->GetEnumFeature("GainSelector")->SetValue("AnalogAll");		//增益通道选择
    m_objRemoteFeatureControlPtr->GetEnumFeature("GainAuto")->SetValue(m_GainAuto);			//自动增益
    if (m_GainAuto == "Off")
    {
        m_objRemoteFeatureControlPtr->GetFloatFeature("Gain")->SetValue(m_Gain);
    }
    else
    {
        m_objRemoteFeatureControlPtr->GetFloatFeature("AutoGainMin")->SetValue(m_AutoGainMin);
        m_objRemoteFeatureControlPtr->GetFloatFeature("AutoGainMax")->SetValue(m_AutoGainMax);
    }
    //白平衡通道
    m_objRemoteFeatureControlPtr->GetEnumFeature("BalanceRatioSelector")->SetValue(m_BalanceRatioSelector);
    //自动白平衡
    m_objRemoteFeatureControlPtr->GetEnumFeature("BalanceWhiteAuto")->SetValue(m_BalanceWhiteAuto);
    if (m_BalanceWhiteAuto == "Off")
    {
        m_objRemoteFeatureControlPtr->GetFloatFeature("BalanceRatio")->SetValue(m_BalanceRatio);
    }
    //引脚选择
    m_objRemoteFeatureControlPtr->GetEnumFeature("LineSelector")->SetValue("Line1");
    //引脚方向
    m_objRemoteFeatureControlPtr->GetEnumFeature("LineMode")->SetValue("Output");
    //引脚电平反转
    m_objRemoteFeatureControlPtr->GetBoolFeature("LineInverter")->SetValue(true);
    //引脚输出源
    m_objRemoteFeatureControlPtr->GetEnumFeature("LineSource")->SetValue("Strobe");

}

void qt_Mer::on_openDevice_clicked()
{
    if (!isOpenCam)
    {
        cout << "打开设备" << endl;
        updateInput();
        ui->label_Img->setText("");
        ui->startCapture->setEnabled(true);
        ui->stopCapture->setEnabled(true);
        try
        {
            if (!isInstance)
            {
                cout << "初始化" << endl;
                //初始化
                IGXFactory::GetInstance().Init();
                cout << "isinstance" << endl;
                //枚举设备
                gxdeviceinfo_vector vectorDeviceInfo;
                IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
                if (0 == vectorDeviceInfo.size())
                {
                    ui->label_Img->setText("无可用设备！");
                    cout << "无可用设备!" << endl;
                    ui->startCapture->setEnabled(false);
                    ui->stopCapture->setEnabled(false);
                    return;
                }
                isInstance = true;
            }
            //相机控制指针
            m_objDevicePtr = IGXFactory::GetInstance().OpenDeviceBySN(m_SN, GX_ACCESS_EXCLUSIVE);
            m_objRemoteFeatureControlPtr = m_objDevicePtr->GetRemoteFeatureControl();
            m_objFeatureControlPtr = m_objDevicePtr->GetFeatureControl();
            uint32_t nStreamNum = m_objDevicePtr->GetStreamCount();
            if ((nStreamNum > 0))//存在风险!
            {
                m_objStreamPtr = m_objDevicePtr->OpenStream(0);
            }
            cout << "相机指针注册完成" << endl;

            ui->groupBox_2->setEnabled(true);
            ui->groupBox_3->setEnabled(true);
            updateUI();
            isOpenCam = true;

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
}

void qt_Mer::on_startCapture_clicked()
{
    if (isOpenCam)
    {
        if (!isOpenStream)
        {
            //更新输入，打开相机并设置
            updateInput();
            openAndSet(m_SN);
            if (!isOpenStream)
            {
                if (isShow)
                {
                    if (!timer->isActive())
                    {
                        timer->start(200); // 如果定时器未运行，启动定时器
                    }
                }
                cout << "开始采集" << endl;
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
            //发送停采命令
            cout << "停止采集" << endl;
            m_objRemoteFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
            m_objStreamPtr->StopGrab();
            m_objStreamPtr->UnregisterCaptureCallback();//注销采集回调
            cout << "Unregister" << endl;
            m_objStreamPtr->Close(); //关闭相机流
            cout << "CloseStream" << endl;
            isOpenStream = false;
            ui->startCapture->setEnabled(true);
        }
        m_objDevicePtr->Close(); //关闭相机，释放相机资源
        cout << "CloseDevice" << endl;
        
        //cout << "释放资源" << endl;
        //IGXFactory::GetInstance().Uninit(); //释放GxIAPICPPEx申请的所有资源

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
        cout << "停止采集" << endl;
        //发送停采命令
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
    IGXFactory::GetInstance().Uninit(); //释放GxIAPICPPEx申请的所有资源
    cout << "设备掉线，请检查并重新连接。" << endl;
}

void qt_Mer::updateFrameID(const QString& newFrameID)
{
    ui->frameID->setText(newFrameID);
}

void qt_Mer::DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam)
{
    std::cout << "DoOnImageCaptured线程的ID为: " << std::this_thread::get_id() << std::endl;
    firstCollect = true;
    //GX_PIXEL_FORMAT_ENTRY emPixelFormat =objImageDataPointer->GetPixelFormat();
    QString m_FrameID = QString::number(objImageDataPointer->GetFrameID());
    emit frameIDUpdated(m_FrameID);

    string filePath = outputPicFolder + m_FrameID.toStdString() + ".tiff";

    void* pBuffer = objImageDataPointer->GetBuffer();

    // 创建对象并将其指针存入队列中
    cv::Mat matImg = cv::Mat::zeros(cv::Size(m_width, m_height), CV_8UC1);
    memcpy(matImg.data, pBuffer, m_width * m_height);
    ImageDataWithFrameID* imageDataWithFrameIDPtr = new ImageDataWithFrameID();
    imageDataWithFrameIDPtr->imageData = matImg;
    imageDataWithFrameIDPtr->filePath = filePath;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        g_imageQueue.push(*imageDataWithFrameIDPtr);
    }//代码块，结束时销毁，管理互斥锁的加锁和解锁
    delete imageDataWithFrameIDPtr;
    // 唤醒消费者线程
    m_conditionVariable.notify_all();

    //ImageDataWithFrameID* getPtr = new ImageDataWithFrameID();
    //getPtr = g_imageQueue.front();
    //g_imageQueue.pop();
    //string filePath = outputPicFolder + getPtr->frameID.toStdString() + ".tiff";

    //string filePath = outputPicFolder + to_string(objImageDataPointer->GetFrameID()) + ".tiff";
    //cv::Mat bayerImage(m_height, m_width, CV_8UC1, pBuffer); // 假设pBayerData是指向Bayer格式数据的指针

    //cv::imwrite(filePath, getPtr->imageData);
    //delete getPtr;


    //ofstream ofs(Path, ios::app);
    //ofs << m_FrameID.toStdString() << endl;
    //ofs.close();

    //pictureSaveBayer(objImageDataPointer, pBuffer);
    
    //假设原始数据是BayerRG8图像，Bayer 转RGB24
    //void* pRGB24Buffer = NULL;
    //pRGB24Buffer = objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, true);
    //capturedImg = cv::Mat::zeros(cv::Size(m_width, m_height), CV_8UC3);
    //memcpy(capturedImg.data, pRGB24Buffer, m_width * m_height * 3);
    //cv::flip(capturedImg, capturedImg, 0);	//图像翻转
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
        std::cout << "saveThread线程的ID为: " << std::this_thread::get_id() << std::endl;
    while (isSave)
    {
        ImageDataWithFrameID* getPtr = new ImageDataWithFrameID();
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            std::cout << "wait lock " << std::endl;
            // 等待图像队列不为空
            m_conditionVariable.wait(lock, [this]() { return (!g_imageQueue.empty() || !isSave); });
            if (!isSave)
            {
                cout<< "nosave" << endl;
                return;
            }
            // 取出队列头部的图像
            *getPtr = g_imageQueue.front();
            g_imageQueue.pop();
        }
        ui->frameRemain->setText(QString::number(g_imageQueue.size()));
        cout << "pop"<< g_imageQueue.size() << "save" << endl;
        // 保存图像
        cv::imwrite(getPtr->filePath, getPtr->imageData);

        // 展示图像
        //cv::Mat bayer2rgb = cv::Mat::zeros(cv::Size(m_width, m_height), CV_8UC3);
        //cvtColor(getPtr->imageData, bayer2rgb, CV_BayerRG2BGR);
        //cv::flip(bayer2rgb, bayer2rgb, 0);	//图像翻转
        {
            std::lock_guard<std::mutex> lock(matToShowMutex);
            matToShow = getPtr->imageData;
        }
        // 释放ImageData对象的内存
        delete getPtr;
        //if (g_imageQueue.empty() && !isOpenStream)
        //{
        //    isSave = false;
        //}
    }
}

void qt_Mer::pictureSave(CImageDataPointer& objImageDataPointer, Mat capturedImg, QString m_FrameID)	// 保存图片
{
    string filePath = outputPicFolder + to_string(objImageDataPointer->GetFrameID()) + ".jpg";
    cv::imwrite(filePath, capturedImg);
    cout << this->m_SN << "图片保存成功" << m_FrameID.toStdString() << std::endl;
}

void qt_Mer::pictureSaveBayer(CImageDataPointer& objImageDataPointer, void* pRGB24Buffer)	// 保存图片
{
    string filePath = outputPicFolder + to_string(objImageDataPointer->GetFrameID()) + ".tiff";
    cv::Mat bayerImage(m_height, m_width, CV_8UC1, pRGB24Buffer); // 假设pBayerData是指向Bayer格式数据的指针
    cv::imwrite(filePath, bayerImage);
}

void qt_Mer::showMatInLabel()
{
    if (firstCollect)
    {
        if (isShow)
        {
            std::cout << "showMatInLabel内线程的ID为: " << std::this_thread::get_id() << std::endl;
            cv::Mat img = cv::Mat::zeros(cv::Size(m_width, m_height), CV_8UC1);
            cout << "show" << endl;
            {
                std::lock_guard<std::mutex> lock(matToShowMutex);
                img = matToShow;
            }
            QImage imgForDisplay;
            if (img.channels() == 3)//RGB Img
            {
                cv::cvtColor(img, img, cv::COLOR_BGR2RGB);//颜色空间转换
                imgForDisplay = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
            }
            else//Gray Img
            {
                imgForDisplay = QImage((const unsigned char*)(img.data), img.cols, img.rows, img.cols * img.channels(), QImage::Format_Indexed8);
            }
            QPixmap Qimg = QPixmap::fromImage(imgForDisplay);
            ui->label_Img->setPixmap(Qimg);

            if (!isOpenStream && g_imageQueue.empty())  //流关闭且图像队列空
            {
                if (timer->isActive()) {
                    timer->stop(); // 如果定时器正在运行，停止定时器
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
        timer->stop(); // 如果定时器正在运行，停止定时器
    }
    else {
        timer->start(200); // 如果定时器未运行，启动定时器
    }
}

void qt_Mer::folderCreated(string SN)	// 创建文件夹
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
        cout << outputPicFolder << "图片输出路径创建成功" << endl;
    }
    else {
        cout << "图片输出路径创建失败" << endl;
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

    cout << "写入成功" << endl;

    ofs.close();
}

void qt_Mer::loadSettings()
{
    if (fs::exists("./settings.txt")) 
    {
        ifstream ifs;
        ifs.open("./settings.txt", ios::in); // 读文件

        ifs >> m_fps;
        ifs >> m_ExposureTime;

        ifs >> m_GainAuto;
        ifs >> m_Gain;
        ifs >> m_AutoGainMin;
        ifs >> m_AutoGainMax;

        ifs >> m_BalanceRatioSelector;
        ifs >> m_BalanceWhiteAuto;
        ifs >> m_BalanceRatio;

        cout << "读取成功" << endl;
        settingsToUI();
    }
    else 
    {
        std::cout << "File does not exist" << std::endl;
    }

}

void qt_Mer::settingsToUI()
{
    //基本参数
    ui->fps->setText(QString::number(m_fps));
    ui->exposeTime->setText(QString::number(m_ExposureTime));  //设定曝光时间
    //模拟控制
    ui->gainAuto->setCurrentText(static_cast<QString>(m_GainAuto));	//自动增益选择 Once Continuous Off
    ui->gainValue->setText(QString::number(m_Gain));	//	off时增益值
    ui->gainValueMin->setText(QString::number(m_AutoGainMin));
    ui->gainValueMax->setText(QString::number(m_AutoGainMax));

    ui->balanceRatioSelector->setCurrentText(static_cast<QString>(m_BalanceRatioSelector));		//白平衡通道选择 Red Green Blue
    ui->balanceRatio->setText(QString::number(m_BalanceRatio));	//off时白平衡系数
    ui->balanceWhiteAuto->setCurrentText(static_cast<QString>(m_BalanceWhiteAuto));	//自动白平衡选择 Once Continuous Off
    cout << "读入面板数据" << endl;
};


