#ifndef MISC_UTILITIES_H
#define MISC_UTILITIES_H

#include <ctime>
#include <math.h>

#include <QDebug>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QFileIconProvider>
#include <QDateTime>
#include <QFontDialog>
#include <QColorDialog>
#include <QDesktopWidget>
#include <QTreeWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QLayout>
#include <QLayoutItem>
#include <QApplication>

#include <QtXml/QDomDocument>
#include <QtXml/QDomNodeList>
#include <QtXml/QDomElement>

#include "type_defs.h"
#include "string_utilities.h"
#include "cpp_utilities.h"

///////////////////////////////////////////////////////////

#define	_CHAR_SLASH		'/'
#define	_CHAR_BACKSLASH		'\\'
#define	_CHAR_DOT		'.'

#define	_TEXT_SLASH		QString(_CHAR_SLASH)
#define	_TEXT_BACKSLASH		QString(_CHAR_BACKSLASH)
#define	_TEXT_DOT		QString(_CHAR_DOT)

///////////////////////////////////////////////////////////

#define		__ASSERT(x)	//Q_ASSERT(x)

///////////////////////////////////////////////////////////

template<typename T> class _CPredQVector : public QVector<T>{

public:

	virtual void operator()(const T& t){this->push_back(t);}

};

template<typename T> class _CPredQList : public QList<T>{

public:

	virtual void operator()(const T& t){this->push_back(t);}

};

template<typename T> class _CPredQSet : public QSet<T>{

public:

	virtual void operator()(const T& t){this->insert(t);}

};

///////////////////////////////////////////////////////////

class _CTemporaryFile{

private:

	QString		 m_sTmpDir;

public:

	_CTemporaryFile(void) : m_sTmpDir(QDir::tempPath()){return;}
	_CTemporaryFile(const QString& sTmpDir)
		: m_sTmpDir(sTmpDir)
	{
		return;
	}

	QString _getTempFile(const QString& sPrefix0="", const QString& sSuffix0="")
	{
		//2014.9.13 allows to customize the prefix as well;
		QString sTmpFn, sPre=sPrefix0, sSuf=sSuffix0; sSuf.replace(QString("."), QString(""));
		if(sPre.isEmpty()) sPre="~ph";
		if(sSuf.isEmpty()) sSuf="tmp";
		do{
			QString sName=QString("%1%2_%3.%4")
				      .arg(sPre)
				      .arg(time(NULL), 0, 16)
				      .arg(qrand(), 0, 16)
				      .arg(sSuf)
				      ;
			QFileInfo f(m_sTmpDir, sName);
			if(!f.exists()){
				sTmpFn=f.filePath();
				break;
			}
		}while(true);
		return sTmpFn;
	}

//	QString _getTempDir(const QString& sInfo = "")
//	{
//		QString sTmpFn;
//		do{
//			QString sName = QString("%1-%2-%3")
//					.arg(sInfo)
//					.arg(time(NULL), 0, 16)
//					.arg(qrand(), 0, 16)
//					;
//			QFileInfo f(m_sTmpDir, sName);
//			if(!f.exists()){
//				sTmpFn=f.filePath();
//				break;
//			}
//		}while(true);
//		return sTmpFn;
//	}

};

///////////////////////////////////////////////////////////

class _CDeferFileDeletion{

private:

	QString				m_sFnToDel;
	bool				m_bTrackErr; //2014.4.9 whether to keep the file name in buffer if failed to delete it;

	static QStringList		s_vFailed; //2014.4.9 files that have problems being deleted;

public:

	~_CDeferFileDeletion(void){perform();}
	_CDeferFileDeletion(const QString& sFn, bool bTrackErr=true)
		: m_sFnToDel(sFn)
		, m_bTrackErr(bTrackErr)
	{
		return;
	}

	bool perform(void)
	{
		QFile::remove(m_sFnToDel);
		bool bExist=QFileInfo(m_sFnToDel).exists();
		if(bExist && m_bTrackErr){
			s_vFailed << m_sFnToDel;
//			qDebug() << "Failure deleting file: " << m_sFnToDel;
		}
		return bExist;
	}

public:

	static QStringList& getFailed(void){return s_vFailed;}
	static void clearFailed(void){s_vFailed.clear();}

};

///////////////////////////////////////////////////////////

class _CLocalFile{

public:

	static QString invalidChars(){return QString("\\/:*?\"<>|");}

	static bool isFileNameValid(const QString& sName)
	{
		bool bRes = true;
		if(!sName.isEmpty()){
			QString sBad = invalidChars();
			for(int sub = 0; sub < sBad.size(); sub++){
				QChar ch = sBad[sub];
				if(sName.indexOf(ch) >= 0){
					bRes = false;
					break;
				}
			}
		}else{
			bRes = false;
		}
		return bRes;
	}

	static QString eliminateBeginingSlashes(const QString& sPath0)
	{
		QString sPath = sPath0;
		int nSlash = 0, nLen = sPath.size();
		for(int i=0; i<nLen; ++i){
			if(sPath[i] == _CHAR_SLASH || sPath[i] == _CHAR_BACKSLASH){
				nSlash++;
			}else{
				break;
			}
		}
		if(nSlash>0) sPath = sPath.right(sPath.size()-nSlash);
		return sPath;
	}

	static QString eliminateTrailingSlashes(const QString& sPath0)
	{
		QString sPath = sPath0;
		int nSlash = 0, nLen = sPath.size();
		for(int i=nLen-1; i>=0; --i){
			if(sPath[i] == _CHAR_SLASH || sPath[i] == _CHAR_BACKSLASH){
				nSlash++;
			}else{
				break;
			}
		}
		if(nSlash>0) sPath = sPath.left(sPath.size()-nSlash);
		return sPath;
	}

	static QString ensureBeginingSlashes(const QString& sPath0, QChar chSlash = _CHAR_SLASH)
	{
		QString sPath = sPath0;
		int nSlash = 0, nLen = sPath.size();
		for(int i=0; i<nLen; ++i){
			if(sPath[i] == _CHAR_SLASH || sPath[i] == _CHAR_BACKSLASH){
				nSlash++;
			}else{
				break;
			}
		}
		if(nSlash==0) sPath = chSlash + sPath;
		else if(nSlash>1) sPath = sPath.right(nSlash-1); //keep only one slash begining;
		return sPath;
	}

	static QString ensureTrailingSlashes(const QString& sPath0, QChar chSlash = _CHAR_SLASH)
	{
		QString sPath = sPath0;
		#if 0
		if(!sPath.isEmpty()){
			sPath = eliminateTrailingSlashes(sPath0);
			sPath += chSlash;
		}
		#else
		int nSlash = 0, nLen = sPath.size();
		for(int i=nLen-1; i>=0; --i){
			if(sPath[i] == _CHAR_SLASH || sPath[i] == _CHAR_BACKSLASH){
				nSlash++;
			}else{
				break;
			}
		}
		if(nSlash==0) sPath += chSlash;
		else if(nSlash>1) sPath = sPath.left(sPath.size()-nSlash+1); //keep only one slash trailing;
		#endif
		return sPath;
	}

	static QString concatenate(const QString& sLeft0, const QString& sRight0, QChar chSlash)
	{return concatenate(sLeft0, sRight0, QString(chSlash));}

	static QString concatenate(const QString& sLeft0, const QString& sRight0, const QString& sSlash = _TEXT_SLASH)
	{
		QString sLeft = eliminateTrailingSlashes(sLeft0);
		QString sRight = eliminateBeginingSlashes(sRight0);
		return sLeft + sSlash + sRight;
	}

