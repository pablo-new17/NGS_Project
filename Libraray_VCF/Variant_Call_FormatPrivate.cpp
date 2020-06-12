#include <QDebug>
#include <QRegularExpression>

#include "Variant_Call_FormatPrivate.h"

Variant_Call_FormatPrivate::Variant_Call_FormatPrivate(Variant_Call_Format* parent)
	: QObject(parent)
	, q_ptr(parent)
{
	this->m_Device = nullptr;
	this->m_Record_Part = false;
	this->m_Last_Record = nullptr;
}

Variant_Call_FormatPrivate::~Variant_Call_FormatPrivate()
{
	this->m_Device->close();

	if(this->m_Last_Record)
		delete this->m_Last_Record;

	foreach(VO_Field* field, this->m_Infos)
	{
		delete field;
	}
	foreach(VO_Field* field, this->m_Filters)
	{
		delete field;
	}
	foreach(VO_Field* field, this->m_Formats)
	{
		delete field;
	}
}

bool Variant_Call_FormatPrivate::Open()
{
	Q_Q(Variant_Call_Format);

	if(this->m_Device == nullptr)
	{
		emit q->Last_Error("File does not assigned");
		return false;
	}

	if (this->m_Device->open(QIODevice::ReadOnly))
	{
		this->m_Stream.setDevice(m_Device);

		this->m_Stream.reset();
		return true;
	}

	emit q->Last_Error("File open fail");
	return false;
}

VO_Field* Variant_Call_FormatPrivate::Read_Header()
{
	if (!this->m_Stream.atEnd() && this->m_Record_Part == false)
	{
		QString line = this->m_Stream.readLine();

		// Get samples headers(title)
		if (line.startsWith("#CHROM"))
		{
			QStringList header = line.mid(1).split(QRegExp("\\s+"));
			if (header.size() > 9)
			{
				for (int i = 9 ; i< header.size(); ++i)
					this->m_Samples_head.append(header[i]);
			}

			this->m_Record_Part = true;
		}

		// get Special keywords
		else if (line.startsWith("##"))
		{
			// get  Meta-information lines

			QRegularExpression ex("^##(.*?)=(.*)");
			QRegularExpressionMatch match = ex.match(line);
			QString Meta = match.captured(1);
			QString Contain = match.captured(2);

			return this->Parse_Metadata(Meta, Contain);
		}
	}

	return nullptr;
}

VO_Record* Variant_Call_FormatPrivate::Read_Record()
{
	Q_Q(Variant_Call_Format);

	this->m_Current_Position = this->m_Stream.pos();

	QString Data = this->m_Stream.readLine();
	QStringList row = Data.split(QRegExp("\\s+"));

	if (row.count() < 8)
	{
		q->Last_Error("not enough line. Are you sure it's a VCF?");
		return nullptr;
	}

	VO_Record* Record = new VO_Record();
	QStringList alts;
	QVariantMap infos;

	Record->CHROM	= row[0];
	Record->POS	= row[1].toInt();
	Record->ID	= row[2];
	Record->REF	= row[3];

	// Split alternative.. A,C => [A,C]
	if (row[4].contains(","))
		alts = row[4].split(",");
	else
		alts.append(row[4]);
	Record->ALT	= alts;
	Record->QUAL    = row[5].toInt();
	Record->FILTER  = row[6];

	// parse INFO
	for (QString infomation : row[7].split(";"))
	{
		QStringList s = infomation.split("=");
		if (s.count() == 2)
		{
			QVariant val;
			// check if multi values
			if (s[1].contains(","))
				val = s[1].split(",");
			else
				val = s[1];

			infos.insert(s[0], val);
		}
		else
		{
			infos.insert(infomation, "");
		}
	}
	Record->INFO	= infos;

	// Parse samples if exists
	if (row.count() > 8)
	{
		Record->FORMAT	= row[8];

		// remain fields
		Record->OTHER.clear();
		for (int i = 9 ; i < row.size(); ++i)
		{
			Record->OTHER.append(row[i]);
		}

	}

	emit q->Record_Added(this->m_Current_Position, Record);
	this->m_Last_Record = Record;

	return Record;
}


VO_Field* Variant_Call_FormatPrivate::Parse_Metadata(QString Meta_Key, QString Value)
{
	Q_Q(Variant_Call_Format);
	VO_Field* data = new VO_Field();

	if(Value.startsWith("<"))
	{
		QString Meta_Head_Data = Value.mid(1, Value.size()-2);

		while (!Meta_Head_Data.isEmpty())
		{
			if(Meta_Head_Data.startsWith("ID="))
			{
				data->ID = Meta_Head_Data.section(",", 0, 0).section("=", 1, 1).trimmed();
				Meta_Head_Data = Meta_Head_Data.section(",", 1).trimmed();
			}
			else if(Meta_Head_Data.startsWith("Number="))
			{
				data->Number = Meta_Head_Data.section(",", 0, 0).section("=", 1, 1).trimmed();
				Meta_Head_Data = Meta_Head_Data.section(",", 1).trimmed();
			}
			else if(Meta_Head_Data.startsWith("Type="))
			{
				data->Number = Meta_Head_Data.section(",", 0, 0).section("=", 1, 1).trimmed();
				Meta_Head_Data = Meta_Head_Data.section(",", 1).trimmed();
			}
			else if(Meta_Head_Data.startsWith("Description="))
			{
				data->Description = Meta_Head_Data.section("\",", 0, 0).trimmed();
				Meta_Head_Data = Meta_Head_Data.section("\",", 1).trimmed();

				if(!data->Description.endsWith('"'))
					data->Description.append('\"');
				data->Description = data->Description.section("=", 1, 1).trimmed();
				data->Description = data->Description.mid(1, data->Description.size()-2);
			}
			else if(Meta_Head_Data.startsWith("Source="))
			{
				data->Source = Meta_Head_Data.section("\",", 0, 0).trimmed();
				Meta_Head_Data = Meta_Head_Data.section("\",", 1).trimmed();

				if(!data->Source.endsWith('"'))
					data->Source.append('\"');
				data->Source = data->Source.section("=", 1, 1).trimmed();
				data->Source = data->Source.mid(1, data->Source.size()-2);
			}
			else if(Meta_Head_Data.startsWith("Version="))
			{
				data->Version = Meta_Head_Data.section("\",", 0, 0).trimmed();
				Meta_Head_Data = Meta_Head_Data.section("\",", 1).trimmed();

				if(!data->Version.endsWith('"'))
					data->Version.append('\"');
				data->Version = data->Version.section("=", 1, 1).trimmed();
				data->Version = data->Version.mid(1, data->Version.size()-2);
			}
			else
			{
				data->Others = Meta_Head_Data.trimmed();
				Meta_Head_Data = "";
				qDebug() << "Unknown data:" << data->Others;
			}
		}
	}
	else
	{
		data->Others = Value;
	}

	if(Meta_Key=="INFO")
		this->m_Infos.insert(Meta_Key, data);
	else if(Meta_Key=="FILTER")
		this->m_Filters.insert(Meta_Key, data);
	else if(Meta_Key=="FORMAT")
		this->m_Formats.insert(Meta_Key, data);
	else
		this->m_Headers.insert(Meta_Key, Value);

	emit q->Header_Added(Meta_Key, data);

	return data;
}


