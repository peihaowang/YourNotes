
#include "common_headers.h"
#include "HelpWidget.h"

QHelpWidget::QHelpWidget(QWidget* parent)
	: QWidget(parent)
{
	m_pTreeContents = new QTreeWidget(this);
	m_pTreeContents->setFrameShape(QFrame::NoFrame);
	m_pTreeContents->setHeaderHidden(true);
	QObject::connect(m_pTreeContents, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

	m_pBrowserPage = new QTextBrowser(this);
	m_pBrowserPage->setFrameShape(QFrame::NoFrame);

	QSplitter* pSplitter = new QSplitter(Qt::Horizontal, this);
	pSplitter->setFrameShape(QFrame::NoFrame);
	pSplitter->setStyleSheet("QSplitter::handle{background-color: #c3d9ff;}");
	pSplitter->addWidget(m_pTreeContents);
	pSplitter->addWidget(m_pBrowserPage);

	QList<int> vSizes; vSizes << 100 << 300;
	pSplitter->setSizes(vSizes);

	QHBoxLayout* pLayout = new QHBoxLayout;
	pLayout->setContentsMargins(0, 0, 0, 0);
	pLayout->addWidget(pSplitter);

	loadContents();

	QWidget::setLayout(pLayout);
}

void QHelpWidget::loadContents()
{
	m_pTreeContents->clear();

	QString sContent = _CTextFile(":/docs/contents.xml").load("");
	QDomDocument xDoc;
	if(xDoc.setContent(sContent)){
		loadChildren(NULL, xDoc.documentElement());
	}

	if(m_pTreeContents->topLevelItemCount() > 0){
		m_pTreeContents->setCurrentItem(m_pTreeContents->topLevelItem(0));
	}
}

void QHelpWidget::loadChildren(QTreeWidgetItem* pParent, const QDomElement& xEle)
{
	QDomElement xNext = xEle.firstChildElement();
	while(!xNext.isNull()){
		QString sName, sUrl;
		if(xNext.hasAttribute("title")) sName = xNext.attribute("title", "");
		if(xNext.hasAttribute("url")) sUrl = xNext.attribute("url", "");

		QTreeWidgetItem* pItem = new QTreeWidgetItem;
		pItem->setText(0, sName);
		pItem->setData(0, Qt::UserRole, QVariant(sUrl));
		if(pParent){
			pParent->addChild(pItem);
		}else{
			m_pTreeContents->addTopLevelItem(pItem);
		}
		loadChildren(pItem, xNext);
		if(!pItem->parent()){
			pItem->setExpanded(true);
		}

		xNext = xNext.nextSiblingElement();
	}
}

void QHelpWidget::onCurrentItemChanged(QTreeWidgetItem* pCur, QTreeWidgetItem* pPre)
{
	Q_UNUSED(pPre);
	m_pBrowserPage->clear();
	if(pCur){
		QString sUrl = pCur->data(0, Qt::UserRole).toString();
		sUrl = _CLocalFile::concatenate(":/docs/", sUrl);
		QString sHtml = _CTextFile(sUrl).load("");
		if(!sHtml.isEmpty()){
			m_pBrowserPage->setHtml(sHtml);
		}
	}
}
