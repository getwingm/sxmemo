#ifndef XXMEMO_H
#define XXMEMO_H


class MemoEdit;

/*
1.只提供三种颜色，用于区别不同情况。
2.全部改为右键菜单。*/

struct MemoInfo
{
	QRect winRt;
	bool bTranslucent;
	QByteArray htmlContent;//Html格式的内容。
};

struct MemoInfoList
{
	quint32 version; 
	QList<MemoInfo>  memos;

	MemoInfoList()
	{
		version = 1001;
	}
};

class SXMemo : public QObject
{
	Q_OBJECT

public:
	explicit SXMemo(QObject *parent=NULL);
	~SXMemo();

	void execute();
private slots:
	void on_trayMenu_activated(QSystemTrayIcon::ActivationReason reason);
	void on_trayMenu_createNew();
	void on_trayMenu_about();
	void on_trayMenu_setup();
	void on_trayMenu_startup(bool bchecked);
	void on_trayMenu_quit();
	void on_trayMenu_reset();
	void on_trayMenu_foreground();
	void on_memoEdit_aboutToClose();
	void on_memoEdit_modifyChanged();
	void on_configure_idok_clicked();
	void on_tryMenu_timeout();

private:
	void restoreMemos();
	void updateMemos();

private:
	QList<QPointer<MemoEdit>> m_memos;
	QPointer<QSystemTrayIcon> m_trayIcon;
	QPointer<KDialogBase> m_pWindowAbout;
	QPointer<KDialogBase> m_pWindowSetup;
	QTimer m_timer;
};

#endif // XXMEMO_H
