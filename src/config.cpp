#include "config.h"

Config::Config()
{
    QString exePath = QCoreApplication::applicationDirPath() ; /* This can ensure the path as the exe path*/
    //QString exePath = QDir::currentPath() ;

    // Config init
    QString dirName = "LTPCWS_conf" ;
    //confBaseDir = QDir(exePath + "/" + dirName) ; // Always use '/' as separator
    confBaseDir =  QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + dirName ;
    if(!confBaseDir.exists())
    {
        confBaseDir.mkpath(confBaseDir.absolutePath()) ;
        //qDebug() << baseDir.absolutePath() ;
    }
    basicTrainConfPath = confBaseDir.absolutePath() + "/" + "basic_train_conf.conf" ;
    basicTestConfPath = confBaseDir.absolutePath() + "/" + "basic_test_conf.conf" ;
    customTrainConfPath = confBaseDir.absolutePath() + "/" + "custom_train_conf.conf" ;
    customTestConfPath = confBaseDir.absolutePath() + "/" + "custom_test_conf.conf" ;
    predictInputTmpFilePath = confBaseDir.absolutePath() + "/" + "predictinput.tmp" ;
    // ltp-cws Exe Path Conf

    QString cwsExeDir = exePath + "/" + "cws_bin" ;
    getPlatform(platform) ;
    if(platform == P_WIN32)
    {
        cwsExePath = cwsExeDir + "/win32/" + "otcws.exe" ;
    }
    else if(platform == P_LINUX32)
    {
        cwsExePath = cwsExeDir + "/linux32/" + "otcws" ;
    }
    else if(platform == P_OSX)
    {
        cwsExePath = cwsExeDir + "/osx/" + "otcws" ;
    }
    else
    {
        cwsExePath = cwsExeDir + "/others/" + "otcws" ;
    }
    hasRightCwsExe = QFile::exists(cwsExePath) ;

#ifdef DEBUG
    qDebug() << (hasRightCwsExe ? QObject::tr("CWS程序准备就绪\n") : QObject::tr("CWS未找到\n"))
             << cwsExePath <<"\n"
             << platform <<"\n" ;
#endif
    basicModeTrainIntro = QObject::tr("本模块用于训练基础模型（推荐使用上述LTP分词模型作为基础模型）。\n"
                                        "选择相应的路径，点击训练按钮即开始训练。"
                                        "训练一般耗时较长，请耐心等待。" );
    customModeTrainIntro = QObject::tr("本模块用于基础模型的基础上训练用户自定义的个性化模型。\n"
                                  "选择相应的路径，"
                                  "点击训练按钮即开始训练。训练一般耗时较长，请耐心等待。") ;

    basicModePredictIntro = QObject::tr("使用基础模型对输入本文做分词处理") ;
    customModePredictIntro = QObject::tr("使用个性化模型对输入文本做分词处理") ;
}

bool Config::saveTrainConfig(bool isCustomMode ,QString trainingSetPath ,QString devingSetPath ,
                             QString modelSavingPath , QString max_ite ,QString basicModelPath)
{
   QString currentTrainConf = isCustomMode ? customTrainConfPath : basicTrainConfPath ;
   QFile trainF(currentTrainConf) ;
   if(!trainF.open(QIODevice::WriteOnly | QIODevice::Text))
   {
       //qDebug() << "训练配置文件写入失败——找不到训练配置文件" ;
       return false ;
   }
   QTextStream out(&trainF) ;
   //out.setCodec("utf8") ; /// this will cause the limit that  can't read Chinese in Windows where default code is GB18030
   out << "[train]" << "\n"
          << "reference = " << trainingSetPath <<"\n"
          << "development = " << devingSetPath <<"\n"
          << "algorithm = pa" <<"\n"
          << "max-iter = " << max_ite <<"\n"
          << "rare-feature-threshold = 0" <<"\n"
          << "model = " << modelSavingPath << "\n" ;
   if(isCustomMode)
   {
       out << "baseline-model = " << basicModelPath << "\n" ;
   }
   return true ;
}
bool Config::loadTrainConfig(bool isCustomMode ,QString & trainingSetPath ,QString & devingSetPath ,
                             QString & modelSavingPath , QString & max_ite ,QString & basicModelPath)
{
    QString trainConf = isCustomMode ? customTrainConfPath : basicTrainConfPath ;
    QFile rf(trainConf) ;
    if(!rf.open(QFile::ReadOnly|QFile::Text))
    {
        return false ;
    }
    QTextStream in(&rf) ;
    //in.setCodec("utf8") ;
    while(!in.atEnd())
    {
        QString line = in.readLine().trimmed() ;
        QStringList parts = line.split(QRegExp("\\s+=\\s+")) ;
        if(parts.length() != 2) continue ;
        QString key = parts[0] ;
        QString val = parts[1] ;
        if(key == "reference") trainingSetPath = val ;
        else if(key == "development") devingSetPath = val ;
        else if(key == "max-iter") max_ite = val ;
        else if(key == "baseline-model") basicModelPath = val ;
        else if(key == "model") {modelSavingPath = val ;}
    }
    if(! isCustomMode) basicModelPath = "" ;
    return true ;
}
bool Config::savePredictInputContent(QString &content)
{
    QFile fo(predictInputTmpFilePath) ;
    if(!fo.open(QFile::WriteOnly | QFile::Text))
    {
        return false ;
    }
    QTextStream fos(&fo) ;
    fos.setCodec("utf8") ;
    fos << content ;
    return true ;
}

