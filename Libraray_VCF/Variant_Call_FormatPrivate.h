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
	VO_Field* Read_Header();
	VO_Record* Read_Record();

	VO_Field* Parse_Metadata(QString Meta_Key, QString Value);

public:
	QIODevice*			m_Device;
	QTextStream			m_Stream;

	QStringList			m_Samples_head;
	QHash<QString, QString>		m_Headers;
	QHash<QString, VO_Field*>	m_Infos;
	QHash<QString, VO_Field*>	m_Filters;
	QHash<QString, VO_Field*>	m_Formats;

	bool				m_Record_Part;
	VO_Record*			m_Last_Record;
	qint64				m_Current_Position;
};

#endif // VARIANT_CALL_FORMATPRIVATE_H
