#ifndef DATABASE_GTFPRIVATE_H
#define DATABASE_GTFPRIVATE_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>

#include "Database_GTF.h"

class Database_GTFPrivate : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Database_GTFPrivate)
	Q_DECLARE_PUBLIC(Database_GTF)
private:
	Database_GTF* const q_ptr;

public:
	explicit Database_GTFPrivate(Database_GTF *parent = nullptr);

signals:

public:
	QString		m_Database_Name;
	QString		m_Database_Path;

	bool		m_Connected;
	QString		m_Connection_String;
	QSqlDatabase	m_Database;
	QMutex		m_Mutex_Lock;



public slots:
	// Common database function
	bool Check_SQLite_Engine();
	bool Generete_Connection_String();
	bool Open_SQLite_Database();

	// SQL query function
	bool SQL_Exec(QSqlQuery& Query, const bool Show_Error = true);
	bool SQL_Exec_MutexLock(QSqlQuery& Query, const bool Show_Error = true);

	QStringList Get_Gene_name_SQLite(QString Chome, qint64 Pos);
};

#endif // DATABASE_GTFPRIVATE_H
