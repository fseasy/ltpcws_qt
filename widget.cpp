#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    mainLayout = new QGridLayout(parent) ;
    setLayout(mainLayout) ;

    createLeftBox() ;
    mainLayout->addWidget(leftBox , 1,1) ;

    createTrainWidget() ;
    createTestWidget() ;

    stackedWidget = new QStackedWidget ;
    stackedWidget->addWidget(trainWidget) ;
    stackedWidget->addWidget(testWidget) ;

    mainLayout->addWidget(stackedWidget , 1 , 2 , 1 , 5) ;

    bindSwitchWidget() ;
}
void Widget::createLeftBox()
{
    leftBox = new QGroupBox(tr("选项")) ;
    QVBoxLayout *leftBoxLayout = new QVBoxLayout ;
    leftBox->setLayout(leftBoxLayout) ;

    trainWidgetSwitchBtn = new QPushButton(tr("训练")) ;
    testWidgetSwitchBtn = new QPushButton(tr("分词测试")) ;

    leftBoxLayout->addWidget(trainWidgetSwitchBtn) ;
    leftBoxLayout->addWidget(testWidgetSwitchBtn) ;
}

void Widget::createTrainWidget()
{
    trainWidget = new QWidget() ;
    QVBoxLayout *trainWidgetLayout = new QVBoxLayout ;
    trainWidget->setLayout(trainWidgetLayout) ;
    trainWidgetLayout->addWidget(new QPushButton(tr("训练面板"))) ;
}

void Widget::createTestWidget()
{
    testWidget = new QWidget() ;
    QVBoxLayout *testWidgetLayout = new QVBoxLayout ;
    testWidget->setLayout(testWidgetLayout) ;
    testWidgetLayout->addWidget(new QPushButton(tr("测试面板"))) ;
}

void Widget::bindSwitchWidget()
{
    connect(trainWidgetSwitchBtn , SIGNAL(clicked()) , this , SLOT(switchWidget())) ;
    connect(testWidgetSwitchBtn , SIGNAL(clicked()) , this , SLOT(switchWidget())) ;
}

void Widget::switchWidget()
{
    QPushButton *clickedBtn = static_cast<QPushButton *>(QObject::sender()) ;
    if(clickedBtn == trainWidgetSwitchBtn)
    {
        stackedWidget->setCurrentWidget(trainWidget) ;
    }
    else if(clickedBtn == testWidgetSwitchBtn)
    {
        stackedWidget->setCurrentWidget(testWidget) ;
    }
}

Widget::~Widget()
{

}
