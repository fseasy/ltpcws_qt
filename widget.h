#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QFileDialog>
#include <QPlainTexTEdit>
#include <QSpacerItem>

#include <QtDebug>
#include <functional>
#include "config.h"
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    enum TrainMode{BasicTrainMode , CustomTrainMode} ;
    enum TestMode{BasicTestMode , CustomTestMode} ;
    ~Widget();
private :
    // UI
    void createLeftBox() ;
    void createTrainWidget();
    void createTestWidget() ;
    QGridLayout* createPathSelectView(QString label ,QLineEdit * & edit ,bool isOpenFile=true) ;
    void setLayoutItemsEnabled(QLayout * layout,bool enable) ;
    void bindSwitchWidget() ;

    QGridLayout *mainLayout ;

    QGroupBox *leftBox ;
    QPushButton *trainWidgetSwitchBtn ;
    QPushButton *testWidgetSwitchBtn ;

    QLineEdit *trainPathEditTrain ;
    QLineEdit *trainPathEditDev ;
    QLineEdit *trainPathEditBasicModel ;
    QLineEdit *trainPathEditModelSaving ;
    QLineEdit *trainMaxIte ;
    QPlainTextEdit *trainEditLog ;

    QWidget *trainWidget ;
    QWidget *testWidget ;
    QStackedWidget *stackedWidget ;

    // Logic state

    Config config ;
    TrainMode trainMode ;
    TestMode testMode ;

public slots :

    void switchWidget() ;


};

#endif // WIDGET_H
