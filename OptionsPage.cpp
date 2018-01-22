
#include "common_headers.h"
#include "OptionsPage.h"

QOptionsPage::QOptionsPage(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* pLayoutFont = new QVBoxLayout;
	pLayoutFont->setSpacing(5);
	{
		m_pCmbUI_FontFamily = new QFontComboBox(this);
		QObject::connect(m_pCmbUI_FontFamily, SIGNAL(currentIndexChanged(int)), this, SLOT(onOptionChanged()));

		m_pCmbUI_FontSize = new QComboBox(this);
		{
			m_pCmbUI_FontSize->setEditable(false);
			m_pCmbUI_FontSize->addItem("9", QVariant(9));
			m_pCmbUI_FontSize->addItem("10", QVariant(10));
			m_pCmbUI_FontSize->addItem("11", QVariant(11));
			m_pCmbUI_FontSize->addItem("12", QVariant(12));
			m_pCmbUI_FontSize->addItem("15", QVariant(15));
			m_pCmbUI_FontSize->addItem("18", QVariant(18));
			m_pCmbUI_FontSize->addItem("20", QVariant(20));
			m_pCmbUI_FontSize->addItem("24", QVariant(24));
		}
		QObject::connect(m_pCmbUI_FontSize, SIGNAL(currentIndexChanged(int)), this, SLOT(onOptionChanged()));

		QLabel* pTitle = new QLabel(_lc("Options.Interface.Title", "Interface"), this);
		pTitle->setStyleSheet("QLabel{font-weight: bold;}");

		QFrame* pSeparator = new QFrame(this);
		pSeparator->setFrameShadow(QFrame::Sunken);
		pSeparator->setFrameShape(QFrame::HLine);

		QFormLayout* pLayoutContent = new QFormLayout;
		pLayoutContent->setSpacing(10);
		pLayoutContent->addRow(_lc("Options.Interface.Family", "Font Family"), m_pCmbUI_FontFamily);
		pLayoutContent->addRow(_lc("Options.Interface.Size", "Font Size"), m_pCmbUI_FontSize);

		pLayoutFont->addWidget(pTitle);
		pLayoutFont->addWidget(pSeparator);
		pLayoutFont->addLayout(pLayoutContent);
	}

	QVBoxLayout* pLayoutLanguage = new QVBoxLayout;
	pLayoutLanguage->setSpacing(5);
	{
		m_pListLocale = new QListWidget(this);

		//System language
		{
			QListWidgetItem* pItem = new QListWidgetItem;
			pItem->setText(_lc("Options.Language.Default", "Default"));
			pItem->setData(Qt::UserRole, QVariant(""));
			m_pListLocale->addItem(pItem);
		}

		QDir xDir(":/lang"); QStringList vFiles = xDir.entryList();
		Q_FOREACH(QString sFile, vFiles){
			QString sLang = QFileInfo(sFile).baseName().toUpper();
			QString sFn = _CLocalFile::concatenate(":/lang", sFile);
			_CKeyValEntries xKV(_CTextFile(sFn).load(""));
			QString sDispName = xKV("App.Language.DisplayName", sLang);
			QString sTranslator = xKV("App.Language.Translator", "");

			QListWidgetItem* pItem = new QListWidgetItem;
			pItem->setText(sDispName);
			pItem->setData(Qt::UserRole, QVariant(sLang));
			if(!sTranslator.isEmpty()) pItem->setToolTip(QString("Translated by %1").arg(sTranslator));
			m_pListLocale->addItem(pItem);
		}

		QObject::connect(m_pListLocale, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onOptionChanged()));
		QObject::connect(m_pListLocale, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onRelaunchChanged()));

		QLabel* pTitle = new QLabel(_lc("Options.Language.Title", "Language"), this);
		pTitle->setStyleSheet("QLabel{font-weight: bold;}");

		QFrame* pSeparator = new QFrame(this);
		pSeparator->setFrameShadow(QFrame::Sunken);
		pSeparator->setFrameShape(QFrame::HLine);

		pLayoutLanguage->addWidget(pTitle);
		pLayoutLanguage->addWidget(pSeparator);
		pLayoutLanguage->addWidget(m_pListLocale);
	}

	QVBoxLayout* pLayoutReminder = new QVBoxLayout;
	pLayoutReminder->setSpacing(5);
	{
		m_pCmbReminder_RingSound = new QComboBox(this);
		{
			m_pCmbReminder_RingSound->setEditable(false);

			_CPairVector<QString, _CAppSettings::_TReminderSound> v;
			v[_lc("Options.Reminder.Sound.Mute", "Mute")] = _CAppSettings::RS_Mute;
			v[_lc("Options.Reminder.Sound.Chimes", "Chimes")] = _CAppSettings::RS_Chimes;
			v[_lc("Options.Reminder.Sound.Ding", "Ding")] = _CAppSettings::RS_Ding;

			_CPairVector<QString, _CAppSettings::_TReminderSound>::const_iterator it;
			__EACH(it, v){
				QString sLabel = it->first;
				_CAppSettings::_TReminderSound nVal = it->second;

				m_pCmbReminder_RingSound->addItem(sLabel, QVariant((uint)nVal));
			}
		}
		QObject::connect(m_pCmbReminder_RingSound, SIGNAL(currentIndexChanged(int)), this, SLOT(onOptionChanged()));

//		m_pMediaObject = new Phonon::MediaObject(this);
//		Phonon::AudioOutput* pAudioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
//		Phonon::createPath(m_pMediaObject, pAudioOutput);
		m_xSoundReminder.setLoopCount(1);
		m_xSoundReminder.setVolume(1.0);

		m_pBtnReminder_RingSoundTest = new QPushButton(_lc("Options.Reminder.Sound.Test", "Test ..."), this);
		m_pBtnReminder_RingSoundTest->setIcon(QIcon(":/images/btn_play.png"));
		QObject::connect(m_pBtnReminder_RingSoundTest, SIGNAL(clicked()), this, SLOT(onTestRingSound()));

		QHBoxLayout* pLayoutSound = new QHBoxLayout;
		pLayoutSound->addWidget(m_pCmbReminder_RingSound);
		pLayoutSound->addWidget(m_pBtnReminder_RingSoundTest);

		m_pCmbReminder_RingInterval = new QComboBox(this);
		{
			m_pCmbReminder_RingInterval->setEditable(false);

			_CPairVector<QString, int> v;
			v[_lc("Options.Reminder.Interval.Forever", "Forever")] = -1;
			v[_lc("Options.Reminder.Interval.30sec", "Every 30 seconds")] = 30 * 1000;
			v[_lc("Options.Reminder.Interval.1min", "Every 1 minutes")] = 1 * 60 * 1000;
			v[_lc("Options.Reminder.Interval.3min", "Every 3 minutes")] = 3 * 60 * 1000;
			v[_lc("Options.Reminder.Interval.5min", "Every 5 minutes")] = 5 * 60 * 1000;
			v[_lc("Options.Reminder.Interval.10min", "Every 10 minutes")] = 10 * 60 * 1000;
			v[_lc("Options.Reminder.Interval.15min", "Every 15 minutes")] = 15 * 60 * 1000;
			v[_lc("Options.Reminder.Interval.30min", "Every 30 minutes")] = 30 * 60 * 1000;

			_CPairVector<QString, int>::const_iterator it;
			__EACH(it, v){
				QString sLabel = it->first;
				int nMSec = it->second;

				m_pCmbReminder_RingInterval->addItem(sLabel, QVariant(nMSec));
			}
		}
		QObject::connect(m_pCmbReminder_RingInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(onOptionChanged()));

		m_pSpin_Reminder_ShockFactor = new QSpinBox(this);
		m_pSpin_Reminder_ShockFactor->setMinimum(0);
		m_pSpin_Reminder_ShockFactor->setMaximum(20);
		QObject::connect(m_pSpin_Reminder_ShockFactor, SIGNAL(valueChanged(int)), this, SLOT(onOptionChanged()));

		QLabel* pTitle = new QLabel(_lc("Options.Reminder.Title", "Reminder"), this);
		pTitle->setStyleSheet("QLabel{font-weight: bold;}");

		QFrame* pSeparator = new QFrame(this);
		pSeparator->setFrameShadow(QFrame::Sunken);
		pSeparator->setFrameShape(QFrame::HLine);

		QFormLayout* pLayoutContent = new QFormLayout;
		pLayoutContent->setSpacing(10);
		pLayoutContent->addRow(_lc("Options.Reminder.Sound", "Sound"), pLayoutSound);
		pLayoutContent->addRow(_lc("Options.Reminder.Interval", "Interval"), m_pCmbReminder_RingInterval);
		pLayoutContent->addRow(_lc("Options.Reminder.ShockFactor", "Shock Factor"), m_pSpin_Reminder_ShockFactor);

		pLayoutReminder->addWidget(pTitle);
		pLayoutReminder->addWidget(pSeparator);
		pLayoutReminder->addLayout(pLayoutContent);
	}

	QVBoxLayout* pLayoutNoteFmt = new QVBoxLayout;
	pLayoutNoteFmt->setSpacing(5);
	{
		m_pCmbNote_FontFamily = new QFontComboBox(this);
		QObject::connect(m_pCmbNote_FontFamily, SIGNAL(currentIndexChanged(int)), this, SLOT(onOptionChanged()));

		m_pCmbNote_FontSize = new QComboBox(this);
		{
			m_pCmbNote_FontSize->setEditable(false);
			m_pCmbNote_FontSize->addItem("9", QVariant(9));
			m_pCmbNote_FontSize->addItem("12", QVariant(10));
			m_pCmbNote_FontSize->addItem("15", QVariant(11));
			m_pCmbNote_FontSize->addItem("18", QVariant(12));
			m_pCmbNote_FontSize->addItem("20", QVariant(20));
			m_pCmbNote_FontSize->addItem("24", QVariant(24));
			m_pCmbNote_FontSize->addItem("28", QVariant(28));
			m_pCmbNote_FontSize->addItem("32", QVariant(32));
			m_pCmbNote_FontSize->addItem("40", QVariant(40));
		}
		QObject::connect(m_pCmbNote_FontSize, SIGNAL(currentIndexChanged(int)), this, SLOT(onOptionChanged()));

		m_pCmbNote_Foreground = new QComboBox(this);
		{
			_CPairVector<QString, QColor>::const_iterator it;
			__EACH(it, g_xOpt.m_vColorsList){
				QString sName = it->first;
				QColor clColor = it->second;
				QIcon xIcon;
				QSize szIcon = QSize(16, 14);
				{
					QPixmap xImg(szIcon.width(), szIcon.height()); xImg.fill(Qt::transparent);
					QPainter xPainter(&xImg);
					xPainter.setPen(QPen(QBrush(Qt::black), 1));
					xPainter.setBrush(QBrush(clColor));
					xPainter.drawRect(0, 0, szIcon.width() - 1, szIcon.height() - 1);
					xIcon = QIcon(xImg);
				}
				m_pCmbNote_Foreground->addItem(xIcon, sName, QVariant(clColor));
			}
		}
		QObject::connect(m_pCmbNote_Foreground, SIGNAL(currentIndexChanged(int)), this, SLOT(onOptionChanged()));

		m_pCmbNote_Background = new QComboBox(this);
		{
			_CPairVector<QString, QColor>::const_iterator it;
			__EACH(it, g_xOpt.m_vPageColors){
				QString sName = it->first;
				QColor clColor = it->second;
				QIcon xIcon;
				QSize szIcon = QSize(16, 14);
				{
					QPixmap xImg(szIcon.width(), szIcon.height()); xImg.fill(Qt::transparent);
					QPainter xPainter(&xImg);
					xPainter.setPen(QPen(QBrush(Qt::black), 1));
					xPainter.setBrush(QBrush(clColor));
					xPainter.drawRect(0, 0, szIcon.width() - 1, szIcon.height() - 1);
					xIcon = QIcon(xImg);
				}
				m_pCmbNote_Background->addItem(xIcon, sName, QVariant(clColor));
			}
		}
		QObject::connect(m_pCmbNote_Background, SIGNAL(currentIndexChanged(int)), this, SLOT(onOptionChanged()));

		QLabel* pTitle = new QLabel(_lc("Options.Notes.Title", "Notes"), this);
		pTitle->setStyleSheet("QLabel{font-weight: bold;}");

		QFrame* pSeparator = new QFrame(this);
		pSeparator->setFrameShadow(QFrame::Sunken);
		pSeparator->setFrameShape(QFrame::HLine);

		QFormLayout* pLayoutContent = new QFormLayout;
		pLayoutContent->setSpacing(10);
		pLayoutContent->addRow(_lc("Options.Notes.Family", "Font Family"), m_pCmbNote_FontFamily);
		pLayoutContent->addRow(_lc("Options.Notes.Size", "Font Size"), m_pCmbNote_FontSize);
		pLayoutContent->addRow(_lc("Options.Notes.Foreground", "Foreground"), m_pCmbNote_Foreground);
		pLayoutContent->addRow(_lc("Options.Notes.Background", "Background"), m_pCmbNote_Background);

		pLayoutNoteFmt->addWidget(pTitle);
		pLayoutNoteFmt->addWidget(pSeparator);
		pLayoutNoteFmt->addLayout(pLayoutContent);
	}

	QVBoxLayout* pLayoutCentral = new QVBoxLayout;
	pLayoutCentral->setContentsMargins(0, 0, 0, 0);
	pLayoutCentral->setSpacing(30);
	pLayoutCentral->addLayout(pLayoutFont);
	pLayoutCentral->addLayout(pLayoutLanguage);
	pLayoutCentral->addLayout(pLayoutReminder);
	pLayoutCentral->addLayout(pLayoutNoteFmt);

	QWidget* pScrollWidget = new QWidget(this);
	pScrollWidget->setLayout(pLayoutCentral);

	QScrollArea* pScrollArea = new QScrollArea(this);
	pScrollArea->setFrameShape(QFrame::NoFrame);
	pScrollArea->setAlignment(Qt::AlignHCenter);
