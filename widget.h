#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
private :

    void createLeftBox() ;
    void createTrainWidget();
    void createTestWidget() ;

    void bindSwichWidget() ;

    QGridLayout *mainLayout ;

    QGroupBox *leftBox ;
    QPushButton *trainWidgetSwitchBtn ;
    QPushButton *testWidgetSwitchBtn ;

    QWidget *trainWidget ;
    QWidget *testWidget ;
    QStackedWidget *stackedWidget ;

    void bindSwitchWidget() ;
public slots :

    void switchWidget() ;


};

#endif // WIDGET_H
