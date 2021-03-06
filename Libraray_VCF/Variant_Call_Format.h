#ifndef VARIANT_CALL_FORMAT_H
#define VARIANT_CALL_FORMAT_H

/// Document :
/// https://en.wikipedia.org/wiki/Variant_Call_Format
/// https://samtools.github.io/hts-specs/VCFv4.2.pdf
/// https://github.com/samtools/hts-specs

#include <QObject>
#include <QIODevice>
#include <QVariant>

#include "Libraray_VCF_global.h"

class VO_Field
{
public:
	QString ID;		// required
	QString Number;		// required
	QString Type;		// required: Integer, Float, Flag, Character, and String
	QString Description;	// required
	QString Source;		// recommended
	QString Version;	// recommended
	QString Others;		// for extention
};

class VO_Record
{
public:
	QString CHROM;
	quint64 POS;
	QString ID;
	QString REF;
	QStringList ALT;
	quint64 QUAL;
	QString FILTER;
	QVariantMap INFO;
	QString FORMAT;
	QStringList OTHER;

	QString toJSON(QString Field_Index_Numbers = "1234567890");
	QJsonObject JSON(QString Field_Index_Numbers = "1234567890");
};

class Variant_Call_FormatPrivate;
class LIBRARAY_VCF_EXPORT Variant_Call_Format : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE_D(m_Private, Variant_Call_Format)
	Q_CLASSINFO("作者", "Pablo Lee")
	Q_CLASSINFO("描述", "NGS Variant Call Format Reader")
	Q_CLASSINFO("版本號碼", "0.0.2.20200612")
	Q_DISABLE_COPY(Variant_Call_Format)
private:
	const QScopedPointer<Variant_Call_FormatPrivate> m_Private;

public:
	explicit Variant_Call_Format(QObject* parent = nullptr);
	explicit Variant_Call_Format(const QString& Filename, QObject* parent = Q_NULLPTR);
	explicit Variant_Call_Format(QIODevice* Device, QObject* parent = Q_NULLPTR);
	~Variant_Call_Format();

	QString fileformat();
	QString fileDate();
	QString reference();

	QHash<QString, VO_Field*> Infos();
	QHash<QString, VO_Field*> Filters();
	QHash<QString, VO_Field*> Formats();

	VO_Record* getCurrent_Record();

signals:
	void Last_Error(QString Error_Message);
	void Header_Added(QString Meta_Key, VO_Field* New_Head_Field);
	void Record_Added(qint64 Position, VO_Record* New_Record);

public slots:
	bool Open();
	bool Head();
	bool Next();
	bool SetPos(qint64 pos);

};

#endif // VARIANT_CALL_FORMAT_H