//	pScrollArea->setWidgetResizable(true);
	pScrollArea->setWidget(pScrollWidget);

	m_pBtnSave = new QPushButton(_lc("Btn.SaveOptions", "&Save changes"), this);
	m_pBtnDiscard = new QPushButton(_lc("Btn.DiscardOptions", "&Discard changes"), this);
	QObject::connect(m_pBtnSave, SIGNAL(clicked()), this, SLOT(onSaveOptions()));
	QObject::connect(m_pBtnDiscard, SIGNAL(clicked()), this, SLOT(onDiscardOptions()));

	QHBoxLayout* pLayoutButtons = new QHBoxLayout;
	pLayoutButtons->addStretch();
	pLayoutButtons->addWidget(m_pBtnSave);
	pLayoutButtons->addWidget(m_pBtnDiscard);

	QVBoxLayout* pLayoutMain = new QVBoxLayout;
	pLayoutMain->addWidget(pScrollArea);
	pLayoutMain->addLayout(pLayoutButtons);

	QWidget::setAutoFillBackground(true);
	{
		QPalette xPal = QWidget::palette();
		xPal.setBrush(QPalette::Background, QBrush(QColor(249, 249, 249)));
		QWidget::setPalette(xPal);
	}

	QWidget::setLayout(pLayoutMain);

	discardChanges();
}

