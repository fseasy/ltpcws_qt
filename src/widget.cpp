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
    createAboutWidget() ;

    stackedWidget = new QStackedWidget ;
    stackedWidget->addWidget(trainWidget) ;
    stackedWidget->addWidget(testWidget) ;
    stackedWidget->addWidget(aboutWidget) ;
    mainLayout->addWidget(stackedWidget , 1 , 2 , 1 , 5) ;

    // Logit Init
    trainMode = CustomTrainMode ;
    predictMode = CustomPredictMode ;

    bindSwitchWidget() ;

    // style sheet
    LoadWidgetStyle() ;
    setWindowIcon(QIcon(":/res/images/ltpcws.ico")) ;
    setWindowTitle(tr("LTP分词版(LTP-CWS)")) ;
    resize(800,450) ;

}
void Widget::createLeftBox()
{
    leftBox = new QGroupBox(tr("选项")) ;
    QVBoxLayout *leftBoxLayout = new QVBoxLayout ;
    leftBox->setLayout(leftBoxLayout) ;

    trainWidgetSwitchBtn = new QPushButton() ;
    trainWidgetSwitchBtn->setIcon(QIcon(":/res/images/train.png")) ;
    trainWidgetSwitchBtn->setIconSize(QSize(60,60)) ;
    trainWidgetSwitchBtn->setFlat(false) ;

    testWidgetSwitchBtn = new QPushButton() ;
    testWidgetSwitchBtn->setIcon(QIcon(":/res/images/predict.png")) ;
    testWidgetSwitchBtn->setIconSize(QSize(60,60)) ;
    testWidgetSwitchBtn->setFlat(true) ;

    aboutWidgetSwitchBtn = new QPushButton() ;
    aboutWidgetSwitchBtn->setIcon(QIcon(":/res/images/about.png")) ;
    aboutWidgetSwitchBtn->setIconSize(QSize(60,60)) ;
    aboutWidgetSwitchBtn->setFlat(true) ;

    leftBoxLayout->addWidget(trainWidgetSwitchBtn) ;
    leftBoxLayout->addWidget(testWidgetSwitchBtn) ;
    leftBoxLayout->addWidget(aboutWidgetSwitchBtn) ;
}

