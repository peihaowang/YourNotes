#include <QEvent>

#include "common_headers.h"
#include "NoteStg.h"

#include "NotePage.h"
#include "NoteList.h"
#include "OptionsPage.h"
#include "HelpWidget.h"
#include "MyStyle.h"
#include "MainWindow.h"

///////////////////////////////////////////////////////////

QUnifiedTabWidget::QUnifiedTabWidget(QWidget* parent)
	: QWidget(parent)
	, m_ptLastMousePressed(-1, -1)
{
	QHBoxLayout* pLayoutLogo = new QHBoxLayout;
	pLayoutLogo->setContentsMargins(10, 0, 0, 0);
	pLayoutLogo->setSpacing(5);
	{
		QLabel* pLogoIcon = new QLabel(this);
		pLogoIcon->setPixmap(QIcon(":/images/ico_logo.ico").pixmap(32, 32));

		QLabel* pLogoText = new QLabel(_APP_NAME, this);
		pLogoText->setStyleSheet("QLabel{font-weight:bold;}");

		pLayoutLogo->addWidget(pLogoIcon);
		pLayoutLogo->addWidget(pLogoText);
	}

	m_pBtnMinimize = new QToolButton(this);
	m_pBtnMinimize->setAutoRaise(true);
	m_pBtnMinimize->setToolTip(_lc("App.Minimize", "Minimize"));
//	m_pBtnMinimize->setIcon(QWidget::style()->standardIcon(QStyle::SP_TitleBarMinButton));
	m_pBtnMinimize->setIcon(QIcon(":/images/btn_minimize.png"));
#if defined(Q_OS_MAC)
    m_pBtnMinimize->setStyleSheet("QToolButton{background-color: transparent; border: 0px none transparent; padding: 2px;}"
        "QToolButton::hover{background-color: rgba(200, 200, 200, 150); border: 1px solid gray; border-radius: 2px;}"
        "QToolButton::pressed{background-color: rgba(150, 150, 150, 150); border: 1px solid gray; border-radius: 2px;}"
    );
#endif
    QObject::connect(m_pBtnMinimize, SIGNAL(clicked()), this, SLOT(onMinBtnClicked()));

	m_pBtnClose = new QToolButton(this);
	m_pBtnClose->setAutoRaise(true);
	m_pBtnClose->setToolTip(_lc("App.Close", "Close"));
//	m_pBtnClose->setIcon(QWidget::style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	m_pBtnClose->setIcon(QIcon(":/images/btn_close.png"));
#if defined(Q_OS_MAC)
    m_pBtnClose->setStyleSheet("QToolButton{background-color: transparent; border: 0px none transparent; padding: 2px;}"
        "QToolButton::hover{background-color: rgba(200, 200, 200, 150); border: 1px solid gray; border-radius: 2px;}"
        "QToolButton::pressed{background-color: rgba(150, 150, 150, 150); border: 1px solid gray; border-radius: 2px;}"
    );
#endif
    QObject::connect(m_pBtnClose, SIGNAL(clicked()), this, SLOT(onCloseBtnClicked()));

	QHBoxLayout* pLayoutSysBtns = new QHBoxLayout;
	pLayoutSysBtns->setContentsMargins(0, 0, 10, 0);
	pLayoutSysBtns->setSpacing(5);
	pLayoutSysBtns->addWidget(m_pBtnMinimize);
	pLayoutSysBtns->addWidget(m_pBtnClose);

	m_pGrpTabButtons = new QButtonGroup(this);
	m_pGrpTabButtons->setExclusive(true);

	m_pLayoutTabButtons = new QHBoxLayout;
	m_pLayoutTabButtons->setContentsMargins(0, 0, 0, 0);
	m_pLayoutTabButtons->setSpacing(0);
	m_pLayoutTabButtons->setAlignment(Qt::AlignLeft);

	QHBoxLayout* pLayoutTabBar = new QHBoxLayout;
	pLayoutTabBar->setContentsMargins(0, 0, 0, 1);
	pLayoutTabBar->setSpacing(20);
	pLayoutTabBar->addLayout(pLayoutLogo);
	pLayoutTabBar->addLayout(m_pLayoutTabButtons);
	pLayoutTabBar->addStretch();
	pLayoutTabBar->addLayout(pLayoutSysBtns);

	m_pWidgetTabBar = new QWidget(this);
	m_pWidgetTabBar->setLayout(pLayoutTabBar);
	m_pWidgetTabBar->setStyleSheet(".QWidget{background:white; border-bottom: 1px solid gray;}");
	m_pWidgetTabBar->setMouseTracking(true);
	m_pWidgetTabBar->installEventFilter(this);

	m_pLayoutWidgets = new QStackedLayout;

	QVBoxLayout* pLayoutCentral = new QVBoxLayout;
	pLayoutCentral->setSpacing(0);
	pLayoutCentral->setContentsMargins(0, 0, 0, 0);
	pLayoutCentral->addWidget(m_pWidgetTabBar);
	pLayoutCentral->addLayout(m_pLayoutWidgets);

	QWidget::setLayout(pLayoutCentral);
}