void QOptionsPage::saveChanges()
{
	g_xOpt.m_ftUIFont.setFamily(m_pCmbUI_FontFamily->currentFont().family());
	g_xOpt.m_ftUIFont.setPointSize(m_pCmbUI_FontSize->itemData(m_pCmbUI_FontSize->currentIndex(), Qt::UserRole).toInt());

	{
		QListWidgetItem* pItem = m_pListLocale->currentItem();
		if(pItem){
			g_xOpt.m_sUILanguage = pItem->data(Qt::UserRole).toString();
		}
	}

	g_xOpt.m_nReminder_RingSound = m_pCmbReminder_RingSound->itemData(m_pCmbReminder_RingSound->currentIndex(), Qt::UserRole).toUInt();
	g_xOpt.m_nReminder_RingInterval = m_pCmbReminder_RingInterval->itemData(m_pCmbReminder_RingInterval->currentIndex(), Qt::UserRole).toInt();
	g_xOpt.m_nReminder_ShockFactor = m_pSpin_Reminder_ShockFactor->value();

	g_xOpt.m_ftDefNoteFont.setFamily(m_pCmbNote_FontFamily->currentFont().family());
	g_xOpt.m_ftDefNoteFont.setPointSize(m_pCmbNote_FontSize->itemData(m_pCmbNote_FontSize->currentIndex(), Qt::UserRole).toInt());
	g_xOpt.m_clDefNoteForeColor = m_pCmbNote_Foreground->itemData(m_pCmbNote_Foreground->currentIndex(), Qt::UserRole).value<QColor>();
	g_xOpt.m_clDefNoteBackColor = m_pCmbNote_Background->itemData(m_pCmbNote_Background->currentIndex(), Qt::UserRole).value<QColor>();

	m_bModified = false;
	m_bRelaunch = false;

	m_pBtnSave->setEnabled(false);
	m_pBtnDiscard->setEnabled(false);

	emit optionsSaved();
}