void Widget::createTrainWidget()
{
    trainWidget = new QWidget() ;
    QGridLayout * trainLayout = new QGridLayout ;
    trainWidget->setLayout(trainLayout) ;

    //  train model
    QGroupBox * trainModelBox = new QGroupBox(tr("训练模型选择")) ;
    //trainModelBox->setFlat(true) ;
    QButtonGroup *trainModeBtnGroup = new QButtonGroup() ;
    QRadioButton * basicRadio = new QRadioButton(tr("基础模型训练") , trainWidget) ;
    QRadioButton * customRadio = new QRadioButton(tr("个性化模型训练") , trainWidget) ;
    trainModeBtnGroup->addButton(basicRadio) ;
    trainModeBtnGroup->addButton(customRadio) ;
    QLabel * trainModelInfo = new QLabel() ;
    trainModelInfo->setWordWrap(true) ;
    QGridLayout *modelLayout = new QGridLayout ;
    modelLayout->addWidget(basicRadio , 0 , 0) ;
    modelLayout->addWidget(customRadio , 0 , 1) ;
    modelLayout->addWidget(trainModelInfo , 1 , 0 , 2 , 2) ;
    trainModelBox->setLayout(modelLayout) ;
    trainModelBox->setMinimumHeight(150) ; // avoid height change as model changing
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
    trainEditLog->setReadOnly(true) ;
    trainEditLog->setFont(QFont("consolas",10)) ;
    QGroupBox * logBox = new QGroupBox(tr("训练日志输出")) ;
    QGridLayout *logLayout = new QGridLayout ;
    logLayout->addWidget(trainEditLog , 0 ,0  ) ;
    logBox->setLayout(logLayout) ;
    logBox->setMaximumHeight(150);
    trainLayout->addWidget(logBox , 9 , 0 ,3, 6) ;

    // bind action for train radio
    std::function<void(QAbstractButton *)> radioSwitchHandler = [=](QAbstractButton *button)
    {
      if(button == basicRadio)
      {
         setLayoutItemsEnabled(basicModelLayout,false) ;
         trainMode = BasicTrainMode ;
         trainModelInfo->setText(config.basicModeTrainIntro) ;
      }
      else if(button == customRadio)
      {
         setLayoutItemsEnabled(basicModelLayout,true) ;
         trainMode = CustomTrainMode ;
         trainModelInfo->setText(config.customModeTrainIntro) ;
      }
      preSetAllTrainPathSelectViews() ;
    } ;
    connect(trainModeBtnGroup , static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked) ,
            radioSwitchHandler) ;
    // Bind action for train btn
    connect(trainBtn , QPushButton::clicked , [=]()
    {
        bool isCustomMode = trainMode == CustomTrainMode ? true : false ;
        bool checkPathState = checkReadPathValid(trainPathEditTrain->text())
                              && checkReadPathValid(trainPathEditDev->text())
                              && checkWritePathValid(trainPathEditModelSaving->text()) ;
        if(isCustomMode){checkPathState = checkPathState && checkReadPathValid(trainPathEditBasicModel->text());}
        if(!checkPathState)
        {
            QMessageBox::information(this , tr("路径设置错误") ,
                                     tr("路径检查错误！请检查路径配置是否正确。然后重新点击按钮。")) ;
            return ;
        }
        bool confSavingState = config.saveTrainConfig(isCustomMode , trainPathEditTrain->text() , trainPathEditDev->text() ,
                               trainPathEditModelSaving->text() , trainMaxIte->text() ,
                               trainPathEditBasicModel->text()) ;
        //qDebug() << ( confSavingState ? "trainning config saving ok" : "trainning config saving failed" );
        if(!confSavingState)
        {
            QMessageBox::information(this , tr("内部错误") ,
                                     tr("内部训练配置文件写入错误！")) ;
            return ;
        }
        ///QMessageBox::information(this , "path" , config.getConfInfo()) ;
        // Start Train
        trainEditLog->clear() ;
        if(!config.getCwsExeState())
        {
            QMessageBox::information(this , tr("内部错误") ,
                                     tr("未找到适合该操作系统的分词程序\n请下载源代码编译后放在cws_bin/others目录下")) ;
            return ;
        }
        QStringList params ;
        config.getTrainParams(isCustomMode,params) ;
        QProcess *trainProcess = new QProcess(this) ;
        connect(trainProcess , QProcess::started , [=]()
        {
            //qDebug() << "process started" ;
            trainBtn->setDisabled(true) ;
            // radio button should also be set disable
            for(const auto & radioBtn : trainModeBtnGroup->buttons()){ radioBtn->setDisabled(true) ;}
        }) ;
        connect(trainProcess , QProcess::readyReadStandardError , [=]()
        {
            // format log for display
            QString readedLog =  trainProcess->readAllStandardError().trimmed() ;
            QString separator = "__" ;
            readedLog.replace(QRegExp("(\\[TRACE\\]|\\[ERROR\\]|\\[INFO\\])"),separator + "\\1") ;
            QStringList logs = readedLog.split(separator , QString::SkipEmptyParts) ;

            QString strongS = "<b>" , strongE = "</b>" ;
            QString redS = "<span style='color:red'>" , redE = "</span>" ;
            QString enter = "<br/>" ;
            for(const QString & log : logs)
            {
                QString formatedLog = log ;
                if(formatedLog.indexOf("Model for iteration") != -1)
                {
                    formatedLog = strongS + formatedLog + strongE ;
                }
                else if(formatedLog.indexOf("Best result") != -1)
                {
                    formatedLog = redS + formatedLog + redE ;
                }
                else
                {
                    formatedLog.replace(QRegExp("\\[TRACE\\]") , strongS + "[TRACE]" + strongE);
                    formatedLog.replace(QRegExp("\\[ERROR\\]") , redS + "[ERROR]" + redE) ;
                }
                formatedLog = formatedLog + enter ;
                //qDebug() << formatedLog ;
                trainEditLog->appendHtml(formatedLog);
            }


        }) ;
        connect(trainProcess , static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished) , [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            trainBtn->setEnabled(true) ;
            for(const auto & radioBtn : trainModeBtnGroup->buttons() ){ radioBtn->setEnabled(true) ;}
            //qDebug() << "exitCode" << exitCode
            //         << exitStatus ;
        }) ;
        trainProcess->start(config.getCwsExePath() , params) ;


    }) ;
    // Init state
    trainMode = CustomTrainMode ;
    customRadio->setChecked(true) ;
    radioSwitchHandler(customRadio) ;
}

