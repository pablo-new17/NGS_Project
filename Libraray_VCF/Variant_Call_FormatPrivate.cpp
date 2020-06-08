#include <QRegularExpression>

#include "Variant_Call_FormatPrivate.h"

Variant_Call_FormatPrivate::Variant_Call_FormatPrivate(Variant_Call_Format* parent)
	: QObject(parent)
	, q_ptr(parent)
{
	this->m_Device = nullptr;

}

Variant_Call_FormatPrivate::~Variant_Call_FormatPrivate()
{
	this->m_Device->close();
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
		this->Read_Header();
		return true;
	}

	emit q->Last_Error("File open fail");
	return false;
}

void Variant_Call_FormatPrivate::Read_Header()
{
	this->m_Stream.reset();

	while (!this->m_Stream.atEnd())
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

			// end of Head section
			return;
		}

		// get Special keywords
		else if (line.startsWith("##"))
		{
			// get  Meta-information lines

			QRegularExpression ex("^##(.*?)=(.*)");
			QRegularExpressionMatch match = ex.match(line);
			QString Meta = match.captured(1);
			QString Contain = match.captured(2);

			this->m_Meta_Datas.insert(Meta, Contain);

		}
	}

	return;
}

VO_Record* Variant_Call_FormatPrivate::Read_Record()
{
	Q_Q(Variant_Call_Format);

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

	return Record;
}

bool Variant_Call_FormatPrivate::Parse_Metadata(QString Meta_Key)
{
	if(!this->m_Meta_Datas.contains(Meta_Key))	return false;

	while (this->m_Meta_Datas.contains(Meta_Key))
	{
		QString Value = this->m_Meta_Datas.value(Meta_Key);
		this->m_Meta_Datas.remove(Meta_Key, Value);

		if(Value.startsWith("<"))
		{
			QRegularExpression ex("^<ID=(.+),Number=(.+),Type=(.+),Description=\"(.+)\",Source=\"(.+)\",Version=\"(.+)\"(.+)");
			QRegularExpressionMatch match = ex.match(Value);
			VO_Field data;

			data.ID = match.captured(1);
			data.Number = match.captured(2);
			data.Type = match.captured(3);
			data.Description = match.captured(4);
			data.Source = match.captured(5);
			data.version = match.captured(6);
			data.Others = match.captured(7);

			if(Meta_Key == "INFO")		// get Information field format
				this->m_Infos.insert(data.ID, data);
			else if(Meta_Key == "FILTER")	// get Filter field format
				this->m_Filters.insert(data.ID, data);
			else if(Meta_Key == "FORMAT")	// get Individual format field format
				this->m_Formats.insert(data.ID, data);
			else
				this->m_Unknown.insert(data.ID, data);
		}
		else
		{
			this->m_Headers.insert(Meta_Key, Value);
		}
	}

	return true;
}


