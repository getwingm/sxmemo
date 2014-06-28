#include "stdafx.h"
#include "sxmemo.h"
#include <QtGui/QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	KResource::addSearchPath("app", KUtility::osExeInstancePath());
	KResource::addSearchPath("res", KUtility::osExeInstancePath());

	QString exepath = QApplication::instance()->applicationDirPath();
	QString respath = exepath + "/../";
	respath = QDir::cleanPath(respath);		
	KResource::addSearchPath("appdir", respath);
	KResource::addSearchPath("app", respath);
	KResource::addSearchPath("res", respath);

	SXMemo w;
	w.execute();
	return a.exec();
}
