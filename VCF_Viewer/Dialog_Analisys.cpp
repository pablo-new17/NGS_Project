#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>

#include "Dialog_Analisys.h"
#include "ui_Dialog_Analisys.h"

Dialog_Analisys::Dialog_Analisys(QString VCF_Filename, QString GTF_Filename, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Dialog_Analisys)
{
	ui->setupUi(this);

	QFileInfo* VCF_Info = new QFileInfo(VCF_Filename);
	this->m_File_Size = VCF_Info->size();

	this->m_work = new Worker();
	this->m_work->moveToThread(&this->m_thread);

	this->m_work->setVCF_Filename(VCF_Filename);
	this->m_work->setGFT_Filename(GTF_Filename);

	connect(&this->m_thread, &QThread::started, this->m_work, &Worker::run);
	connect(&this->m_thread, &QThread::finished, this->m_work, &Worker::deleteLater);
	connect(this->m_work, &Worker::Position, this, &Dialog_Analisys::Position);
	connect(this->m_work, &Worker::Finished, this, &Dialog_Analisys::Finished);

	this->m_thread.start();
}

Dialog_Analisys::~Dialog_Analisys()
{
	delete ui;

	this->m_work->Stop();

	this->m_thread.quit();
	this->m_thread.wait();
}

void Dialog_Analisys::Position(qint64 Count)
{
	ui->progressBar->setValue(Count * 100 / this->m_File_Size);
}

void Dialog_Analisys::Finished()
{
	this->close();
}



