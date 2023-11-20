#pragma once
#include "mainWindow.h"

MainWindow::MainWindow(QMainWindow* parent)
    : QMainWindow(parent)
{
    // �����˵���
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // �������ļ����˵�
    QMenu* fileMenu = menuBar->addMenu("device");

    // ������ˢ�¡��˵���
    QAction* refreshAction = new QAction("refresh", this);
    fileMenu->addAction(refreshAction);

    // ����ˢ�²˵�����źźͲ�
    connect(refreshAction, &QAction::triggered, this, &MainWindow::refresh);

    this->resize(1120, 900);

    getSNmain();

    for (const auto& pair : snMap)
    {
        std::cout << pair << std::endl;
        // �����µı�ǩҳ����
        QWidget* newTabContent = new qt_Mer(pair);
        QVBoxLayout* layout = new QVBoxLayout;
        newTabContent->setLayout(layout);

        // ���µı�ǩҳ��ӵ���ǩҳ�ؼ���
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
    numSN = vectorDeviceInfo.size();
    if (0 == vectorDeviceInfo.size())
    {
        cout << "�޿����豸!" << endl;
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
    cout << "�ͷ���Դ" << endl;
    IGXFactory::GetInstance().Uninit(); //�ͷ�GxIAPICPPEx�����������Դ
}

void MainWindow::refresh()
{
    gxdeviceinfo_vector vectorDeviceInfo;
    IGXFactory::GetInstance().UpdateDeviceList(1000, vectorDeviceInfo);
    numSN = vectorDeviceInfo.size();
    if (0 == vectorDeviceInfo.size())
    {
        cout << "�޿����豸!" << endl;
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
    std::set_intersection(snSet.begin(), snSet.end(), newSnSet.begin(), newSnSet.end(), std::inserter(commonSet, commonSet.begin())); //ȡ����
    for (const auto& sn : snSet)    //ԭ�����У�����û�еģ�ɾȥ
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
                cout << sn << "δ��sn_Index_Map��" << endl;
            }
            if (index >= 0)
            {
                cout << "�Ƴ�" << sn << endl;
                tabWidget.removeTab(index);
                sn_Index_Map.erase(it);
            }
        }
    }
    for (const auto& sn : newSnSet) ////�������У�����û�еģ����
    {
        if (commonSet.count(sn) == 0)
        {
            std::cout << sn << std::endl;
            // �����µı�ǩҳ����
            QWidget* newTabContent = new qt_Mer(sn);
            QVBoxLayout* layout = new QVBoxLayout;
            newTabContent->setLayout(layout);

            // ���µı�ǩҳ��ӵ���ǩҳ�ؼ���
            const char* pairSN = sn;
            int sn_Index = tabWidget.addTab(newTabContent, pairSN);
            sn_Index_Map[sn] = sn_Index;
        }
    }
    snMap = new_snMap;
}