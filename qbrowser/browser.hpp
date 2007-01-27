#ifndef _OpenPL2_browser_h_
#define _OpenPL2_browser_h_ INCLUDED

#include <QtGui>
#include <libpl2.h>

class PL2Browser : public QWidget {
    Q_OBJECT

public:
    PL2Browser(
		pl2Pak *pak
	);

private:
	void
	hook(
		const char *msg
	);

	static
	void
	hookC(
		const char *msg,
		void *user
	);
	pl2Pak *pak;
	QSplitter *main;
	QListWidget *list;
	QTextEdit *console;

public slots:
	void
	pakSelect(
		int id
	);

};


#endif

