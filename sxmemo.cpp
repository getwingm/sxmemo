#include "stdafx.h"
#include "sxmemo.h"
#include "memoedit.h"

QDataStream &operator<<(QDataStream &ds, const MemoInfo &mi)
{
	ds << mi.winRt << mi.htmlContent << mi.bTranslucent;
	return ds;
}

QDataStream &operator>>(QDataStream &ds, MemoInfo &mi)
{
	ds >> mi.winRt >> mi.htmlContent >> mi.bTranslucent;
	return ds;
}

SXMemo::SXMemo(QObject *parent)
	: QObject(parent)
{
	m_trayIcon = new QSystemTrayIcon(this);
	QObject::connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_trayMenu_activated(QSystemTrayIcon::ActivationReason)));
	m_trayIcon->setToolTip("善学备忘录");
	m_trayIcon->setIcon(QIcon("res:/Resources/image/sxmemo.png"));
	m_trayIcon->show();
	QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(on_tryMenu_timeout()));
	m_timer.start(1000*30);
}

SXMemo::~SXMemo()
{

}

void SXMemo::execute()
{
	restoreMemos();
}

void SXMemo::on_trayMenu_activated( QSystemTrayIcon::ActivationReason reason )
{
	if(reason == QSystemTrayIcon::DoubleClick)
	{
		on_trayMenu_foreground();
		return;
	}

	KMenu *menu = KMenu::createPopupMenu();
	menu->setBackgroundColor(QColor(255,255,0,128));
	menu->addAction("新建", this, SLOT(on_trayMenu_createNew()));
	menu->addAction("最前端", this, SLOT(on_trayMenu_foreground()));
	menu->addAction("重置窗口", this, SLOT(on_trayMenu_reset()));
	{
		QAction *action = menu->addAction("自动启动", this, SLOT(on_trayMenu_startup(bool)));
		bool bhas = KUtility::hasWindowStartRun("sxmemo");
		action->setCheckable(true);
		action->setChecked(bhas);
	}
	menu->addAction("设置", this, SLOT(on_trayMenu_setup()));
	menu->addAction("关于", this, SLOT(on_trayMenu_about()));
	menu->addAction("退出", this, SLOT(on_trayMenu_quit()));
	menu->exec();
}

void SXMemo::on_trayMenu_createNew()
{
	QRect workRt = KUtility::desktopWorkArea();
	QPoint centerPt = workRt.center();
	QRect winRt(centerPt.x() - 50, centerPt.y() - 50, 100, 100);
	MemoEdit *edit = MemoEdit::createEdit(winRt, "", false, false, this);
	m_memos.push_back(edit);
	QObject::connect(edit, SIGNAL(aboutToClose()), this, SLOT(on_memoEdit_aboutToClose()));
	QObject::connect(edit, SIGNAL(modifyChanged()), this, SLOT(on_memoEdit_modifyChanged()));
}

void SXMemo::on_trayMenu_about()
{
	if(m_pWindowAbout)
	{
		m_pWindowAbout->activateWindow();
		return;
	}

	KDialog *dlg = qobject_cast<KDialog*>(KXmlUI::windowFromFile("res:/Resources/layout/about.xml"));
	m_pWindowAbout = dlg;
	KTextItem *textItem = dlg->findChild<KTextItem*>("aboutText");
	QString myHint = "<html>感谢Qt开源，本程序基于Qt-GraphicsView体系开发。作者：心路 <a href=\"http://www.kxtry.com\">http://www.kxtry.com</a></html>";
	textItem->setText(myHint);
	dlg->doModal();
	qDeleteLater(dlg);
}

void SXMemo::on_trayMenu_setup()
{
	if(m_pWindowSetup)
	{
		m_pWindowSetup->activateWindow();
		return;
	}

	KDialog *dlg = qobject_cast<KDialog*>(KXmlUI::windowFromFile("res:/Resources/layout/configure.xml"));
	m_pWindowSetup = dlg;
	KCheckButton *desktop = m_pWindowSetup->findChild<KCheckButton*>("desktop");
	KCheckButton *launch = m_pWindowSetup->findChild<KCheckButton*>("launch");
	bool bdesktop = KUtility::hasDesktopShortcut("善学备忘录");
	bool blaunch = KUtility::hasQuickLaunchShortcut("善学备忘录");
	desktop->setChecked(bdesktop);
	launch->setChecked(blaunch);
	KPushButton *btnOk = dlg->findChild<KPushButton*>("idok");
	QObject::connect(btnOk, SIGNAL(clicked()), this, SLOT(on_configure_idok_clicked()));
	KPushButton *btnCanncel = dlg->findChild<KPushButton*>("idcancel");
	QObject::connect(btnCanncel, SIGNAL(clicked()), dlg, SLOT(reject()));
	dlg->doModal();
	qDeleteLater(dlg);
}