	static QString dirOf(const QString& sPath, bool bSlash)
	{
		//2014.2.26 ensure a trailing slash if result is not empty;
		QString sDir, sTmp = sPath; sTmp.replace(_CHAR_BACKSLASH, _CHAR_SLASH);
		if(!sTmp.isEmpty()){
			int p=sTmp.lastIndexOf(_CHAR_SLASH);
			if(p>=0){
				if(bSlash) p++;
				sDir=sTmp.left(p);
			}
		}
		return sDir;
	}

	static QString parentOf(const QString& sPath)
	{
		//2014.2.26 ensure a trailing slash if result is not empty;
		QString sDir, sTmp = sPath; sTmp.replace(_CHAR_BACKSLASH, _CHAR_SLASH);

		//be sure to first get rid of the trialing slash;
		while(!sTmp.isEmpty() && sTmp.at(sTmp.size()-1)==_CHAR_SLASH){
			sTmp=sTmp.left(sTmp.size()-1);
		}

		if(!sTmp.isEmpty()){
			int p=sTmp.lastIndexOf(_CHAR_SLASH);
			if(p>=0){
				sDir=sTmp.left(p); //excluding the trailing slash;
			}
		}
		return sDir;
	}

	static QString leafOf(const QString& sPath)
	{
		QString sLeaf = sPath; sLeaf.replace(_CHAR_BACKSLASH, _CHAR_SLASH);
		if(!sLeaf.isEmpty()){
			int p = sLeaf.lastIndexOf(_CHAR_SLASH);
			if(p >= 0){
				sLeaf = sLeaf.right(sLeaf.size()-p-1);
			}
		}
		return sLeaf;
	}

	static QString titleOf(const QString& sFn)
	{
		QString sTitle;
		if(!sFn.isEmpty()){
			sTitle = leafOf(sFn);
			int p = sTitle.lastIndexOf('.');
			if(p > 0){
				sTitle = sTitle.left(p);
			}
		}
		return sTitle;
	}

	static QString extOf(const QString& sFn, bool bDot)
	{
		QString sExt, sLeaf = leafOf(sFn);;
		if(!sLeaf.isEmpty()){
			int p = sLeaf.lastIndexOf('.');
			if(p >= 0){
				if(!bDot) p++;
				sExt = sLeaf.right(sLeaf.size()-p);
			}
		}
		return sExt;
	}

	static QString qualifyFilePath(const QString& sPath0, QChar chSep=_CHAR_SLASH)
	{
		QString sRes, sPath=sPath0;
		if(!sPath.isEmpty()){

			if(chSep!=_CHAR_SLASH) sPath.replace(_CHAR_SLASH, chSep);
			if(chSep!=_CHAR_BACKSLASH) sPath.replace(_CHAR_BACKSLASH, chSep);

			_CTextSplitter v(sPath, chSep, QString::SkipEmptyParts);
			if(!v.isEmpty()){
				//2014.4.4 consider of relative paths, like this: './abc/def/./hij/..'
				bool bRelative=(v[0]==".");
				v.removeAll(".");
				if(bRelative) v.insert(0, ".");
				sRes=v;
			}

			//Retain the leading/trailing slashes if existing;
			QChar chFirst=sPath[0], chLast=sPath[sPath.size()-1];
			if(!sRes.isEmpty()){
				if(chFirst==chSep){
					sRes=QString(chSep)+sRes;

					//2015.1.22 consider of UNC path: //t400s/mysoft/...
					if(sPath.size()>2 && sPath[1]==chSep){
						sRes=QString(chSep)+sRes;
					}
				}
			}

			if(chLast==chSep){
				sRes+=QString(chSep);
			}
		}
		return sRes;
	}

	static bool contains(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 contains the path1
		return isAncestorOf(sPath0, sPath1) || isEquivalent(sPath0, sPath1);
	}

	static bool isParentOf(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 is the parent of the path1
		if(sPath0.isEmpty() || sPath1.isEmpty()){
			return false;
		}

		QChar chSep=_CHAR_SLASH;

		_CTextSplitter vParent(qualifyFilePath(sPath0, chSep), chSep, QString::SkipEmptyParts);
		_CTextSplitter vChild(qualifyFilePath(sPath1, chSep), chSep, QString::SkipEmptyParts);

		if(vChild.size() != vParent.size() + 1){
			return false;
		}

		for(int i=0; i < vParent.size(); ++i){
			if(vParent[i] != vChild[i]){
				return false;
			}
		}

		return true;
	}

	static bool isChildOf(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 is a child of path1
		return isParentOf(sPath1, sPath0);
	}

	static bool isAncestorOf(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 is the ancestor of the path1
		if(sPath0.isEmpty() || sPath1.isEmpty()){
			return false;
		}

		QChar chSep=_CHAR_SLASH;

		_CTextSplitter vAnce(qualifyFilePath(sPath0, chSep), chSep, QString::SkipEmptyParts);
		_CTextSplitter vDesc(qualifyFilePath(sPath1, chSep), chSep, QString::SkipEmptyParts);

		if(vAnce.size() >= vDesc.size()){
			return false;
		}

		for(int i = 0; i < vAnce.size(); ++i){
			if(vAnce[i] != vDesc[i]){
				return false;
			}
		}

		return true;
	}

	static bool isDescendantOf(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 is the descendant of the path1
		return isAncestorOf(sPath1, sPath0);
	}

	static bool isEquivalent(const QString& sPath0, const QString& sPath1)
	{
		//Return whether the path0 is equivalent as the path1
		if(sPath0.isEmpty() || sPath1.isEmpty()){
			return false;
		}

		QChar chSep=_CHAR_SLASH;

		_CTextSplitter v0(qualifyFilePath(sPath0, chSep), chSep, QString::SkipEmptyParts);
		_CTextSplitter v1(qualifyFilePath(sPath1, chSep), chSep, QString::SkipEmptyParts);

		if(v1.size() != v0.size())
		{
			return false;
		}

		for(int i = 0; i < v0.size(); ++i){
			if(v0[i] != v1[i]){
				return false;
			}
		}

		return true;
	}

	static QString movePath(const QString& sPath, const QString& sFrom, const QString& sTo)
	{
		return changeRoot(sPath, sFrom, sTo);
	}

	static QString changeRoot(const QString& sPath0, const QString& sFrom0, const QString& sTo0)
	{
		QString sNewPath;
		if(!sPath0.isEmpty()){
			QString sPath = qualifyFilePath(sPath0, _CHAR_SLASH);
			QString sFrom = qualifyFilePath(sFrom0, _CHAR_SLASH); sFrom = ensureTrailingSlashes(sFrom, _CHAR_SLASH);
			QString sTo = qualifyFilePath(sTo0, _CHAR_SLASH); sTo = ensureTrailingSlashes(sTo, _CHAR_SLASH);
			if(contains(sFrom, sPath)){
				int pos = sPath.indexOf(sFrom);
				if(pos == 0){
					QString sRight = sPath.right(sPath.size() - sFrom.size());
					if(sTo.isEmpty()){
						sNewPath = sRight;
					}else{
						sNewPath = concatenate(sTo, sRight, _TEXT_SLASH);
					}
				}
			}
		}
		return sNewPath;
	}

	static QString changeExtension(const QString& sFn, const QString& sExtNew)
	{
		QString sFnNew;
		if(!sFn.isEmpty()){
			QString sDir = dirOf(sFn, true); //true: including a slash;
			QString sTitle = titleOf(sFn);
			if(!sTitle.isEmpty()){
				QString sExt=sExtNew;
				if(!sExt.isEmpty() && sExt[0]!=_CHAR_DOT){
					sExt=_TEXT_DOT+sExt;
				}
				sFnNew = sDir + sTitle + sExt;
			}
		}
		return sFnNew;
	}

