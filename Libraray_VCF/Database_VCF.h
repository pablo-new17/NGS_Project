#ifndef DATABASE_VCF_H
#define DATABASE_VCF_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QVariant>

#include "Libraray_VCF_global.h"

class Database_VCFPrivate;
class LIBRARAY_VCF_EXPORT Database_VCF: public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE_D(m_Private, Database_VCF)
	Q_CLASSINFO("作者", "Pablo Lee")
	Q_CLASSINFO("描述", "Database for VCF")
	Q_CLASSINFO("版本號碼", "0.0.2.20200612")
	Q_DISABLE_COPY(Database_VCF)
private:
	const QScopedPointer<Database_VCFPrivate> m_Private;

public:
	explicit Database_VCF(const QString Database_Name, const QString Database_Path, QObject *parent = nullptr);
	explicit Database_VCF(const QString Database_Name, const QString Hostname, const quint16 Port, const QString Username, const QString Password, QObject *parent = nullptr);

	~Database_VCF();

	bool getConnected() const;
	QSqlDatabase getDatabase() const;
	QString getDatabase_Filename() const;

	void CloseDatabase();

signals:
	void Last_Error(const QString Error_Message);

public slots:
	bool Create_Table();
	int Read_Record_Count();

	QList<qint64> Read_Record_IDs();

	bool Insert_Header(QMap<QString, QVariant> Record_Data);
	bool Insert_Record(QMap<QString, QVariant> Record_Data);

	QMap<QString, QVariant> Read_Header_By_ID(const QString Class, const QString ID);
	QMap<QString, QVariant> Read_Record_By_Location(const int ID);
	QList<qint64> Read_Record_By_Qual(qint64 Qual);
};

#endif // DATABASE_VCF_H
