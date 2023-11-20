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

qt_Mer::qt_Mer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    isInstance = false;
    isOpenCam = false;
    //isOpenStream = false;
    //isOffline = false;
    isTrigger = false;
    exposeTime = 10000;
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
    ui.gainValue->setValidator(validator_Gain);
    ui.gainValueMin->setValidator(validator_Gain);
    ui.gainValueMax->setValidator(validator_Gain);
    ui.fps->setValidator(validator_Fps);
    ui.balanceRatio->setValidator(validator_BalanceRatio);

    connect(ui.gainAuto, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &qt_Mer::updateGainState);  //增益框
    connect(ui.balanceWhiteAuto, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &qt_Mer::updateBalanceState);   //白平衡框
    //connect(ui.openDevice, &QPushButton::clicked, this, &qt_Mer::updateButtonOn);    //采集按钮亮
    connect(ui.closeDevice, &QPushButton::clicked, this, &qt_Mer::updateButtonOff);    //采集按钮暗
    connect(ui.gainValueMin, &QLineEdit::textChanged, this, &qt_Mer::validateGainminInput);    //增益上下限对比
    connect(ui.gainValueMax, &QLineEdit::textChanged, this, &qt_Mer::validateGainmaxInput);    //增益上下限对比
    connect(ui.gainValue, &QLineEdit::textChanged, this, &qt_Mer::validateGainInput);    //增益值检验
    connect(ui.balanceRatio, &QLineEdit::textChanged, this, &qt_Mer::validateBalanceRatioInput);    //白平衡系数检验
    connect(ui.fps, &QLineEdit::textChanged, this, &qt_Mer::validateFpsInput);    //fps检验
    connect(ui.exposeTime, &QLineEdit::textChanged, this, &qt_Mer::validateExposeTimeInput);    //曝光时间检验
    connect(ui.saveSettings, &QPushButton::clicked, this, &qt_Mer::writeSettings);    //保存配置
    connect(ui.loadSettings, &QPushButton::clicked, this, &qt_Mer::loadSettings);    //读取配置

    if (ui.gainAuto->currentText() == "Continuous")
    {
        ui.gainValue->setEnabled(false);
        ui.gainValueMin->setEnabled(true);
        ui.gainValueMax->setEnabled(true);
    }
    else 
    {
        ui.gainValue->setEnabled(true);
        ui.gainValueMin->setEnabled(false);
        ui.gainValueMax->setEnabled(false);
    }
    if (ui.gainAuto->currentText() == "Continuous") {
        ui.balanceRatio->setEnabled(false);
    }
    else 
    {
        ui.balanceRatio->setEnabled(true);
    }

    ui.startCapture->setEnabled(false);
    ui.stopCapture->setEnabled(false);
    ui.groupBox_2->setEnabled(true);
    ui.groupBox_3->setEnabled(true);


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
    numSN = vectorDeviceInfo.size();
    if (0 == vectorDeviceInfo.size())
    {
        ui.label_Img->setText("无可用设备！");
        cout << "无可用设备!" << endl;
        ui.startCapture->setEnabled(false);
        ui.stopCapture->setEnabled(false);
        return;
    }
    ui.snList->clear();
    for (int i = 0; i < vectorDeviceInfo.size(); i++)
    {
        gxstring SN = vectorDeviceInfo[i].GetSN();
        ui.snList->addItem(static_cast<QString>(SN));
        snMap[SN] = i;
    }

    updateSNList(vectorDeviceInfo);

    isOpenStreamVector.resize(vectorDeviceInfo.size(), 0);
    isInstance = true;
}


qt_Mer::~qt_Mer()
{}

void qt_Mer::updateGainState(int index)
{
    if (ui.gainAuto->itemText(index) == "Continuous") {
        ui.gainValue->setEnabled(false);
        ui.gainValueMin->setEnabled(true);
        ui.gainValueMax->setEnabled(true);
    }
    else {
        ui.gainValue->setEnabled(true);
        ui.gainValueMin->setEnabled(false);
        ui.gainValueMax->setEnabled(false);
    }
}

void qt_Mer::updateBalanceState(int index)
{
    if (ui.gainAuto->itemText(index) == "Continuous") {
        ui.balanceRatio->setEnabled(false);
    }
    else {
        ui.balanceRatio->setEnabled(true);
    }
}

void qt_Mer::updateButtonOn() {
    ui.startCapture->setEnabled(true);
    ui.stopCapture->setEnabled(true);
}