bool QUnifiedTabWidget::eventFilter(QObject* object, QEvent* event)
{
	if(object == m_pWidgetTabBar){
		if(event->type() == QEvent::MouseButtonPress){
			QMouseEvent* pME = static_cast<QMouseEvent*>(event);
			m_ptLastMousePressed = pME->globalPos();
		}else if(event->type() == QEvent::MouseButtonRelease){
			m_ptLastMousePressed = QPoint(-1, -1);
		}else if(event->type() == QEvent::MouseMove){
			if(m_ptLastMousePressed.x() >= 0 && m_ptLastMousePressed.y() >= 0){
				QMouseEvent* pME = static_cast<QMouseEvent*>(event);
				QPoint ptMouse = pME->globalPos();

				QWidget* pParent = QWidget::parentWidget();
				if(pParent){
					pParent->move(pParent->x() + (ptMouse.x() - m_ptLastMousePressed.x()), pParent->y() + (ptMouse.y() - m_ptLastMousePressed.y()));
				}
				m_ptLastMousePressed = ptMouse;
			}
		}
	}
	return QWidget::eventFilter(object, event);
}

int QUnifiedTabWidget::count() const
{
	return m_pLayoutWidgets->count();
}

int QUnifiedTabWidget::currentIndext() const
{
	return m_pLayoutWidgets->currentIndex();
}

void QUnifiedTabWidget::setCurrentIndex(int nIndex)
{
	if(nIndex >= 0 && nIndex < count()){
		QToolButton* pBtnTab = m_vListTabButtons[nIndex];
		if(pBtnTab){
			pBtnTab->setChecked(true);
		}

		m_pLayoutWidgets->setCurrentIndex(nIndex);
		emit currentIndexChanged(nIndex);
	}
}

void QUnifiedTabWidget::addTab(QWidget* pWidget, const QIcon& xIcon, const QString& sText)
{
	QToolButton* pBtnTab = new QToolButton(this);
	pBtnTab->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	pBtnTab->setCheckable(true);
	pBtnTab->setIconSize(QSize(24, 24));
	pBtnTab->setFixedHeight(50);
	pBtnTab->setIcon(xIcon);
	pBtnTab->setText(sText);
	if(count() == 0){
//		pBtnTab->setStyleSheet("QToolButton{border: 0px none transparent; border-left:1px solid gray; border-right:1px solid gray; color:black; background-color:lightgray;}"
//					"QToolButton::checked, QToolButton::pressed{border: 0px none transparent; border-left:1px solid gray; border-right:1px solid gray; color:white; background-color:darkgray; font-weight: bold;}"
//		);
		pBtnTab->setStyleSheet("QToolButton{border: 0px none transparent; border-left:1px solid gray; border-right:1px solid gray; color:black; background-color:white;}"
					"QToolButton::hover{border: 0px none transparent; border-left:1px solid gray; border-right:1px solid gray; color:black; background-color:#dbedff;}"
					"QToolButton::checked, QToolButton::pressed{border: 0px none transparent; border-left:1px solid gray; border-right:1px solid gray; color:white; background-color:#3399ff; font-weight: bold;}"
		);
	}else{
//		pBtnTab->setStyleSheet("QToolButton{border: 0px none transparent; border-right:1px solid gray; color:black; background-color:lightgray;}"
//					"QToolButton::checked, QToolButton::pressed{border: 0px none transparent; border-right:1px solid gray; color:white; background-color:darkgray; font-weight: bold;}"
//		);
		pBtnTab->setStyleSheet("QToolButton{border: 0px none transparent; border-right:1px solid gray; color:black; background-color:white;}"
					"QToolButton::hover{border: 0px none transparent; border-right:1px solid gray; color:black; background-color:#dbedff;}"
					"QToolButton::checked, QToolButton::pressed{border: 0px none transparent; border-right:1px solid gray; color:white; background-color:#3399ff; font-weight: bold;}"
		);
	}
	QObject::connect(pBtnTab, SIGNAL(clicked()), this, SLOT(onTabButtonClicked()));

	m_pLayoutTabButtons->addWidget(pBtnTab);
	m_vListTabButtons << pBtnTab;
	m_pGrpTabButtons->addButton(pBtnTab);

	m_pLayoutWidgets->addWidget(pWidget);

	if(count() == 1){
		setCurrentIndex(0);
	}
}