	static QString relativePath(const QString& sPath0, const QString& sRoot0)
	{
		QString sRelPath;
		if(!sPath0.isEmpty() && !sRoot0.isEmpty()){
			//The 'sRoot' must be a folder path, rather than file;
			QString sRoot = ensureTrailingSlashes(sRoot0, _CHAR_SLASH);
			QString sPath = sPath0;
			sPath.replace(_CHAR_BACKSLASH, _CHAR_SLASH);
			sRoot.replace(_CHAR_BACKSLASH, _CHAR_SLASH);
			if(sPath.size() > sRoot.size()){
				//forcedly compare with case sensitivity;
				if(sPath.indexOf(sRoot, 0, Qt::CaseSensitive) == 0){
					sRelPath = sPath.right(sPath.size() - sRoot.size());
				}
			}
		}
		return sRelPath;
	}

	static QString fileUrlToLocalPath(const QString& sUrl)
	{
		//2014.4.1 http://en.wikipedia.org/wiki/File_URI_scheme
		//Unix:
		//file://localhost/etc/fstab
		//file:///etc/fstab
		//Windows:
		//file://localhost/c|/WINDOWS/clock.avi
		//file://localhost/c:/WINDOWS/clock.avi
		//file:///c|/WINDOWS/clock.avi
		//file:///c:/WINDOWS/clock.avi
		//file:///c:/path/to/the%20file.txt
		//2014.5.10 consider of the relative paths for NYF7, e.g.
		//file://./temp/abc.txt
		//2015.3.2 consider of the UNC shared file paths, e.g.
		//file://///servername/share/file.txt
		//http://rubenlaguna.com/wp/2007/04/20/firefox-and-file-windows-unc-paths/
		QString sFn, sTag="file://";
		int p=sUrl.indexOf(sTag, 0, Qt::CaseInsensitive);
		if(p==0){
			sFn=sUrl.right(sUrl.size()-sTag.size());

#if defined(Q_OS_WIN)
			//get rid of the leading slashes if any, only for windows;
			if(!sFn.isEmpty()){
				int nLeadingSlashes=0;
				for(int i=0; i<sFn.size(); ++i){
					QChar ch=sFn[i];
					if(ch=='/' || ch=='\\'){
						nLeadingSlashes++;
					}else{
						break;
					}
				}

				//2015.3.2 consider of file://///servername/share/file.txt
				//and consider of malformed ones: file:////servername/share/file.txt
				if(nLeadingSlashes>=2) nLeadingSlashes-=2;

				if(nLeadingSlashes>0){
					sFn=sFn.right(sFn.size()-nLeadingSlashes);
				}
			}

			//handle the drive separator ":" for windows file paths;
			sFn=sFn.replace("|/", ":/");
#endif
			//get rid of the 'localhost/' prefix if existing;
			if(!sFn.isEmpty()){
				QString sLocalHost="localhost/";
				int p=sFn.indexOf(sLocalHost, 0);
				if(p==0){
					sFn=sFn.right(sFn.size()-sLocalHost.size());
				}
			}
		}
		return sFn;
	}

	static QString fileType(const QString& sSuffix)
	{
		QString sType;
		{
#if defined(Q_OS_WIN)
			//2014.11.14 use UNICODE version of WIN32 API instead;
			QString sFileName="none."+sSuffix;
			SHFILEINFOW xSFI; memset(&xSFI, 0, sizeof(xSFI));
//			::SHGetFileInfoW(sFileName.toUtf8().constData(), FILE_ATTRIBUTE_NORMAL, &xSFI, sizeof(xSFI), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
			std::vector<WCHAR> vBuf(sFileName.size()+1, 0);
			sFileName.toWCharArray(&vBuf[0]);
			::SHGetFileInfoW(&vBuf[0], FILE_ATTRIBUTE_NORMAL, &xSFI, sizeof(xSFI), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
			sType = QString::fromWCharArray(&xSFI.szTypeName[0]);
#else
			QString sFileName = _CTemporaryFile()._getTempFile("", sSuffix);
			_CDeferFileDeletion dfd(sFileName); Q_UNUSED(dfd);
			if(!QFileInfo(sFileName).exists()){
				QFile f(sFileName);
				if(f.open(QIODevice::Truncate | QIODevice::WriteOnly)){
					f.close();
				}
			}
			sType = QFileIconProvider().type(QFileInfo(sFileName));
#endif
		}

		return sType;
	}


};

///////////////////////////////////////////////////////////

class _CFileLock{

public:

	static QString makeLockFile(const QString& sFn)
	{
#if defined(Q_OS_WIN)
		QString sDirLck=QDir::tempPath();
#elif defined(Q_OS_LINUX)
		//QString sDirLck="/var/lock";
		QString sDirLck=QDir::tempPath();
#elif defined(Q_OS_MAC)
		QString sDirLck=QDir::tempPath(); //"/var/run" -> unwritable;
#endif
		QString sFnLck=_CLocalFile::concatenate(sDirLck, QFileInfo(sFn).fileName()+".lock");
		return sFnLck;
	}

	static bool release(const QString& sFn)
	{
		bool bSucc=false;
		QString sFnLck=makeLockFile(sFn);
qDebug() << "Unlock:" << sFnLck;

		QFileInfo xFI(sFnLck);
		if(xFI.exists()){
			bSucc=QFile::remove(sFnLck);
		}
		return bSucc;
	}

	static bool lock(const QString& sFn)
	{
		QString sFnLck=makeLockFile(sFn);
qDebug() << "Lock:" << sFnLck;

		bool bAlreadyLocked=false;
		{
			QFileInfo xFI(sFnLck);
			if(xFI.exists()){
				int nInterval=QDateTime::currentDateTime().toTime_t()-xFI.lastModified().toTime_t();
				if(nInterval>=0 && nInterval<30){
					bAlreadyLocked=true;
				}else{
					//if the lock file not updated in 30secs, consider that pid died and auto-release the lock;
					QFile::remove(sFnLck);
				}
			}
		}

		bool bSucc=false;
		if(!bAlreadyLocked){
			{
				QFile f(sFnLck);
				if(f.open(QFile::WriteOnly | QFile::Truncate)){
					f.close();
				}
			}
			bSucc=QFileInfo(sFnLck).exists();
		}

		return bSucc;
	}

	static bool update(const QString& sFn)
	{
		bool bSucc=false;
		QString sFnLck=makeLockFile(sFn);
//qDebug() << "Update:" << sFnLck;

		QFileInfo xFI(sFnLck);
		if(xFI.exists()){
			QFile f(sFnLck);
			if(f.open(QFile::WriteOnly | QFile::Truncate)){
				f.close();
				bSucc=true;
			}
		}
		return bSucc;
	}

};

///////////////////////////////////////////////////////////

//class _CFileSizeScale{

//protected:

//	int64_t			m_nSize;

//protected:

//	double toKB()const{return (double)m_nSize / 1024.0;}
//	double toMB()const{return toKB() / 1024.0;}
//	double toGB()const{return toMB() / 1024.0;}
//	double toTB()const{return toGB() / 1024.0;}

//public:

//	_CFileSizeScale(int64_t nBytes) : m_nSize(nBytes){return;}

//	int64_t size()const{return m_nSize;}

//	operator QString()const{return toString();}
//	QString toString(int nPrecision = 1)const
//	{
//		double nKB = toKB();
//		double nMB = toMB();
//		double nGB = toGB();
//		double nTB = toTB();

//		double vSizes[] = {nKB, nMB, nGB, nTB};
//		static QString vUnits[] = {"KiB", "MiB", "GiB", "TiB"};
//		__ASSERT(sizeof(vSizes)/sizeof(vSizes[0]) == sizeof(vUnits)/sizeof(vUnits[0]));

//		double nSize = 0.0;
//		QString sUnit = "";

//		for(size_t i=0; i < sizeof(vSizes)/sizeof(vSizes[0]); ++i){
//			nSize = vSizes[i];
//			sUnit = vUnits[i];
//			if(nSize < 1024.0){
//				break;
//			}
//		}

//		QString sSize = QString::number(nSize, 'f', nPrecision);
//		return QString("%1 %2").arg(sSize).arg(sUnit);
//	}

//};

///////////////////////////////////////////////////////////

class _CFileSizeScale{

private:

