#ifndef DIALOG_ANALISYS_H
#define DIALOG_ANALISYS_H

#include <QDialog>
#include <QThread>
#include <QMutex>

#include "Variant_Call_Format.h"
#include "Database_VCF.h"
#include "Database_GTF.h"
#include "Worker.h"

namespace Ui {
	class Dialog_Analisys;
}

class Dialog_Analisys : public QDialog
{
	Q_OBJECT

public:
	explicit Dialog_Analisys(QString VCF_Filename, QString GTF_Filename, QWidget *parent = nullptr);
	~Dialog_Analisys();

public slots:
	void Position(qint64 Count);
	void Finished();

private:
	Ui::Dialog_Analisys *ui;
	int m_File_Size;

	QThread		m_thread;
	Worker*		m_work;
	QMutex		m_mutex;

	Variant_Call_Format* m_VCF;
	Database_VCF* m_DB_VCF;
	Database_GTF* m_DB_GFT;
};

#endif // DIALOG_ANALISYS_H
