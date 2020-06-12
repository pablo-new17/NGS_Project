#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>

#include "Variant_Call_Format.h"
#include "Database_VCF.h"
#include "slot_test.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QString Filename =  QString("./test.vcf");
	QString Field_Number;
	int Index = 1;

	while (Index < argc)
	{
		QString argument = QString(argv[Index]);
		if(argument.startsWith("-j"))
		{
			QStringList fields = argument.split("=");
			if(fields.size()==2)
			{
				Field_Number = fields[1];
			}
			else
				Field_Number = "1234567890";

		}
		else
		{
			Filename = QString(argv[Index]);
		}

		Index++;
	}


	Variant_Call_Format* reader = new Variant_Call_Format(Filename);
	Database_VCF* DB = nullptr;
	QFileInfo* info = new QFileInfo(Filename);
	if(info->exists())
	{
		QString Path = info->path();
		QString file = info->fileName().append(".db");

		info = new QFileInfo(Path+"/"+file);
		if(!info->exists())
		{
			DB = new Database_VCF(file, Path);
		}
	}
	slot_test* msg = new slot_test(reader, DB);

	if (reader->Open())
	{
		while (reader->Head())
		{

		}
		qDebug() << reader->fileformat();
		qDebug() << reader->fileDate();
		qDebug() << reader->reference();

//		bool aaa = true;
		while (reader->Next())
		{
//			if(Field_Number.isEmpty())
//				qDebug() << reader->getCurrent_Record()->CHROM
//					 << reader->getCurrent_Record()->POS
//					 << reader->getCurrent_Record()->ID
//					 << reader->getCurrent_Record()->REF
//					 << reader->getCurrent_Record()->ALT
//					 << reader->getCurrent_Record()->QUAL ;
//			else
//				qDebug() << reader->getCurrent_Record()->toJSON(Field_Number);

//			if(aaa)
//			{
//				aaa = false;
//				reader.SetPos(1533);
//			}
		}
	}
	else
	{
		qDebug()<< "Error open";
	}

	return 0;
}