void SXMemo::on_trayMenu_quit()
{
	QApplication::quit();
}

void SXMemo::restoreMemos()
{
	QString path = KUtility::osAppDataPath() + "\\shanxue\\memos.dat";
	QFile memosFile(path);
	if(memosFile.open(QFile::ReadOnly))
	{
		QByteArray data = memosFile.readAll();
		QBuffer buffer(&data);
		if(buffer.open(QBuffer::ReadOnly))
		{
			QDataStream in(&buffer);
			quint32 version;
			MemoInfoList mil;
			in >> version;
			if(mil.version == version)
			{
				in >> mil.memos;
			}
			else
			{

			}
			for(int i = 0; i < mil.memos.count(); i++)
			{
				const MemoInfo& mi = mil.memos.at(i);
				MemoEdit *edit = MemoEdit::createEdit(mi.winRt, QString::fromUtf8(mi.htmlContent.data()), true, false, this);
				m_memos.push_back(edit);
				QObject::connect(edit, SIGNAL(aboutToClose()), this, SLOT(on_memoEdit_aboutToClose()));
				QObject::connect(edit, SIGNAL(modifyChanged()), this, SLOT(on_memoEdit_modifyChanged()));
			}
		}
	}
}

void SXMemo::updateMemos()
{
	MemoInfoList mil;
	for(QList<QPointer<MemoEdit>>::iterator iter = m_memos.begin(); iter != m_memos.end(); iter++)
	{
		MemoEdit *edit = *iter;
		KTextEdit *textEdit = edit->textEdit();
		KWindow *window = edit->mainWindow();
		QRect winRt = window->geometry();
		QString html = textEdit->toHtml();
		QByteArray ba = html.toUtf8();
		MemoInfo mi;
		mi.htmlContent = ba;
		mi.winRt = winRt;
		mil.memos.push_back(mi);
	}
	QByteArray ba;
	QBuffer buffer(&ba);
	if(buffer.open(QBuffer::WriteOnly))
	{
		QDataStream ds(&buffer);
		ds << mil.version << mil.memos;
	}

	QString path = KUtility::osAppDataPath() + "\\shanxue\\memos.dat";
	KUtility::osCreateDirectory(path, true);
	QFile memosFile(path);
	if(memosFile.open(QFile::WriteOnly))
	{
		memosFile.write(ba);
	}
}

void SXMemo::on_memoEdit_aboutToClose()
{
	MemoEdit *obj = qobject_cast<MemoEdit*>(sender());
	m_memos.removeOne(obj);
	updateMemos();
}

void SXMemo::on_memoEdit_modifyChanged()
{
	updateMemos();
}

void SXMemo::on_trayMenu_foreground()
{
	for(QList<QPointer<MemoEdit>>::iterator iter = m_memos.begin(); iter != m_memos.end(); iter++)
	{
		MemoEdit *edit = *iter;
		KTextEdit *textEdit = edit->textEdit();
		KWindow *window = edit->mainWindow();
		window->activateWindow();
	}
}

void SXMemo::on_trayMenu_startup(bool bchecked)
{
	if(bchecked)
	{
		KUtility::createWindowStartRun(QApplication::applicationFilePath(), "sxmemo");
	}
	else
	{
		KUtility::removeWindowStartRun("sxmemo");
	}
}

void SXMemo::on_configure_idok_clicked()
{
	KCheckButton *desktop = m_pWindowSetup->findChild<KCheckButton*>("desktop");
	KCheckButton *launch = m_pWindowSetup->findChild<KCheckButton*>("launch");
	if(desktop->checked())
	{
		KUtility::createDesktopShortcut(QApplication::applicationFilePath(), "善学备忘录");
	}
	else
	{
		KUtility::removeDesktopShortcut("善学备忘录");
	}
	if(launch->checked())
	{
		KUtility::createQuickLaunchShortcut(QApplication::applicationFilePath(), "善学备忘录");
	}
	else
	{
		KUtility::removeQuickLaunchShortcut("善学备忘录");
	}
	m_pWindowSetup->setAccepted();
}

void SXMemo::on_tryMenu_timeout()
{
	m_trayIcon->setIcon(QIcon("res:/Resources/image/sxmemo.png"));
}

void SXMemo::on_trayMenu_reset()
{
	for(QList<QPointer<MemoEdit>>::iterator iter = m_memos.begin(); iter != m_memos.end(); iter++)
	{
		QPointer<MemoEdit> edit = *iter;
		if(edit == NULL)
		{
			continue;
		}
		KPopupWindow *pWindow = qobject_cast<KPopupWindow*>(edit->mainWindow());
		pWindow->moveCenter(NULL);
	}
}