void Widget::createTestWidget()
{
    testWidget = new QWidget() ;
    QGridLayout *testLayout = new QGridLayout() ;
    testWidget->setLayout(testLayout) ;

    /** Model Config part */
    // UI
    QGroupBox * modelConfBox = new QGroupBox(tr("模型配置")) ;
    QGridLayout *modelConfLayout = new QGridLayout() ;
    modelConfBox->setLayout(modelConfLayout) ;
    QRadioButton *basicModelRadio = new QRadioButton(tr("基础模型")) ;
    QRadioButton *customModelRadio = new QRadioButton(tr("个性化模型")) ;
    QLabel * modeInfo = new QLabel() ;
    modeInfo->setWordWrap(true) ;
    QGridLayout *basicModelPathView = createPathSelectView(tr("基础模型路径") , testPathEditBasicModel) ;
    QGridLayout *customModelPathView = createPathSelectView(tr("个性化模型路径") , testPathEditCustomModel) ;
    modelConfLayout->addWidget(basicModelRadio,0,0) ;
    modelConfLayout->addWidget(customModelRadio , 0, 1) ;
    modelConfLayout->addWidget(modeInfo , 1, 0 , 2 ,2 ) ;
    modelConfLayout->addLayout(basicModelPathView , 3,0,1,2) ;
    modelConfLayout->addLayout(customModelPathView ,4,0,1,2) ;
    testLayout->addWidget(modelConfBox , 0 , 0 , 6 , 6) ;
    // Logic
    QButtonGroup *modelConfBtnGroup = new QButtonGroup() ;
    modelConfBtnGroup->addButton(basicModelRadio) ;
    modelConfBtnGroup->addButton(customModelRadio) ;
    // Event
    std::function<void(QAbstractButton *)> modelConfRadioBtnClickHandler = [=](QAbstractButton * btn)
    {
        if(btn == basicModelRadio)
        {
            modeInfo->setText(config.basicModePredictIntro) ;
            setLayoutItemsEnabled(customModelPathView , false) ;
            predictMode = BasicPredictMode ;
        }
        else if(btn == customModelRadio)
        {
            modeInfo->setText(config.customModePredictIntro) ;
            setLayoutItemsEnabled(customModelPathView , true) ;
            predictMode = CustomPredictMode ;
        }
        preSetAllPredictPathSelectViews() ;

    } ;

    connect(modelConfBtnGroup , static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked) ,
            modelConfRadioBtnClickHandler) ;


    /** INPUT Editor */
    // UI
    QGroupBox * inputBox = new QGroupBox(tr("输入")) ;
    QGridLayout * inputLayout = new QGridLayout() ;
    inputBox->setLayout(inputLayout) ;
    QPlainTextEdit * inputEditor = new QPlainTextEdit() ;
    inputEditor->setFont(QFont("Microsoft YaHei" , 10)) ;
    QPushButton *loadFileBtn = new QPushButton(tr("从文件中加载(UTF8编码)")) ;
    QPushButton *clearBtn = new QPushButton(tr("清除输入")) ;
    QPushButton *cwsBtn = new QPushButton(tr("分词")) ;
    inputLayout->addWidget(inputEditor , 0 , 0 , 5 , 6) ;
    inputLayout->addWidget(loadFileBtn , 5 , 4 , 1 , 1) ;
    inputLayout->addWidget(clearBtn , 5 , 5 , 1, 1) ;
    inputLayout->addWidget(cwsBtn , 5,0,1,1) ;
    inputBox->setMaximumHeight(150) ; // max height 150
    testLayout->addWidget(inputBox , 6 , 0 , 6 , 6) ;
    // UI logic
    connect(clearBtn , QPushButton::clicked , inputEditor , QPlainTextEdit::clear) ;
    connect(loadFileBtn , QPushButton::clicked ,[=]()
    {
        QString fName = QFileDialog::getOpenFileName(this , "",QDir::homePath()) ;
        if(fName.isEmpty()) return ;
        QFile fi(fName) ;
        if(!fi.open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox::information(this , tr("读取文件失败") ,
                                     tr("打开文件失败")) ;
            return ;
        }
        inputEditor->clear() ;
        QTextStream fis(&fi) ;
        fis.setCodec("utf8") ;
        while(!fis.atEnd())
        {
            QString line = fis.readLine() ;
            inputEditor->appendPlainText(line) ;
        }
    }) ;

    /** result display */
    // UI
    QGroupBox *displayBox = new QGroupBox("分词结果") ;
    QGridLayout *displayLayout = new QGridLayout() ;
    displayBox->setLayout(displayLayout) ;
    QPlainTextEdit *displayRstEditor = new QPlainTextEdit() ;
    displayRstEditor->setReadOnly(true) ;
    displayRstEditor->setWordWrapMode(QTextOption::NoWrap) ;
    //displayRstEditor->setTabStopWidth(40) ; // '\t' width
    displayRstEditor->setFont(QFont("Microsoft YaHei",10)) ;
    QCheckBox *isWordWrapBtn = new QCheckBox(tr("自动换行")) ;
    QPushButton *saveBtn = new QPushButton(tr("保存到文件")) ;
    displayLayout->addWidget(displayRstEditor , 0 , 0 , 5 , 6) ;
    displayLayout->addWidget(isWordWrapBtn , 5 , 0) ;
    displayLayout->addWidget(saveBtn , 5 , 1 ) ;
    testLayout->addWidget(displayBox , 12 , 0 , 6 , 6) ;

    // UI logic
    connect(isWordWrapBtn , static_cast<void(QCheckBox::*)(int)>(&QCheckBox::stateChanged) , [=](int state)
    {
        if(state == Qt::Unchecked)
        {
            displayRstEditor->setWordWrapMode(QTextOption::NoWrap) ;
        }
        else if(state == Qt::Checked)
        {
            displayRstEditor->setWordWrapMode(QTextOption::WordWrap) ;
        }
    }) ;
    connect(saveBtn , QPushButton::clicked , [=]()
    {
        QString fName = QFileDialog::getSaveFileName(this , "" , QDir::homePath() ,tr("Text files (*.txt);;Other files (*.*)") ) ;
        if(fName.isEmpty())
        {
            return ;
        }
        QFile fo(fName) ;
        if(!fo.open(QFile::WriteOnly | QFile::Text))
        {
            QMessageBox::information(this , tr("写入文件失败") ,
                                     tr("打开文件失败")) ;
            return ;
        }
        QTextStream fos(&fo) ;
        fos.setCodec("utf8") ;
        fos << displayRstEditor->toPlainText().replace(TAB_REPLACE_STR , "\t") ;
    }) ;

    // LTP_CWS predict process
    connect(cwsBtn , QPushButton::clicked , [=]()
    {
        bool isCustomMode = (predictMode == CustomPredictMode) ;
        QString basicModelPath = testPathEditBasicModel->text() ;
        QString customModelPath = testPathEditCustomModel->text() ;
        // check Model path
        bool checkModelPathState = checkReadPathValid(basicModelPath) ;
        if(isCustomMode){checkModelPathState = checkModelPathState && checkReadPathValid(customModelPath) ;}
        if(!checkModelPathState)
        {
            QMessageBox::information(this , tr("模型路径错误") ,
                                     tr("模型路径不存在！请重新设置路径然后重新点击按钮!")) ;
            return ;
        }
        bool saveState = config.savePredictConfig(isCustomMode,basicModelPath,customModelPath) ;
        if(!saveState)
        {
            QMessageBox::information(this , tr("内部错误") ,
                                     tr("配置文件保存失败")) ;
            return ;
        }
        displayRstEditor->clear() ;
        QString inputContent = inputEditor->toPlainText() ;
        saveState = config.savePredictInputContent(inputContent) ;
        if(!saveState)
        {
            QMessageBox::information(this , tr("内部错误") ,
                                     tr("输入内容转存文件错误")) ;
            return ;
        }

        // start Predict
        QProcess * predictProcess = new QProcess() ;
        connect(predictProcess , QProcess::started , [=]()
        {
            cwsBtn->setEnabled(false) ;
            for(const auto & radioBtn : modelConfBtnGroup->buttons()){radioBtn->setEnabled(false) ;}
        }) ;
        connect(predictProcess , static_cast<void(QProcess::*)(int , QProcess::ExitStatus)>(&QProcess::finished) ,
                [=](int exitCode , QProcess::ExitStatus status)
        {
            cwsBtn->setEnabled(true) ;
            for(const auto &radioBtn : modelConfBtnGroup->buttons()){radioBtn->setEnabled(true) ;}
            //qDebug() << exitCode << status ;
        }) ;
        connect(predictProcess , QProcess::readyReadStandardOutput , [=]()
        {
            QString readedCont = predictProcess->readAllStandardOutput() ;
            //displayRstEditor->appendPlainText(readedCont) ;
            readedCont.replace("\t",TAB_REPLACE_STR) ;
            displayRstEditor->insertPlainText(readedCont) ;
        }) ;
#ifdef DEBUG
        connect(predictProcess , QProcess::readyReadStandardError , [=]()
        {
            QString readedCont = predictProcess->readAllStandardError() ;
            qDebug() << readedCont ;
        }) ;
#endif
        QStringList params ;
        config.getPredictParams(isCustomMode , params) ;
#ifdef DEBUG
        qDebug() << params ;
#endif
        predictProcess->start(config.getCwsExePath() , params) ;

    });

    /*** Init State */
    predictMode = CustomPredictMode ;
    customModelRadio->setChecked(true) ;
    modelConfRadioBtnClickHandler(customModelRadio) ;
}

