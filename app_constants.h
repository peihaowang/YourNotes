#ifndef APP_CONSTANTS_H
#define APP_CONSTANTS_H

//essential macros for the app;
#define		_APP_NAME				"YourNotes"
#define		_APP_TITLE				"YourNotes Desktop"
#define		_APP_VER_MAIN				1
#define		_APP_VER_MINOR				0
#define		_APP_VER_REVISION			0
#define		_APP_VER_STATE				(QString("B")+"e"+"t"+"a"+"-"+"0"+"1") //"Beta-01"

#define		_APP_VER_STR_BASE			QString("%1.%2.%3").arg(_APP_VER_MAIN).arg(_APP_VER_MINOR).arg(_APP_VER_REVISION)
#define		_APP_VER_STR_SHORT			QString("%1").arg(_APP_VER_STR_BASE)
#define		_APP_VER_STR_FULL			QString("%1%2").arg(_APP_VER_STR_SHORT).arg(QString(_APP_VER_STATE).isEmpty() ? "" : " " + QString(_APP_VER_STATE))

#define		_APP_TITLE_FULL				QString("%1 %2").arg(_APP_TITLE).arg(_APP_VER_STR_FULL)
#define		_APP_COPYRIGHT_NOTICE			QString("Copyright %1 Peihao Wang. All Rights Reserved.").arg((QDate::currentDate().year() == 2018) ? QString::number(2018) : QString("2018 - %1").arg(QDate::currentDate().year()))

//website URLs;
#define		_APP_URL_WEBSITE			"https://github.com/peihaowang"
#define		_APP_URL_CGI				"mailto:wangpeihao@gmail.com"

//note storage specific constants;
#define		_STG_FILE_DEFNOTE_HTML			"_~_~_note.html"
#define		_STG_FILE_DEFNOTE_ATTR			"_~_~_attr.ini"

//note storage specific file extension names;
#define		_STG_EXT_CONFIG				".ini"
#define		_STG_EXT_BACKUP				".bak"
#define		_STG_EXT_REMOVED			".remove"

//internal data

//tags for OS platform;
#if defined(Q_OS_WIN32)
#define		_APP_OS_PLATFORM			"Win32"
#elif defined(Q_OS_MAC)
#define		_APP_OS_PLATFORM			"MacOSX"
#elif defined(Q_OS_LINUX) && defined(amd64)
#define		_APP_OS_PLATFORM			"Linux_amd64"
#elif defined(Q_OS_LINUX) && defined(i386)
#define		_APP_OS_PLATFORM			"Linux_i386"
#elif defined(Q_OS_LINUX) && defined(__arm__)
#define		_APP_OS_PLATFORM			"Linux_arm"
#endif

#endif // APP_CONSTANTS_H
