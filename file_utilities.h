#ifndef FILE_UTILITIES_H
#define FILE_UTILITIES_H

#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QBuffer>
#include <QByteArray>
#include <QTextCodec>
#include <QTextStream>

///////////////////////////////////////////////////////////

class _CBinaryIODevice : public QIODevice{

public:

	_CBinaryIODevice(void){return;}

protected:

	static qint64 _copy(QIODevice& fSrc, QIODevice& fDst, qint64 nSize, int nBlock)
	{
		qint64 nBytes=-1;
		if(fSrc.isOpen() && fDst.isOpen()){
			if(nSize<0 || nSize>fSrc.size()) nSize=fSrc.size();
			if(nBlock<=0 || nBlock>1024*1024*64) nBlock=1024*64;

			nBytes=0; std::vector<char> vBuf(nBlock);
			while(nSize>0){
				qint64 nRead=qMin(nSize, (qint64)nBlock);
				nRead=fSrc.read(&vBuf[0], nRead);
				if(nRead>0){
					fDst.write(&vBuf[0], nRead);
					nBytes+=nRead;
					nSize-=nRead;
				}
			}
		}
		return nBytes;
	}

public:

	int _read(std::vector<char>& vBuf)
	{
		int nBytes=-1; QIODevice& f=*this;
		if(f.isOpen()){
			int nLen=(int)f.size();
			if(nLen>0){
				vBuf.resize(nLen);
				nBytes=f.read(&vBuf[0], nLen);
			}
		}
		return nBytes;
	}

	int _read(QByteArray& vBuf)
	{
		int nBytes=-1; QIODevice& f=*this;
		if(f.isOpen()){
			int nLen=(int)f.size();
			if(nLen>0){
				vBuf.resize(nLen);
				nBytes=f.read(vBuf.data(), nLen);
			}
		}
		return nBytes;
	}

	int _write(const char* lpBuf, int nSize)
	{
		int nBytes=-1; QIODevice& f=*this;
		if(f.isOpen()){
			if(lpBuf && nSize>0){
				f.write(lpBuf, nSize);
				nBytes=nSize;
			}
		}
		return nBytes;
	}

	int operator>>(std::vector<char>& vBuf){return _read(vBuf);}
	int operator<<(const std::vector<char>& vBuf){return _write(&vBuf[0], vBuf.size());}

	int operator>>(QByteArray& vBuf){return _read(vBuf);}
	int operator<<(const QByteArray& vBuf){return _write(vBuf.data(), vBuf.size());}

	int _copy_from(QIODevice& fSrc, int nSize=-1, int nBlock=1024*1024){return _copy(fSrc, *this, nSize, nBlock);}
	int _copy_to(QIODevice& fDst, int nSize=-1, int nBlock=1024*1024){return _copy(*this, fDst, nSize, nBlock);}
};

///////////////////////////////////////////////////////////
/*
struct _CBinaryFileRead : public _CBinaryFile{
	_CBinaryFileRead(const QString& sFn)
	{
		if(!sFn.isEmpty()){
			setFileName(sFn);
			open(QIODevice::ReadOnly);
		}
	}
};

struct _CBinaryFileWrite : public _CBinaryFile{
	_CBinaryFileWrite(const QString& sFn)
	{
		if(!sFn.isEmpty()){
			setFileName(sFn);
			open(QIODevice::WriteOnly);
		}
	}

};
*/
///////////////////////////////////////////////////////////

class _CTextFile{

private:

	QString			m_sFn;

public:

	static QString codecName(const QByteArray& vBuf, bool bHtml=false)
	{
		QString sName;
		QTextCodec* pDef=QTextCodec::codecForLocale();
		QTextCodec* p=bHtml ? QTextCodec::codecForHtml(vBuf, pDef) : QTextCodec::codecForUtfText(vBuf, pDef);
		if(p){
			QByteArray v=p->name();
			sName=v;
			//qDebug() << "->detectCodecName: " << sName;
		}
		return sName;
	}

	///////////////////////////////////////////////////

