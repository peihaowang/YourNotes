#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <ctime>
#include <map>
#include <iterator>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QAction>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QByteArray>
#include <QMap>

#include "app_constants.h"
#include "platform_specific.h"
#include "cpp_utilities.h"
#include "string_utilities.h"
#include "file_utilities.h"

///////////////////////////////////////////////////////////

class QAbstractButton;
class _CAppSettings{

public:

	QFont							m_ftDefFont;

	_CPairVector<QString, QColor>				m_vPageColors;
	_CPairVector<QString, QColor>				m_vColorsList;

	QFont							m_ftUIFont;

	QString							m_sUILanguage;
	bool							m_bAutoDetectSysLanguage;

	QFont							m_ftDefNoteFont;
	QColor							m_clDefNoteForeColor;
	QColor							m_clDefNoteBackColor;

	enum _TReminderSound{RS_Mute, RS_Chimes, RS_Ding};
	uint							m_nReminder_RingSound;
	QMap<uint, QString>					m_mReminderSounds;
	int							m_nReminder_RingInterval;	//in msecs
	int							m_nReminder_RingDuration;	//in msecs
	int							m_nReminder_ShockFactor;

protected:

	_CKeyValEntries						m_mLocale;

public:

	_CAppSettings(void);

protected:

	void _init(void);

	void _ddx(_CKeyValEntries& xKV, bool bGet);

	void _validate_settings(void);

public:

	void loadLangIni(void);
	void load(const QString& sIniFn);
	void saveIni(const QString& sIniFn);

public:

	QString msgOf(const QString& sKey, const QString& sDef)const;
	QString capOf(const QString& sKey, const QString& sDef)const;
	QString hintOf(const QString& sKey, const QString& sDef) const;

};

///////////////////////////////////////////////////////////

extern _CAppSettings g_xOpt;

///////////////////////////////////////////////////////////

#define		_lc(key, def)		g_xOpt.msgOf(key, def)
#define		_cap(key, def)		g_xOpt.capOf(key, def)
#define		_hint(key, def)		g_xOpt.hintOf(key, def)

///////////////////////////////////////////////////////////

#define		_TEMPDIR		g_xOpt.getPathToTempFiles()
#define		_TEMPFILE(sExt)		_CTemporaryFile(_TEMPDIR)._getTempFile("", sExt)

///////////////////////////////////////////////////////////

#endif // APP_SETTINGS_H
