#include <QApplication>
#include <QtGui>
#include <QMediaPlayer>
#include <QMediaContent>
#include <QMediaPlaylist>

#include "common_headers.h"
#include "MenuColor.h"
#include "DlgSetReminder.h"
#include "NotePage.h"

QNotePage::QNotePage(const QString& sName, QWidget *parent)
	: QFrame(parent)
	, m_sNoteName(sName)
	, m_ptLastMousePos(-1, -1)
	, m_bFocusToContextMenu(false)
	, m_pAnimationBlink(NULL)
	, m_bBlinkOnMove(false)
//	, m_xSoundReminder("./sounds/wav_chimes.wav")
{
//	initPageColors();

	m_xSoundReminder.setLoopCount(QSoundEffect::Infinite);
	m_xSoundReminder.setVolume(1.0);

//	m_pMediaPlayer = new QMediaPlayer(this);
//	QObject::connect(m_pMediaObject, SIGNAL(aboutToFinish()), this, SLOT(onRingAboutToFinish()));
//	Phonon::AudioOutput* pAudioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
//	Phonon::createPath(m_pMediaObject, pAudioOutput);

//	m_xSoundReminder.setLoops(-1);	//Forever

	QWidget::setAttribute(Qt::WA_DeleteOnClose, true);
#if defined(Q_OS_MAC)
	QWidget::setAttribute(Qt::WA_MacAlwaysShowToolWindow, true);
#endif
	QWidget::setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	QFrame::setFrameShape(QFrame::Box);
	QWidget::setAutoFillBackground(true);

//	m_pEditTitle = new QLineEdit(this);
//	m_pEditTitle->setText("Note 1");
//	m_pEditTitle->setFrame(false);
//	m_pEditTitle->setAlignment(Qt::AlignCenter);
//	{
//		QPalette xPal = m_pEditTitle->palette();
//		xPal.setColor(QPalette::All, QPalette::Base, Qt::transparent);
//		m_pEditTitle->setPalette(xPal);
//	}
//	m_pEditTitle->setVisible(false);

	m_pTimerReminder = new QTimer(this);
	QObject::connect(m_pTimerReminder, SIGNAL(timeout()), this, SLOT(onReminderAlarm()));

	m_pBtnNewNote = new QToolButton(this);
	m_pBtnNewNote->setVisible(false);
	m_pBtnNewNote->setIcon(QIcon(":/images/btn_new_note.png"));
	m_pBtnNewNote->setToolTip(_lc("Info.NewNote", "Create a new note beside"));
	m_pBtnNewNote->setAutoRaise(true);
	m_pBtnNewNote->setCursor(Qt::ArrowCursor);
#if defined(Q_OS_MAC)
	m_pBtnNewNote->setStyleSheet("QToolButton{background-color: transparent; border: 0px none transparent; padding: 2px;}"
		"QToolButton::hover{background-color: rgba(200, 200, 200, 150); border: 1px solid gray; border-radius: 2px;}"
		"QToolButton::pressed{background-color: rgba(150, 150, 150, 150); border: 1px solid gray; border-radius: 2px;}"
	);
#endif
	QObject::connect(m_pBtnNewNote, SIGNAL(clicked()), this, SLOT(onNewPage()));

	m_pBtnDeleteNote = new QToolButton(this);
	m_pBtnDeleteNote->setVisible(false);
	m_pBtnDeleteNote->setIcon(QIcon(":/images/btn_del_note.png"));
	m_pBtnDeleteNote->setToolTip(_lc("Info.DelNote", "Hide this note"));
	m_pBtnDeleteNote->setAutoRaise(true);
	m_pBtnDeleteNote->setCursor(Qt::ArrowCursor);
#if defined(Q_OS_MAC)
	m_pBtnDeleteNote->setStyleSheet("QToolButton{background-color: transparent; border: 0px none transparent; padding: 2px;}"
		"QToolButton::hover{background-color: rgba(200, 200, 200, 150); border: 1px solid gray; border-radius: 2px;}"
		"QToolButton::pressed{background-color: rgba(150, 150, 150, 150); border: 1px solid gray; border-radius: 2px;}"
	);
#endif
	QObject::connect(m_pBtnDeleteNote, SIGNAL(clicked()), this, SLOT(onDeletePage()));

	m_pBtnChangeColor = new QLabel(this);
	m_pBtnChangeColor->setVisible(false);
	m_pBtnChangeColor->installEventFilter(this);
	m_pBtnChangeColor->setToolTip(_lc("Info.ChangeBackground", "Click to change the background color"));

	QHBoxLayout* pLayoutHeader = new QHBoxLayout;
	pLayoutHeader->setContentsMargins(2, 2, 2, 2);
	pLayoutHeader->addWidget(m_pBtnNewNote, 0, Qt::AlignLeft | Qt::AlignVCenter);
//	pLayoutHeader->addWidget(m_pEditTitle, 0, Qt::AlignCenter);
	pLayoutHeader->addWidget(m_pBtnDeleteNote, 0, Qt::AlignRight | Qt::AlignVCenter);

	m_pWidgetTitleBar = new QWidget(this);
	m_pWidgetTitleBar->setAutoFillBackground(true);
	m_pWidgetTitleBar->installEventFilter(this);
	m_pWidgetTitleBar->setCursor(Qt::OpenHandCursor);
	m_pWidgetTitleBar->setLayout(pLayoutHeader);

	m_pRichEdit = new QTextEdit(this);
	m_pRichEdit->installEventFilter(this);
	m_pRichEdit->setFrameShape(QFrame::NoFrame);
	m_pRichEdit->setContextMenuPolicy(Qt::CustomContextMenu);
	{
		QPalette xPal = m_pRichEdit->palette();
		xPal.setColor(QPalette::All, QPalette::Base, Qt::transparent);
		m_pRichEdit->setPalette(xPal);
	}
//	{
//		QFont ftFont = QApplication::font(); ftFont.setPointSize(20);
//		m_pRichEdit->setFont(ftFont);
//	}
	m_pRichEdit->verticalScrollBar()->setVisible(false);

	QObject::connect(m_pRichEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
	QObject::connect(m_pRichEdit, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenu(QPoint)));
	createActions();

	QSizeGrip* pSizeGrip = new QSizeGrip(this);
	pSizeGrip->setStyleSheet("QSizeGrip{image: url(\":/images/ico_sizegrip.png\"); margin: 2px;}");
	pSizeGrip->setToolTip(_lc("Info.ResizeHandle", "Drag to resize note"));

	QHBoxLayout* pLayoutBottom = new QHBoxLayout;
	pLayoutBottom->setContentsMargins(2, 0, 0, 0);
	pLayoutBottom->setSpacing(0);
	pLayoutBottom->addWidget(m_pBtnChangeColor, 0, Qt::AlignBottom | Qt::AlignLeft);
	pLayoutBottom->addWidget(pSizeGrip, 0, Qt::AlignBottom | Qt::AlignRight);

	QVBoxLayout* pLayout = new QVBoxLayout;
	pLayout->setContentsMargins(0, 0, 0, 0);
	pLayout->setSpacing(0);
	pLayout->addWidget(m_pWidgetTitleBar);
	pLayout->addWidget(m_pRichEdit);
	pLayout->addLayout(pLayoutBottom);

	setPageColor(QColor(252, 250, 177));

	QWidget::setLayout(pLayout);

	m_pWidgetTitleBar->setFixedHeight(m_pWidgetTitleBar->height());
}

