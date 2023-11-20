#pragma once
#include "qt_Mer.h"
#include "QMenuBar.h"

//标签页必须作为成员声明，不能在构造函数内声明



class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    std::map<gxstring, int> sn_Index_Map;
    std::vector<gxstring> snMap;
    std::vector<gxstring> new_snMap;
    int numSN;
    qt_Mer* singlePage;
    // 创建标签页控件
    QTabWidget tabWidget;
    gxstring showSN;

    //std::condition_variable m_conditionVariable;
    //std::mutex m_mutex;
    //std::queue<ImageDataWithFrameID> m_imageQueue; // 图像队列

public slots:

public:
    MainWindow(QMainWindow* parent = nullptr);
    void getSNmain();
    void refresh();
};
