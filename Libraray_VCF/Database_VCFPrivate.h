#ifndef DATABASE_VCFPRIVATE_H
#define DATABASE_VCFPRIVATE_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QMap>

#include "Database_VCF.h"
#include "DB_Schema.h"

class Database_VCFPrivate : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Database_VCFPrivate)
	Q_DECLARE_PUBLIC(Database_VCF)
private:
	Database_VCF* const q_ptr;

public:
	explicit Database_VCFPrivate(Database_VCF *parent = nullptr);

signals:


public:
	QMap<QString, DB_Schema> m_DB_Schema_Header;
	QMap<QString, DB_Schema> m_DB_Schema_Record;

	QString		m_Database_Name;
	QString		m_Database_Path;
	QString		m_Hostname;
	quint16		m_Port;
	QString		m_Username;
	QString		m_Password;
	QString		m_Database_Type;

	bool		m_Connected;
	QString		m_Connection_String;
	QSqlDatabase	m_Database;
	QMutex		m_Mutex_Lock;

public slots:
	void Build_Schema();

	// Common database function
	bool Check_SQLite_Engine();
	bool Check_MySQL_Engine();
	bool Generete_Connection_String();
	bool Open_SQLite_Database();
	bool Open_MySQL_Database();

	// SQL query function
	bool SQL_Exec(QSqlQuery& Query, const bool Show_Error = true);
	bool SQL_Exec_MutexLock(QSqlQuery& Query, const bool Show_Error = true);

	// CRUD for SQLite
	bool Create_Table_SQLite();

	int Read_Record_Count_SQLite();
	QList<qint64> Read_Record_Locations_SQLite();
	bool Inset_Header_SQLite(const QMap<QString, QVariant> Record_Data);
	bool Inset_Record_SQLite(const QMap<QString, QVariant> Record_Data);

	QMap<QString, QVariant> Read_Header_SQLite(const QString Class, const QString ID);
	QMap<QString, QVariant> Read_Record_SQLite(const int Location);
	QList<qint64> Read_Record_By_Qual_SQLite(qint64 Qual);
};

#endif // DATABASE_VCFPRIVATE_H
