#include <QLabel>
#include <QPushButton>
#include <QDateTimeEdit>

#include "common_headers.h"
#include "NoteStg.h"
#include "DlgSetReminder.h"

QDlgSetReminder::QDlgSetReminder(const QDateTime& tDef, QWidget* parent)
	: QDialog(parent)
{
	QDialog::setWindowTitle("Set reminder");
	QDialog::setWindowIcon(QIcon(":/images/btn_set_reminder.png"));

	m_pStaticInvalidReminder = new QLabel("The specified time has expired, please select a future time", this);
	m_pStaticInvalidReminder->setStyleSheet("QLabel{color: red;}");
	m_pStaticInvalidReminder->setWordWrap(true);
	m_pStaticInvalidReminder->setVisible(false);

	QTimer* pTimerDetector = new QTimer(this);
	pTimerDetector->start(1000);
	QObject::connect(pTimerDetector, SIGNAL(timeout()), this, SLOT(onDetectReminderTime()));

	m_pTimeReminder = new QDateTimeEdit(this);
	m_pTimeReminder->setDateTime(tDef);
	QObject::connect(m_pTimeReminder, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(onDetectReminderTime()));

	QLabel* pStaticReminder = new QLabel("&Select reminder time", this);
	pStaticReminder->setBuddy(m_pTimeReminder);

	m_pBtnOk = new QPushButton("&OK", this);
	QPushButton* pBtnCancel = new QPushButton("&Cancel", this);
	QObject::connect(m_pBtnOk, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(pBtnCancel, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout* pLayoutButton = new QHBoxLayout;
	pLayoutButton->addStretch();
	pLayoutButton->addWidget(m_pBtnOk);
	pLayoutButton->addWidget(pBtnCancel);

	QVBoxLayout* pLayoutMain = new QVBoxLayout;
	pLayoutMain->addWidget(pStaticReminder);
	pLayoutMain->addWidget(m_pTimeReminder);
	pLayoutMain->addWidget(m_pStaticInvalidReminder);
	pLayoutMain->addStretch();
	pLayoutMain->addLayout(pLayoutButton);

	QDialog::setLayout(pLayoutMain);

//	QDialog::resize(_CGuiUtils::sizeOfWidget(this, 0.0, 0.1, true));
}

void QDlgSetReminder::onDetectReminderTime()
{
	QDateTime tReminder = m_pTimeReminder->dateTime();
	bool bExpired = (QDateTime::currentDateTime().secsTo(tReminder) <= 0);
	m_pStaticInvalidReminder->setVisible(bExpired);
	m_pBtnOk->setEnabled(!bExpired);
	QDialog::resize(QDialog::sizeHint());
}

void QDlgSetReminder::accept()
{
	m_tReminder = m_pTimeReminder->dateTime();
	QDialog::accept();
}