void QOptionsPage::discardChanges()
{
	m_pCmbUI_FontFamily->setCurrentFont(g_xOpt.m_ftUIFont);
	{
		int nIndex = m_pCmbUI_FontSize->findData(QVariant(g_xOpt.m_ftUIFont.pointSize()));
		if(nIndex >= 0){
			m_pCmbUI_FontSize->setCurrentIndex(nIndex);
		}
	}

	{
		m_pListLocale->setCurrentItem(NULL);
		for(int i = 0; i < m_pListLocale->count(); i++){
			QListWidgetItem* pItem = m_pListLocale->item(i);
			if(pItem){
				QString sID = pItem->data(Qt::UserRole).toString();
				if(sID.compare(g_xOpt.m_sUILanguage, Qt::CaseInsensitive) == 0){
					m_pListLocale->setCurrentItem(pItem);
					break;
				}
			}
		}
	}

	{
		int nIndex = m_pCmbReminder_RingSound->findData(QVariant(g_xOpt.m_nReminder_RingSound));
		if(nIndex >= 0){
			m_pCmbReminder_RingSound->setCurrentIndex(nIndex);
		}
	}
	{
		int nIndex = m_pCmbReminder_RingInterval->findData(QVariant(g_xOpt.m_nReminder_RingInterval));
		if(nIndex >= 0){
			m_pCmbReminder_RingInterval->setCurrentIndex(nIndex);
		}
	}
	m_pSpin_Reminder_ShockFactor->setValue(g_xOpt.m_nReminder_ShockFactor);

	m_pCmbNote_FontFamily->setCurrentFont(g_xOpt.m_ftDefNoteFont);
	{
		int nIndex = m_pCmbNote_FontSize->findData(QVariant(g_xOpt.m_ftDefNoteFont.pointSize()));
		if(nIndex >= 0){
			m_pCmbNote_FontSize->setCurrentIndex(nIndex);
		}
	}

	{
		int nIndex = m_pCmbNote_Foreground->findData(QVariant(g_xOpt.m_clDefNoteForeColor));
		if(nIndex >= 0){
			m_pCmbNote_Foreground->setCurrentIndex(nIndex);
		}
	}

	{
		int nIndex = m_pCmbNote_Background->findData(QVariant(g_xOpt.m_clDefNoteBackColor));
		if(nIndex >= 0){
			m_pCmbNote_Background->setCurrentIndex(nIndex);
		}
	}

	m_bModified = false;
	m_bRelaunch = false;

	m_pBtnSave->setEnabled(false);
	m_pBtnDiscard->setEnabled(false);
}