	int64_t		m_nSize;
	QString		m_sDef;
	bool		m_bSeperator;

	QString _format(void)
	{
		QString sUnit; int64_t n=m_nSize; double d=m_nSize; int nPrec=0;
		int64_t kb=_LL(1024);
		int64_t mb=_LL(1024)*_LL(1024);
		int64_t gb=_LL(1024)*_LL(1024)*_LL(1024);
		int64_t tb=_LL(1024)*_LL(1024)*_LL(1024)*_LL(1024);
		int64_t pb=_LL(1024)*_LL(1024)*_LL(1024)*_LL(1024)*_LL(1024);
		int64_t eb=_LL(1024)*_LL(1024)*_LL(1024)*_LL(1024)*_LL(1024)*_LL(1024);

		//2013.12.6 warning C4307: '*' : integral constant overflow
		//int64_t zb=(int64_t)1024*1024*1024*1024*1024*1024*1024;
		//int64_t yb=(int64_t)1024*1024*1024*1024*1024*1024*1024*1024;

		if(n<=0){
			d=0;
		}else if(n<kb){
			d=n;
			nPrec=0;
			sUnit=("B");
		}else if(n<mb){
			d/=kb;
			nPrec=1;
			sUnit=("KiB");
		}else if(n<gb){
			d/=mb;
			nPrec=1;
			sUnit=("MiB");
		}else if(n<tb){
			d/=gb;
			nPrec=2;
			sUnit=("GiB");
		}else if(n<pb){
			d/=tb;
			nPrec=2;
			sUnit=("TiB");
		}else if(n<eb){
			d/=pb;
			nPrec=3;
			sUnit=("PiB");
		}else{
			d/=eb;
			nPrec=3;
			sUnit=("EiB");
		}
		/*
		}else if(n<zb){
			d/=eb;
			nPrec=3;
			sUnit=("EiB");
		}else if(n<yb){
			d/=zb;
			nPrec=3;
			sUnit=("ZiB");
		}else{
			d/=yb;
			nPrec=3;
			sUnit=("YiB");
		}
		*/

		QString s=m_sDef;
		if(d>0){
			s.setNum(d, 'f', nPrec);
			if(m_bSeperator) s+=" ";
			s+=sUnit;
		}
		return s;
	}

public:

	_CFileSizeScale(int64_t nSize, const QString& sDef="", bool bSeperator=true)
		: m_nSize(nSize), m_sDef(sDef), m_bSeperator(bSeperator)
	{
		return;
	}

	operator QString(){return _format();}

};

///////////////////////////////////////////////////////////

class _CDateTimeSmart : public QDateTime{

protected:

	QString smartString()const
	{
		QString s;
		QDate d = date();
		if(d == QDate::currentDate()){
			QTime t=time();
			//s=t.toString(Qt::TextDate); //that's: 23:59:58;
			s = t.toString("hh:mm"); //we need 23:59;
		}else{
			s = d.toString(Qt::DefaultLocaleShortDate);
		}
		return s;
	}

public:

	_CDateTimeSmart(const QDateTime& dt) : QDateTime(dt){return;}

	operator QString()const{return smartString();}

};

///////////////////////////////////////////////////////////

class _CTimeSmart : public QTime{

public:

	_CTimeSmart(const QTime& t) : QTime(t){return;}
	_CTimeSmart(int nSecond)
	{
		int h = ::floor(nSecond / (60 * 60));
		int m = ::floor((nSecond % (60 * 60)) / 60);
		int s = (nSecond % (60 * 60)) % 60;
		int ms = 0;

		(*this) = QTime(h, m, s, ms);
	}

};

///////////////////////////////////////////////////////////

class _CArrowCursor{

public:

	~_CArrowCursor(void){QApplication::restoreOverrideCursor();}
	_CArrowCursor(void){QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));}

};

class _CWaitCursor{

public:

	~_CWaitCursor(void){QApplication::restoreOverrideCursor();}
	_CWaitCursor(void){QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));}

};

class _CBeginUpdateDisplay{

private:

	QWidget *		m_pWidget;

public:

	~_CBeginUpdateDisplay(void)
	{
		if(m_pWidget){
			m_pWidget->setUpdatesEnabled(true);
			m_pWidget->setAttribute(Qt::WA_OpaquePaintEvent, false);
		}
	}
	_CBeginUpdateDisplay(QWidget* pWidget)
		: m_pWidget(pWidget)
	{
		if(m_pWidget){
			m_pWidget->setAttribute(Qt::WA_OpaquePaintEvent, true);
			m_pWidget->setUpdatesEnabled(false);
		}
	}

};

template<typename TWidget> class _CTempSetSortingEnabled{

private:

	TWidget *		m_pWidget;
	bool			m_bSortingEnabled;

public:

	~_CTempSetSortingEnabled(void)
	{
		if(m_pWidget){
			m_pWidget->setSortingEnabled(m_bSortingEnabled);
		}
	}
	_CTempSetSortingEnabled(TWidget* pWidget, bool Enable)
		: m_pWidget(pWidget)
		, m_bSortingEnabled(false)
	{
		if(m_pWidget){
			m_bSortingEnabled = m_pWidget->isSortingEnabled();
			m_pWidget->setSortingEnabled(Enable);
		}
	}

};

class _CAppAlert
{

public:

	_CAppAlert(QWidget* pWnd = NULL){QApplication::alert(pWnd, 0);}

};

///////////////////////////////////////////////////////////

class _CPixmapEx{

protected:

	QPixmap			m_xPixmap;
	std::vector<QColor>	m_vBackgrounds;

protected:

	void initBackgrounds()
	{
		m_vBackgrounds.push_back(Qt::white);
		m_vBackgrounds.push_back(Qt::black);
		m_vBackgrounds.push_back(Qt::red);
		m_vBackgrounds.push_back(Qt::green);
		m_vBackgrounds.push_back(Qt::blue);
		m_vBackgrounds.push_back(Qt::yellow);
		m_vBackgrounds.push_back(Qt::cyan);
		m_vBackgrounds.push_back(Qt::magenta);
		m_vBackgrounds.push_back(QColor(255, 127, 39));
		m_vBackgrounds.push_back(QColor(181, 230, 29));
		m_vBackgrounds.push_back(QColor(0, 162, 232));
//		for(int r = 0; r < 255; r++){
//			for(int g = 0; g < 255; g++){
//				for(int b = 0; b < 255; b++){
//					m_vBackgrounds.push_back(QColor(r, g, b));
//				}
//			}
//		}
	}

public:

	_CPixmapEx()
	{
		initBackgrounds();
	}

	_CPixmapEx(const QPixmap& xPixmap)
	{
		m_xPixmap = xPixmap;
		initBackgrounds();
	}

	_CPixmapEx(const QImage& xImage)
	{
		m_xPixmap = QPixmap::fromImage(xImage);
		initBackgrounds();
	}

	_CPixmapEx(const QString& sPath)
	{
		load(sPath);
		initBackgrounds();
	}

	void load(const QString& sPath)
	{
		m_xPixmap.load(sPath);
	}

	const QPixmap& pixmap() const
	{
		return m_xPixmap;
	}

