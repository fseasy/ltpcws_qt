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
    if(!confBaseDir.exists(cwsExeDir))
    {
        hasRightCwsExe = false ;
    }
    else
    {
        hasRightCwsExe = true ;
    }
    getPlatform(platform) ;
    if(platform == P_WIN32)
    {
        basicCwsExePath = cwsExeDir + "/win32/" + "otcws.exe" ;
        customCwsExePath = cwsExeDir + "/win32/" + "otcws-customized.exe" ;
    }
    else if(platform == P_LINUX32)
    {
        basicCwsExePath = cwsExeDir + "/linux32/" + "otcws" ;
        customCwsExePath = cwsExeDir + "/linux32/" + "otcws-customized" ;
    }
    else if(platform == P_OSX)
    {
        basicCwsExePath = cwsExeDir + "/osx/" + "otcws" ;
        customCwsExePath= cwsExeDir + "/osx/" + "otcws-customized" ;
    }
    else
    {
        basicCwsExePath = cwsExeDir + "/others/" + "otcws" ;
        customCwsExePath = cwsExeDir + "/others/" + "otcws-customized" ;
    }
    hasRightCwsExe = QFile::exists(basicCwsExePath) && QFile::exists(customCwsExePath) ;

//    qDebug() << (hasRightCwsExe ? QObject::tr("CWS程序准备就绪\n") : QObject::tr("CWS未找到\n"))
//             << basicCwsExePath <<"\n"
//             << customCwsExePath <<"\n"
//             << platform <<"\n" ;

    basicModeTrainIntro = QObject::tr("本模块用于训练基础模型（推荐使用上述LTP分词模型作为基础模型）。\n"
                                        "选择相应的路径，点击训练按钮即开始训练。"
                                        "训练一般耗时较长，请耐心等待。" );
    customModeTrainIntro = QObject::tr("本模块用于基础模型的基础上训练用户自定义的个性化模型。\n"
                                  "选择相应的路径，"
                                  "点击训练按钮即开始训练。训练一般耗时较长，请耐心等待。") ;

    basicModePredictIntro = QObject::tr("使用基础模型对输入本文做分词处理") ;
    customModePredictIntro = QObject::tr("使用个性化模型对输入文本做分词处理") ;
}

bool Config::saveTrainConfigAndSetState(bool isCustomMode ,QString trainingSetPath ,QString devingSetPath ,
                             QString modelSavingPath , QString max_ite ,QString basicModelPath)
{
   currentTrainConf = isCustomMode ? customTrainConfPath : basicTrainConfPath ;
   QFile trainF(currentTrainConf) ;
   if(!trainF.open(QIODevice::WriteOnly | QIODevice::Text))
   {
       //qDebug() << "训练配置文件写入失败——找不到训练配置文件" ;
       return false ;
   }
   QTextStream out(&trainF) ;
   //out.setCodec("utf8") ; /// this will cause the limit that  can't read Chinese in Windows where default code is GB18030
   out << "[train]" << "\n"
          << "train-file = " << trainingSetPath <<"\n"
          << "holdout-file = " << devingSetPath <<"\n"
          << "algorithm = pa" <<"\n"
          << "enable-incremental-training = 1" <<"\n"
          << "max-iter = " << max_ite <<"\n"
          << "rare-feature-threshold = 0" <<"\n" ;
   if(isCustomMode)
   {
       out << "baseline-model-file = " << basicModelPath << "\n"
           << "customized-model-name = " << modelSavingPath <<"\n" ;
   }
   else
   {
       out << "model-name = " << modelSavingPath << "\n" ;
   }
   currentCwsExePath = isCustomMode ? customCwsExePath : basicCwsExePath ;
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
        if(key == "train-file") trainingSetPath = val ;
        else if(key == "holdout-file") devingSetPath = val ;
        else if(key == "max-iter") max_ite = val ;
        else if(key == "baseline-model-file") basicModelPath = val ;
        else if(key == "customized-model-name") modelSavingPath = val ;
        else if(key == "model-name") {modelSavingPath = val ; basicModelPath = "" ;} // Means it is basicTrainMode
    }
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

bool Config::savePredictConfigAndSetState(bool isCustomMode ,QString basicModelPath ,
                                          QString customModelPath)
{
    currentPredictConf = isCustomMode ? customTestConfPath : basicTestConfPath ;
    QFile fo(currentPredictConf) ;
    if(!fo.open(QFile::WriteOnly | QFile::Text))
    {
        return false ;
    }
    QTextStream fos(&fo) ;
    //fos.setCodec("utf8") ;
    fos <<"[test]" <<"\n"
        <<"test-file = " << predictInputTmpFilePath <<"\n" ;
    if(isCustomMode)
    {
       fos <<"customized-model-file = " << customModelPath << "\n"
           <<"baseline-model-file = " << basicModelPath <<"\n" ;
    }
    else
    {
       fos <<"model-file = " <<basicModelPath <<"\n" ;
    }
    currentCwsExePath = isCustomMode ? customCwsExePath :  basicCwsExePath ;
    return true ;
}
bool Config::loadPredictConfig(bool isCustomMode , QString &basicModelPath , QString &customModelPath)
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
        if(key == "model-file")
        {
            basicModelPath = val ;
            customModelPath = "" ;
        }
        else if(key == "customized-model-file"){ customModelPath = val ;}
        else if(key == "baseline-model-file"){ basicModelPath = val ;}
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

QString Config::getCurrentTrainConf()
{
    return currentTrainConf ;
}

QString Config::getCurrentPredictConf()
{
    return currentPredictConf ;
}

QString Config::getCurrentCwsExePath()
{
    return currentCwsExePath ;
}

QString Config::getConfInfo()
{
    QStringList confPaths ;
    confPaths << basicTrainConfPath << customTrainConfPath << basicTestConfPath << customTestConfPath ;
    return confPaths.join("\n") ;
}