	static QString load(QIODevice& xIO, const QString& sCodec="")
	{
		QString sTxt;
		try{
			QTextStream ts(&xIO);
			if(!sCodec.isEmpty() && sCodec!="System") ts.setCodec(sCodec.toStdString().c_str());
			sTxt=ts.readAll();
		}catch(...){
			qDebug() << "**Error loading text. sCodec=" << sCodec;
		}
		return sTxt;
	}

	static QString load(const QByteArray& vBuf, const QString& sDefCodec)
	{
		QString sTxt, sCodec=sDefCodec;
		if(sCodec.isEmpty() || sCodec.toLower()=="auto") sCodec=codecName(vBuf); //auto-detect codec if necessary;
		QBuffer b; b.setData(vBuf);
		if(b.open(QIODevice::ReadOnly)){
			sTxt=load((QIODevice&)b, sCodec);
		}
		return sTxt;
	}

	static QString load(const QString& sFn, const QString& sDefCodec)
	{
		QString sTxt;
		{
//			qDebug() << "->loadTextFile(sFn=" << sFn << ", sCodec=" << sDefCodec << ")";
			QFile f(sFn);
			if(f.open(QIODevice::ReadOnly|QIODevice::Text)){
				int nLen=f.size();
				if(nLen>0){
					QByteArray v=f.read(nLen);
					sTxt=load(v, sDefCodec);
				}
				f.close();
			}
		}
		return sTxt;
	}

	///////////////////////////////////////////////////

	static bool save(QIODevice& xIO, const QString& sTxt, const QString& sCodec)
	{
		bool bSucc=false;
		try{
			QTextStream ts(&xIO);
			if(!sCodec.isEmpty()) ts.setCodec(sCodec.toStdString().c_str());
			ts << sTxt;
			bSucc=true;
		}catch(...){
		}
		return bSucc;
	}

	static qint64 save(const QString& sFn, const QString& sTxt, const QString& sCodec)
	{
		qint64 nBytes=-1;
//		qDebug() << "->saveText(sFn=" << sFn << ", nLen=" << sTxt.size() << ", sCodec=" << sCodec << ")";
		QFile f(sFn);
		if(f.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text)){
			if(save(f, sTxt, sCodec)){
				nBytes=f.size();
			}
			f.close();
		}
		return nBytes;
	}

	static qint64 saveAnsi(const QString& sFn, const QString& sTxt)
	{
		QTextCodec* pDef=QTextCodec::codecForLocale();
		return save(sFn, sTxt, pDef ? pDef->name() : "");
	}

	static qint64 saveUtf8(const QString& sFn, const QString& sTxt, bool bBOM)
	{
		qint64 nBytes=-1;
//		qDebug() << "->saveUtf8(sFn=" << sFn << ", nLen=" << sTxt.size() << ", bBOM=" << bBOM << ")";
		QFile f(sFn);
		if(f.open(QIODevice::WriteOnly|QIODevice::Truncate)){
			const char* _BOM_UTF8="\xEF\xBB\xBF";
			QByteArray v; if(bBOM) v.append(_BOM_UTF8);
			v.append(sTxt.toUtf8());
			f.write(v);
			nBytes=f.size();
			f.close();
		}
		return nBytes;
	}

	///////////////////////////////////////////////////

public:

	_CTextFile(const QString& sFn) : m_sFn(sFn){return;}

	QString fileName(void)const{return m_sFn;}
	void fileName(const QString& sFn){m_sFn=sFn;}

	QString load(const QString& sCodec)const
	{
		return load(m_sFn, sCodec);
	}

	qint64 save(const QString& sTxt, const QString& sCodec)const
	{
		return save(m_sFn, sTxt, sCodec);
	}
	qint64 saveAnsi(const QString& sTxt)const
	{
		return saveAnsi(m_sFn, sTxt);
	}

	qint64 saveUtf8(const QString& sTxt, bool bBOM)const
	{
		return saveUtf8(m_sFn, sTxt, bBOM);
	}

};

#endif // FILE_UTILITIES_H