void qt_Mer::updateButtonOff() {
    ui.startCapture->setEnabled(false);
    ui.stopCapture->setEnabled(false);
}

void qt_Mer::validateGainminInput()
{
    int value1 = ui.gainValueMin->text().toInt();
    int value2 = ui.gainValueMax->text().toInt();

    if (value1 >= value2)
    {
        // 如果Min的值大于等于Max的值，将Min的值设置为Max的值减1
        ui.gainValueMin->setText(QString::number(value2 - 1));
    }
    if (value1 < 0)
    {
        ui.gainValueMin->setText(QString::number(0));
    }
}

void qt_Mer::validateGainmaxInput()
{
    int value1 = ui.gainValueMin->text().toInt();
    int value2 = ui.gainValueMax->text().toInt();

    if (value1 >= value2)
    {
        ui.gainValueMax->setText(QString::number(value1 + 1));
    }
    if (value2 > 24)
    {
        ui.gainValueMax->setText(QString::number(24));
    }
}

void qt_Mer::validateGainInput()
{
    int value1 = ui.gainValue->text().toInt();
    if (value1 > 24)
    {
        // 如果值大于24，则设为24
        ui.gainValue->setText(QString::number(24));
    }
    if (value1 < 0)
    {
        // 如果值小于0，则设为0
        ui.gainValue->setText(QString::number(0));
    }
}

void qt_Mer::validateBalanceRatioInput() {
    int value1 = ui.balanceRatio->text().toInt();
    if (value1 > 4000)
    {
        ui.balanceRatio->setText(QString::number(4000));
    }
    if (value1 < 1)
    {
        ui.balanceRatio->setText(QString::number(1));
    }
}
void qt_Mer::validateFpsInput() {
    int value1 = ui.fps->text().toInt();
    if (value1 > 210)
    {
        ui.fps->setText(QString::number(210));
    }
    if (value1 < 1)
    {
        ui.fps->setText(QString::number(1));
    }
}

void qt_Mer::validateExposeTimeInput() {
    int value1 = ui.exposeTime->text().toInt();
    int fps = ui.fps->text().toInt();
    int max = floor(1000000/fps);
    if (value1 > max)
    {
        ui.exposeTime->setText(QString::number(max));
    }
    if (value1 < 100)
    {
        ui.exposeTime->setText(QString::number(100));
    }
}

void qt_Mer::updateInput()
{
    //基本参数
    m_fps = ui.fps->text().toInt();
    m_ExposureTime = ui.exposeTime->text().toInt();  //设定曝光时间
    //模拟控制
    m_GainAuto = ui.gainAuto->currentText().toStdString().c_str();	//自动增益选择 Once Continuous Off
    m_Gain = ui.gainValue->text().toInt();	//	off时增益值
    m_AutoGainMin = ui.gainValueMin->text().toInt();
    m_AutoGainMax = ui.gainValueMax->text().toInt();

    m_BalanceRatioSelector = ui.balanceRatioSelector->currentText().toStdString().c_str();		//白平衡通道选择 Red Green Blue
    m_BalanceRatio = ui.balanceRatio->text().toInt();	//off时白平衡系数
    m_BalanceWhiteAuto = ui.balanceWhiteAuto->currentText().toStdString().c_str();	//自动白平衡选择 Once Continuous Off
    cout << "读入面板数据" << endl;
};

gxstring qt_Mer::getCurrentSN()
{
    QByteArray get_currentSN = ui.snList->currentText().toLatin1();
    char* SN = get_currentSN.data();
    gxstring gxSN = SN;
    return gxSN;
}