QNotePage::~QNotePage()
{
	m_pTimerReminder->stop();
	stopAlarming();
}

void QNotePage::initDefFmt(const QFont& ftFont, const QColor& clFore, const QColor& clBack)
{
	m_pRichEdit->setFont(ftFont);
	m_pRichEdit->setTextColor(clFore);
	setPageColor(clBack);
}

void QNotePage::setVisible(bool bVisible)
{
	if(bVisible != QWidget::isVisible()){
		QPropertyAnimation* pAnim = new QPropertyAnimation(this, "windowOpacity", this);
		pAnim->setDuration(200);
		if(bVisible){
			QWidget::setVisible(true);

			pAnim->setStartValue(QVariant(0.0));
			pAnim->setEndValue(QVariant(QWidget::hasFocus() ? 1.0 : 0.6));
			QObject::connect(pAnim, SIGNAL(finished()), this, SLOT(onVisible()));
		}else{
			pAnim->setStartValue(QVariant(QWidget::windowOpacity()));
			pAnim->setEndValue(QVariant(0.0));
			QObject::connect(pAnim, SIGNAL(finished()), this, SLOT(onInvisible()));
		}
		pAnim->start(QAbstractAnimation::DeleteWhenStopped);
	}
}

//void QNotePage::initPageColors()
//{
//	g_xOpt.m_mPageColors[QString("Yellow")] = QColor(252, 250, 177);
//	g_xOpt.m_mPageColors[QString("Red")] = QColor(254, 143, 152);
//	g_xOpt.m_mPageColors[QString("Pink")] = QColor(242, 198, 242);
//	g_xOpt.m_mPageColors[QString("Green")] = QColor(194, 244, 190);
//	g_xOpt.m_mPageColors[QString("Blue")] = QColor(192, 225, 245);
//	g_xOpt.m_mPageColors[QString("Aqua")] = QColor(158, 233, 226);
//	g_xOpt.m_mPageColors[QString("White")] = QColor(237, 237, 237);
//}

