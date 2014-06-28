#include "stdafx.h"
#include "memoedit.h"

MemoEdit::MemoEdit( QObject *parent/*=NULL*/ )
 : QObject(parent)
{
	m_pMainWindow = qobject_cast<KPopupWindow*>(KXmlUI::windowFromFile("app:/resources/layout/sxmemo.xml"));
	m_pMsgEdit = m_pMainWindow->findChild<KTextEdit*>("memoEdit");

	QObject::connect(m_pMsgEdit, SIGNAL(contextMenu(QMenu*,bool*)), this, SLOT(on_textEdit_contextMenu(QMenu*,bool*)));
	QObject::connect(m_pMsgEdit->realWidget(), SIGNAL(mouseEvent(QEvent*,bool*)), this, SLOT(on_textEdit_mouseEvent(QEvent*,bool*)));
	QObject::connect(m_pMsgEdit->realWidget(), SIGNAL(textChanged()), this, SLOT(on_textEdit_textChanged()));
	m_pMsgEdit->setHasContextMenu(true);
	m_pMainWindow->installEventFilter(this);

	QObject::connect(m_pMainWindow, SIGNAL(aboutToClose()), this, SIGNAL(aboutToClose()));
}

MemoEdit::~MemoEdit()
{

}

void MemoEdit::execute()
{

}

void MemoEdit::on_textEdit_contextMenu( QMenu* menu, bool *pbContinue )
{
	*pbContinue = false;
	KMenu *pmenu = qobject_cast<KMenu*>(menu);
	if(pmenu)
	{
		pmenu->setBackgroundColor(QColor(255,255,0,200));
	}

	if(m_pMsgEdit->isReadOnly())
	{
		{
			QAction *action = menu->addAction("开启编辑");
			action->setProperty("id", 3002);
			QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
		}

		{
			QAction *action = menu->addAction("关闭窗口");
			action->setProperty("id", 3003);
			QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
		}

		return;
	}
	{
		QAction *action = menu->addAction("红色");
		action->setProperty("id", 1001);
		action->setProperty("color", QColor(Qt::red));
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
	}
	{
		QAction *action = menu->addAction("绿色");
		action->setProperty("id", 1002);
		action->setProperty("color", QColor(Qt::green));
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
	}
	{
		QAction *action = menu->addAction("蓝色");
		action->setProperty("id", 1003);
		action->setProperty("color", QColor(Qt::blue));
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
	}
	{
		QAction *action = menu->addAction("黑色");
		action->setProperty("id", 1004);
		action->setProperty("color", QColor(Qt::black));
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
	}
	menu->addSeparator();
	{
		QAction *action = menu->addAction("常规");
		action->setProperty("id", 2001);
		QFont f = m_pMsgEdit->currentFont();
		f.setStrikeOut(false);
		action->setProperty("font", f);
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
	}
	{
		QAction *action = menu->addAction("删除线");
		action->setProperty("id", 2002);
		QFont f = m_pMsgEdit->currentFont();
		f.setStrikeOut(true);
		action->setProperty("font", f);
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
	}
	menu->addSeparator();
	{
		QAction *action = new QAction("半透明", NULL);
		bool bTrans = m_pMainWindow->translucent();
		action->setCheckable(true);
		action->setChecked(bTrans);
		action->setProperty("id", 3001);
		menu->addAction(action);
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
	}
	{
		QAction *action = menu->addAction("退出编辑");
		action->setProperty("id", 3002);
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
	}
	{
		QAction *action = menu->addAction("关闭窗口");
		action->setProperty("id", 3003);
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_textEdit_contextMenu_itemClicked(bool)));
	}
	*pbContinue = false;
}

void MemoEdit::on_textEdit_contextMenu_itemClicked( bool bChecked )
{
	QAction *action = qobject_cast<QAction*>(sender());
	if(action == NULL)
		return;
	int id = action->property("id").toInt();
	if(id >= 1000 && id < 2000)
	{
		//颜色
		QVariant vcolor = action->property("color");
		QColor color = vcolor.value<QColor>();
		m_pMsgEdit->setTextColor(color);
		KTextCursor cursor = m_pMsgEdit->textCursor();
		if(cursor.hasSelection())
		{
			cursor.clearSelection();
			m_pMsgEdit->setTextCursor(cursor);
		}
	}
	else if(id >= 2000 && id < 3000)
	{
		//字体
		QVariant vfont = action->property("font");
		QFont f = vfont.value<QFont>();
		m_pMsgEdit->setCurrentFont(f);
	}
	else if(id == 3001)
	{
		//
		bool bcheck = action->isChecked();
		m_bTranslucent = bcheck;
		m_pMainWindow->setTranslucent(bcheck);
	}
	else if(id == 3002)
	{
		bool bReadOnly = m_pMsgEdit->isReadOnly();
		m_pMsgEdit->setReadOnly(!bReadOnly);
	}
	else if(id == 3003)
	{
		m_pMainWindow->close();
	}
}

void MemoEdit::on_textEdit_mouseEvent( QEvent* e, bool *pbHandled )
{
	if(e->type() == QEvent::MouseButtonDblClick)
	{
		QMouseEvent *me = (QMouseEvent*)e;
		if(me->button() != Qt::LeftButton)
			return;
		bool bReadOnly = m_pMsgEdit->isReadOnly();
		if(bReadOnly)
		{
			m_pMsgEdit->setReadOnly(false);
		}
	}
}

bool MemoEdit::eventFilter( QObject *w, QEvent *e )
{
	if(e->type() == QEvent::WindowDeactivate)
	{
		QMetaObject::invokeMethod(this, "on_textEdit_readOnly", Qt::QueuedConnection);
	}
	return false;
}

void MemoEdit::on_textEdit_readOnly()
{
	if(m_pMsgEdit->isModified())
	{
		emit modifyChanged();
	}
	m_pMsgEdit->setReadOnly(true);
}

KTextEdit * MemoEdit::textEdit()
{
	return m_pMsgEdit;
}

KWindow * MemoEdit::mainWindow()
{
	return m_pMainWindow;
}

MemoEdit * MemoEdit::createEdit( const QRect& rt, const QString& html, bool bReadOnly, bool bTranslucent, QObject* parent )
{
	MemoEdit *edit = new MemoEdit();
	KWindow *window = edit->mainWindow();
	window->setGeometry(rt);
	KTextEdit *pTextEdit = edit->textEdit();
	pTextEdit->setText(html);
	pTextEdit->setReadOnly(bReadOnly);
	window->setProperty("translucent", bTranslucent);
	window->show();
	return edit;
}

void MemoEdit::on_textEdit_textChanged()
{
	QTextDocument *doc = m_pMsgEdit->document();
	doc->adjustSize();
	QSizeF s = doc->size();
	QSizeF winSize = s + QSizeF(20,20);
	m_pMainWindow->resize(winSize.toSize());
}
