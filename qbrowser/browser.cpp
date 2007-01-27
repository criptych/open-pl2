#include <QApplication>
#include <browser.hpp>

void PL2Browser::pakSelect(int id) {
	void *a = pl2Pak_unpackID(pak, id, NULL);
	free(a);

}

void PL2Browser::hook(const char *msg) {
	char buf[512];
	strncpy(buf, msg, sizeof(buf));
	if (buf[strlen(buf)-1]=='\n')
		buf[strlen(buf)-1]='\0';
	console->append(buf);
}

void PL2Browser::hookC(const char *msg, void *user) {
	PL2Browser *self = (PL2Browser*)user;
	if (self==NULL)
		return;

	self->hook(msg);
}

PL2Browser::PL2Browser(pl2Pak *lpak) {
	pak = lpak;
	list = new QListWidget();

	for (u32 i=0;i<lpak->EntryCount;i++)
		list->addItem(lpak->Entries[i].Name);

	list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QObject::connect(list, SIGNAL(currentRowChanged(int)), this, SLOT(pakSelect(int)));

	console = new QTextEdit();
	console->setReadOnly(true);
	console->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	console->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	console->verticalScrollBar()->

	QGroupBox *info = new QGroupBox("Status");
	info->setMinimumSize(500,120);
	info->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	info->setFlat(true);
	info->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
	info->layout()->addWidget(console);
	info->layout()->setMargin(0);
	info->layout()->setSpacing(0);



	main = new QSplitter(Qt::Vertical);
	main->addWidget(list);
	main->addWidget(info);

	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(main);
	setLayout(mainLayout);

	setWindowTitle("Open PL2 Browser");

	for (u32 i=0;;i++) {
		const char *msg = pl2_outputGet(i);
		if (msg==NULL)
			break;
		hook(msg);
	}
	pl2_outputHook(PL2Browser::hookC, this);
}

static void pl2_critical() {
	QString myerror;
	for (u32 i=0;;i++) {
		const char *msg = pl2_outputGet(i);
		if (msg==NULL)
			break;
		myerror.append(msg);
	}
	QMessageBox::critical(NULL, "OpenPL2 Browser", myerror, 0);
}

int main(int argc, char *argv[]) {
	const char *filename=NULL;
	QApplication app(argc, argv);

	if (argc>=2)
		filename = argv[1];

	if (filename==NULL || filename[0]=='\0')
		filename = QFileDialog::getOpenFileName(NULL, "Choose file to open...", "", "PL2 Paks (*.pl2)").toLocal8Bit();

	if (filename==NULL || filename[0]=='\0') {
		QMessageBox::information(NULL, "OpenPL2 Browser", "No file was selected or given on the command line!", 0);
		return 1;
	}

	pl2Pak *myPak = pl2Pak_load(filename);
	if (myPak==NULL) {
		output_error("Failed to open '%s' pak file\n", filename);
		pl2_critical();
		return 1;
	}
	output_message("Opened pak file '%s'\n", filename);

	PL2Browser browser(myPak);
	browser.show();
	return app.exec();
}

