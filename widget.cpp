#include "widget.h"
#include "config.h"
#include <QIntValidator>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    // UI init
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

    // Logit Init
    trainMode = CustomTrainMode ;
    testMode = CustomTestMode ;

    bindSwitchWidget() ;
    resize(600,300) ;

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
    QGridLayout * trainLayout = new QGridLayout ;
    trainWidget->setLayout(trainLayout) ;

    //  train model
    QGroupBox * trainModelBox = new QGroupBox(tr("训练模型选择")) ;
    //trainModelBox->setFlat(true) ;
    QButtonGroup *trainBtnGroup = new QButtonGroup() ;
    QRadioButton * basicRadio = new QRadioButton(tr("基础模型训练") , trainWidget) ;
    QRadioButton * customRadio = new QRadioButton(tr("个性化模型训练") , trainWidget) ;
    trainBtnGroup->addButton(basicRadio) ;
    trainBtnGroup->addButton(customRadio) ;
    QLabel * trainModelInfo = new QLabel() ;
    trainModelInfo->setText(config.basicModelIntro) ;
    trainModelInfo->setWordWrap(true) ;
    QGridLayout *modelLayout = new QGridLayout ;
    modelLayout->addWidget(basicRadio , 0 , 0) ;
    modelLayout->addWidget(customRadio , 0 , 1) ;
    modelLayout->addWidget(trainModelInfo , 1 , 0 , 2 , 2) ;
    trainModelBox->setLayout(modelLayout) ;
    trainLayout->addWidget(trainModelBox , 0,0,3,6) ;

    // path config
    QGroupBox * pathConfigBox = new QGroupBox(tr("路径设置")) ;
    QVBoxLayout *pathConfLayout = new QVBoxLayout ;
    pathConfLayout->addLayout(createPathSelectView(tr("训练集路径") , trainPathEditTrain )) ;
    pathConfLayout->addLayout(createPathSelectView(tr("开发集路径") , trainPathEditDev)) ;
    QGridLayout * basicModelLayout = createPathSelectView(tr("基础模型路径") , trainPathEditBasicModel) ;
    pathConfLayout->addLayout(basicModelLayout) ;
    trainMaxIte = new QLineEdit("5") ;
    QRegExp validatorReg("[1-9][0-9].") ;
    trainMaxIte->setValidator(new QRegExpValidator(validatorReg , this)) ;
    QLabel * maxIteLabel = new QLabel(tr("最大迭代次数")) ;
    QGridLayout * iteLayout = new QGridLayout ;
    iteLayout->addWidget(maxIteLabel,0,0) ;
    iteLayout->addWidget(trainMaxIte,0,1,1,5) ;
    pathConfLayout->addLayout(iteLayout) ;
    pathConfLayout->addLayout(createPathSelectView(tr("模型保存路径") , trainPathEditModelSaving  , false)) ;
    pathConfigBox->setLayout(pathConfLayout) ;
    trainLayout->addWidget(pathConfigBox , 3 , 0 , 5 , 6) ;

    // Train Button
    QPushButton *trainBtn = new QPushButton(tr("训练")) ;
    trainLayout->addWidget(trainBtn , 8 , 0) ;

    trainLayout->setRowMinimumHeight(8,40) ;
    // Train Log
    trainEditLog = new QPlainTextEdit() ;
    QGroupBox * logBox = new QGroupBox(tr("训练日志输出")) ;
    QGridLayout *logLayout = new QGridLayout ;
    logLayout->addWidget(trainEditLog , 0 ,0  ) ;
    logBox->setLayout(logLayout) ;
    logBox->setMaximumHeight(150);
    trainLayout->addWidget(logBox , 10 , 0 ,1, 6) ;

    // bind action for train radio
    std::function<void(QAbstractButton *)> radioSwitchHandler = [=](QAbstractButton *button)
    {
      if(button == basicRadio)
      {
         setLayoutItemsEnabled(basicModelLayout,false) ;
         trainMode = BasicTrainMode ;
      }
      else if(button == customRadio)
      {
         setLayoutItemsEnabled(basicModelLayout,true) ;
         trainMode = CustomTrainMode ;
      }
    } ;
    connect(trainBtnGroup , static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked) ,
            radioSwitchHandler) ;
    // Init state
    trainMode = CustomTrainMode ;
    customRadio->setChecked(true) ;
    radioSwitchHandler(customRadio) ;
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
QGridLayout* Widget::createPathSelectView(QString  label , QLineEdit *& edit,bool isOpenFile)
{
    QGridLayout *layout = new QGridLayout ;
    QLabel *labelWidget = new QLabel(label) ;
    edit = new QLineEdit() ;
    QPushButton *btn = new QPushButton() ;
    if(isOpenFile)
    {
        btn->setText(tr("浏览")) ;
    }
    else
    {
        btn->setText(tr("保存")) ;
    }
    connect(btn , QPushButton::clicked , [=]()
    {
        QString fName = "";
        if(isOpenFile)
        {
           fName =  QFileDialog::getOpenFileName(this , label , "" , "" );

        }
        else
        {
           fName = QFileDialog::getSaveFileName(this , label , "" , "") ;
        }
        if(!fName.isEmpty())
        {
            edit->setText(fName) ;
        }

    }) ;
    layout->addWidget(labelWidget,0,0) ;
    layout->addWidget(edit,0,1,1,4) ;
    layout->addWidget(btn,0,5) ;
    return layout ;
}
void Widget::setLayoutItemsEnabled(QLayout *layout,bool enable)
{
    for(int i = 0 ; i < layout->count() ; ++i)
    {
        layout->itemAt(i)->widget()->setEnabled(enable) ;
    }
}

Widget::~Widget()
{

}
