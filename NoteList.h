#ifndef NOTELIST_H
#define NOTELIST_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

class _CNoteStg;

class QNoteList : public QTreeWidget
{

	Q_OBJECT

protected:

	_CNoteStg *			m_pStg;

	bool				m_bBlockChkSignal;

	struct _CSortState{
		int			m_iLogicalIndex;
		Qt::SortOrder		m_iSortOrder;

		_CSortState() : m_iLogicalIndex(0), m_iSortOrder(Qt::AscendingOrder){return;}
	};
	_CSortState			m_xCurrentSortState;

	class _CTreeWidgetItem : public QTreeWidgetItem{
	public:
		_CTreeWidgetItem(QTreeWidget* pTree) : QTreeWidgetItem(pTree){return;}
		virtual bool operator<(const QTreeWidgetItem &other)const;
	};

	enum _TCol{
		_COL_CONTENT = 0
		, _COL_CREATED
		, _COL_DISPLAY
		, _COL_READONLY
	};

public:

	explicit QNoteList(_CNoteStg* pDb, QWidget* parent = 0);

	int indexOfNote(const QString& sName) const;

	void addNote(const QString& sName);
	void removeNote(const QString& sName);
	void refreshNote(const QString& sName, bool bAutoAddDel);

protected:

	void loadNotes();

	virtual void resizeEvent(QResizeEvent* e);

signals:

	void noteVisibilityChanged(const QString& sName, bool bVisible);
	void noteReadOnlyChanged(const QString& sName, bool bReadOnly);

protected slots:

	void onNoteDisplayChecked(QTreeWidgetItem* pItem, int nCol);
	void onNoteReadOnlyChecked(QTreeWidgetItem* pItem, int nCol);
	void onAttempToSortByColumn(int iLogicalIndex);

};

#endif // NOTELIST_H