bool Config::savePredictConfig(bool isCustomMode ,QString basicModelPath ,
                               QString customModelPath , QString lexiconPath)
{
    QString currentPredictConf = isCustomMode ? customTestConfPath : basicTestConfPath ;
    QFile fo(currentPredictConf) ;
    if(!fo.open(QFile::WriteOnly | QFile::Text))
    {
        return false ;
    }
    QTextStream fos(&fo) ;
    //fos.setCodec("utf8") ;
    fos <<"[test]" <<"\n" ;
    fos <<"lexicon = " << lexiconPath ;
    if(isCustomMode)
    {
       fos <<"model = " << customModelPath << "\n"
           <<"baseline-model = " << basicModelPath <<"\n" ;
    }
    else
    {
       fos <<"model = " <<basicModelPath <<"\n" ;
    }
    return true ;
}
bool Config::loadPredictConfig(bool isCustomMode , QString &basicModelPath , QString &customModelPath ,
                               QString &lexiconPath)
{
    QString currentConf = isCustomMode ? customTestConfPath : basicTestConfPath ;
    QFile fi(currentConf) ;
    if(!fi.open(QFile::ReadOnly | QFile::Text))
    {
        return false ;
    }
    QTextStream fis(&fi) ;
    //fis.setCodec("utf8") ;
    while(!fis.atEnd())
    {
        QString line = fis.readLine().trimmed() ;
        QStringList parts = line.split(QRegExp("\\s+=\\s+")) ;
        if(parts.length() != 2) continue ;
        QString key = parts[0] ;
        QString val = parts[1] ;
        if(key == "model")
        {
            basicModelPath = val ;
            customModelPath = "" ;
            if(isCustomMode) customModelPath = val ;
            else { basicModelPath = val ; customModelPath = "" ;}
        }
        else if(key == "baseline-model"){ basicModelPath = val ;}
        else if(key == "lexicon") {lexiconPath = val ;}
    }
    return true ;
}

void Config::getPlatform(Platform &curPlatform)
{
#if defined(Q_OS_WIN)
    curPlatform = P_WIN32 ;
#elif defined(Q_OS_LINUX)
    curPlatform = P_LINUX32 ;
#elif defined(Q_OS_MAC)
    curPlatform = P_OSX ;
#else
    curPlatform = P_OTHERS ;
#endif
}
bool Config::getCwsExeState()
{
    return hasRightCwsExe ;
}

bool Config::getTrainParams(bool isCustomMode , QStringList &params)
{
    QString reference , development , modelSavingPath , max_ite , basicModelPath ;
    bool loadState ;
    loadState = loadTrainConfig(isCustomMode , reference , development ,
                                modelSavingPath , max_ite , basicModelPath) ;
    if(isCustomMode)
    {
        params <<"customized-learn"
               << "--baseline-model" << basicModelPath ;
    }
    else
    {
        params << "learn" ;

    }
    params << "--model" << modelSavingPath
           << "--reference" << reference
           << "--development" << development
           << "--max-iter" << max_ite
           << "--rare-feature-threshold" <<  0
           << "--algorithm" << "ap" ;

    return loadState ;
}

bool Config::getPredictParams(bool isCustomMode , QStringList &params)
{
    QString basicModelPath , customModelPath , lexiconPath ;
    bool loadState ;
    loadState = loadPredictConfig(isCustomMode , basicModelPath , customModelPath , lexiconPath) ;
    if(isCustomMode)
    {
        params << "customized-test"
               << "--model" << customModelPath
               << "--baseline-model" << basicModelPath ;
    }
    else
    {
        params << "test"
               << "--model" << basicModelPath ;
    }
    params << "--input" << predictInputTmpFilePath ;
    // check wheather lexicon path is valid
    if( lexiconPath != "" && QFile::exists(lexiconPath))
    {
        params << "--lexicon" << lexiconPath ;
    }
    return loadState ;
}

QString Config::getCwsExePath()
{
    return cwsExePath ;
}

QString Config::getConfInfo()
{
    QStringList confPaths ;
    confPaths << basicTrainConfPath << customTrainConfPath << basicTestConfPath << customTestConfPath ;
    return confPaths.join("\n") ;
}
