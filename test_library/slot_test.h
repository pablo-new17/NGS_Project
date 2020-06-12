#ifndef SLOT_TEST_H
#define SLOT_TEST_H

#include <QObject>

#include "Variant_Call_Format.h"
#include "Database_VCF.h"
#include "Database_GTF.h"

class slot_test : public QObject
{
	Q_OBJECT
	Database_VCF* m_VCF;
	Database_GTF* m_GFT;
public:
	explicit slot_test(Variant_Call_Format* vcf, Database_VCF* DB, QObject *parent = nullptr);

signals:

public slots:
	void Last_Error(QString Error_Message);
	void Header_Added(QString Meta_Key, VO_Field* New_Head_Field);
	void Record_Added(qint64 Position, VO_Record* New_Record);
};

#endif // SLOT_TEST_H