void Widget::createAboutWidget()
{
//    aboutWidget = new QWebView(this) ;
//    aboutWidget->load(QUrl("qrc:/res/html/about.htm")) ;
//    aboutWidget->show() ;
    aboutWidget = new QTextBrowser() ;
    aboutWidget->setReadOnly(true) ;
    aboutWidget->setOpenExternalLinks(true) ;
    aboutWidget->setStyleSheet("border:0") ;
    QFile fi(":/res/html/about.htm") ;
    if(fi.open(QFile::ReadOnly | QFile::Text))
    {
        QString content = fi.readAll() ;
        fi.close() ;
        aboutWidget->setHtml(content) ;
    }

}
void Widget::LoadWidgetStyle()
{
    QFile styleF(":/res/html/widget.css") ;
    if(styleF.open(QFile::ReadOnly|QFile::Text))
    {
        QString styleStr = styleF.readAll() ;
        setStyleSheet(styleStr) ;
        // qDebug() << styleStr ;
        styleF.close() ;
    }
    else
    {
        //qDebug() << "failed to load Style" ;
    }
}

void Widget::bindSwitchWidget()
{

    connect(trainWidgetSwitchBtn , SIGNAL(clicked()) , this , SLOT(switchWidget())) ;
    connect(testWidgetSwitchBtn , SIGNAL(clicked()) , this , SLOT(switchWidget())) ;
    connect(aboutWidgetSwitchBtn , SIGNAL(clicked()) , this , SLOT(switchWidget())) ;
}