void QNotePage::createActions()
{
	m_pActionNewPage = new QAction(this);
	m_pActionNewPage->setText(_lc("Menu.NewNote", "&New Note"));
	m_pActionNewPage->setIcon(QIcon(":/images/btn_new_note.png"));
	QObject::connect(m_pActionNewPage, SIGNAL(triggered()), this, SLOT(onNewPage()));

	m_pActionDeletePage = new QAction(this);
	m_pActionDeletePage->setText(_lc("Menu.DelNote", "&Delete Note"));
	m_pActionDeletePage->setIcon(QIcon(":/images/btn_del_note.png"));
	QObject::connect(m_pActionDeletePage, SIGNAL(triggered()), this, SLOT(onDeletePage()));

	m_pActionSetReminder = new QAction(this);
	m_pActionSetReminder->setText(_lc("Menu.Note.SetReminder", "Set Re&minder ..."));
	m_pActionSetReminder->setIcon(QIcon(":/images/btn_set_reminder.png"));
	QObject::connect(m_pActionSetReminder, SIGNAL(triggered()), this, SLOT(onSetReminder()));

	m_pActionCancelReminder = new QAction(this);
	m_pActionCancelReminder->setText(_lc("Menu.Note.CancelReminder", "Cance&l Reminder ..."));
	m_pActionCancelReminder->setEnabled(false);
	QObject::connect(m_pActionCancelReminder, SIGNAL(triggered()), this, SLOT(onCancelReminder()));

	m_pActionUndo = new QAction(this);
	m_pActionUndo->setText(_lc("Menu.Edit.Undo", "&Undo"));
	m_pActionUndo->setShortcut(QKeySequence::Undo);
	m_pActionUndo->setIcon(QIcon(":/images/btn_undo.png"));
	m_pActionUndo->setEnabled(false);
	QObject::connect(m_pActionUndo, SIGNAL(triggered()), m_pRichEdit, SLOT(undo()));

	m_pActionRedo = new QAction(this);
	m_pActionRedo->setText(_lc("Menu.Edit.Redo", "&Redo"));
	m_pActionRedo->setShortcut(QKeySequence::Redo);
	m_pActionRedo->setIcon(QIcon(":/images/btn_redo.png"));
	m_pActionRedo->setEnabled(false);
	QObject::connect(m_pActionRedo, SIGNAL(triggered()), m_pRichEdit, SLOT(redo()));

	m_pActionCut = new QAction(this);
	m_pActionCut->setText(_lc("Menu.Edit.Cut", "Cu&t"));
	m_pActionCut->setShortcut(QKeySequence::Cut);
	m_pActionCut->setIcon(QIcon(":/images/btn_cut.png"));
	QObject::connect(m_pActionCut, SIGNAL(triggered()), m_pRichEdit, SLOT(cut()));

	m_pActionCopy = new QAction(this);
	m_pActionCopy->setText(_lc("Menu.Edit.Copy", "&Copy"));
	m_pActionCopy->setShortcut(QKeySequence::Copy);
	m_pActionCopy->setIcon(QIcon(":/images/btn_copy.png"));
	m_pActionCopy->setEnabled(false);
	QObject::connect(m_pActionCopy, SIGNAL(triggered()), m_pRichEdit, SLOT(copy()));

	m_pActionPaste = new QAction(this);
	m_pActionPaste->setText(_lc("Menu.Edit.Paste", "&Paste"));
	m_pActionPaste->setShortcut(QKeySequence::Paste);
	m_pActionPaste->setIcon(QIcon(":/images/btn_paste.png"));
	QObject::connect(m_pActionPaste, SIGNAL(triggered()), m_pRichEdit, SLOT(paste()));

	m_pActionSelectAll = new QAction(this);
	m_pActionSelectAll->setText(_lc("Menu.Edit.SelectAll", "&Select All"));
	m_pActionSelectAll->setShortcut(QKeySequence::SelectAll);
	QObject::connect(m_pActionPaste, SIGNAL(triggered()), m_pRichEdit, SLOT(selectAll()));

	m_pActionBold = new QAction(this);
	m_pActionBold->setText(_lc("Menu.Format.Bold", "&Bold"));
	m_pActionBold->setCheckable(true);
	m_pActionBold->setShortcut(QKeySequence::Bold);
	m_pActionBold->setIcon(QIcon(":/images/btn_bold.png"));
	QObject::connect(m_pActionBold, SIGNAL(triggered()), this, SLOT(onBold()));

	m_pActionItalic = new QAction(this);
	m_pActionItalic->setText(_lc("Menu.Format.Italic", "&Italic"));
	m_pActionItalic->setCheckable(true);
	m_pActionItalic->setShortcut(QKeySequence::Italic);
	m_pActionItalic->setIcon(QIcon(":/images/btn_italic.png"));
	QObject::connect(m_pActionItalic, SIGNAL(triggered()), this, SLOT(onItalic()));

	m_pActionUnderline = new QAction(this);
	m_pActionUnderline->setText(_lc("Menu.Format.Underline", "Und&erline"));
	m_pActionUnderline->setCheckable(true);
	m_pActionUnderline->setShortcut(QKeySequence::Underline);
	m_pActionUnderline->setIcon(QIcon(":/images/btn_underline.png"));
	QObject::connect(m_pActionUnderline, SIGNAL(triggered()), this, SLOT(onUnderline()));

	m_pActionStrikeOut = new QAction(this);
	m_pActionStrikeOut->setText(_lc("Menu.Format.Strikeout", "Strike&out"));
	m_pActionStrikeOut->setCheckable(true);
	m_pActionStrikeOut->setShortcut(QKeySequence("Ctrl+K"));
	m_pActionStrikeOut->setIcon(QIcon(":/images/btn_strikeout.png"));
	QObject::connect(m_pActionStrikeOut, SIGNAL(triggered()), this, SLOT(onStrikeOut()));

	m_pActionTextFont = new QAction(this);
	m_pActionTextFont->setText(_lc("Menu.Format.Font", "&Font ..."));
	m_pActionTextFont->setIcon(QIcon(":/images/btn_font.png"));
	QObject::connect(m_pActionTextFont, SIGNAL(triggered()), this, SLOT(onTextFont()));

	m_pMenuHighlightColor = new QMenuColor(this);
	m_pMenuHighlightColor->setTitle(_lc("Menu.Format.Highlight", "&Highlight ..."));
	m_pMenuHighlightColor->setIcon(QIcon(":/images/btn_highlight.png"));
	QObject::connect(m_pMenuHighlightColor, SIGNAL(colorChose(QColor)), m_pRichEdit, SLOT(setTextColor(QColor)));

	m_pMenuBackgroundColor = new QMenu(this);
	m_pMenuBackgroundColor->setTitle(_lc("Menu.Format.Background", "B&ackground ..."));
	m_pMenuBackgroundColor->setIcon(QIcon(":/images/btn_background.png"));
	_CPairVector<QString, QColor>::const_iterator it;
	for(it = g_xOpt.m_vPageColors.begin(); it != g_xOpt.m_vPageColors.end(); it++){
		QAction* pActionColor = new QAction(this);
		pActionColor->setText(it->first);

		QSize szIcon = QSize(16, 14);
		{
			QPixmap xIcon(szIcon.width(), szIcon.height()); xIcon.fill(Qt::transparent);
			QPainter xPainter(&xIcon);
			xPainter.setPen(QPen(QBrush(Qt::black), 1));
			xPainter.setBrush(QBrush(it->second));
			xPainter.drawRect(0, 0, szIcon.width() - 1, szIcon.height() - 1);

			pActionColor->setIcon(QIcon(xIcon));
		}

		QVariant v; v.setValue(it->second);
		pActionColor->setData(v);

		QObject::connect(pActionColor, SIGNAL(triggered()), this, SLOT(onChangeBackground()));

		m_pMenuBackgroundColor->addAction(pActionColor);
	}

	QObject::connect(m_pRichEdit, SIGNAL(undoAvailable(bool)), this, SLOT(onUndoAvailable(bool)));
	QObject::connect(m_pRichEdit, SIGNAL(redoAvailable(bool)), this, SLOT(onRedoAvailable(bool)));
	QObject::connect(m_pRichEdit, SIGNAL(copyAvailable(bool)), this, SLOT(onCopyAvailable(bool)));
}

