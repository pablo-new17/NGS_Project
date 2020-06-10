#include <QDebug>
#include <QRegularExpression>

#include "Variant_Call_FormatPrivate.h"

Variant_Call_FormatPrivate::Variant_Call_FormatPrivate(Variant_Call_Format* parent)
	: QObject(parent)
	, q_ptr(parent)
{
	this->m_Device = nullptr;
	this->m_Database = nullptr;
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

	if(this->m_Database != nullptr)
	{
		QMap<QString, QVariant> Record_Data;
		Record_Data.insert("起始位置", this->m_Current_Position);
		Record_Data.insert("染色體", Record->CHROM);
		Record_Data.insert("位置", Record->POS);
		Record_Data.insert("品質", Record->QUAL);

		this->m_Database->Insert_Record(Record_Data);
	}

	this->m_Current_Record = Record;
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
			VO_Field data;
			QString Meta_Head_Data = Value.mid(1, Value.size()-2);

			while (!Meta_Head_Data.isEmpty())
			{
				if(Meta_Head_Data.startsWith("ID="))
				{
					data.ID = Meta_Head_Data.section(",", 0, 0).section("=", 1, 1).trimmed();
					Meta_Head_Data = Meta_Head_Data.section(",", 1).trimmed();
				}
				else if(Meta_Head_Data.startsWith("Number="))
				{
					data.Number = Meta_Head_Data.section(",", 0, 0).section("=", 1, 1).trimmed();
					Meta_Head_Data = Meta_Head_Data.section(",", 1).trimmed();
				}
				else if(Meta_Head_Data.startsWith("Type="))
				{
					data.Number = Meta_Head_Data.section(",", 0, 0).section("=", 1, 1).trimmed();
					Meta_Head_Data = Meta_Head_Data.section(",", 1).trimmed();
				}
				else if(Meta_Head_Data.startsWith("Description="))
				{
					data.Description = Meta_Head_Data.section("\",", 0, 0).trimmed();
					Meta_Head_Data = Meta_Head_Data.section("\",", 1).trimmed();

					if(!data.Description.endsWith('"'))
						data.Description.append('\"');
					data.Description = data.Description.section("=", 1, 1).trimmed();
					data.Description = data.Description.mid(1, data.Description.size()-2);
				}
				else if(Meta_Head_Data.startsWith("Source="))
				{
					data.Source = Meta_Head_Data.section("\",", 0, 0).trimmed();
					Meta_Head_Data = Meta_Head_Data.section("\",", 1).trimmed();

					if(!data.Source.endsWith('"'))
						data.Source.append('\"');
					data.Source = data.Source.section("=", 1, 1).trimmed();
					data.Source = data.Source.mid(1, data.Source.size()-2);
				}
				else if(Meta_Head_Data.startsWith("Version="))
				{
					data.Version = Meta_Head_Data.section("\",", 0, 0).trimmed();
					Meta_Head_Data = Meta_Head_Data.section("\",", 1).trimmed();

					if(!data.Version.endsWith('"'))
						data.Version.append('\"');
					data.Version = data.Version.section("=", 1, 1).trimmed();
					data.Version = data.Version.mid(1, data.Version.size()-2);
				}
				else
				{
					data.Others = Meta_Head_Data.trimmed();
					Meta_Head_Data = "";
					qDebug() << "Unknown data:" << data.Others;
				}
			}

			QMap<QString, QVariant> Record_Data;
			Record_Data.insert("識別碼", data.ID);
			Record_Data.insert("號碼", data.Number);
			Record_Data.insert("型態", data.Type);
			Record_Data.insert("描述", data.Description);
			Record_Data.insert("來源", data.Source);
			Record_Data.insert("版本", data.Version);
			Record_Data.insert("其他", data.Others);

			if(Meta_Key == "INFO")		// get Information field format
			{
				this->m_Infos.insert(data.ID, data);

				if(this->m_Database != nullptr)
				{
					Record_Data.insert("類別", QString("INFO"));
					this->m_Database->Insert_Header(Record_Data);
				}
			}
			else if(Meta_Key == "FILTER")	// get Filter field format
			{
				this->m_Filters.insert(data.ID, data);

				if(this->m_Database != nullptr)
				{
					Record_Data.insert("類別", QString("FILTER"));
					this->m_Database->Insert_Header(Record_Data);
				}
			}
			else if(Meta_Key == "FORMAT")	// get Individual format field format
			{
				this->m_Formats.insert(data.ID, data);

				if(this->m_Database != nullptr)
				{
					Record_Data.insert("類別", QString("FORMAT"));
					this->m_Database->Insert_Header(Record_Data);
				}
			}
			else
			{
				this->m_Unknown.insert(data.ID, data);

				if(this->m_Database != nullptr)
				{
					Record_Data.insert("類別", QString("UNKNON"));
					this->m_Database->Insert_Header(Record_Data);
				}
			}
		}
		else
		{
			this->m_Headers.insert(Meta_Key, Value);
		}
	}

	return true;
}


