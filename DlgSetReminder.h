#ifndef DLGSETREMINDER_H
#define DLGSETREMINDER_H

#include <QDateTime>
#include <QDialog>

class QLabel;
class QPushButton;
class QDateTimeEdit;

class QDlgSetReminder : public QDialog
{

	Q_OBJECT

public:

	QDateTime		m_tReminder;

protected:

	QDateTimeEdit *		m_pTimeReminder;
	QPushButton *		m_pBtnOk;
	QLabel *		m_pStaticInvalidReminder;

public:

	explicit QDlgSetReminder(const QDateTime& tDef, QWidget* parent = 0);

signals:

protected slots:

	void onDetectReminderTime();

public slots:

	virtual void accept();

};

#endif // DLGSETREMINDER_H
