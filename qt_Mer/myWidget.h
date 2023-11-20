#pragma once

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class MyWidget : public QWidget
{
public:
    QWidget* centralWidget;
    QWidget* widget_2;
    QVBoxLayout* verticalLayout;
    QGroupBox* groupBox;
    QGridLayout* gridLayout_3;
    QPushButton* startCapture;
    QPushButton* closeDevice;
    QPushButton* stopCapture;
    QPushButton* openDevice;
    QWidget* widget_3;
    QHBoxLayout* horizontalLayout;
    QPushButton* saveSettings;
    QPushButton* loadSettings;
    QGroupBox* groupBox_2;
    QGridLayout* gridLayout_2;
    QLabel* label;
    QLabel* label_12;
    QLabel* label_4;
    QLabel* label_13;
    QComboBox* balanceRatioSelector;
    QLabel* label_3;
    QLineEdit* gainValueMin;
    QLabel* label_11;
    QLineEdit* balanceRatio;
    QComboBox* balanceWhiteAuto;
    QLabel* label_2;
    QComboBox* gainAuto;
    QLineEdit* gainValue;
    QLineEdit* gainValueMax;
    QGroupBox* groupBox_3;
    QLabel* label_5;
    QLabel* label_6;
    QLineEdit* exposeTime;
    QLineEdit* fps;
    QGroupBox* groupBox_4;
    QGridLayout* gridLayout;
    QLabel* label_7;
    QLabel* factoryName;
    QLabel* label_8;
    QLabel* basicModel;
    QLabel* label_9;
    QLabel* width;
    QLabel* label_10;
    QLabel* height;
    QFrame* frame;
    QLabel* label_Img;
    QWidget* widget_5;
    QGridLayout* gridLayout_6;
    QWidget* widget;
    QGridLayout* gridLayout_4;
    QLabel* label_14;
    QLineEdit* frameID;
    QWidget* widget_4;
    QGridLayout* gridLayout_5;
    QLabel* label_15;
    QLineEdit* frameRemain;
    QPushButton* showButton;

public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent)
    {

        this->resize(1112, 817);
        centralWidget = new QWidget(this);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->resize(1112, 817);
        widget_2 = new QWidget(centralWidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setGeometry(QRect(740, 30, 351, 721));
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(widget_2);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        startCapture = new QPushButton(groupBox);
        startCapture->setObjectName(QString::fromUtf8("startCapture"));

        gridLayout_3->addWidget(startCapture, 1, 0, 1, 1);

        closeDevice = new QPushButton(groupBox);
        closeDevice->setObjectName(QString::fromUtf8("closeDevice"));

        gridLayout_3->addWidget(closeDevice, 0, 1, 1, 1);

        stopCapture = new QPushButton(groupBox);
        stopCapture->setObjectName(QString::fromUtf8("stopCapture"));

        gridLayout_3->addWidget(stopCapture, 1, 1, 1, 1);

        openDevice = new QPushButton(groupBox);
        openDevice->setObjectName(QString::fromUtf8("openDevice"));

        gridLayout_3->addWidget(openDevice, 0, 0, 1, 1);


        verticalLayout->addWidget(groupBox);

        widget_3 = new QWidget(widget_2);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        widget_3->setMinimumSize(QSize(0, 116));
        horizontalLayout = new QHBoxLayout(widget_3);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        saveSettings = new QPushButton(widget_3);
        saveSettings->setObjectName(QString::fromUtf8("saveSettings"));

        horizontalLayout->addWidget(saveSettings);

        loadSettings = new QPushButton(widget_3);
        loadSettings->setObjectName(QString::fromUtf8("loadSettings"));

        horizontalLayout->addWidget(loadSettings);


        verticalLayout->addWidget(widget_3);

        groupBox_2 = new QGroupBox(widget_2);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setAutoFillBackground(false);
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 1, 0, 1, 1);

        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_2->addWidget(label_12, 6, 0, 1, 1);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 5, 0, 1, 1);

        label_13 = new QLabel(groupBox_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout_2->addWidget(label_13, 7, 0, 1, 1);

        balanceRatioSelector = new QComboBox(groupBox_2);
        balanceRatioSelector->addItem(QString());
        balanceRatioSelector->addItem(QString());
        balanceRatioSelector->addItem(QString());
        balanceRatioSelector->setObjectName(QString::fromUtf8("balanceRatioSelector"));
        balanceRatioSelector->setEditable(false);

        gridLayout_2->addWidget(balanceRatioSelector, 5, 1, 1, 1);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 3, 0, 1, 1);

        gainValueMin = new QLineEdit(groupBox_2);
        gainValueMin->setObjectName(QString::fromUtf8("gainValueMin"));

        gridLayout_2->addWidget(gainValueMin, 3, 1, 1, 1);

        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout_2->addWidget(label_11, 4, 0, 1, 1);

        balanceRatio = new QLineEdit(groupBox_2);
        balanceRatio->setObjectName(QString::fromUtf8("balanceRatio"));

        gridLayout_2->addWidget(balanceRatio, 7, 1, 1, 1);

        balanceWhiteAuto = new QComboBox(groupBox_2);
        balanceWhiteAuto->addItem(QString());
        balanceWhiteAuto->addItem(QString());
        balanceWhiteAuto->addItem(QString());
        balanceWhiteAuto->setObjectName(QString::fromUtf8("balanceWhiteAuto"));

        gridLayout_2->addWidget(balanceWhiteAuto, 6, 1, 1, 1);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 2, 0, 1, 1);

        gainAuto = new QComboBox(groupBox_2);
        gainAuto->addItem(QString());
        gainAuto->addItem(QString());
        gainAuto->addItem(QString());
        gainAuto->setObjectName(QString::fromUtf8("gainAuto"));
        gainAuto->setEnabled(true);
        gainAuto->setEditable(false);

        gridLayout_2->addWidget(gainAuto, 1, 1, 1, 1);

        gainValue = new QLineEdit(groupBox_2);
        gainValue->setObjectName(QString::fromUtf8("gainValue"));
        gainValue->setMaxLength(32768);

        gridLayout_2->addWidget(gainValue, 2, 1, 1, 1);

        gainValueMax = new QLineEdit(groupBox_2);
        gainValueMax->setObjectName(QString::fromUtf8("gainValueMax"));

        gridLayout_2->addWidget(gainValueMax, 4, 1, 1, 1);


        verticalLayout->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(widget_2);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(30, 34, 101, 16));
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(30, 69, 131, 16));
        exposeTime = new QLineEdit(groupBox_3);
        exposeTime->setObjectName(QString::fromUtf8("exposeTime"));
        exposeTime->setGeometry(QRect(160, 34, 131, 21));
        fps = new QLineEdit(groupBox_3);
        fps->setObjectName(QString::fromUtf8("fps"));
        fps->setGeometry(QRect(160, 69, 131, 21));

        verticalLayout->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(widget_2);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        gridLayout = new QGridLayout(groupBox_4);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_7 = new QLabel(groupBox_4);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 0, 0, 1, 1);

        factoryName = new QLabel(groupBox_4);
        factoryName->setObjectName(QString::fromUtf8("factoryName"));

        gridLayout->addWidget(factoryName, 0, 1, 1, 1);

        label_8 = new QLabel(groupBox_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 1, 0, 1, 1);

        basicModel = new QLabel(groupBox_4);
        basicModel->setObjectName(QString::fromUtf8("basicModel"));

        gridLayout->addWidget(basicModel, 1, 1, 1, 1);

        label_9 = new QLabel(groupBox_4);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 2, 0, 1, 1);

        width = new QLabel(groupBox_4);
        width->setObjectName(QString::fromUtf8("width"));

        gridLayout->addWidget(width, 2, 1, 1, 1);

        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 3, 0, 1, 1);

        height = new QLabel(groupBox_4);
        height->setObjectName(QString::fromUtf8("height"));

        gridLayout->addWidget(height, 3, 1, 1, 1);


        verticalLayout->addWidget(groupBox_4);

        frame = new QFrame(centralWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(30, 50, 671, 641));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        frame->setLineWidth(1);
        label_Img = new QLabel(frame);
        label_Img->setObjectName(QString::fromUtf8("label_Img"));
        label_Img->setGeometry(QRect(10, 70, 640, 512));
        label_Img->setAutoFillBackground(true);
        label_Img->setFrameShape(QFrame::StyledPanel);
        label_Img->setFrameShadow(QFrame::Plain);
        label_Img->setTextFormat(Qt::PlainText);
        label_Img->setScaledContents(true);
        label_Img->setAlignment(Qt::AlignCenter);
        widget_5 = new QWidget(frame);
        widget_5->setObjectName(QString::fromUtf8("widget_5"));
        widget_5->setGeometry(QRect(10, 10, 341, 56));
        gridLayout_6 = new QGridLayout(widget_5);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        widget = new QWidget(widget_5);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout_4 = new QGridLayout(widget);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        label_14 = new QLabel(widget);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout_4->addWidget(label_14, 0, 0, 1, 1);

        frameID = new QLineEdit(widget);
        frameID->setObjectName(QString::fromUtf8("frameID"));
        frameID->setReadOnly(true);

        gridLayout_4->addWidget(frameID, 0, 1, 1, 1);


        gridLayout_6->addWidget(widget, 0, 0, 1, 1);

        widget_4 = new QWidget(widget_5);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));
        gridLayout_5 = new QGridLayout(widget_4);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        label_15 = new QLabel(widget_4);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout_5->addWidget(label_15, 0, 0, 1, 1);

        frameRemain = new QLineEdit(widget_4);
        frameRemain->setObjectName(QString::fromUtf8("frameRemain"));
        frameRemain->setReadOnly(true);

        gridLayout_5->addWidget(frameRemain, 0, 1, 1, 1);


        gridLayout_6->addWidget(widget_4, 0, 1, 1, 1);

        showButton = new QPushButton(widget_5);
        showButton->setObjectName(QString::fromUtf8("showButton"));

        gridLayout_6->addWidget(showButton, 0, 2, 1, 1);

        retranslateUi(this);

        balanceRatioSelector->setCurrentIndex(0);
    }
    void retranslateUi(QWidget* qt_MerClass)
    {
        qt_MerClass->setWindowTitle(QApplication::translate("qt_MerClass", "DH_MER", nullptr));
        groupBox->setTitle(QApplication::translate("qt_MerClass", "\350\256\276\345\244\207\346\216\247\345\210\266", nullptr));
        startCapture->setText(QApplication::translate("qt_MerClass", "\345\274\200\345\247\213\351\207\207\351\233\206", nullptr));
        closeDevice->setText(QApplication::translate("qt_MerClass", "\345\205\263\351\227\255\350\256\276\345\244\207", nullptr));
        stopCapture->setText(QApplication::translate("qt_MerClass", "\345\201\234\346\255\242\351\207\207\351\233\206", nullptr));
        openDevice->setText(QApplication::translate("qt_MerClass", "\346\211\223\345\274\200\350\256\276\345\244\207", nullptr));
        saveSettings->setText(QApplication::translate("qt_MerClass", "\344\277\235\345\255\230\351\205\215\347\275\256", nullptr));
        loadSettings->setText(QApplication::translate("qt_MerClass", "\350\257\273\345\217\226\351\205\215\347\275\256", nullptr));
        groupBox_2->setTitle(QApplication::translate("qt_MerClass", "\346\250\241\346\213\237\346\216\247\345\210\266", nullptr));
        label->setText(QApplication::translate("qt_MerClass", "\345\242\236\347\233\212\346\250\241\345\274\217", nullptr));
        label_12->setText(QApplication::translate("qt_MerClass", "\347\231\275\345\271\263\350\241\241\346\250\241\345\274\217", nullptr));
        label_4->setText(QApplication::translate("qt_MerClass", "\347\231\275\345\271\263\350\241\241\351\200\232\351\201\223", nullptr));
        label_13->setText(QApplication::translate("qt_MerClass", "\347\231\275\345\271\263\350\241\241\347\263\273\346\225\260", nullptr));
        balanceRatioSelector->setItemText(0, QApplication::translate("qt_MerClass", "Red", nullptr));
        balanceRatioSelector->setItemText(1, QApplication::translate("qt_MerClass", "Blue", nullptr));
        balanceRatioSelector->setItemText(2, QApplication::translate("qt_MerClass", "Green", nullptr));

        balanceRatioSelector->setCurrentText(QApplication::translate("qt_MerClass", "Red", nullptr));
        label_3->setText(QApplication::translate("qt_MerClass", "\350\207\252\345\212\250\345\242\236\347\233\212\344\270\213\351\231\220", nullptr));
        gainValueMin->setText(QApplication::translate("qt_MerClass", "0", nullptr));
        label_11->setText(QApplication::translate("qt_MerClass", "\350\207\252\345\212\250\345\242\236\347\233\212\344\270\212\351\231\220", nullptr));
        balanceRatio->setText(QApplication::translate("qt_MerClass", "1", nullptr));
        balanceWhiteAuto->setItemText(0, QApplication::translate("qt_MerClass", "Continuous", nullptr));
        balanceWhiteAuto->setItemText(1, QApplication::translate("qt_MerClass", "Off", nullptr));
        balanceWhiteAuto->setItemText(2, QApplication::translate("qt_MerClass", "Once", nullptr));

        balanceWhiteAuto->setCurrentText(QApplication::translate("qt_MerClass", "Continuous", nullptr));
        label_2->setText(QApplication::translate("qt_MerClass", "\345\242\236\347\233\212\345\200\274(0-24dB)", nullptr));
        gainAuto->setItemText(0, QApplication::translate("qt_MerClass", "Continuous", nullptr));
        gainAuto->setItemText(1, QApplication::translate("qt_MerClass", "Off", nullptr));
        gainAuto->setItemText(2, QApplication::translate("qt_MerClass", "Once", nullptr));

        gainValue->setText(QApplication::translate("qt_MerClass", "16", nullptr));
        gainValueMax->setText(QApplication::translate("qt_MerClass", "16", nullptr));
        groupBox_3->setTitle(QApplication::translate("qt_MerClass", "\345\237\272\346\234\254\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        label_5->setText(QApplication::translate("qt_MerClass", "\346\233\235\345\205\211\346\227\266\351\227\264(us)", nullptr));
        label_6->setText(QApplication::translate("qt_MerClass", "\345\270\247\347\216\207", nullptr));
        exposeTime->setText(QApplication::translate("qt_MerClass", "10000", nullptr));
        exposeTime->setPlaceholderText(QString());
        fps->setText(QApplication::translate("qt_MerClass", "20", nullptr));
        groupBox_4->setTitle(QApplication::translate("qt_MerClass", "\350\256\276\345\244\207\344\277\241\346\201\257", nullptr));
        label_7->setText(QApplication::translate("qt_MerClass", "\345\216\202\345\225\206\345\220\215\347\247\260", nullptr));
        factoryName->setText(QString());
        label_8->setText(QApplication::translate("qt_MerClass", "\345\237\272\346\234\254\345\236\213\345\217\267", nullptr));
        basicModel->setText(QString());
        label_9->setText(QApplication::translate("qt_MerClass", "\345\233\276\345\203\217\345\256\275\345\272\246", nullptr));
        width->setText(QString());
        label_10->setText(QApplication::translate("qt_MerClass", "\345\233\276\345\203\217\351\253\230\345\272\246", nullptr));
        height->setText(QString());
        label_Img->setText(QString());
        label_14->setText(QApplication::translate("qt_MerClass", "\346\200\273\351\207\207\351\233\206\346\225\260", nullptr));
        frameID->setText(QString());
        label_15->setText(QApplication::translate("qt_MerClass", "\345\276\205\344\277\235\345\255\230\346\225\260", nullptr));
        frameRemain->setText(QString());
        showButton->setText(QApplication::translate("qt_MerClass", "\345\261\225\347\244\272\345\233\276\345\203\217", nullptr));
    } // retranslateUi
};