void Widget::switchWidget()
{
    QPushButton *clickedBtn = static_cast<QPushButton *>(QObject::sender()) ;
    trainWidgetSwitchBtn->setFlat(true) ;
    testWidgetSwitchBtn->setFlat(true) ;
    aboutWidgetSwitchBtn->setFlat(true) ;
    clickedBtn->setFlat(false) ;
    if(clickedBtn == trainWidgetSwitchBtn)
    {
        stackedWidget->setCurrentWidget(trainWidget) ;
    }
    else if(clickedBtn == testWidgetSwitchBtn)
    {
        stackedWidget->setCurrentWidget(testWidget) ;
    }
    else if(clickedBtn == aboutWidgetSwitchBtn)
    {
        stackedWidget->setCurrentWidget(aboutWidget) ;
    }
}
QGridLayout* Widget::createPathSelectView(QString  label , QLineEdit *& edit,bool isOpenFile)
{
    QGridLayout *layout = new QGridLayout ;
    QLabel *labelWidget = new QLabel(label) ;
    edit = new QLineEdit() ;
    edit->setFont(QFont("consolas" , 9)) ;
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

void Widget::preSetAllTrainPathSelectViews()
{
    QString trainingSetPath , devingSetPath , basicModelPath ,
            maxIte , modelSavingPath ;
    bool isCustomMode = ( trainMode == CustomTrainMode ) ;
    bool loadStatus = config.loadTrainConfig(isCustomMode , trainingSetPath , devingSetPath , modelSavingPath ,
                           maxIte , basicModelPath) ;
    if(loadStatus == true)
    {
        trainPathEditTrain->setText(trainingSetPath) ;
        trainPathEditDev->setText(devingSetPath) ;
        trainPathEditModelSaving->setText(modelSavingPath) ;
        trainMaxIte->setText(maxIte) ;
        if(isCustomMode)
        {
            trainPathEditBasicModel->setText(basicModelPath) ;
        }
        else {trainPathEditBasicModel->clear() ;}
    }
}
void ::Widget::preSetAllPredictPathSelectViews()
{
    QString basicModelPath , customModelPath ;
    bool isCustomMode = (predictMode == CustomPredictMode) ;
    bool loadState = config.loadPredictConfig(isCustomMode , basicModelPath , customModelPath) ;
    if(loadState == true)
    {
        testPathEditBasicModel->setText(basicModelPath) ;
        if(isCustomMode){testPathEditCustomModel->setText(customModelPath) ;}
        else {testPathEditCustomModel->clear() ;}
    }
}

void Widget::setLayoutItemsEnabled(QLayout *layout,bool enable)
{
    for(int i = 0 ; i < layout->count() ; ++i)
    {
        layout->itemAt(i)->widget()->setEnabled(enable) ;
    }
}
bool Widget::checkReadPathValid(QString path)
{
    return QFile::exists(path) ;
}

bool Widget::checkWritePathValid(QString path)
{
    QFile fo(path) ;
    if(fo.open(QFile::WriteOnly))
    {
        fo.close() ;
        (new QDir())->remove(path) ;

        return true ;
    }
    else { return false ;}
}

Widget::~Widget()
{

}