void QUnifiedTabWidget::removeTab(QWidget* pWidget)
{
	int nIndex = m_pLayoutWidgets->indexOf(pWidget);
	if(nIndex >= 0 && nIndex < count()){
		QToolButton* pBtnTab = m_vListTabButtons[nIndex];
		if(pBtnTab){
			m_pLayoutTabButtons->removeWidget(pBtnTab);
			m_vListTabButtons.removeAll(pBtnTab);
			m_pGrpTabButtons->removeButton(pBtnTab);
		}

		m_pLayoutWidgets->removeWidget(pWidget);
	}
}

void QUnifiedTabWidget::onTabButtonClicked()
{
	QToolButton* pBtnTab = qobject_cast<QToolButton*>(QObject::sender());
	if(pBtnTab){
		int nIndex = m_vListTabButtons.indexOf(pBtnTab);
		if(nIndex >= 0){
			setCurrentIndex(nIndex);
		}
	}
}

///////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	QMainWindow::setWindowTitle(QString(_APP_TITLE_FULL));
	QMainWindow::setWindowIcon(QIcon(":/images/ico_logo.ico"));
	QApplication::setQuitOnLastWindowClosed(false);
//	QApplication::setStyle(new QMyScrollBarStyle);

	refreshInterface();

	m_pStg = new _CNoteStg;
	QString sFldPath = _CLocalFile::concatenate(QDir::homePath(), ".YourNotesStorage");
	if(!QFileInfo(sFldPath).exists()){
		m_pStg->createStg(sFldPath);
	}
	if(QFileInfo(sFldPath).exists() && !m_pStg->isOpen()){
		m_pStg->openStg(sFldPath);
	}
	if(!m_pStg->isOpen()){
		QMessageBox::critical(NULL, _APP_TITLE, _lc("Prompt.Warn.AccessDb", "Cannot access the database."));
		QApplication::quit();
	}

	m_pActionNewNote = new QAction(_lc("Menu.NewNote", "&New Note"), this);
	m_pActionNewNote->setIcon(QIcon(":/images/btn_new_note.png"));
	QObject::connect(m_pActionNewNote, SIGNAL(triggered()), this, SLOT(onNewNote()));

	m_pActionManageNotes = new QAction(_lc("Menu.ManageNotes", "&Manage Notes ..."), this);
	QObject::connect(m_pActionManageNotes, SIGNAL(triggered()), this, SLOT(onManageNotes()));

	m_pActionOptions = new QAction(_lc("Menu.Options", "&Options ..."), this);
	QObject::connect(m_pActionOptions, SIGNAL(triggered()), this, SLOT(onOptions()));

	m_pActionHelp = new QAction(_lc("Menu.Help", "&Help ..."), this);
	QObject::connect(m_pActionHelp, SIGNAL(triggered()), this, SLOT(onHelp()));

	m_pActionAbout = new QAction(_lc("Menu.About", "&About ..."), this);
	m_pActionAbout->setIcon(QIcon(":/images/btn_about.png"));
	QObject::connect(m_pActionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

	m_pActionExit = new QAction(_lc("Menu.Exit", "&Exit"), this);
	QObject::connect(m_pActionExit, SIGNAL(triggered()), this, SLOT(onExit()));

	QMenu* pMenu = new QMenu(this);
	pMenu->addAction(m_pActionNewNote);
	pMenu->addSeparator();
	pMenu->addAction(m_pActionManageNotes);
	pMenu->addAction(m_pActionOptions);
	pMenu->addAction(m_pActionHelp);
	pMenu->addSeparator();
	pMenu->addAction(m_pActionAbout);
	pMenu->addSeparator();
	pMenu->addAction(m_pActionExit);

	m_pTrayIcon = new QSystemTrayIcon(QIcon(":/images/ico_logo.ico"), this);
	m_pTrayIcon->setContextMenu(pMenu);
	m_pTrayIcon->show();
	m_pTrayIcon->showMessage(_APP_NAME, _lc("Prompt.Info.StartUp", "YourNotes has started up."));
	QObject::connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayIconActivate(QSystemTrayIcon::ActivationReason)));

	m_pNoteList = new QNoteList(m_pStg, this);
	m_pNoteList->setFrameShape(QFrame::NoFrame);
	QObject::connect(m_pNoteList, SIGNAL(noteVisibilityChanged(QString,bool)), this, SLOT(onNoteVisibilityChanged(QString,bool)));
	QObject::connect(m_pNoteList, SIGNAL(noteReadOnlyChanged(QString,bool)), this, SLOT(onNoteReadOnlyChanged(QString,bool)));

	m_pOptionsPage = new QOptionsPage(this);
	QObject::connect(m_pOptionsPage, SIGNAL(optionsSaved()), this, SLOT(onOptionsSaved()));

	m_pHelpWidget = new QHelpWidget(this);

	m_pTabNavigation = new QUnifiedTabWidget(this);
	m_pTabNavigation->addTab(m_pNoteList, QIcon(":/images/ico_notes.png"), _lc("Tab.Notes", "Notes"));
	m_pTabNavigation->addTab(m_pOptionsPage, QIcon(":/images/ico_options.png"), _lc("Tab.Options", "Options"));
	m_pTabNavigation->addTab(m_pHelpWidget, QIcon(":/images/ico_help.png"), _lc("Tab.Help", "Help"));
	QObject::connect(m_pTabNavigation, SIGNAL(minBtnClicked()), this, SLOT(onShowMinimized()));
	QObject::connect(m_pTabNavigation, SIGNAL(closeBtnClicked()), this, SLOT(close()));

	setWindowVisible(false);
	QMainWindow::setCentralWidget(m_pTabNavigation);
	QMainWindow::resize(_CGuiUtils::sizeOfWidget(this, 0.6, 0.6, true));

	std::vector<QString> v; m_pStg->listNotes(v, false);
	Q_FOREACH(QString sName, v){
		_CNoteStg::_CNote xNote; m_pStg->loadNote(sName, xNote);
		createNotePage(sName, NULL);
	}
}