void QNotePage::onUIActions()
{
	bool bEditable = !m_pRichEdit->isReadOnly();

	m_pActionPaste->setEnabled(bEditable && m_pRichEdit->canPaste());
	m_pActionBold->setEnabled(bEditable);
	m_pActionItalic->setEnabled(bEditable);
	m_pActionUnderline->setEnabled(bEditable);
	m_pActionStrikeOut->setEnabled(bEditable);
	m_pActionTextFont->setEnabled(bEditable);
	m_pMenuHighlightColor->setEnabled(bEditable);
	m_pMenuBackgroundColor->setEnabled(bEditable);
}

void QNotePage::setContent(const QString& sContent)
{
	m_pRichEdit->setHtml(sContent);
}

QString QNotePage::content() const
{
	return m_pRichEdit->toHtml();
}

QString QNotePage::plainText() const
{
	return m_pRichEdit->toPlainText();
}

void QNotePage::setTitle(const QString& sTitle)
{
//	m_pEditTitle->setText(sTitle);
	Q_UNUSED(sTitle);
}

QString QNotePage::title() const
{
//	return m_pEditTitle->text();
	return "";
}

QColor QNotePage::pageColor() const
{
	QPalette xPal = QWidget::palette();
	return xPal.color(QPalette::Window);
}

