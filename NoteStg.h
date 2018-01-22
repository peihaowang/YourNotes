#ifndef DBITEM_H
#define DBITEM_H

#include <QRgb>
#include <QFile>
#include <QDir>
#include <QString>
#include <QStringList>

#include "platform_specific.h"
#include "cpp_utilities.h"

///////////////////////////////////////////////////////////

class _CNoteStg{

private:

	QDir				m_xOpenedFld;
	bool				m_bDirty;
	bool				m_bOpened;

protected:

//	int64_t importFileFromMem(const QString& sSsgFn, const void* pBuf, int64_t nLen, bool bRecycle) const;
//	int64_t importFile(const QString& sSsgFn, const QString& sWinFn, bool bRecycle) const;

//	int64_t exportFileToMem(const QString& sSsgFn, void* pBuf, int64_t nLen) const;
//	int64_t exportFile(const QString& sSsgFn, const QString& sWinFn) const;

//	//int64_t catchFileToMem(const QString& sSsgFn, QByteArray& vBuf) const;
//	//int64_t catchFile(const QString& sSsgFn, const QString& sWinFn) const;

//	QString loadText(const QString& sSsgFn, const QString& sCodec="")const;
//	int64_t saveAnsi(const QString& sSsgFn, const QString& sTxt, bool bRecycle) const;
//	int64_t saveUtf8(const QString& sSsgFn, const QString& sTxt, bool bRecycle, bool bBOM) const;

public:

	virtual ~_CNoteStg(void){return;}
	_CNoteStg(void){return;}

	QString folderPath(void) const {return m_xOpenedFld.path();}

	bool createStg(const QString& sPath);
	bool openStg(const QString& sPath);
	bool closeStg(){
		bool bSucc = false;
		if(m_bOpened){
			m_xOpenedFld = QDir();
			m_bOpened = false;
			bSucc = true;
		}
		return bSucc;
	}
	bool isOpen() const {return m_bOpened;}

	bool createFolder(const QString& sName) const;
	bool removeNote(const QString& sName);
	bool trashNote(const QString& sName) const;
	bool untrashNote(const QString& sName);

	void listNotes(std::vector<QString>& v, bool bTrash) const;

	QDateTime createdTime(const QString& sName) const;
	QDateTime modifiedTime(const QString& sName) const;

	bool exists(const QString& sName) const;
	bool isTrash(const QString& sName) const;

	bool renameNote(const QString& sName, const QString& sNewName);

	QString getUniqueNoteName(long nInit)const;


public:

	enum _TAppDataPos{
		AppData_PosX = 0
		, AppData_PosY
		, AppData_Width
		, AppData_Height
		, AppData_Color
		, AppData_ReadOnly
		, AppData_Reminder
	};

	struct _CNote{
		int		m_nPosX;
		int		m_nPosY;
		int		m_nWidth;
		int		m_nHeight;
		QColor		m_clPage;
		bool		m_bReadOnly;
		QString		m_sTitle;
		QString		m_sContent;
		time64_t	m_tReminder;

		_CNote()
			: m_nPosX(-1)
			, m_nPosY(-1)
			, m_nWidth(-1)
			, m_nHeight(-1)
			, m_clPage(252, 250, 177)
			, m_bReadOnly(false)
			, m_tReminder(0)
		{
			return;
		}
	};

	bool loadNote(const QString& sName, _CNote& xNote) const;
	bool saveNote(const QString& sName, const _CNote& xNote) const;

};

#endif // DBITEM_H
