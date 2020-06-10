#ifndef VARIANT_CALL_FORMATPRIVATE_H
#define VARIANT_CALL_FORMATPRIVATE_H

#include <QObject>
#include <QIODevice>
#include <QTextStream>

#include "Variant_Call_Format.h"
#include "Database_VCF.h"

class Variant_Call_FormatPrivate : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Variant_Call_FormatPrivate)
	Q_DECLARE_PUBLIC(Variant_Call_Format)
private:
	Variant_Call_Format* const q_ptr;

public:
	explicit Variant_Call_FormatPrivate(Variant_Call_Format *parent = nullptr);
	~Variant_Call_FormatPrivate();

signals:

public slots:
	bool Open();
	void Read_Header();
	VO_Record* Read_Record();

	bool Parse_Metadata(QString Meta_Key);

public:
	QIODevice*			m_Device;
	QTextStream			m_Stream;

	QString				m_File_Format;
	QStringList			m_Samples_head;
	QMultiHash<QString, QString>	m_Meta_Datas;

	QHash<QString, QString>		m_Headers;
	QHash<QString, VO_Field>	m_Infos;
	QHash<QString, VO_Field>	m_Filters;
	QHash<QString, VO_Field>	m_Formats;
	QHash<QString, VO_Field>	m_Unknown;

	VO_Record*			m_Current_Record;
	qint64				m_Current_Position;
	Database_VCF*			m_Database;
};

#endif // VARIANT_CALL_FORMATPRIVATE_H