void QNotePage::setPageColor(const QColor& clPage)
{
	{
		QPalette xPal = QWidget::palette();
		xPal.setColor(QPalette::All, QPalette::Window, clPage);
		QWidget::setPalette(xPal);
	}
	{
		QPalette xPal = m_pWidgetTitleBar->palette();
		xPal.setColor(QPalette::All, QPalette::Window, clPage.darker(105));
		m_pWidgetTitleBar->setPalette(xPal);
	}

	QPixmap xPixmap(14, 14); xPixmap.fill(Qt::transparent);
	QPainter xPainter(&xPixmap);
	xPainter.setBrush(QBrush(Qt::transparent));
	xPainter.setPen(QPen(Qt::black));
	{
		QPainterPath xPath;
		xPath.lineTo(0, 10);
		xPath.lineTo(10, 10);
		xPath.lineTo(0, 0);
		xPainter.drawPath(xPath);
	}

	m_pBtnChangeColor->setPixmap(xPixmap);

	{
		QString sQss = "QScrollBar{border: none; width: 12px; background: transparent;}"
				"QScrollBar::handle{border: 1px solid %clHandle%; border-radius: 2px; background: %clHandle%; width: 12px;}"
				"QScrollBar::add-line{background: transparent;}"
				"QScrollBar::sub-line{background: transparent;}"
				"QScrollBar::add-page{background: transparent;}"
				"QScrollBar::sub-page{background: transparent;}"
				;
		sQss.replace("%clHandle%", clPage.darker(200).name());
		m_pRichEdit->setStyleSheet(sQss);
	}
}

bool QNotePage::readOnly() const
{
	return m_pRichEdit->isReadOnly();
}

void QNotePage::setReadOnly(bool bRO)
{
	m_pRichEdit->setReadOnly(bRO);
}

