#ifndef OPTIONSPAGE_H
#define OPTIONSPAGE_H

#include <QWidget>
#include <QSoundEffect>

class QLabel;
class QLineEdit;
class QSplider;
class QSpinBox;
class QCheckBox;
class QPushButton;
class QComboBox;
class QFontComboBox;
class QListWidget;
class QMediaPlayer;

class QOptionsPage : public QWidget
{

	Q_OBJECT

public:

protected:

	bool			m_bModified;
	bool			m_bRelaunch;

	QFontComboBox *		m_pCmbUI_FontFamily;
	QComboBox *		m_pCmbUI_FontSize;

	QListWidget *		m_pListLocale;

	QComboBox *		m_pCmbReminder_RingSound;
	QComboBox *		m_pCmbReminder_RingInterval;
	QSpinBox *		m_pSpin_Reminder_ShockFactor;
	QPushButton *		m_pBtnReminder_RingSoundTest;
	QSoundEffect		m_xSoundReminder;

	QFontComboBox *		m_pCmbNote_FontFamily;
	QComboBox *		m_pCmbNote_FontSize;
	QComboBox *		m_pCmbNote_Foreground;
	QComboBox *		m_pCmbNote_Background;

	QPushButton *		m_pBtnSave;
	QPushButton *		m_pBtnDiscard;

public:

	explicit QOptionsPage(QWidget* parent = 0);

	bool isModified() const {return m_bModified;}

	void saveChanges();
	void discardChanges();

signals:

	void optionsSaved();

protected slots:

	void onTestRingSound();

	void onOptionChanged();
	void onRelaunchChanged();

	void onSaveOptions();
	void onDiscardOptions();

};



#endif // OPTIONSPAGE_H
