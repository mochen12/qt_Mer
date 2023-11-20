#pragma once
#include "mainWindow.h"

MainWindow::MainWindow(QMainWindow* parent)
    : QMainWindow(parent)
{
    // 创建菜单栏
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // 创建“文件”菜单
    QMenu* fileMenu = menuBar->addMenu("device");

    // 创建“刷新”菜单项
    QAction* refreshAction = new QAction("refresh", this);
    fileMenu->addAction(refreshAction);

    // 连接刷新菜单项的信号和槽
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refresh);

    this->resize(1120, 900);

    getSNmain();

    for (const auto& pair : snMap)
    {
        std::cout << pair << std::endl;
        // 创建新的标签页内容
        QWidget* newTabContent = new qt_Mer(pair);
        QVBoxLayout* layout = new QVBoxLayout;
        newTabContent->setLayout(layout);

        // 将新的标签页添加到标签页控件中
        const char* pairSN = pair;
        int sn_Index = tabWidget.addTab(newTabContent, pairSN);
        sn_Index_Map[pair] = sn_Index;
        //std::thread saveThread(&MainWindow::saveThread, this);
    }

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(&tabWidget);
    //mainLayout->addWidget(openTabButton);
    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);
}


void MainWindow::getSNmain()
{
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
        cout << "无可用设备!" << endl;
        //ui->startCapture->setEnabled(false);
        //ui->stopCapture->setEnabled(false);
        return;
    }
    for (int i = 0; i < vectorDeviceInfo.size(); i++)
    {
        gxstring SN = vectorDeviceInfo[i].GetSN();
        //ui->snList->addItem(static_cast<QString>(SN));
        snMap.push_back(SN);
    }
    cout << "释放资源" << endl;
    IGXFactory::GetInstance().Uninit(); //释放GxIAPICPPEx申请的所有资源
}

void MainWindow::refresh()
{
    gxdeviceinfo_vector vectorDeviceInfo;
    IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
    numSN = vectorDeviceInfo.size();
    if (0 == vectorDeviceInfo.size())
    {
        cout << "无可用设备!" << endl;
        //ui->startCapture->setEnabled(false);
        //ui->stopCapture->setEnabled(false);
        return;
    }

    vector<gxstring>().swap(new_snMap);
    for (int i = 0; i < vectorDeviceInfo.size(); i++)
    {
        gxstring SN = vectorDeviceInfo[i].GetSN();
        //ui->snList->addItem(static_cast<QString>(SN));
        new_snMap.push_back(SN);
    }

    for (const auto& pair : new_snMap)
    {
        std::cout << pair << std::endl;

    }
    std::set<gxstring> snSet(snMap.begin(), snMap.end());
    std::set<gxstring> newSnSet(new_snMap.begin(), new_snMap.end());
    std::set<gxstring> commonSet;
    std::set_intersection(snSet.begin(), snSet.end(), newSnSet.begin(), newSnSet.end(), std::inserter(commonSet, commonSet.begin())); //取交集
    for (const auto& sn : snSet)    //原容器有，交集没有的，删去
    {
        if (commonSet.count(sn) == 0)
        {
            int index;
            auto it = sn_Index_Map.find(sn);
            if (it != sn_Index_Map.end())
            {
                index = it->second;
            }
            else
            {
                cout << sn << "未在sn_Index_Map内" << endl;
            }
            if (index >= 0)
            {
                cout << "移除" << sn << endl;
                tabWidget.removeTab(index);
                sn_Index_Map.erase(it);
            }
        }
    }
    for (const auto& sn : newSnSet) ////新容器有，交集没有的，添加
    {
        if (commonSet.count(sn) == 0)
        {
            std::cout << sn << std::endl;
            // 创建新的标签页内容
            QWidget* newTabContent = new qt_Mer(sn);
            QVBoxLayout* layout = new QVBoxLayout;
            newTabContent->setLayout(layout);

            // 将新的标签页添加到标签页控件中
            const char* pairSN = sn;
            int sn_Index = tabWidget.addTab(newTabContent, pairSN);
            sn_Index_Map[sn] = sn_Index;
        }
    }
    snMap = new_snMap;
}