QDateTime QNotePage::reminder() const
{
	return m_tReminder;
}

void QNotePage::setReminder(const QDateTime& tReminder)
{
	m_tReminder = tReminder;
	m_pActionCancelReminder->setEnabled((m_tReminder.toTime_t() != 0));
	m_pTimerReminder->stop();
	if(tReminder.toTime_t() > 0){
		int nSecs = QDateTime::currentDateTime().secsTo(m_tReminder);
		if(nSecs > 0){
			m_pTimerReminder->start(nSecs * 1000);
		}else{
			onReminderAlarm();
		}
	}
}

bool QNotePage::isAlarming() const
{
	return (isBlinking() || isRinging());
}

void QNotePage::startAlarming()
{
	if(!isAlarming()){
		blink();
		ring();
	}
}

void QNotePage::stopAlarming()
{
	if(isAlarming()){
		stopBlinking();
		stopRinging();
	}
}

void QNotePage::onReminderAlarm()
{
	m_pTimerReminder->stop();
	startAlarming();
	if(g_xOpt.m_nReminder_RingInterval > 0) QTimer::singleShot(g_xOpt.m_nReminder_RingDuration, this, SLOT(onStopReminderAlarm()));
}

void QNotePage::onStopReminderAlarm()
{
	stopAlarming();
	if(g_xOpt.m_nReminder_RingInterval > 0){
		QTimer::singleShot(g_xOpt.m_nReminder_RingInterval, this, SLOT(onReminderAlarm()));
	}else{
		onReminderAlarm();
	}
}

bool QNotePage::isBlinking() const
{
	return (m_pAnimationBlink!=NULL);
}

void QNotePage::blink()
{
	if(!m_pAnimationBlink){
		m_pAnimationBlink = new QPropertyAnimation(this, "geometry", this);
		m_pAnimationBlink->setDuration(200);
		m_pAnimationBlink->setLoopCount(-1);	//animate forever

		int k = g_xOpt.m_nReminder_ShockFactor, x = QWidget::x(), y = QWidget::y(), w = QWidget::width(), h = QWidget::height();
		for(int i = 0; i <= 20; i++){
			double nStep = ((double)i)/10.0/2;
			double nAngle = ((double)i)/10.0*M_PI;
			double nOffset = sin(nAngle)*k;

			m_pAnimationBlink->setKeyValueAt(nStep, QVariant(QRect(x - nOffset, y, w, h)));
		}

		m_pAnimationBlink->start(QAbstractAnimation::DeleteWhenStopped);
	}
}

void QNotePage::stopBlinking()
{
	if(m_pAnimationBlink){
		m_pAnimationBlink->stop();
		m_pAnimationBlink = NULL;
	}
	QWidget::move(m_ptPos);
}

bool QNotePage::isRinging() const
{
	return !m_sCurrentSoundUrl.isEmpty();
}

void QNotePage::ring()
{
	uint nSound = g_xOpt.m_nReminder_RingSound;
	const QMap<uint, QString>& m = g_xOpt.m_mReminderSounds;
	QMap<uint, QString>::const_iterator it = m.find(nSound);
	if(it != m.end()){
		m_sCurrentSoundUrl = it.value();
//		m_pMediaObject->setCurrentSource(Phonon::MediaSource(m_sCurrentSoundUrl));
//		m_pMediaPlayer->setMedia(QMediaContent(m_sCurrentSoundUrl));
//		m_pMediaPlayer->playlist()->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
//		m_pMediaPlayer->play();
		if(m_xSoundReminder.isPlaying()) m_xSoundReminder.stop();
		m_xSoundReminder.setSource(QUrl::fromLocalFile(m_sCurrentSoundUrl));
		m_xSoundReminder.play();
	}else{
		stopRinging();
	}
}

void QNotePage::stopRinging()
{
	m_sCurrentSoundUrl = "";
	if(m_xSoundReminder.isPlaying()) m_xSoundReminder.stop();
//	m_pMediaPlayer->stop();
//	m_pMediaPlayer->playlist()->clear();
//	m_pMediaObject->clearQueue();
}

