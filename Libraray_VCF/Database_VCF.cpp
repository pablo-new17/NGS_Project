#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutexLocker>

#include "Database_VCF.h"
#include "Database_VCFPrivate.h"

Database_VCF::Database_VCF(const QString Database_Name, const QString Database_Path, QObject* parent)
	: QObject(parent)
	, m_Private(new Database_VCFPrivate(this))
{
	Q_D(Database_VCF);

	d->m_Database_Name = Database_Name;
	d->m_Database_Path = Database_Path;
	d->m_Connected = false;

	d->Build_Schema();
	d->Check_SQLite_Engine();
	d->Generete_Connection_String();
	d->Open_SQLite_Database();
}

Database_VCF::Database_VCF(const QString Database_Name, const QString Hostname, const quint16 Port, const QString Username, const QString Password, QObject* parent)
	: QObject(parent)

{
	Q_D(Database_VCF);

	d->m_Database_Name = Database_Name;
	d->m_Hostname = Hostname;
	d->m_Port = Port;
	d->m_Username = Username;
	d->m_Password = Password;
	d->m_Connected = false;

	d->Build_Schema();
	d->Check_MySQL_Engine();
	d->Generete_Connection_String();
	d->Open_MySQL_Database();
}

Database_VCF::~Database_VCF()
{
	Q_D(Database_VCF);

	this->CloseDatabase();
	d->m_Connected = false;
	d->m_Database_Type = "Deleted";
}





bool Database_VCF::getConnected() const
{
	Q_D(const Database_VCF);

	return d->m_Connected;
}

QSqlDatabase Database_VCF::getDatabase() const
{
	Q_D(const Database_VCF);

	return d->m_Database;
}

QString Database_VCF::getDatabase_Filename() const
{
	Q_D(const Database_VCF);

	return d->m_Database.databaseName();
}

void Database_VCF::CloseDatabase()
{
	Q_D(Database_VCF);

	{
		d->m_Database.close();

		QSqlDatabase::removeDatabase(d->m_Connection_String);
	}
}



bool Database_VCF::Create_Table()
{
	Q_D(Database_VCF);
	bool Result = false;

	if(!d->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(d->m_Database_Type == "SQLITE")
	{
		return d->Create_Table_SQLite();
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

int Database_VCF::Read_Record_Count()
{
	Q_D(Database_VCF);
	int Result = 0;

	if(!d->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(d->m_Database_Type == "SQLITE")
	{
		return d->Read_Record_Count_SQLite();
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

QList<qint64> Database_VCF::Read_Record_IDs()
{
	Q_D(Database_VCF);
	QList<qint64> Result;
	Result.clear();

	if(!d->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(d->m_Database_Type == "SQLITE")
	{
		return d->Read_Record_Locations_SQLite();
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

bool Database_VCF::Insert_Header(QMap<QString, QVariant> Record_Data)
{
	Q_D(Database_VCF);
	bool Result = false;

	if(!d->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(d->m_Database_Type == "SQLITE")
	{
		return d->Inset_Header_SQLite(Record_Data);
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}
bool Database_VCF::Insert_Record(QMap<QString, QVariant> Record_Data)
{
	Q_D(Database_VCF);
	bool Result = false;

	if(!d->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(d->m_Database_Type == "SQLITE")
	{
		return d->Inset_Record_SQLite(Record_Data);
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

QMap<QString, QVariant> Database_VCF::Read_Header_By_ID(const QString Class, const QString ID)
{
	Q_D(Database_VCF);
	QMap<QString, QVariant> Result;
	Result.clear();

	if(!d->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(d->m_Database_Type == "SQLITE")
	{
		return d->Read_Header_SQLite(Class, ID);
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}
QMap<QString, QVariant> Database_VCF::Read_Record_By_Location(const int ID)
{
	Q_D(Database_VCF);
	QMap<QString, QVariant> Result;
	Result.clear();

	if(!d->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(d->m_Database_Type == "SQLITE")
	{
		return d->Read_Record_SQLite(ID);
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

QList<qint64> Database_VCF::Read_Record_By_Qual(qint64 Qual)
{
	Q_D(Database_VCF);
	QList<qint64> Result;
	Result.clear();

	if(!d->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(d->m_Database_Type == "SQLITE")
	{
		return d->Read_Record_By_Qual_SQLite(Qual);
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}




