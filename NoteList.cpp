#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextDocument>
#include <QPainter>

#include "common_headers.h"
#include "NoteStg.h"
#include "NoteList.h"

///////////////////////////////////////////////////////////

bool QNoteList::_CTreeWidgetItem::operator<(const QTreeWidgetItem& other) const
{
	//2014.4.14 Customize sorting of QTreeWidget
	//http://www.qtforum.org/article/25281/customize-sorting-of-qtreewidget.html?s=e835cf0d2cc3ec9144ed5def1e3f9d904366b523#post89340
	//A QTreeWidget is sorted alphabetically by default, how can I change this behavior, e.g to sort numerically instead?
	//http://qt-project.org/faq/answer/a_qtreewidget_is_sorted_alphabetically_by_default_how_can_i_change_this_beh

	int iSortCol = treeWidget()->sortColumn();

	bool bLess=false;
	switch(iSortCol){
		case _COL_CREATED:
		{
			QVariant v1=data(iSortCol, Qt::UserRole);
			QVariant v2=other.data(iSortCol, Qt::UserRole);
			QDateTime d1 = v1.value<QDateTime>();
			QDateTime d2 = v2.value<QDateTime>();
			bLess = d1 < d2;
			break;
		}
		default:
		{
			QString d1 = text(iSortCol);
			QString d2 = other.text(iSortCol);
			bLess = d1 < d2;
			break;
		}
	}

	return bLess;
}

///////////////////////////////////////////////////////////

QNoteList::QNoteList(_CNoteStg* pDb, QWidget* parent)
	: QTreeWidget(parent)
	, m_pStg(pDb)
	, m_bBlockChkSignal(false)
{
	QTreeWidget::setAlternatingRowColors(true);
	QTreeWidget::setRootIsDecorated(false);
	QTreeWidget::setTextElideMode(Qt::ElideRight);
	QTreeWidget::setSortingEnabled(true);

	{
		_CTextSplitter vHeaders(_lc("List.Header.Notes", "Content|Created Time|Display|Read Only"), "|");
		QTreeWidget::setColumnCount(vHeaders.size());
		QTreeWidget::header()->setStretchLastSection(false);
		QTreeWidget::header()->setSectionsClickable(true);
		QTreeWidget::header()->setSortIndicatorShown(true);
		QObject::connect(QTreeWidget::header(), SIGNAL(sectionClicked(int)), this, SLOT(onAttempToSortByColumn(int)));

//		QTreeWidget::setHeaderLabels(vHeaders);
		QTreeWidget::headerItem()->setText(0, vHeaders[_COL_CONTENT]);
		QTreeWidget::headerItem()->setText(1, vHeaders[_COL_CREATED]);

		QTreeWidget::headerItem()->setText(_COL_DISPLAY, "");
		QTreeWidget::headerItem()->setIcon(_COL_DISPLAY, QIcon(":/images/ico_display.png"));
		QTreeWidget::headerItem()->setToolTip(_COL_DISPLAY, vHeaders[_COL_DISPLAY]);
		QTreeWidget::header()->setSectionResizeMode(_COL_DISPLAY, QHeaderView::Fixed);

		QTreeWidget::headerItem()->setText(_COL_READONLY, "");
		QTreeWidget::headerItem()->setIcon(_COL_READONLY, QIcon(":/images/ico_readonly.png"));
		QTreeWidget::headerItem()->setToolTip(_COL_READONLY, vHeaders[_COL_READONLY]);
		QTreeWidget::header()->setSectionResizeMode(_COL_READONLY, QHeaderView::Fixed);
	}

	QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onNoteDisplayChecked(QTreeWidgetItem*,int)));
	QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onNoteReadOnlyChecked(QTreeWidgetItem*,int)));

	loadNotes();

	//2018.1.20 Sort after loading notes
	QTreeWidget::sortByColumn(_COL_CREATED, Qt::DescendingOrder);
	m_xCurrentSortState.m_iLogicalIndex = _COL_CREATED;
	m_xCurrentSortState.m_iSortOrder = Qt::DescendingOrder;
}

int QNoteList::indexOfNote(const QString& sName) const
{
	int nIndex = -1;
	for(int i = 0; i < QTreeWidget::topLevelItemCount(); i++){
		QTreeWidgetItem* pItem = QTreeWidget::topLevelItem(i);
		if(pItem->data(_COL_CONTENT, Qt::UserRole).toString() == sName){
			nIndex = i;
			break;
		}
	}
	return nIndex;
}

void QNoteList::addNote(const QString& sName)
{
	_CTreeWidgetItem* pItem = new _CTreeWidgetItem(this);
	pItem->setData(_COL_CONTENT, Qt::UserRole, QVariant(sName));
	refreshNote(sName, false);
}

void QNoteList::removeNote(const QString& sName)
{
	for(int i = 0; i < QTreeWidget::topLevelItemCount(); i++){
		QTreeWidgetItem* pItem = QTreeWidget::topLevelItem(i);
		if(pItem->data(_COL_CONTENT, Qt::UserRole).toString() == sName){
			QTreeWidget::takeTopLevelItem(i);
			break;
		}
	}
}