//void QNotePage::onRingAboutToFinish()
//{
//	if(!m_sCurrentSoundUrl.isEmpty()){
//		m_pMediaObject->enqueue(Phonon::MediaSource(m_sCurrentSoundUrl));
//	}
//}

bool QNotePage::eventFilter(QObject* object, QEvent* event)
{
	if(object == m_pWidgetTitleBar){
		if(event->type() == QEvent::MouseButtonPress){
			QMouseEvent* pME = static_cast<QMouseEvent*>(event);
			if(pME){
				m_ptLastMousePos = pME->globalPos();
				m_pWidgetTitleBar->setCursor(Qt::ClosedHandCursor);
				if(isBlinking()){
					stopBlinking();
					m_bBlinkOnMove = true;
				}
			}
		}else if(event->type() == QEvent::MouseButtonRelease){
			QMouseEvent* pME = static_cast<QMouseEvent*>(event);
			if(pME){
				m_ptLastMousePos = QPoint(-1, -1);
				m_pWidgetTitleBar->setCursor(Qt::OpenHandCursor);
				if(m_bBlinkOnMove){
					blink();
					m_bBlinkOnMove = false;
				}
			}
		}else if(event->type() == QEvent::MouseMove){
			QMouseEvent* pME = static_cast<QMouseEvent*>(event);
			if(pME){
				if(m_ptLastMousePos.x() >= 0 && m_ptLastMousePos.y() >= 0){
					int x = QWidget::x() + (pME->globalPos().x() - m_ptLastMousePos.x());
					int y = QWidget::y() + (pME->globalPos().y() - m_ptLastMousePos.y());

					setPosition(x, y);

					m_ptLastMousePos = pME->globalPos();
				}
			}
		}
	}else if(object == m_pRichEdit){
		if(event->type() == QEvent::FocusIn && !m_bFocusToContextMenu){
			m_pBtnNewNote->setVisible(true);
			m_pBtnDeleteNote->setVisible(true);
			m_pBtnChangeColor->setVisible(true);
			m_pRichEdit->verticalScrollBar()->setVisible(true);

			QPropertyAnimation* pAnim = new QPropertyAnimation(this, "windowOpacity", this);
			pAnim->setDuration(200);

			pAnim->setStartValue(QVariant(QWidget::windowOpacity()));
			pAnim->setEndValue(QVariant(1.0));

			pAnim->start(QAbstractAnimation::DeleteWhenStopped);

			if(isAlarming()){
				stopAlarming();
				setReminder(QDateTime::fromTime_t(0));
			}

			emit focusIn();
		}else if(event->type() == QEvent::FocusOut && !m_bFocusToContextMenu){
			m_pBtnNewNote->setVisible(false);
			m_pBtnDeleteNote->setVisible(false);
			m_pBtnChangeColor->setVisible(false);
			m_pRichEdit->verticalScrollBar()->setVisible(false);

			QPropertyAnimation* pAnim = new QPropertyAnimation(this, "windowOpacity", this);
			pAnim->setDuration(200);

			pAnim->setStartValue(QVariant(QWidget::windowOpacity()));
			pAnim->setEndValue(QVariant(0.6));

			pAnim->start(QAbstractAnimation::DeleteWhenStopped);

			emit focusOut();
		}
	}else if(object == m_pBtnChangeColor){
		if(event->type() == QEvent::MouseButtonRelease && !readOnly()){
			onNextColor();
		}
	}

	return QWidget::eventFilter(object, event);
}

void QNotePage::onSetReminder()
{
	QDateTime tDef = QDateTime::currentDateTime();
	if(m_tReminder.toTime_t() > 0){
		tDef = m_tReminder;
	}
	QDlgSetReminder dlg(tDef, NULL);
	if(dlg.exec()){
		setReminder(dlg.m_tReminder);
	}
}

void QNotePage::onCancelReminder()
{
	setReminder(QDateTime::fromTime_t(0));
}

void QNotePage::onNextColor()
{
	QColor clPage(Qt::transparent);
	QList<QColor> vPageColors;
	{
		_CPairVector<QString, QColor>::const_iterator it;
		for(it = g_xOpt.m_vPageColors.begin(); it != g_xOpt.m_vPageColors.end(); it++){
			vPageColors << it->second;
		}
	}
	int nPos = vPageColors.indexOf(pageColor());
	if(nPos < 0 || nPos >= vPageColors.size() - 1){
		clPage = vPageColors[0];
	}else{
		clPage = vPageColors[nPos + 1];
	}

	setPageColor(clPage);
}

