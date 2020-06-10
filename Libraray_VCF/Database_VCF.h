#ifndef DATABASE_VCF_H
#define DATABASE_VCF_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QVariant>

#include "DB_Schema.h"

class Database_VCF: public QObject
{
	Q_OBJECT

public:
	explicit Database_VCF(const QString Database_Name, const QString Database_Path, QObject *parent = nullptr);
	explicit Database_VCF(const QString Database_Name, const QString Hostname, const quint16 Port, const QString Username, const QString Password, QObject *parent = nullptr);

	~Database_VCF();

	bool getConnected() const;
	QString getDatabase_Filename() const;

	void CloseDatabase();

public slots:


signals:
	void Last_Error(const QString Error_Message);

private:
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
	bool Create_Table();
	int Read_Record_Count();

	QList<qint64> Read_Record_IDs();

	bool Insert_Header(QMap<QString, QVariant> Record_Data);
	bool Insert_Record(QMap<QString, QVariant> Record_Data);

	QMap<QString, QVariant> Read_Header_By_ID(const QString Class, const QString ID);
	QMap<QString, QVariant> Read_Record_By_Location(const int ID);
	QList<qint64> Read_Record_By_Qual(qint64 Qual);

private slots:
	void Build_Schema();

	bool Check_SQLite_Engine();
	bool Check_MySQL_Engine();
	bool Generete_Connection_String();
	bool Open_SQLite_Database();
	bool Open_MySQL_Database();

	bool SQL_Exec(QSqlQuery& Query, const bool Show_Error = true);
	bool SQL_Exec_MutexLock(QSqlQuery& Query, const bool Show_Error = true);

	//CRUD
	bool Create_Table_SQLite();

	int Read_Record_Count_SQLite();
	QList<qint64> Read_Record_Locations_SQLite();
	bool Inset_Header_SQLite(const QMap<QString, QVariant> Record_Data);
	bool Inset_Record_SQLite(const QMap<QString, QVariant> Record_Data);

	QMap<QString, QVariant> Read_Header_SQLite(const QString Class, const QString ID);
	QMap<QString, QVariant> Read_Record_SQLite(const int Location);

	QList<qint64> Read_Record_By_Qual_SQLite(qint64 Qual);
};

#endif // DATABASE_VCF_H