void qt_Mer::openAndSet(int snIndex)
{
    cout << "打开设备，设置参数" << endl;
    //打开当前设备
    /*设备信息获取*/
    m_deviceInfo = m_objDevicePtrVector[snIndex]->GetDeviceInfo();
    m_firmName = m_deviceInfo.GetVendorName();
    m_modelName = m_deviceInfo.GetModelName();
    //m_serialNumName = m_deviceInfo.GetSN();
    //m_ipName = m_deviceInfo.GetIP();
    //m_maskName = m_deviceInfo.GetSubnetMask();
    m_macName = m_deviceInfo.GetMAC();
    m_width = m_objRemoteFeatureControlPtrVector[snIndex]->GetIntFeature("Width")->GetValue();
    m_height = m_objRemoteFeatureControlPtrVector[snIndex]->GetIntFeature("Height")->GetValue();
    m_bitDepth = m_objRemoteFeatureControlPtrVector[snIndex]->GetEnumFeature("PixelSize")->GetValue();

    /*设备初始设置*/
    //采集帧率调节
    m_objRemoteFeatureControlPtrVector[snIndex]->GetEnumFeature("AcquisitionFrameRateMode")->SetValue("On");
    m_objRemoteFeatureControlPtrVector[snIndex]->GetFloatFeature("AcquisitionFrameRate")->SetValue(m_fps);
    //曝光时间(us)
    m_objRemoteFeatureControlPtrVector[snIndex]->GetEnumFeature("ExposureMode")->SetValue("Timed");
    m_objRemoteFeatureControlPtrVector[snIndex]->GetFloatFeature("ExposureTime")->SetValue(m_ExposureTime);
    //增益
    m_objRemoteFeatureControlPtrVector[snIndex]->GetEnumFeature("GainSelector")->SetValue("AnalogAll");		//增益通道选择
    m_objRemoteFeatureControlPtrVector[snIndex]->GetEnumFeature("GainAuto")->SetValue(m_GainAuto);			//自动增益
    if (m_GainAuto == "Off")
    {
        m_objRemoteFeatureControlPtrVector[snIndex]->GetFloatFeature("Gain")->SetValue(m_Gain);
    }
    else
    {
        m_objRemoteFeatureControlPtrVector[snIndex]->GetFloatFeature("AutoGainMin")->SetValue(m_AutoGainMin);
        m_objRemoteFeatureControlPtrVector[snIndex]->GetFloatFeature("AutoGainMax")->SetValue(m_AutoGainMax);
    }
    //白平衡通道
    m_objRemoteFeatureControlPtrVector[snIndex]->GetEnumFeature("BalanceRatioSelector")->SetValue(m_BalanceRatioSelector);
    //自动白平衡
    m_objRemoteFeatureControlPtrVector[snIndex]->GetEnumFeature("BalanceWhiteAuto")->SetValue(m_BalanceWhiteAuto);
    if (m_BalanceWhiteAuto == "Off")
    {
        m_objRemoteFeatureControlPtrVector[snIndex]->GetFloatFeature("BalanceRatio")->SetValue(m_BalanceRatio);
    }
    //引脚选择
    m_objRemoteFeatureControlPtrVector[snIndex]->GetEnumFeature("LineSelector")->SetValue("Line1");
    //引脚方向
    m_objRemoteFeatureControlPtrVector[snIndex]->GetEnumFeature("LineMode")->SetValue("Output");
    //引脚电平反转
    m_objRemoteFeatureControlPtrVector[snIndex]->GetBoolFeature("LineInverter")->SetValue(true);
    //引脚输出源
    m_objRemoteFeatureControlPtrVector[snIndex]->GetEnumFeature("LineSource")->SetValue("Strobe");

}