void QNotePage::onBold()
{
	m_pRichEdit->setFontWeight(m_pRichEdit->fontWeight() == QFont::Bold ? QFont::Normal : QFont::Bold);
}

void QNotePage::onItalic()
{
	m_pRichEdit->setFontItalic(!m_pRichEdit->fontItalic());
}

void QNotePage::onUnderline()
{
	m_pRichEdit->setFontUnderline(!m_pRichEdit->fontUnderline());
}

void QNotePage::onStrikeOut()
{
	QFont font = m_pRichEdit->currentFont();
	font.setStrikeOut(!font.strikeOut());
	m_pRichEdit->setCurrentFont(font);
}

void QNotePage::onTextFont()
{
	QFont font;
	QFontDialog dlg(m_pRichEdit->currentFont(), NULL);
	dlg.setWindowFlags(Qt::WindowStaysOnTopHint);
	dlg.setWindowTitle(_lc("Dlg.Font.Format", "Select a font for the selected text"));
	if(dlg.exec() == QFontDialog::Accepted){
		font = dlg.selectedFont();
		m_pRichEdit->setCurrentFont(font);
	}
}

void QNotePage::onChangeBackground()
{
	QAction* pActionColor = qobject_cast<QAction*>(QObject::sender());
	if(pActionColor){
		QColor clColor = pActionColor->data().value<QColor>();
		if(clColor.isValid()){
			setPageColor(clColor);
		}
	}
}

void QNotePage::onCursorPositionChanged()
{
	m_pActionBold->setChecked(m_pRichEdit->fontWeight() == QFont::Bold);
	m_pActionItalic->setChecked(m_pRichEdit->fontItalic());
	m_pActionUnderline->setChecked(m_pRichEdit->fontUnderline());
	m_pActionStrikeOut->setChecked(m_pRichEdit->currentFont().strikeOut());
}

void QNotePage::onUndoAvailable(bool bAvailable)
{
	m_pActionUndo->setEnabled(bAvailable && !m_pRichEdit->isReadOnly());
}

void QNotePage::onRedoAvailable(bool bAvailable)
{
	m_pActionRedo->setEnabled(bAvailable && !m_pRichEdit->isReadOnly());
}

void QNotePage::onCopyAvailable(bool bAvailable)
{
	m_pActionCut->setEnabled(bAvailable && !m_pRichEdit->isReadOnly());
	m_pActionCopy->setEnabled(bAvailable);
}

void QNotePage::onCustomContextMenu(const QPoint& pos)
{
	onUIActions();

	QMenu* pMenu = new QMenu(this);
	pMenu->addAction(m_pActionNewPage);
	pMenu->addAction(m_pActionDeletePage);
	pMenu->addSeparator();
	pMenu->addAction(m_pActionSetReminder);
	pMenu->addAction(m_pActionCancelReminder);
	pMenu->addSeparator();
	pMenu->addAction(m_pActionUndo);
	pMenu->addAction(m_pActionRedo);
	pMenu->addSeparator();
	pMenu->addAction(m_pActionCut);
	pMenu->addAction(m_pActionCopy);
	pMenu->addAction(m_pActionPaste);
	pMenu->addSeparator();
	pMenu->addAction(m_pActionSelectAll);
	pMenu->addSeparator();
	pMenu->addAction(m_pActionBold);
	pMenu->addAction(m_pActionItalic);
	pMenu->addAction(m_pActionUnderline);
	pMenu->addAction(m_pActionStrikeOut);
	pMenu->addSeparator();
	pMenu->addAction(m_pActionTextFont);
	pMenu->addSeparator();
	pMenu->addMenu(m_pMenuHighlightColor);
	pMenu->addMenu(m_pMenuBackgroundColor);

	_CTempValueChange<bool> _tvc(m_bFocusToContextMenu, true); Q_UNUSED(_tvc);
	pMenu->exec(m_pRichEdit->mapToGlobal(pos));
	pMenu->deleteLater();
}