MainWindow::~MainWindow()
{
	delete m_pStg;
}

void MainWindow::refreshInterface()
{
	QApplication::setFont(g_xOpt.m_ftUIFont);

//	Q_FOREACH(QNotePage* pNote, m_vNotePages){
//		if(pNote){
//			if(pNote->isAlarming()){
//				pNote->stopAlarming();
//				pNote->startAlarming();
//			}
//		}
//	}
}

void MainWindow::setWindowVisible(bool bVisible)
{
	if(bVisible){
		QMainWindow::setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
		QMainWindow::showNormal();
		QMainWindow::activateWindow();
	}else{
#if defined(Q_OS_WIN)
		QMainWindow::setWindowFlags(Qt::Tool);
		QMainWindow::setVisible(false);
#else
		QMainWindow::showMinimized();
#endif
	}
}

QNotePage* MainWindow::notePageOfPath(const QString& sName) const
{
	QNotePage* pRes = NULL;
	Q_FOREACH(QNotePage* pNote, m_vNotePages){
		if(pNote){
			if(_CLocalFile::isEquivalent(pNote->noteName(), sName)){
				pRes = pNote;
				break;
			}
		}
	}
	return pRes;
}

QNotePage* MainWindow::createNotePage(const QString& sName, QNotePage* pParent)
{
	QNotePage* pNote = NULL;
	if(m_pStg->exists(sName)){
		pNote = new QNotePage(sName);
		_CNoteStg::_CNote xNote; m_pStg->loadNote(sName, xNote);

		pNote->setTitle(xNote.m_sTitle);
		pNote->setContent(xNote.m_sContent);
		pNote->setPageColor(xNote.m_clPage);
		pNote->setReadOnly(xNote.m_bReadOnly);

		if(xNote.m_nPosX >= 0 && xNote.m_nPosY >= 0){
			pNote->setPosition(xNote.m_nPosX, xNote.m_nPosY);
		}else{
			QPoint ptPos(QApplication::desktop()->geometry().right() - pNote->sizeHint().width() - 50, 50);
			if(pParent){
				ptPos = QPoint(pParent->getPosition().x() - 50, pParent->getPosition().y() + 50);
			}
			pNote->setPosition(ptPos);
		}
		if(xNote.m_nWidth >= 0 && xNote.m_nHeight >= 0){
			pNote->resize(xNote.m_nWidth, xNote.m_nHeight);
		}else{
			pNote->resize(300, 300);
		}

		QDateTime tReminder = QDateTime::fromTime_t((uint)xNote.m_tReminder);
		pNote->setReminder(tReminder);

		QObject::connect(pNote, SIGNAL(focusOut()), this, SLOT(onNotePageFocusOut()));
		QObject::connect(pNote, SIGNAL(newPage()), this, SLOT(onNewNote()));
		QObject::connect(pNote, SIGNAL(deletePage()), this, SLOT(onDeleteNote()));

		pNote->show();

		m_vNotePages << pNote;

		//2018.1.16 Update notes list
		m_pNoteList->refreshNote(pNote->noteName(), true);
	}
	return pNote;
}

