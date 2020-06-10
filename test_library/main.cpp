#include <QDebug>
#include <QCoreApplication>

#include "Variant_Call_Format.h"

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


	Variant_Call_Format reader(Filename);
	if (reader.Open())
	{
		qDebug() << reader.fileformat();
		qDebug() << reader.fileDate();
		qDebug() << reader.reference();

		reader.Infos();
		reader.Filters();
		reader.Formats();

		while (reader.Next())
		{
			if(Field_Number.isEmpty())
				qDebug() << reader.getCurrent_Record()->CHROM
					 << reader.getCurrent_Record()->POS
					 << reader.getCurrent_Record()->ID
					 << reader.getCurrent_Record()->REF
					 << reader.getCurrent_Record()->ALT
					 << reader.getCurrent_Record()->QUAL ;
			else
				qDebug() << reader.getCurrent_Record()->toJSON(Field_Number);

		}
	}
	else
	{
		qDebug()<< "Error open";
	}

	return 0;
}
