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
#include <QScrollBar>
#include <QSpacerItem>

#include <QProcess>
#include <QtDebug>
#include <functional>
#include <QTextCodec>
#include <QByteArray>
#include "config.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    enum TrainMode{BasicTrainMode , CustomTrainMode} ;
    enum PredictMode{BasicPredictMode , CustomPredictMode} ;
    ~Widget();
private :
    // Main UI
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

    // train only
    void preSetAllTrainPathSelectViews() ;

    QLineEdit *trainPathEditTrain ;
    QLineEdit *trainPathEditDev ;
    QLineEdit *trainPathEditBasicModel ;
    QLineEdit *trainPathEditModelSaving ;
    QLineEdit *trainMaxIte ;
    QPlainTextEdit *trainEditLog ;

    // predict(test) only
    void preSetAllPredictPathSelectViews() ;

    QLineEdit *testPathEditBasicModel ;
    QLineEdit *testPathEditCustomModel ;

    QWidget *trainWidget ;
    QWidget *testWidget ;
    QStackedWidget *stackedWidget ;

    // Logic state
    bool checkReadPathValid(QString path) ;
    bool checkWritePathValid(QString path) ;
    Config config ;
    TrainMode trainMode ;
    PredictMode predictMode ;

public slots :

    void switchWidget() ;


};

#endif // WIDGET_H