void qt_Mer::on_openDevice_clicked()
{
    cout << "打开设备" << endl;
    updateInput();
    ui.label_Img->setText("");
    ui.startCapture->setEnabled(true);
    ui.stopCapture->setEnabled(true);
    try
    {
        if (!isInstance)
        {
            cout << "初始化" << endl;
            //初始化
            IGXFactory::GetInstance().Init();
            //枚举设备
            gxdeviceinfo_vector vectorDeviceInfo;
            IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
            if (0 == vectorDeviceInfo.size())
            {
                ui.label_Img->setText("无可用设备！");
                cout << "无可用设备!" << endl;
                ui.startCapture->setEnabled(false);
                ui.stopCapture->setEnabled(false);
                return;
            }
            numSN = vectorDeviceInfo.size();
            updateSNList(vectorDeviceInfo);
            isInstance = true;
        }
        //相机控制指针容器初始化

        vector <CGXDevicePointer>().swap(m_objDevicePtrVector);
        vector<CGXFeatureControlPointer>().swap(m_objRemoteFeatureControlPtrVector);
        vector<CGXFeatureControlPointer>().swap(m_objFeatureControlPtrVector);
        vector<CGXStreamPointer>().swap(m_objStreamPtrVector);
        for (const auto& pair : snMap) 
        {
            std::cout << pair.first << " : " << pair.second << std::endl;
            m_objDevicePtrVector.push_back(IGXFactory::GetInstance().OpenDeviceBySN(pair.first, GX_ACCESS_EXCLUSIVE));
            m_objRemoteFeatureControlPtrVector.push_back(m_objDevicePtrVector[pair.second]->GetRemoteFeatureControl());
            m_objFeatureControlPtrVector.push_back(m_objDevicePtrVector[pair.second]->GetFeatureControl());
            uint32_t nStreamNum = m_objDevicePtrVector[pair.second]->GetStreamCount();
            if ((nStreamNum > 0))//存在风险!
            {
                m_objStreamPtrVector.push_back(m_objDevicePtrVector[pair.second]->OpenStream(0));
            }
        }

        //获取当前选定SN索引
        for (int i = 0; i < numSN; i++)
        {
            openAndSet(i);
        }
        ui.groupBox_2->setEnabled(true);
        ui.groupBox_3->setEnabled(true);
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

void qt_Mer::on_startCapture_clicked()
{
    snIndex = snMap[getCurrentSN()];
    if (isOpenCam)
    {
        if (!isOpenStreamVector[snIndex])
        {
            cout << "开始采集" << endl;
            QtCaptureEventHandler* pCaptureEventHandler = NULL;///<采集回调对象
            gxstring SN = getCurrentSN();
            folderCreated(static_cast<string>(getCurrentSN()));
            pCaptureEventHandler = new QtCaptureEventHandler();
            pCaptureEventHandler->m_RunningSN = SN;
            pCaptureEventHandler->m_OutputPicFolder = outputPicFolder;

            m_objStreamPtrVector[snIndex]->RegisterCaptureCallback(pCaptureEventHandler, NULL);
            //m_objStreamPtrVector[snIndex]->RegisterCaptureCallback(this, NULL);
            isOpenStreamVector[snIndex] = true;

            m_objStreamPtrVector[snIndex]->StartGrab();
            m_objRemoteFeatureControlPtrVector[snIndex]->GetCommandFeature("AcquisitionStart")->Execute();
        }
    }
    ui.groupBox_2->setEnabled(false);
    ui.groupBox_3->setEnabled(false);
}
void qt_Mer::on_closeDevice_clicked()
{
    snIndex = snMap[getCurrentSN()];
    if (isOpenCam)
    {
        for (int i = 0; i < numSN; i++)
        {
        
            if (isOpenStreamVector[i])
            {
                //发送停采命令
                cout << "停止采集" << endl;
                m_objRemoteFeatureControlPtrVector[i]->GetCommandFeature("AcquisitionStop")->Execute();
                m_objStreamPtrVector[i]->StopGrab();
                m_objStreamPtrVector[i]->UnregisterCaptureCallback();//注销采集回调
                m_objStreamPtrVector[i]->Close(); //关闭相机流
                isOpenStreamVector[i] = false;
            }
            //m_objDevicePtrVector[i]->Close(); //关闭相机，释放相机资源
        }
        cout << "释放资源" << endl;
        IGXFactory::GetInstance().Uninit(); //释放GxIAPICPPEx申请的所有资源

        ui.groupBox_2->setEnabled(true);
        ui.groupBox_3->setEnabled(true);

        isOpenCam = false;
        
    }
    isInstance = false;
}
void qt_Mer::on_stopCapture_clicked()
{
    snIndex = snMap[getCurrentSN()];
    if (isOpenStreamVector[snIndex])
    {
        cout << "停止采集" << endl;
        //发送停采命令
        m_objRemoteFeatureControlPtrVector[snIndex]->GetCommandFeature("AcquisitionStop")->Execute();
        m_objStreamPtrVector[snIndex]->StopGrab();
        isOpenStreamVector[snIndex] = false;
    }
    ui.groupBox_2->setEnabled(true);
    ui.groupBox_3->setEnabled(true);
}

void qt_Mer::showMatInLabel(Mat matImg, QString m_FrameID, gxstring m_RunningSN)
{
    cout << "showMat" << endl;
    if (m_RunningSN == getCurrentSN())
    {
        //cout << (m_RunningSN == getCurrentSN()) << endl;
        Mat src = matImg.clone();
        QImage imgForDisplay;
        if (matImg.channels() == 3)//RGB Img
        {
            cv::cvtColor(src, src, cv::COLOR_BGR2RGB);//颜色空间转换
            imgForDisplay = QImage(src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);
        }
        else//Gray Img
        {
            imgForDisplay = QImage((const unsigned char*)(src.data), src.cols, src.rows, src.cols * src.channels(), QImage::Format_Indexed8);
        }
        QPixmap Qimg = QPixmap::fromImage(imgForDisplay);
        ui.label_Img->setPixmap(Qimg);
        ui.frameID->setText(m_FrameID);
    }
}


void qt_Mer::updateUI()
{
    ui.width->setText(QString::number(m_width));
    ui.height->setText(QString::number(m_height));
    ui.factoryName->setText(m_firmName);
    ui.basicModel->setText(m_modelName);
}
void qt_Mer::DoOnDeviceOfflineEvent(void* pUserParam)
{
    isOffline = true;
    IGXFactory::GetInstance().Uninit(); //释放GxIAPICPPEx申请的所有资源
    cout << "设备掉线，请检查并重新连接。" << endl;
}


void QtCaptureEventHandler::DoOnImageCaptured(CImageDataPointer& objImageDataPointer, void* pUserParam)
{
    //判断图像是否为完整帧
    if (GX_FRAME_STATUS_SUCCESS == objImageDataPointer->GetStatus())
    {
        //图像获取为完整帧，可以读取图像宽、高、数据格式等
        uint64_t m_width = objImageDataPointer->GetWidth();
        uint64_t m_height = objImageDataPointer->GetHeight();
        //GX_PIXEL_FORMAT_ENTRY emPixelFormat =objImageDataPointer->GetPixelFormat();
        QString m_FrameID = QString::number(objImageDataPointer->GetFrameID());
        void* pRGB24Buffer = NULL;

        capturedImg = cv::Mat::zeros(cv::Size(m_width, m_height), CV_8UC3);
        //假设原始数据是BayerRG8图像，Bayer 转RGB24
        //cout << objImageDataPointer->GetPixelFormat() << endl;// GX_PIXEL_FORMAT_BAYER_RG8
        //pRGB24Buffer = objImageDataPointer->ConvertToRGB24(GX_BIT_2_9, GX_RAW2RGB_NEIGHBOUR, true);
        pRGB24Buffer = objImageDataPointer->ConvertToRGB24(GX_BIT_0_7, GX_RAW2RGB_NEIGHBOUR, true);
        memcpy(capturedImg.data, pRGB24Buffer, m_width * m_height * 3);
        cv::flip(capturedImg, capturedImg, 0);	//图像翻转

        pictureSave(objImageDataPointer, capturedImg);
        
        //cv::imshow(static_cast<string>(m_RunningSN), capturedImg);
        //cv::waitKey(1);
        // 
        //cout << "mainQt->showMatInLabel" << endl;
        //mainQt->showMatInLabel(capturedImg, m_FrameID, m_RunningSN);
        // 
        //emit transMat(capturedImg, m_FrameID);
        // 
        //updateUI();
    }
}

QtCaptureEventHandler::QtCaptureEventHandler(QWidget* parent) : QLabel(parent) 
{

}

QtCaptureEventHandler::~QtCaptureEventHandler() {}

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

void QtCaptureEventHandler::pictureSave(CImageDataPointer& objImageDataPointer, Mat capturedImg)	// 保存图片
{
    string filePath = m_OutputPicFolder + to_string(objImageDataPointer->GetFrameID()) + ".jpg";
    cv::imwrite(filePath, capturedImg);
    cout << this->m_RunningSN <<"图片保存成功" << std::endl;
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
    ui.fps->setText(QString::number(m_fps));
    ui.exposeTime->setText(QString::number(m_ExposureTime));  //设定曝光时间
    //模拟控制
    ui.gainAuto->setCurrentText(static_cast<QString>(m_GainAuto));	//自动增益选择 Once Continuous Off
    ui.gainValue->setText(QString::number(m_Gain));	//	off时增益值
    ui.gainValueMin->setText(QString::number(m_AutoGainMin));
    ui.gainValueMax->setText(QString::number(m_AutoGainMax));

    ui.balanceRatioSelector->setCurrentText(static_cast<QString>(m_BalanceRatioSelector));		//白平衡通道选择 Red Green Blue
    ui.balanceRatio->setText(QString::number(m_BalanceRatio));	//off时白平衡系数
    ui.balanceWhiteAuto->setCurrentText(static_cast<QString>(m_BalanceWhiteAuto));	//自动白平衡选择 Once Continuous Off
    cout << "读入面板数据" << endl;
};

void qt_Mer::updateSNList(gxdeviceinfo_vector vectorDeviceInfo)
{
    ui.snList->clear();
    for (int i = 0; i < vectorDeviceInfo.size(); i++)
    {
        gxstring SN = vectorDeviceInfo[i].GetSN();
        ui.snList->addItem(static_cast<QString>(SN));
        snMap[SN] = i;
    }
}

