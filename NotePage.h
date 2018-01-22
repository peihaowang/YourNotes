#ifndef NOTEPAGE_H
#define NOTEPAGE_H

#include <QtDebug>
#include <QDateTime>
#include <QSoundEffect>
#include <QFrame>

#include "cpp_utilities.h"

class QPropertyAnimation;
class QMediaPlayer;
class QLabel;
class QLineEdit;
class QToolButton;
class QTextEdit;
class QMenu;
class QMenuColor;

class QNotePage : public QFrame
{

	Q_OBJECT

protected:

	QString			m_sNoteName;

	QPoint			m_ptPos;	//2015.10.6 Consider the shock of note will change the real pos
	QPoint			m_ptLastMousePos;

//	QLineEdit *		m_pEditTitle;
	QToolButton *		m_pBtnNewNote;
	QToolButton *		m_pBtnDeleteNote;
	QLabel *		m_pBtnChangeColor;
	QWidget *		m_pWidgetTitleBar;
	QTextEdit *		m_pRichEdit;

//	typedef _CPairVector<QString, QColor> _TPageColors;
//	_TPageColors		m_mPageColors;

	QDateTime		m_tReminder;
	QTimer *		m_pTimerReminder;

	QAction *		m_pActionNewPage;
	QAction *		m_pActionDeletePage;
	QAction *		m_pActionSetReminder;
	QAction *		m_pActionCancelReminder;

	QAction *		m_pActionUndo;
	QAction *		m_pActionRedo;
	QAction *		m_pActionCut;
	QAction *		m_pActionCopy;
	QAction *		m_pActionPaste;
	QAction *		m_pActionSelectAll;
	QAction *		m_pActionBold;
	QAction *		m_pActionItalic;
	QAction *		m_pActionUnderline;
	QAction *		m_pActionStrikeOut;
	QAction *		m_pActionTextFont;
	QMenuColor *		m_pMenuHighlightColor;
	QMenu *			m_pMenuBackgroundColor;

	bool			m_bFocusToContextMenu;

	QPropertyAnimation *	m_pAnimationBlink;
	bool			m_bBlinkOnMove;

	QSoundEffect		m_xSoundReminder;
	QString			m_sCurrentSoundUrl;

public:

	explicit QNotePage(const QString& sName, QWidget *parent = 0);
	virtual ~QNotePage();

	void initDefFmt(const QFont& ftFont, const QColor& clFore, const QColor& clBack);

	void setVisible(bool bVisible);

	void setPosition(const QPoint& ptPos){m_ptPos = ptPos; QWidget::move(ptPos);}
	void setPosition(int x, int y){setPosition(QPoint(x, y));}
	QPoint getPosition() const {return m_ptPos;}

	QString noteName() const {return m_sNoteName;}

	QString content() const;
	QString plainText() const;
	void setContent(const QString& sContent);

	void setTitle(const QString& sTitle);
	QString title() const;

	QColor pageColor() const;
	void setPageColor(const QColor& clPage);

	bool readOnly() const;
	void setReadOnly(bool bRO);

	QDateTime reminder() const;
	void setReminder(const QDateTime& tReminder);

	bool isAlarming() const;
	void startAlarming();
	void stopAlarming();

protected:

//	void initPageColors();

	void createActions();
	void onUIActions();

	bool isBlinking() const;
	void blink();
	void stopBlinking();

	bool isRinging() const;
	void ring();
	void stopRinging();

//	virtual void moveEvent(QMoveEvent* event);
	virtual bool eventFilter(QObject* object, QEvent* event);

signals:

	void focusIn();
	void focusOut();

	void newPage();
	void deletePage();

	void visible();
	void invisible();

protected slots:

	void onNewPage(){emit newPage();}
	void onDeletePage(){emit deletePage();}
	void onSetReminder();
	void onCancelReminder();
	void onNextColor();

	void onBold();
	void onItalic();
	void onUnderline();
	void onStrikeOut();

	void onTextFont();

	void onChangeBackground();

	void onCursorPositionChanged();
	void onUndoAvailable(bool bAvailable);
	void onRedoAvailable(bool bAvailable);
	void onCopyAvailable(bool bAvailable);
	void onCustomContextMenu(const QPoint& pos);

	void onReminderAlarm();
	void onStopReminderAlarm();

//	void onRingAboutToFinish();

	void onVisible(){emit visible();}
	void onInvisible(){QWidget::setVisible(false); emit invisible();}

};

#endif // NOTEPAGE_H