	static void makeTransparent(QPixmap& xPixmap)
	{
		QColor clEample;
		{
			QImage xImage = xPixmap.toImage();
			if(xImage.height() > 0){
				QRgb xExampleRgb = xImage.pixel(0, xImage.height() - 1);
				clEample.setRgb(xExampleRgb);
			}
		}
		QBitmap xMask = xPixmap.createMaskFromColor(clEample, Qt::MaskInColor);
		xPixmap.setMask(xMask);
	}

	void makeTransparent()
	{
		QColor clEample(255, 255, 255);
		{
			QImage xImage = m_xPixmap.toImage();
			if(xImage.height() > 0){
				QRgb xExampleRgb = xImage.pixel(0, xImage.height() - 1);
				clEample.setRgb(xExampleRgb);
			}
		}
		QBitmap xMask = m_xPixmap.createMaskFromColor(clEample, Qt::MaskInColor);
		m_xPixmap.setMask(xMask);
	}

	//For transparent pixmap, and fill with a color that is different form each pixel in the pixmap
	void fillWithAutoColor()
	{
		QColor clBackground(255, 255, 255);
		QImage xImg = m_xPixmap.toImage();
		std::vector<QColor> vColors;
		for(int x = 0; x < xImg.width(); x++){
			for(int y = 0; y < xImg.height(); y++){
				QRgb xRgb = xImg.pixel(x, y);
				vColors.push_back(QColor(xRgb));
			}
		}
		std::vector<QColor>::const_iterator it = m_vBackgrounds.begin();
		for(; it != m_vBackgrounds.end(); it++){
			QColor cl = *it;
			if(std::find(vColors.begin(), vColors.end(), cl) == vColors.end()){
				clBackground = cl;
				break;
			}
		}
		m_xPixmap.fill(clBackground);
		QPainter xPainter(&m_xPixmap);
		xPainter.drawImage(0, 0, xImg);
	}

	static void setPixmapOpacity(QPixmap& xPixmap, int nOpacity)
	{
#if 1
		QPixmap xRes(xPixmap.size()); xRes.fill(Qt::transparent);
		QPainter xPainter(&xRes);
		xPainter.setOpacity((qreal)nOpacity / 255.0);
		xPainter.drawPixmap(0, 0, xPixmap);
		xPixmap = xRes;
#else
		xPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
		xPainter.fillRect(xPixmap.rect(), QColor(0, 0, 0, nOpacity));
#endif
	}

	void setPixmapOpacity(int nOpacity)
	{
		setPixmapOpacity(m_xPixmap, nOpacity);
	}

	QString toBase64(const QString& sType = "png") const
	{
		QByteArray vBuf;
		{
			QBuffer xBuffer(&vBuf);
			m_xPixmap.save(&xBuffer, sType.toLatin1().data());
		}
		return QString(vBuf.toBase64());
	}

};

///////////////////////////////////////////////////////////

static QMap<QString, QIcon>	g_mFileIconsCache;

class _CFileIconUtils{

public:

	static QIcon getSuffixIcon(const QString& sSuffix)
	{
		QIcon xIcon;

		QMap<QString, QIcon>::const_iterator it = g_mFileIconsCache.find(sSuffix);
		if(it == g_mFileIconsCache.end()){
			QString sFn = _CTemporaryFile()._getTempFile("", sSuffix);
			_CDeferFileDeletion dfd(sFn); Q_UNUSED(dfd);
			{
				QFile f(sFn);
				if(f.open(QIODevice::Truncate | QIODevice::WriteOnly)){
					f.close();
				}
			}
			xIcon = QFileIconProvider().icon(sFn);
			if(!xIcon.isNull()) g_mFileIconsCache[sSuffix] = xIcon;
		}else{
			xIcon = it.value();
		}

		return xIcon;
	}

	static QIcon getLocalFileIcon(const QString& sFn)
	{
//		return getSuffixIcon(QFileInfo(sFn).suffix());
		return QFileIconProvider().icon(sFn);
	}

	static QIcon getFolderIcon()
	{
		return QFileIconProvider().icon(QFileIconProvider::Folder);
	}

	static QIcon pasteBadgeIcon(const QIcon& xBase0, const QIcon& xBadge0, Qt::Corner iCornor, const QSize& szBaseSize, const QSize& szBadgeSize)
	{
		//2014.5.1 paste the badge icon in the specified corner;
		QIcon xRes=xBase0;
		if(!xBase0.isNull()){

			QPixmap xBase=xBase0.pixmap(xBase0.actualSize(szBaseSize));
			if(!xBase.isNull() && !xBadge0.isNull()){

//				QSize szBadgeSize=szBaseSize*(3/4);
				QPixmap xBadge = xBadge0.pixmap(xBadge0.actualSize(szBadgeSize));

				if(!xBadge.isNull()){
					QPainter xPainter(&xBase);
					xPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
					xPainter.setOpacity(0.8);
					int x=0, y=0;
					switch(iCornor){
						case Qt::TopLeftCorner:
							x=0;
							y=0;
							break;
						case Qt::TopRightCorner:
							x=xBase.width()-xBadge.width();
							y=0;
							break;
						case Qt::BottomLeftCorner:
							x=0;
							y=xBase.height() - xBadge.height();
							break;
						case Qt::BottomRightCorner:
							x=xBase.width()-xBadge.width();
							y=xBase.height() - xBadge.height();
							break;
					}

					xPainter.drawPixmap(x, y, xBadge);
					xRes=QIcon(xBase);
				}
			}
		}
		return xRes;
	}

};

///////////////////////////////////////////////////////////

//class _CDate : public QDate{

//public:

//	_CDate(void)
//	{
//		clear();
//	}

//	bool isEmpty() const{return QDate::year() == 0 && QDate::month() == 0 && QDate::day() == 0;}
//	void clear(){QDate::setDate(0, 0, 0);}

//	operator QDate() const {return QDate(QDate::year(), QDate::month(), QDate::day());}
//	void operator=(const _CDate& tDate)
//	{
//		QDate::setDate(tDate.year(), tDate.month(), tDate.day());
//	}

//	_CDate operator+(int nDays) const {_CDate tDate; tDate.addDays(nDays); return tDate;}
//	int operator-(const _CDate& xDate) const {return xDate.daysTo(*this);}

//};

struct _CDatePeriod : public std::pair<time_t, time_t>{

public:

	_CDatePeriod(void)
	{
		first = second = -1;
	}

	_CDatePeriod(const time_t& t1, const time_t& t2)
	{
		first = t1;
		second = t2;
	}

	bool empty(void)const{return first == -1 || second == -1;}

	operator long()const{return empty() ? -1L : second - first;}

};

///////////////////////////////////////////////////////////

class _CComDlg{

public:

	static QFont getFont(bool& bOk, QWidget* pParent, const QString& sCaption, const QFont& xInitial)
	{
		bOk = false;
		QFont xSelectedFont;
		QFontDialog dlgFont(xInitial, pParent);
		dlgFont.setWindowTitle(sCaption);
		if(dlgFont.exec() == QFontDialog::Accepted){
			bOk = true;
			xSelectedFont = dlgFont.selectedFont();
		}
		return xSelectedFont;
	}

	static QColor getColor(bool& bOk, QWidget* pParent, const QString& sCaption, const QColor& clInitial)
	{
		bOk = false;
		QColor clSelectedColor;
		QColorDialog dlgColor(clInitial, pParent);
		dlgColor.setWindowTitle(sCaption);
		if(dlgColor.exec() == QColorDialog::Accepted){
			bOk = true;
			clSelectedColor = dlgColor.selectedColor();
		}
		return clSelectedColor;
	}

};

///////////////////////////////////////////////////////////

#if defined(Q_OS_WIN32)

#include <shlobj.h>

