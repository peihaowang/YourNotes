
#include "common_headers.h"

#include "NoteStg.h"

#if defined(Q_OS_UNIX)
#include <utime.h> //2014.5.3 for utimbuf
#endif

////////////////////////////////////////////////////////////

extern void iniGet(const _CKeyValEntries& xKV, const QString& sKey, QColor& xRes, const QColor& xDef);
extern void iniPut(_CKeyValEntries& xKV, const QString& sKey, const QColor& xVal);

///////////////////////////////////////////////////////////

bool _CNoteStg::createStg(const QString& sPath)
{
	return QDir::root().mkdir(sPath);
}

bool _CNoteStg::openStg(const QString& sPath)
{
	bool bSucc = false;
	if(QFileInfo(sPath).exists()){
		m_xOpenedFld = QDir(sPath);
		m_bOpened = true;
	}
	return bSucc;
}

bool _CNoteStg::createFolder(const QString& sName) const
{
	bool bSucc = false;
	if(isOpen()){
		bSucc = m_xOpenedFld.mkdir(sName);
	}
	return bSucc;
}

bool _CNoteStg::removeNote(const QString& sName)
{
	bool bSucc = false;
	if(isOpen()){
		QDir(_CLocalFile::concatenate(m_xOpenedFld.path(), sName)).removeRecursively();
		bSucc = !exists(sName);
	}
	return bSucc;
}

bool _CNoteStg::trashNote(const QString& sName) const
{
	bool bSucc = false;
	if(isOpen() && !isTrash(sName)){
		//2018.1.16 Create a file to mark the folder is removed.
		//Considering renaming cause problems on create/untrash notes, use mark file instead.
		QString sFnMarkFile = _CLocalFile::changeExtension(_CLocalFile::concatenate(m_xOpenedFld.path(), sName), _STG_EXT_REMOVED);
		QFile f(sFnMarkFile);
		if(f.open(QIODevice::ReadWrite | QIODevice::Truncate)){
			f.close();
			bSucc = true;
		}
	}
	return bSucc;
}

bool _CNoteStg::untrashNote(const QString& sName)
{
	bool bSucc = false;
	if(isOpen() && isTrash(sName)){
		QString sFnMarkFile = _CLocalFile::changeExtension(sName, _STG_EXT_REMOVED);
		bSucc = m_xOpenedFld.remove(sFnMarkFile);
	}
	return bSucc;
}

bool _CNoteStg::isTrash(const QString& sName) const
{
	bool bTash = false;
	if(isOpen()){
		QString sFnMarkFile = _CLocalFile::changeExtension(sName, _STG_EXT_REMOVED);
		bTash = exists(sFnMarkFile);
	}
	return bTash;
}

bool _CNoteStg::exists(const QString& sName) const
{
	return isOpen() && m_xOpenedFld.exists(sName);
}

void _CNoteStg::listNotes(std::vector<QString>& v, bool bTrash) const
{
	if(isOpen()){
		QStringList vFlds = m_xOpenedFld.entryList(QDir::Dirs, QDir::NoSort);
		Q_FOREACH(QString sName, vFlds){
			if(sName == "." || sName == "..") continue;
			if(bTrash || !isTrash(sName)){
				v.push_back(sName);
			}
		}
	}
}

QString _CNoteStg::getUniqueNoteName(long nInit) const
{
	QString sNewName;
	if(isOpen()){
		long n=nInit;
		do{
			sNewName = QString::number(n++);
		}while(exists(sNewName) && n < 0x7fffFFFF); //2G limit;
		if(n >= 0x7fffFFFF) sNewName = "";
	}
	return sNewName;
}

bool _CNoteStg::renameNote(const QString& sName, const QString& sNewName)
{
	bool bSucc = false;
	if(isOpen()){
		bSucc = m_xOpenedFld.rename(sName, sNewName);
	}
	return bSucc;
}

QDateTime _CNoteStg::createdTime(const QString& sName) const
{
	QString sPath = _CLocalFile::concatenate(m_xOpenedFld.path(), sName);
	return QFileInfo(sPath).created();
}


QDateTime _CNoteStg::modifiedTime(const QString& sName) const
{
	QString sPath = _CLocalFile::concatenate(m_xOpenedFld.path(), sName);
	return QFileInfo(sPath).lastModified();
}

///////////////////////////////////////////////////////////

bool _CNoteStg::loadNote(const QString& sName, _CNoteStg::_CNote& xNote) const
{
	bool bSucc = false;
	if(isOpen() && exists(sName)){
		QString sParent = _CLocalFile::concatenate(m_xOpenedFld.path(), sName);
		QString sFnAttr = _CLocalFile::concatenate(sParent, _STG_FILE_DEFNOTE_ATTR);
		_CKeyValEntries xKVs(_CTextFile::load(sFnAttr, ""));

		iniGet(xKVs, "Properties.Title", xNote.m_sTitle, QString(""));
		iniGet(xKVs, "Properties.ReadOnly", xNote.m_bReadOnly, bool(false));

		iniGet(xKVs, "Position.X", xNote.m_nPosX, int(-1));
		iniGet(xKVs, "Position.Y", xNote.m_nPosY, int(-1));

		iniGet(xKVs, "Size.Width", xNote.m_nWidth, int(-1));
		iniGet(xKVs, "Size.Height", xNote.m_nHeight, int(-1));

		iniGet(xKVs, "Color.Page", xNote.m_clPage, QColor(252, 250, 177));
		iniGet(xKVs, "Reminder.Timestamp", xNote.m_tReminder, time64_t(0));

		QString sFnContent = _CLocalFile::concatenate(sParent, _STG_FILE_DEFNOTE_HTML);
		xNote.m_sContent = _CTextFile::load(sFnContent, "");

		bSucc = true;
	}
	return bSucc;
}

bool _CNoteStg::saveNote(const QString& sName, const _CNoteStg::_CNote& xNote) const
{
	bool bSucc = false;
	if(isOpen() && exists(sName)){
		QString sParent = _CLocalFile::concatenate(m_xOpenedFld.path(), sName);
		QString sFnAttr = _CLocalFile::concatenate(sParent, _STG_FILE_DEFNOTE_ATTR);
		_CKeyValEntries xKVs;

		iniPut(xKVs, "Properties.Title", xNote.m_sTitle);
		iniPut(xKVs, "Properties.ReadOnly", xNote.m_bReadOnly);

		iniPut(xKVs, "Position.X", xNote.m_nPosX);
		iniPut(xKVs, "Position.Y", xNote.m_nPosY);

		iniPut(xKVs, "Size.Width", xNote.m_nWidth);
		iniPut(xKVs, "Size.Height", xNote.m_nHeight);

		iniPut(xKVs, "Color.Page", xNote.m_clPage);
		iniPut(xKVs, "Reminder.Timestamp", xNote.m_tReminder);

		qint64 nAttrLen = _CTextFile::saveUtf8(sFnAttr, (QString)xKVs, true);

		QString sFnContent = _CLocalFile::concatenate(sParent, _STG_FILE_DEFNOTE_HTML);
		qint64 nContentLen = _CTextFile::saveUtf8(sFnContent, xNote.m_sContent, true);

		bSucc = (nAttrLen >= 0 && nContentLen >= 0);
	}
	return bSucc;
}
