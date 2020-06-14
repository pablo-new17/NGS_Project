#ifndef DATABASE_GTF_H
#define DATABASE_GTF_H

#include <QObject>
#include <QSqlDatabase>

#include "Libraray_VCF_global.h"

class Database_GTFPrivate;
class LIBRARAY_VCF_EXPORT Database_GTF : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE_D(m_Private, Database_GTF)
	Q_CLASSINFO("作者", "Pablo Lee")
	Q_CLASSINFO("描述", "Database for General Transfer Forma(GTF)")
	Q_CLASSINFO("版本號碼", "0.0.1.20200612")
	Q_DISABLE_COPY(Database_GTF)
private:
	const QScopedPointer<Database_GTFPrivate> m_Private;

public:
	explicit Database_GTF(const QString Filename = "hg19.refGene.gtf.db", QObject *parent = nullptr);
	~Database_GTF();

	bool getConnected() const;

	QStringList Gene_name(QString Chome, const qint64 Pos);

signals:
	void Last_Error(const QString Error_Message);

public slots:
	void CloseDatabase();


};

#endif // DATABASE_GTF_H