class _CBrowseForFolder{

private:

	HWND		m_hWnd;
	QString		m_sDescr;
	QString		m_sInitDir;

protected:

	static int CALLBACK __BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
	{
		switch(uMsg){
			case BFFM_INITIALIZED:
			{
				::SendMessageW(hWnd, BFFM_SETSELECTION, true, lpData);
				//CWnd* pWnd=CWnd::FromHandle(hWnd);
				//if(pWnd) pWnd->CenterWindow(::AfxGetMainWnd());
				break;
			}
			case BFFM_SELCHANGED:
			{
				std::vector<WCHAR> vBuf(MAX_PATH+1, (L'\0'));
				LPITEMIDLIST pIdl=(LPITEMIDLIST)lParam;
				if(::SHGetPathFromIDListW(pIdl, &vBuf[0])){
					::SendMessageW(hWnd, BFFM_SETSTATUSTEXTW, 0, (LPARAM)&vBuf[0]);
				}
				break;
			}
		}
		return 0;
	}

public:

	_CBrowseForFolder(HWND hWnd, const QString& sDescr, const QString& sInitDir)
		: m_hWnd(hWnd)
		, m_sDescr(sDescr)
		, m_sInitDir(sInitDir)
	{
		int x=m_sInitDir.size();
		if(x>0){
			m_sInitDir=_CLocalFile::qualifyFilePath(m_sInitDir, '\\');
			if(m_sInitDir.at(x-1)==':'){
				m_sInitDir+='\\';
			}
		}
	}

	QString operator()(void){return exec();}
	QString exec(void)
	{
		BROWSEINFOW bi; ::memset(&bi, 0, sizeof(bi));
		bi.hwndOwner=m_hWnd;
		bi.pidlRoot=0; //default: desktop folder
		bi.pszDisplayName=0;
		bi.lpszTitle=(LPCWSTR)m_sDescr.utf16();
		bi.ulFlags=BIF_RETURNONLYFSDIRS|BIF_STATUSTEXT; //only return file-system folder
		bi.lpfn=__BrowseCallbackProc;
		bi.lParam=(LPARAM)(LPCWSTR)m_sInitDir.utf16(); //set initial path
		bi.iImage=0;

		std::vector<WCHAR> vBuf(MAX_PATH+1024, (L'\0'));
		LPITEMIDLIST pidl=::SHBrowseForFolderW(&bi);
		if(pidl){

			::SHGetPathFromIDListW(pidl, &vBuf[0]);

			// free memory used
			IMalloc* imalloc = 0;
			if( SUCCEEDED(SHGetMalloc(&imalloc)) ){
				imalloc->Free(pidl);
				imalloc->Release();
			}
		}

		return QString::fromUtf16((const ushort*)&vBuf[0]);
	}

};
#endif

///////////////////////////////////////////////////////////

class _CGuiUtils{

public:

	///////////////////////////////////////////////////////////

	static QSize sizeOfWidget(const QWidget* pWidget, double nWidth = 0.0, double nHeight = 0.0, bool bNotSmallerThanHint = true)
	{
		//Accept percentage, nWidth and nHeight specific percentage, give 0 for default
		//sizeHint, bNotSmallerThanHint ensure the size that calculated by this function
		//not smaller than the default sizeHint.
		//Note: the widget specified cannot override sizeHint for calling this function

		bool bResizeWidth = false;
		bool bResizeHeight = false;

		QRect rtScreen = QApplication::desktop()->availableGeometry();
		int w = (nWidth > 0.0) ? (int)((double)(rtScreen.width()) * nWidth) : pWidget->sizeHint().width();
		int h = (nHeight > 0.0) ? (int)((double)(rtScreen.height()) * nHeight) : pWidget->sizeHint().height();

		if(bNotSmallerThanHint){
			if(w >= pWidget->sizeHint().width()){
				bResizeWidth = true;
			}
			if(h >= pWidget->sizeHint().height()){
				bResizeHeight = true;
			}
		}else{
			bResizeWidth = true;
			bResizeHeight = true;
		}
		w = bResizeWidth ? w : pWidget->sizeHint().width();
		h = bResizeHeight ? h : pWidget->sizeHint().height();
		return QSize(w, h);
	}

	static void resizeWidgetsToSameWidth(QList<QWidget*> vWidgets, bool bMethodMax = false, int nDef = -1)
	{
		//Accept a list of widgets, resize the widgets to a size accroding to the method.
		//If the bMethodMax is true, according to the max width, otherwise according to the min width

		Q_UNUSED(nDef);
		if(!vWidgets.empty()){
			int nFixedWidth = vWidgets[0]->sizeHint().width();
			for(int i = 0; i < vWidgets.size(); i++){
				int nWidth = vWidgets[i]->sizeHint().width();
				if(bMethodMax){
					if(nFixedWidth < nWidth){
						nFixedWidth = nWidth;
					}
				}else{
					if(nFixedWidth > nWidth){
						nFixedWidth = nWidth;
					}
				}
			}
			for(int i = 0; i < vWidgets.size(); i++){
				if(bMethodMax){
					vWidgets[i]->setMinimumWidth(nFixedWidth);
				}else{
					vWidgets[i]->setMaximumWidth(nFixedWidth);
				}
			}
		}
	}

	///////////////////////////////////////////////////////////