void QNoteList::refreshNote(const QString& sName, bool bAutoAddDel)
{
	if(!m_pStg->exists(sName)){
		if(bAutoAddDel) removeNote(sName);
	}else{
		int nIndex = indexOfNote(sName);
		if(nIndex < 0){
			if(bAutoAddDel) addNote(sName);
		}else{
			QTreeWidgetItem* pItem = QTreeWidget::topLevelItem(nIndex);

			_CNoteStg::_CNote xNote; m_pStg->loadNote(sName, xNote);

			QColor clPage = xNote.m_clPage;
			QString sContent = xNote.m_sContent;
			QString sPlain, sTxt;
			{
				QTextDocument xDoc;
				xDoc.setHtml(sContent);
				sPlain = xDoc.toPlainText();
				{
					sPlain.replace("\r\n", "\n");
					_CTextSplitter v(sPlain, "\n");

					_CTextSplitter v0("", "\n");
					if(v.size() > 5){
						v0 = v.mid(0, 4);
						v0 << "...";
					}else{
						v0 = v;
					}

					sTxt = (QString)v0;
				}
			}
			QDateTime tCreated = m_pStg->createdTime(sName);
			QString sCreated = (QString)_CDateTimeSmart(tCreated);

			pItem->setText(_COL_CONTENT, sTxt);
			pItem->setText(_COL_CREATED, sCreated);
			pItem->setForeground(_COL_CREATED, QBrush(Qt::gray));

			pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			pItem->setCheckState(_COL_DISPLAY, m_pStg->isTrash(sName) ? Qt::Unchecked : Qt::Checked);
			pItem->setCheckState(_COL_READONLY, xNote.m_bReadOnly ? Qt::Checked : Qt::Unchecked);

			pItem->setToolTip(_COL_CONTENT, sPlain);
			pItem->setData(_COL_CREATED, Qt::UserRole, QVariant(tCreated));

			QIcon xIcon;
			{
				QPixmap xPixmap(16, 16); xPixmap.fill(Qt::transparent);
				QPainter xPainter(&xPixmap);
				xPainter.setPen(QPen(QBrush(Qt::black), 1, Qt::SolidLine));
				xPainter.setBrush(QBrush(clPage, Qt::SolidPattern));
				xPainter.drawRect(0, 0, 15, 15);

				xIcon = QIcon(xPixmap);
			}
			pItem->setIcon(_COL_CONTENT, xIcon);
		}
	}
}

void QNoteList::loadNotes()
{
	_CTempValueChange<bool> _tvc(m_bBlockChkSignal, true); Q_UNUSED(_tvc);
	QTreeWidget::clear();

	std::vector<QString> v; m_pStg->listNotes(v, true);
	Q_FOREACH(QString sName, v){
		addNote(sName);
	}
}

void QNoteList::resizeEvent(QResizeEvent* e)
{
	QTreeWidget::resizeEvent(e);

	QList<int> vSizes; vSizes << 0 << 200 << 30 << 30;
	int nOccupiedSize = 0;
	Q_FOREACH(int nSize, vSizes){
		nOccupiedSize += nSize;
	}

	vSizes[0] = QTreeWidget::viewport()->width() - nOccupiedSize;
	for(int i = 0; i < vSizes.size(); i++){
		QTreeWidget::setColumnWidth(i, vSizes[i]);
	}
}

void QNoteList::onNoteDisplayChecked(QTreeWidgetItem* pItem, int nCol)
{
	if(!m_bBlockChkSignal){
		if(nCol == _COL_DISPLAY){
			QString sName = pItem->data(_COL_CONTENT, Qt::UserRole).toString();
			emit noteVisibilityChanged(sName, pItem->checkState(_COL_DISPLAY) == Qt::Checked);
		}
	}
}

void QNoteList::onNoteReadOnlyChecked(QTreeWidgetItem* pItem, int nCol)
{
	if(!m_bBlockChkSignal){
		if(nCol == _COL_READONLY){
			QString sNoteName = pItem->data(_COL_CONTENT, Qt::UserRole).toString();
			emit noteReadOnlyChanged(sNoteName, pItem->checkState(_COL_READONLY) == Qt::Checked);
		}
	}
}

void QNoteList::onAttempToSortByColumn(int iLogicalIndex)
{
	if(m_xCurrentSortState.m_iLogicalIndex != iLogicalIndex){
		switch(iLogicalIndex){
			case _COL_CONTENT:
				m_xCurrentSortState.m_iSortOrder = Qt::AscendingOrder;
				m_xCurrentSortState.m_iLogicalIndex = iLogicalIndex;
				break;
			case _COL_CREATED:
				m_xCurrentSortState.m_iSortOrder = Qt::DescendingOrder;
				m_xCurrentSortState.m_iLogicalIndex = iLogicalIndex;
				break;
			default:
				break;
		}
	}else{
		m_xCurrentSortState.m_iSortOrder = (m_xCurrentSortState.m_iSortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
	}

	QTreeWidget::header()->setSortIndicator(m_xCurrentSortState.m_iLogicalIndex, m_xCurrentSortState.m_iSortOrder);
	QTreeWidget::sortByColumn(m_xCurrentSortState.m_iLogicalIndex, m_xCurrentSortState.m_iSortOrder);
}
