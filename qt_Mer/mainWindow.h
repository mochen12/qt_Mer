#pragma once
#include "qt_Mer.h"
#include "QMenuBar.h"

//��ǩҳ������Ϊ��Ա�����������ڹ��캯��������



class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    std::map<gxstring, int> sn_Index_Map;
    std::vector<gxstring> snMap;
    std::vector<gxstring> new_snMap;
    int numSN;
    qt_Mer* singlePage;
    // ������ǩҳ�ؼ�
    QTabWidget tabWidget;
    gxstring showSN;

    //std::condition_variable m_conditionVariable;
    //std::mutex m_mutex;
    //std::queue<ImageDataWithFrameID> m_imageQueue; // ͼ�����

public slots:

public:
    MainWindow(QMainWindow* parent = nullptr);
    void getSNmain();
    void refresh();
};