	static bool deselectTree(QTreeWidget* pTree)
	{
		bool bSucc = false;
		if(pTree){
			QList<QTreeWidgetItem*> vSelectedItems = pTree->selectedItems();
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	static bool invertSelectTree(QTreeWidget* pTree)
	{
		bool bSucc = false;
		if(pTree){
			//First get current selected items
			QList<QTreeWidgetItem*> vSelectedItems = pTree->selectedItems();
			//Then select all
			pTree->selectAll();
			//Finally deselect selected items
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	///////////////////////////////////////////////////////////

	static bool deselectList(QListWidget* pList)
	{
		bool bSucc = false;
		if(pList){
			QList<QListWidgetItem*> vSelectedItems = pList->selectedItems();
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	static bool invertSelectList(QListWidget* pList)
	{
		bool bSucc = false;
		if(pList){
			//First get current selected items
			QList<QListWidgetItem*> vSelectedItems = pList->selectedItems();
			//Then select all
			pList->selectAll();
			//Finally deselect selected items
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	///////////////////////////////////////////////////////////

	static bool deselectTable(QTableWidget* pTable)
	{
		bool bSucc = false;
		if(pTable){
			QList<QTableWidgetItem*> vSelectedItems = pTable->selectedItems();
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	static bool invertSelectList(QTableWidget* pTable)
	{
		bool bSucc = false;
		if(pTable){
			//First get current selected items
			QList<QTableWidgetItem*> vSelectedItems = pTable->selectedItems();
			//Then select all
			pTable->selectAll();
			//Finally deselect selected items
			for(int i = 0; i < vSelectedItems.size(); i++){
				vSelectedItems[i]->setSelected(false);
			}
			bSucc = !vSelectedItems.isEmpty();
		}
		return bSucc;
	}

	///////////////////////////////////////////////////////////

	static bool removeWidgetFromLayout(QLayout* pLayout, int nIndex)
	{
		bool bSucc = false;
		if(pLayout){
			QLayoutItem* pLayoutItem = pLayout->itemAt(nIndex);
			if(pLayoutItem){
				pLayout->removeItem(pLayoutItem);

				delete pLayoutItem;
				pLayoutItem = NULL;

				bSucc = true;
			}
		}
		return bSucc;
	}

	static bool clearLayout(QLayout* pLayout, bool bRecursive)
	{
		bool bSucc = false;
		if(pLayout){
			QLayoutItem* pChild = NULL;
			while((pChild = pLayout->takeAt(0)) != 0){
				if(bRecursive && pChild->layout()/*If the layout item is a layout*/){
					clearLayout(pChild->layout(), bRecursive);
				}

				//2014.10.28 need to delete 'widget' as well;
				QWidget* pWidget=pChild->widget();
				if(pWidget) delete pWidget;

				delete pChild;
				pChild = NULL;
			}
			bSucc = (pLayout->count() == 0);
		}
		return bSucc;
	}

	static int indexOfWidgetInLayout(QLayout* pLayout, QWidget* pWidget)
	{
		int nRes = -1;
		if(pLayout && pWidget){
			for(int i = 0; i < pLayout->count(); i++){
				QLayoutItem* pLayoutItem = pLayout->itemAt(i);
				QWidget* pWidgetItem = pLayoutItem ? pLayoutItem->widget() : NULL;
				if(pLayoutItem && pWidgetItem && pWidgetItem == pWidget){
					nRes = i;
					break;
				}
			}
		}
		return nRes;
	}

};

///////////////////////////////////////////////////////////

class _CFontUtils{

protected:

	QFont		m_ftFont;

public:

	_CFontUtils(const QFont& ftFont)
		: m_ftFont(ftFont)
	{
	}

	/* The following two functions are used for converting size of font between general mode(font size: 9,10,12,14,36...)
	 * and html edit mode(font size: 1,2,3,4,5,6,7)
	 */

	/*QFont toHtmlEditFont() const
	{
		QFont ftRes = m_ftFont;
		int nSize = ftRes.pointSize(), nHtmlSize = 0;
		if(nSize <= 9){
			nHtmlSize = 1;
		}else if(nSize <= 10){
			nHtmlSize = 2;
		}else if(nSize <= 12){
			nHtmlSize = 3;
		}else if(nSize <= 14){
			nHtmlSize = 4;
		}else if(nSize <= 18){
			nHtmlSize = 5;
		}else if(nSize <= 24){
			nHtmlSize = 6;
		}else{
			nHtmlSize = 7;
		}
		ftRes.setPointSize(nHtmlSize);
		return ftRes;
	}

	QFont toGeneralFont() const
	{
		QFont ftRes = m_ftFont;
		int nSize = ftRes.pointSize(), nPointSize = 0;
		switch (nSize) {
			case 1:
				nPointSize = 9;
				break;
			case 2:
				nPointSize = 10;
				break;
			case 3:
				nPointSize = 12;
				break;
			case 4:
				nPointSize = 14;
				break;
			case 5:
				nPointSize = 18;
				break;
			case 6:
				nPointSize = 24;
				break;
			case 7:
				nPointSize = 36;
				break;
			default:
				//2014.7.11 if do not match font size, use a default size
				nPointSize = 12;
				break;
		}
		ftRes.setPointSize(nPointSize);
		return ftRes;
	}*/

	/*The function is used to measure a font's bounding size in pixel*/
	QSize boundingSizeOfText(const QString& sText)
	{
		QPixmap xImgBuff(1, 1); xImgBuff.fill(Qt::transparent);
		QPainter xCalcPainter(&xImgBuff);
		xCalcPainter.setFont(m_ftFont);
		int nWidth = xCalcPainter.fontMetrics().width(sText);
		int nHeight = xCalcPainter.fontMetrics().height();
		return QSize(nWidth, nHeight);
	}

	/*The function is used to create a new font with the specified string, width and height, based on the current font
	 * Note that: This function might not create a font with precise font size(the width and height you provide may be a approximation), it can only create a font which is close to the font you want,
	 * the precise font size will be set into pSzPrecise after find a font which is close to the font you want
	 */
	QFont createFontBySize(const QString& sStr, int w, int h, QSize* pSzPrecise) const
	{
		QFont ftFont = m_ftFont;
		int nPointSize = 1;
		while(true){
			ftFont.setPointSize(nPointSize);
			QFontMetrics xFM(ftFont);
			QSize szSize = xFM.size(Qt::TextExpandTabs, sStr);
			if(szSize.width() >= w || szSize.height() >= h){
				if(pSzPrecise) (*pSzPrecise) = szSize;
				break;
			}
			nPointSize++;
		}
		return ftFont;
	}

};

///////////////////////////////////////////////////////////

class _CHtmlFontUtils{

public:

	//2015.1.2 Font Conversion Tables
	//http://jerekdain.com/fontconversion.html

	static QStringList htmlFontSizes(void){return QString("7,10,12,13.5,18,24,36").split(",", QString::SkipEmptyParts);}

	static double cssPointSizeOf(int iHtmlFontSize, double nDefVal=12.0)
	{
		double pt = nDefVal;
		switch (iHtmlFontSize) {
			case 1:
				pt = 7;
				break;
			case 2:
				pt = 10;
				break;
			case 3:
				pt = 12;
				break;
			case 4:
				pt = 13.5;
				break;
			case 5:
				pt = 18;
				break;
			case 6:
				pt = 24;
				break;
			case 7:
				pt = 36;
				break;
		}
		return pt;
	}

	static int htmlFontSizeOf(double nPt, int nDefVal=2)
	{
		int nHtmlFontSize = 0;
		if(nPt <= 0){
			nHtmlFontSize = nDefVal;
		}else if(nPt <= 9.000001){
			nHtmlFontSize = 1;
		}else if(nPt <= 10.000001){
			nHtmlFontSize = 2;
		}else if(nPt <= 12.000001){
			nHtmlFontSize = 3;
		}else if(nPt <= 13.500001){
			nHtmlFontSize = 4;
		}else if(nPt <= 18.000001){
			nHtmlFontSize = 5;
		}else if(nPt <= 24.000001){
			nHtmlFontSize = 6;
		}else{
			nHtmlFontSize = 7;
		}
		return nHtmlFontSize;
	}

};

///////////////////////////////////////////////////////////

class _CStandardPixmapMaker
{

public:

	static QPixmap makeColorBlock(const QColor& clColor, const QSize& szSize, Qt::BrushStyle nFillStyle = Qt::SolidPattern, bool bFrameless = false, const QColor& clBorderColor = QColor(Qt::black), Qt::PenStyle nBorderStyle = Qt::SolidLine, bool bRoundedBorder = false, qreal nRoundedRadius = 0.0)
	{
		QPixmap xImg(szSize); xImg.fill(Qt::transparent);
		QPainter xPainter(&xImg);

		if(!bFrameless) xPainter.setPen(QPen(QBrush(clBorderColor), 1, nBorderStyle));
		else xPainter.setPen(QPen(Qt::NoPen));

		xPainter.setBrush(QBrush(clColor, nFillStyle));
		QRect rect(0, 0, xImg.width() - 1, xImg.height() - 1);

		if(bRoundedBorder) xPainter.drawRoundedRect(rect, nRoundedRadius, nRoundedRadius);
		else xPainter.drawRect(rect);

		return xImg;
	}

};

///////////////////////////////////////////////////////////

class _CColorUtils
{

public:

	/*The policy decided how to handle the alpha value in color structure
	 * Ignored: The color only depends RGB value, the alpha value keeps 255
	 * Transparency: The color depends RGBA value, the alpha value decides the transparency or opacity
	 * TransparenToWhite: The color depends RGB value. And if the alpha is 0, the color will be set to white, otherwise it keeps its original color
	 */
	enum AlphaPolicy{Ignored, Transparency, TransparenToWhite};

	/*This function is used to make formatted text to color
	 * it can convert the following situations:
	 * #EA7BE7(#ea7be7)
	 * rgb(r, g, b)
	 * rgba(r, g, b, a)
	 */
	static QColor parseColor(const QString& sColor, AlphaPolicy nPolicy)
	{
		QColor clColor = QColor(sColor);
		if(!clColor.isValid()){
			int n = 0;
			int nStartPos = sColor.indexOf("rgb");
			if(nStartPos != 0){
				nStartPos = sColor.indexOf("rgba");
				if(nStartPos == 0){
					n = 4;
				}
			}else{
				n = 3;
			}
			if(n > 0 && nStartPos == 0){
				int nLastPos = sColor.indexOf(")", nStartPos);
				if(nLastPos > nStartPos){
					_CTextSplitter v(sColor.mid(n + 1, nLastPos - (n + 1)), ", ");
					if(v.size() >= n){
						int r = v.value(0, "0").toInt()
							, g = v.value(1, "0").toInt()
							, b = v.value(2, "0").toInt()
							, a = v.value(3, "255").toInt()
							;
						clColor = QColor(r, g, b, a);
					}
				}
			}
		}
		if(clColor.isValid()){
			switch(nPolicy){
				case Ignored:
					clColor.setAlpha(255);
					break;
				case Transparency:
					//rgba: #RRGGBBAA, rgb: #RRGGBB
					//By default, if the given color name doesn't mention the alpha, it will be given 255 for opaque
					if(sColor.size() < 9){
						clColor.setAlpha(255);
					}
					break;
				case TransparenToWhite:
					if(clColor.alpha() == 0){
						clColor = Qt::white;
					}
					clColor.setAlpha(255);
					break;
				default:
					break;
			}
		}

		return clColor;
	}

};

///////////////////////////////////////////////////////////

class _CKeyboardUtils{

public:

	static bool noModifier(void){ return (QApplication::keyboardModifiers() == Qt::NoModifier); }

	static bool isCtrlDown(void){ return (QApplication::keyboardModifiers() & Qt::ControlModifier)!=0; }
	static bool isShiftDown(void){ return (QApplication::keyboardModifiers() & Qt::ShiftModifier)!=0; }
	static bool isAltDown(void){ return (QApplication::keyboardModifiers() & Qt::AltModifier)!=0; }
	static bool isMetaDown(void){ return (QApplication::keyboardModifiers() & Qt::MetaModifier)!=0; }

};

///////////////////////////////////////////////////////////

class _CXmlUtils{

protected:

	QDomDocument		m_xDomDoc;

public:

	_CXmlUtils();
	_CXmlUtils(const QString& sContent){setContent(sContent);}

	QString format() const {return m_xDomDoc.toString(8);}
	static QString format(const QString& sContent){return _CXmlUtils(sContent).format();}

	QString content() const {return m_xDomDoc.toString(-1);}
	void setContent(const QString& sContent){m_xDomDoc.setContent(sContent);}

	//The following four functions are usually used for html
	static void parseStyleAttr(const QDomElement& xDomEle, QMap<QString, QString>& mAttr)
	{
		mAttr.clear();
		QString sStyle = xDomEle.attribute("style", "");
		if(!sStyle.isEmpty()){
			_CTextSplitter xStyleTS(sStyle, ";", QString::SkipEmptyParts);
			for(int i = 0; i < xStyleTS.size(); i++){
				QString sAttr = xStyleTS[i].trimmed();
				_CTextSplitter xAttrTS(sAttr, ":");
				if(xAttrTS.size() == 2){
					QString sKey = xAttrTS[0].trimmed();
					QString sValue = xAttrTS[1].trimmed();

					mAttr[sKey] = sValue;
				}
			}
		}
	}

	static void setStyleAttr(QDomElement& xDomEle, const QMap<QString, QString>& mStyle)
	{
		_CTextSplitter xStyleTS("", ";");
		QMap<QString, QString>::const_iterator it;
		for(it = mStyle.begin(); it != mStyle.end(); it++){
			QString sKey = it.key();
			QString sValue = it.value();

			_CTextSplitter xAttrTS("", ":");
			xAttrTS << sKey << sValue;
			xStyleTS << (QString)xAttrTS;
		}
		xDomEle.setAttribute("style", (QString)xStyleTS);
	}

	void parseStyleAttr(QMap<QString, QString>& mAttr) const
	{
		QDomElement xDomEle = m_xDomDoc.documentElement();
		parseStyleAttr(xDomEle, mAttr);
	}

	void setStyleAttr(const QMap<QString, QString>& mStyle)
	{
		QDomElement xDomEle = m_xDomDoc.documentElement();
		if(!xDomEle.isNull()){
			setStyleAttr(xDomEle, mStyle);

			QString sContent;
			QTextStream xTxtStm(&sContent, QIODevice::WriteOnly); xDomEle.save(xTxtStm, -1);
			setContent(sContent);
		}
	}

	QString attribute(const QString& sName, const QString& sDef = "")
	{
		QDomElement xDomEle = m_xDomDoc.documentElement();
		return xDomEle.attribute(sName, sDef);
	}

	void setAttribute(const QString& sName, const QString& sValue)
	{
		QDomElement xDomEle = m_xDomDoc.documentElement();
		xDomEle.setAttribute(sName, sValue);
	}

	template<typename _CPred> void travelDomNodes(_CPred& xPred)
	{
		QDomNodeList vNodeList = m_xDomDoc.childNodes();
		for(int i = 0; i < vNodeList.size(); i++){
			QDomNode xDomNode = vNodeList.item(i);
			xPred(xDomNode);
			travelDomChildNodes(xDomNode, xPred);
		}
	}

	template<typename _CPred> static void travelDomNodes(const QString& sContent, _CPred& xPred)
	{
		_CXmlUtils(sContent).travelDomNodes(xPred);
	}

	template<typename _CPred> static void travelDomChildNodes(const QDomNode& xDomNode, _CPred& xPred)
	{
		QDomNodeList vNodeList = xDomNode.childNodes();
		for(int i = 0; i < vNodeList.size(); i++){
			QDomNode xDomNode = vNodeList.item(i);
			xPred(xDomNode);
			travelDomChildNodes(xDomNode, xPred);
		}
	}

};

///////////////////////////////////////////////////////////

#if defined(Q_OS_WIN)

class _CWinOsPlatformVersion{

private:

	OSVERSIONINFO m_OsVerInfo;

public:

	_CWinOsPlatformVersion(void)
	{
		::memset(&m_OsVerInfo, 0, sizeof(m_OsVerInfo));
		m_OsVerInfo.dwOSVersionInfoSize=sizeof(m_OsVerInfo);
		::GetVersionEx(&m_OsVerInfo);
		return;
	}

	bool isWin8OrLater(void)
	{
		return ( m_OsVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
				&& (m_OsVerInfo.dwMajorVersion >= 6)
				&& (m_OsVerInfo.dwMinorVersion >= 2)
				;
	}

	bool isVistaOrLater(void)
	{
		return ( m_OsVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
				&& (m_OsVerInfo.dwMajorVersion >= 6)
				;
	}

	bool isWin2kOrLater(void)
	{
		return ( m_OsVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
				&& (m_OsVerInfo.dwMajorVersion >= 5)
				;
	}

	bool isWinNT4OrLater(void)
	{
		return ( m_OsVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
				&& (m_OsVerInfo.dwMajorVersion >= 4)
				;
	}

	bool isWin95OrLater(void)
	{
		return ( m_OsVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
				&& ( m_OsVerInfo.dwMajorVersion >= 4 )
				;
	}

	bool isWin98OrLater(void)
	{
		return ( m_OsVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
				&& (
					( m_OsVerInfo.dwMajorVersion > 4 )
					|| ( (m_OsVerInfo.dwMajorVersion == 4) && (m_OsVerInfo.dwMinorVersion > 0) )
					)
				;
	}

};

#endif

///////////////////////////////////////////////////////////

#endif // MISC_UTILITIES_H
