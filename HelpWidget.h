#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include <QDomElement>
#include <QWidget>

class QLabel;
class QTreeWidget;
class QTreeWidgetItem;
class QTextBrowser;

class QHelpWidget : public QWidget
{

	Q_OBJECT

protected:

	QTreeWidget *		m_pTreeContents;
	QTextBrowser *		m_pBrowserPage;

public:

	explicit QHelpWidget(QWidget* parent = 0);

protected:

	void loadContents();
	void loadChildren(QTreeWidgetItem* pParent, const QDomElement& xEle);

signals:

protected slots:

	void onCurrentItemChanged(QTreeWidgetItem* pCur, QTreeWidgetItem* pPre);

};



#endif // HELPWIDGET_H
