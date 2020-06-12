#include <QDebug>

#include "slot_test.h"

slot_test::slot_test(Variant_Call_Format* vcf, Database_VCF* DB, QObject* parent)
	: QObject(parent)
{
	this->m_VCF = DB;
	this->m_GFT = new Database_GTF("hg19.refGene.gtf.db");

	connect(vcf, &Variant_Call_Format::Last_Error, this, &slot_test::Last_Error);
	connect(vcf, &Variant_Call_Format::Header_Added, this, &slot_test::Header_Added);
	connect(vcf, &Variant_Call_Format::Record_Added, this, &slot_test::Record_Added);

	if(this->m_VCF)
		this->m_VCF->Create_Table();
}


void slot_test::Last_Error(QString Error_Message)
{
	qDebug() << "Error_Message" << Error_Message;
}

void slot_test::Header_Added(QString Meta_Key, VO_Field* New_Head_Field)
{
	if(this->m_VCF)
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

			this->m_VCF->Insert_Header(Record_Data);
			qDebug() << "New Header" << Meta_Key << Record_Data;
		}
	}
}

void slot_test::Record_Added(qint64 Position, VO_Record* New_Record)
{
	if(this->m_VCF)
	{
		QMap<QString, QVariant> Record_Data;
		Record_Data.insert("起始位置", Position);
		Record_Data.insert("染色體", New_Record->CHROM);
		Record_Data.insert("位置", New_Record->POS);
		Record_Data.insert("品質", New_Record->QUAL);
		QStringList Names = this->m_GFT->Gene_name(New_Record->CHROM, New_Record->POS);
		if(Names.size())
			Record_Data.insert("名稱", Names.join("|"));

		this->m_VCF->Insert_Record(Record_Data);
		qDebug() << "New VO_Record" << Record_Data;
	}
}

