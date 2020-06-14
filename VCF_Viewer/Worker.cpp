#include <QDebug>
#include <QThread>
#include <QFileInfo>

#include "Worker.h"

Worker::Worker(QObject *parent) : QObject(parent)
{
	this->m_Stop = false;
	this->m_Restart = false;

	this->m_VCF = nullptr;
	this->m_DB_VCF = nullptr;
	this->m_DB_GFT = nullptr;
}

Worker::~Worker()
{
	this->m_Mutex.lock();
	this->m_Stop = true;
	this->m_Mutex.unlock();

	this->m_Condition.wakeAll();

	if(this->m_VCF)		delete this->m_VCF;
	if(this->m_DB_VCF)	delete this->m_DB_VCF;
	if(this->m_DB_GFT)	delete this->m_DB_GFT;
}

void Worker::run()
{
//	forever
	{
		this->m_Mutex.lock();
		QString VCF_Filename = this->m_VCF_Filename;
		QString GFT_Filename = this->m_GFT_Filename;
		this->m_Mutex.unlock();

		// VCF 文字檔
		Variant_Call_Format* m_VCF = new Variant_Call_Format(VCF_Filename, this);
		connect(m_VCF, &Variant_Call_Format::Header_Added, this, &Worker::Header_Added);
		connect(m_VCF, &Variant_Call_Format::Record_Added, this, &Worker::Record_Added);
		// VCF 資料庫
		QFileInfo* VCF_Info = new QFileInfo(VCF_Filename);
		QString Path = VCF_Info->path();
		QString file = VCF_Info->fileName().append(".db");
		this->m_DB_VCF = new Database_VCF(file, Path, this);
		this->m_DB_VCF->Create_Table();
		// GTF 資料庫
		this->m_DB_GFT = new Database_GTF(GFT_Filename, this);

		if(m_VCF->Open())
		{
			int Position = 0;
			while (m_VCF->Head())
			{
				Position++;
				emit this->Position(Position);
			};
			while (m_VCF->Next())
			{

			};
		}

		// parse done
		emit this->Finished();



		this->m_Mutex.lock();
		if(!this->m_Restart)
			this->m_Condition.wait(&this->m_Mutex);
		this->m_Restart = false;
		this->m_Mutex.unlock();


	}
}

void Worker::Stop()
{
	this->m_Mutex.lock();
	this->m_Stop = true;
	this->m_Mutex.unlock();

	this->m_Condition.wakeOne();
}


void Worker::Header_Added(QString Meta_Key, VO_Field* New_Head_Field)
{
	if(Meta_Key=="INFO" || Meta_Key=="FILTER" || Meta_Key=="FORMAT")
	{
		QMap<QString, QVariant> Record_Data;
		Record_Data.insert("類別", Meta_Key);
		Record_Data.insert("識別碼", New_Head_Field->ID);
		Record_Data.insert("號碼", New_Head_Field->Number);
		Record_Data.insert("型態", New_Head_Field->Type);
		Record_Data.insert("描述", New_Head_Field->Description);
		Record_Data.insert("來源", New_Head_Field->Source);
		Record_Data.insert("版本", New_Head_Field->Version);
		Record_Data.insert("其他", New_Head_Field->Others);

		this->m_DB_VCF->Insert_Header(Record_Data);
	}
}

void Worker::Record_Added(qint64 Position, VO_Record* New_Record)
{
	QMap<QString, QVariant> Record_Data;
	Record_Data.insert("起始位置", Position);
	Record_Data.insert("染色體", New_Record->CHROM);
	Record_Data.insert("位置", New_Record->POS);
	Record_Data.insert("品質", New_Record->QUAL);
	QStringList Names = this->m_DB_GFT->Gene_name(New_Record->CHROM, New_Record->POS);
	if(Names.size())
		Record_Data.insert("名稱", Names.join("|"));

	this->m_DB_VCF->Insert_Record(Record_Data);
	emit this->Position(Position);
}

QString Worker::getGFT_Filename() const
{
	return m_GFT_Filename;
}

void Worker::setGFT_Filename(const QString& GFT_Filename)
{
	m_GFT_Filename = GFT_Filename;
}

QString Worker::getVCF_Filename() const
{
	return m_VCF_Filename;
}

void Worker::setVCF_Filename(const QString& VCF_Filename)
{
	m_VCF_Filename = VCF_Filename;
}

