#include "config.h"

Config::Config()
{
    QString exePath = QCoreApplication::applicationDirPath() ; /* This can ensure the path as the exe path*/
    //QString exePath = QDir::currentPath() ;

    // Config init
    QString dirName = "conf" ;
    baseDir = QDir(exePath + "/" + dirName) ; // Always use '/' as separator
    if(!baseDir.exists())
    {
        baseDir.mkpath(baseDir.absolutePath()) ;
        qDebug() << baseDir.absolutePath() ;
    }
    basicTrainConfPath = baseDir.absolutePath() + "/" + "basic_train_conf.conf" ;
    basicTestConfPath = baseDir.absolutePath() + "/" + "basic_test_conf.conf" ;
    customTrainConfPath = baseDir.absolutePath() + "/" + "custom_train_conf.conf" ;
    customTestConfPath = baseDir.absolutePath() + "/" + "custom_test_conf.conf" ;

    // ltp-cws Exe Path Conf
    QString cwsExeDir = exePath + "/" + "cws_bin" ;
    if(!baseDir.exists(cwsExeDir))
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

    qDebug() << (hasRightCwsExe ? QObject::tr("CWS程序准备就绪\n") : QObject::tr("CWS未找到\n"))
             << basicCwsExePath <<"\n"
             << customCwsExePath <<"\n"
             << platform <<"\n" ;

    basicModeIntro = QObject::tr("本模块用于训练基础模型（推荐使用上述LTP分词模型作为基础模型）。\n"
                                        "选择相应的训练集语料、开发集语料以及模型保存路径，点击训练按钮即开始训练。"
                                        "训练一般耗时较长，请耐心等待。生成的模型保存在模型保存路径指定的位置。" );
    customModeIntro = QObject::tr("本模块用于基础模型的基础上训练用户自定义的个性化模型。\n"
                                  "选择相应的训练集语料、开发集语料以及基础模型路径，设置个性化模型保存位置，"
                                  "点击训练按钮即开始训练。训练一般耗时较长，请耐心等待。"
                                  "生成的模型保存在个性化模型保存路径指定的位置。") ;
}

bool Config::saveTrainConfigAndSetState(bool isCustomMode ,QString trainingSetPath ,QString devingSetPath ,
                             QString modelSavingPath , QString max_ite ,QString basicModelPath)
{
   currentTrainConf = isCustomMode ? customTrainConfPath : basicTrainConfPath ;
   QFile trainF(currentTrainConf) ;
   if(!trainF.open(QIODevice::WriteOnly | QIODevice::Text))
   {
       qDebug() << "训练配置文件写入失败——找不到训练配置文件" ;
       return false ;
   }
   QTextStream out(&trainF) ;
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
QString Config::getCurrentCwsExePath()
{
    return currentCwsExePath ;
}
