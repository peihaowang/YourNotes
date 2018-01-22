#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

class QHBoxLayout;
class QStackedLayout;
class QToolButton;
class QButtonGroup;

class QNotePage;
class QNoteList;
class QOptionsPage;
class QHelpWidget;
class _CNoteStg;

class QUnifiedTabWidget : public QWidget{

	Q_OBJECT

protected:

	QHBoxLayout *			m_pLayoutTabButtons;
	QStackedLayout *		m_pLayoutWidgets;
	QWidget *			m_pWidgetTabBar;
	QList<QToolButton*>		m_vListTabButtons;
	QButtonGroup *			m_pGrpTabButtons;

	QToolButton *			m_pBtnMinimize;
	QToolButton *			m_pBtnClose;

	QPoint				m_ptLastMousePressed;

public:

	QUnifiedTabWidget(QWidget* parent = 0);

	int count() const;
	int currentIndext() const;
	void setCurrentIndex(int nIndex);

	void addTab(QWidget* pWidget, const QIcon& xIcon, const QString& sText);
	void removeTab(QWidget* pWidget);

protected:

	virtual bool eventFilter(QObject* object, QEvent* event);

signals:

	void minBtnClicked();
	void closeBtnClicked();

	void currentIndexChanged(int nIndex);

protected slots:

	void onTabButtonClicked();

	void onMinBtnClicked(){emit minBtnClicked();}
	void onCloseBtnClicked(){emit closeBtnClicked();}

};

class MainWindow : public QMainWindow
{
	Q_OBJECT

protected:

	QAction *			m_pActionNewNote;
	QAction *			m_pActionManageNotes;
	QAction *			m_pActionOptions;
	QAction *			m_pActionHelp;
	QAction *			m_pActionAbout;
	QAction *			m_pActionExit;

	QUnifiedTabWidget *			m_pTabNavigation;

	QNoteList *			m_pNoteList;
	QOptionsPage *			m_pOptionsPage;
	QHelpWidget *			m_pHelpWidget;
	QList<QNotePage*>		m_vNotePages;

	QSystemTrayIcon *		m_pTrayIcon;

	_CNoteStg *			m_pStg;

	enum _TAppDataPos{
		AppData_PosX = 0
		, AppData_PosY
		, AppData_Width
		, AppData_Height
		, AppData_Color
		, AppDate_Reminder
	};

	enum _TTabPage{
		Tab_Notes = 0
		, Tab_Options
		, Tab_Help
	};

public:

	MainWindow(QWidget *parent = 0);
	~MainWindow();

	void setWindowVisible(bool bVisible);

protected:

	void refreshInterface();

	QNotePage* notePageOfPath(const QString& sName) const;

	QNotePage* createNotePage(const QString& sName, QNotePage* pParent);
	bool deleteNotePage(QNotePage* pNote);
	bool deleteNotePage(const QString& sName);

	bool saveNote(QNotePage* pNote);
	bool saveAllNotes();

	virtual void changeEvent(QEvent* event);
	virtual void closeEvent(QCloseEvent* event);

protected slots:

	void onShowMinimized(){setWindowVisible(false);}

	void onNewNote();
	void onDeleteNote();

	void onManageNotes();
	void onOptions();
	void onHelp();

	void onAbout();
	void onExit();

	void onNoteVisibilityChanged(const QString& sName, bool bVisible);
	void onNoteReadOnlyChanged(const QString& sName, bool bReadOnly);
	void onNotePageFocusOut();
	void onTrayIconActivate(QSystemTrayIcon::ActivationReason reason);

	void onOptionsSaved(){refreshInterface();}

};

#endif // MAINWINDOW_H