void QOptionsPage::onTestRingSound()
{
	uint nSound = m_pCmbReminder_RingSound->itemData(m_pCmbReminder_RingSound->currentIndex(), Qt::UserRole).toUInt();
	const QMap<uint, QString>& m = g_xOpt.m_mReminderSounds;
	QMap<uint, QString>::const_iterator it = m.find(nSound);
	if(it != m.end()){
		QString sUrl = it.value();
		if(m_xSoundReminder.isPlaying()) m_xSoundReminder.stop();
		m_xSoundReminder.setSource(QUrl::fromLocalFile(sUrl));
		m_xSoundReminder.play();
	}
}

void QOptionsPage::onOptionChanged()
{
	m_bModified = true;

	m_pBtnSave->setEnabled(true);
	m_pBtnDiscard->setEnabled(true);
}

void QOptionsPage::onRelaunchChanged()
{
	m_bRelaunch = true;
}

void QOptionsPage::onSaveOptions()
{
	if(m_bRelaunch){
		QMessageBox::information(this, _APP_TITLE, "The user interface language has been changed; You may re-launch for the change(s) to take effect?", QMessageBox::Ok);
	}
	saveChanges();
}

void QOptionsPage::onDiscardOptions()
{
	if(QMessageBox::question(this, _APP_TITLE, "Discard changes of settings. Proceed?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel) == QMessageBox::Yes){
		discardChanges();
	}
}
