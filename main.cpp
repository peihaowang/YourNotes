#include <QApplication>
#include "common_headers.h"
#include "MainWindow.h"
#include "app_settings.h"

_CAppSettings g_xOpt;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	{
		QString sIniFn = _CLocalFile::changeExtension(QCoreApplication::applicationFilePath(), _STG_EXT_CONFIG);
		g_xOpt.load(sIniFn);
	}

	MainWindow w; Q_UNUSED(w);

	return a.exec();
}
