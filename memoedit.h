#pragma once;

/*
²Ëµ¥»¯
*/
class MemoEdit : public QObject
{
	Q_OBJECT
public:
	explicit MemoEdit(QObject *parent=NULL);
	~MemoEdit();
	void execute();
	KWindow *mainWindow();
	KTextEdit *textEdit();
	static MemoEdit *createEdit(const QRect& rt, const QString& html, bool bReadOnly, bool bTranslucent, QObject* parent);

signals:
	void aboutToClose();
	void modifyChanged();

private slots:
	void on_textEdit_contextMenu(QMenu* menu, bool *pbContinue);
	void on_textEdit_mouseEvent(QEvent* e, bool *pbHandled);
	void on_textEdit_contextMenu_itemClicked(bool bChecked);

	void on_textEdit_readOnly();
	void on_textEdit_textChanged();

private:
	bool eventFilter(QObject *w, QEvent *e);

private:
	QPointer<KPopupWindow> m_pMainWindow;
	QPointer<KTextEdit> m_pMsgEdit;
	bool m_bTranslucent;
};