bool MainWindow::deleteNotePage(QNotePage* pNote)
{
	bool bSucc = false;
	if(!pNote->noteName().isEmpty() && m_pStg->exists(pNote->noteName())){
		saveNote(pNote);
		if(pNote->plainText().isEmpty()){
			bSucc = m_pStg->removeNote(pNote->noteName());
		}else{
			bSucc = m_pStg->trashNote(pNote->noteName());
		}
	}
	QObject::connect(pNote, SIGNAL(invisible()), pNote, SLOT(close()));
	pNote->setVisible(false);
	m_vNotePages.removeAll(pNote);

	//2018.1.16 Update notes list
	m_pNoteList->refreshNote(pNote->noteName(), true);

	return bSucc;
}

bool MainWindow::deleteNotePage(const QString& sName)
{
	bool bSucc = false;
	QNotePage* pNote = notePageOfPath(sName);
	if(pNote) bSucc = deleteNotePage(pNote);
	return bSucc;
}

bool MainWindow::saveNote(QNotePage* pNote)
{
	QString sName = pNote->noteName();
	_CNoteStg::_CNote xNote; m_pStg->loadNote(sName, xNote);
	{
		xNote.m_nPosX = pNote->getPosition().x();
		xNote.m_nPosY = pNote->getPosition().y();
		xNote.m_nWidth = pNote->width();
		xNote.m_nHeight = pNote->height();
		xNote.m_clPage = pNote->pageColor();
		xNote.m_bReadOnly = pNote->readOnly();
		xNote.m_tReminder = (time64_t)pNote->reminder().toTime_t();
		xNote.m_sTitle = pNote->title();
		xNote.m_sContent = pNote->content();
	}
	bool bSucc = m_pStg->saveNote(sName, xNote);

	//2018.1.16 Update notes list
	m_pNoteList->refreshNote(pNote->noteName(), true);

	return bSucc;
}

bool MainWindow::saveAllNotes()
{
	bool bSucc = true;
	Q_FOREACH(QNotePage* pNote, m_vNotePages){
		if(pNote){
			bSucc = saveNote(pNote);
			if(!bSucc){
				break;
			}
		}
	}
	return bSucc;
}

void MainWindow::changeEvent(QEvent* event)
{
	if(event->type() == QEvent::WindowStateChange){
		if(QMainWindow::isMinimized()){
			setWindowVisible(false);
		}
	}
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	QMessageBox mb(this);
	mb.setWindowTitle(_APP_TITLE);
	mb.setIcon(QMessageBox::Question);
	mb.setText(_lc("Prompt.Confirm.CloseWindow", "Do you want to exit YourNotes or hide the main window?"));
	QPushButton* pBtnExit = mb.addButton(_lc("Btn.ExitProgram", "&Exit program"), QMessageBox::YesRole);
	QPushButton* pBtnHide = mb.addButton(_lc("Btn.HideWindow", "&Hide window"), QMessageBox::NoRole);
	mb.addButton(_lc("Btn.Cancel", "&Cancel"), QMessageBox::RejectRole);
	mb.exec();

	QPushButton* pBtnClicked = qobject_cast<QPushButton*>(mb.clickedButton());
	if(pBtnClicked == pBtnExit){
		event->accept();
		onExit();
	}else if(pBtnClicked == pBtnHide){
		event->ignore();
		setWindowVisible(false);
	}else{
		event->ignore();
	}
}

