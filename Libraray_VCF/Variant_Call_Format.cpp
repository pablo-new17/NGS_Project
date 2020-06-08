#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "Variant_Call_Format.h"
#include "Variant_Call_FormatPrivate.h"

QString VO_Record::toJSON(QString Field_Index_Numbers)
{
	QJsonDocument Doc(this->JSON(Field_Index_Numbers));

	return QString(Doc.toJson());
}

QJsonObject VO_Record::JSON(QString Field_Index_Numbers)
{
	QJsonObject JSON;

	if(Field_Index_Numbers.contains("1"))
		JSON["CHROM"] = this->CHROM;

	if(Field_Index_Numbers.contains("2"))
		JSON["POS"] = QString::number(this->POS);

	if(Field_Index_Numbers.contains("3"))
		JSON["ID"] =this->ID;

	if(Field_Index_Numbers.contains("4"))
		JSON["REF"] = this->REF;

	if(Field_Index_Numbers.contains("5"))
	{
		QJsonArray ALT;
		foreach(QString const& entity, this->ALT)
			ALT.append(entity);

		JSON["ALT"] = ALT;
	}

	if(Field_Index_Numbers.contains("6"))
		JSON["QUAL"] = QString::number(this->QUAL);

	if(Field_Index_Numbers.contains("7"))
		JSON["FILTER"] = this->FILTER;

	if(Field_Index_Numbers.contains("8"))
	{
		QJsonArray INFO;
		foreach(QString const& entity, this->INFO.keys())
		{
			QJsonObject Element;
			Element[entity] = this->INFO.value(entity).toString();
			INFO.append(Element);
		}

		JSON["INFO"] = INFO;
	}

	if(Field_Index_Numbers.contains("9"))
		JSON["FORMAT"] = this->FORMAT;

	if(Field_Index_Numbers.contains("0"))
	{
		QJsonArray OTHER;
		foreach(QString const& entity, this->OTHER)
			OTHER.append(entity);

		JSON["OTHER"] = OTHER;
	}

	return JSON;
}




VO_Record* Variant_Call_Format::getCurrent_Record() const
{
	return m_Current_Record;
}

Variant_Call_Format::Variant_Call_Format(QObject *parent)
	: QObject(parent)
	, m_Private(new Variant_Call_FormatPrivate(this))
	, m_Current_Record(nullptr)
{
}

Variant_Call_Format::Variant_Call_Format(const QString& Filename, QObject* parent)
	: QObject(parent)
	, m_Private(new Variant_Call_FormatPrivate(this))
	, m_Current_Record(nullptr)
{
	Q_D(Variant_Call_Format);

	d->m_Device = new QFile(Filename);
}

Variant_Call_Format::Variant_Call_Format(QIODevice* Device, QObject* parent)
	: QObject(parent)
	, m_Private(new Variant_Call_FormatPrivate(this))
	, m_Current_Record(nullptr)
{
	Q_D(Variant_Call_Format);

	d->m_Device = Device;
}

Variant_Call_Format::~Variant_Call_Format()
{

}


QString Variant_Call_Format::fileformat()
{
	Q_D(Variant_Call_Format);

	if(!d->m_Headers.contains("fileformat"))
		d->Parse_Metadata("fileformat");

	return d->m_Headers.value("fileformat");
}
QString Variant_Call_Format::fileDate()
{
	Q_D(Variant_Call_Format);

	if(!d->m_Headers.contains("fileDate"))
		d->Parse_Metadata("fileDate");

	return d->m_Headers.value("fileDate");
}
QString Variant_Call_Format::reference()
{
	Q_D(Variant_Call_Format);

	if(!d->m_Headers.contains("reference"))
		d->Parse_Metadata("reference");

	return d->m_Headers.value("reference");
}


bool Variant_Call_Format::Open()
{
	Q_D(Variant_Call_Format);

	return d->Open();
}

bool Variant_Call_Format::Next()
{
	Q_D(Variant_Call_Format);

	if (d->m_Stream.atEnd())	return false;

	this->m_Current_Record = d->Read_Record();

	return (this->m_Current_Record != nullptr);
}