void MainWindow::onNewNote()
{
	QString sName = m_pStg->getUniqueNoteName(0);
	if(m_pStg->createFolder(sName)){
		QNotePage* pSender = qobject_cast<QNotePage*>(QObject::sender());
		QNotePage* pNewNote = createNotePage(sName, pSender);
		if(pNewNote){
			pNewNote->initDefFmt(g_xOpt.m_ftDefNoteFont, g_xOpt.m_clDefNoteForeColor, g_xOpt.m_clDefNoteBackColor);
			pNewNote->activateWindow();
		}
	}
}

void MainWindow::onDeleteNote()
{
	QNotePage* pNote = qobject_cast<QNotePage*>(QObject::sender());
	if(pNote){
		deleteNotePage(pNote);
	}
}

void MainWindow::onManageNotes()
{
	m_pTabNavigation->setCurrentIndex(Tab_Notes);
	setWindowVisible(true);
}

void MainWindow::onOptions()
{
	m_pTabNavigation->setCurrentIndex(Tab_Options);
	setWindowVisible(true);
}

void MainWindow::onHelp()
{
	m_pTabNavigation->setCurrentIndex(Tab_Help);
	setWindowVisible(true);
}

void MainWindow::onNoteVisibilityChanged(const QString& sName, bool bVisible)
{
	if(bVisible){
		if(m_pStg->untrashNote(sName)){
			createNotePage(sName, NULL);
		}
	}else{
		deleteNotePage(sName);
	}
}

void MainWindow::onNoteReadOnlyChanged(const QString& sName, bool bReadOnly)
{
	_CNoteStg::_CNote xNote; m_pStg->loadNote(sName, xNote);
	xNote.m_bReadOnly = bReadOnly; m_pStg->saveNote(sName, xNote);
	QNotePage* pNote = notePageOfPath(sName);
	if(pNote) pNote->setReadOnly(bReadOnly);
}

void MainWindow::onAbout()
{
	QMessageBox mb;
	mb.setWindowFlags(Qt::WindowStaysOnTopHint);
	mb.setWindowTitle(_APP_TITLE);
	mb.setWindowIcon(QIcon(":/images/ico_logo.ico"));
	mb.setIconPixmap(QIcon(":/images/ico_logo.ico").pixmap(64, 64));
	mb.setTextFormat(Qt::RichText);

	QString sMsg = "<b style='font-size:20px;'>%sAppTitle%</b>"
		       "<br/>%sCopyrights%"
		       "<br/><br/><a href='%sWebsite%'>github.com/peihaowang</a>"
		       "<br/><a href='%sEmail%'>mailto:wangpeihao@gmail.com</a>"
		       "<br/><br/>An open-source application built for efficiency, practicality and convenience."
		       ;
	sMsg.replace("%sAppTitle%", _APP_TITLE_FULL);
	sMsg.replace("%sCopyrights%", _APP_COPYRIGHT_NOTICE);
	sMsg.replace("%sWebsite%", _APP_URL_WEBSITE);
	sMsg.replace("%sEmail%", _APP_URL_CGI);
	mb.setText(sMsg);
	mb.exec();
}

void MainWindow::onExit()
{
	{
		if(m_pStg->isOpen()){
			saveAllNotes();
			m_pStg->closeStg();

		}
	}

	{
		if(m_pOptionsPage->isModified()){
			if(QMessageBox::question(this, _APP_TITLE, "The options have been modified. Do you want to save modifications?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel) == QMessageBox::Yes){
				m_pOptionsPage->saveChanges();
			}
		}
	}

	{
		QString sFnIni = _CLocalFile::changeExtension(QCoreApplication::applicationFilePath(), _STG_EXT_CONFIG);;
		g_xOpt.saveIni(sFnIni);
	}

	qApp->quit();
}

void MainWindow::onNotePageFocusOut()
{
	QNotePage* pNote = qobject_cast<QNotePage*>(QObject::sender());
	if(pNote){
		saveNote(pNote);
	}
}

void MainWindow::onTrayIconActivate(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::Trigger){
		setWindowVisible(true);
	}
}
