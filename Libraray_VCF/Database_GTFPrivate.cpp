#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutexLocker>

#include "Database_GTFPrivate.h"

Database_GTFPrivate::Database_GTFPrivate(Database_GTF* parent)
	: QObject(parent)
	, q_ptr(parent)
{

}

// Common database function
bool Database_GTFPrivate::Check_SQLite_Engine()
{
	Q_Q(Database_GTF);

	if(QSqlDatabase::isDriverAvailable("QSQLITE")==false)
	{
		QString Error_Message = QString(tr("所在的系統裡，找不到 '%1' 資料庫引擎")).arg("SQLite");

		emit q->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return false;
	}

	return true;
}

bool Database_GTFPrivate::Generete_Connection_String()
{
	Q_Q(Database_GTF);

	if(this->m_Database_Name.isEmpty())
	{
		QString Error_Message = QString(tr("尚未設定資料庫名稱"));

		emit q->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return false;
	}

	QString Connection_String = QString("%1").arg(this->m_Database_Name);
	int Count = 1;
	while(QSqlDatabase::contains(Connection_String))
	{
		//若資料庫連接字串 已經存在，則後面補上四碼數字，以便區別。
		//最大至 9999
		Connection_String = QString("%1_%2")
				    .arg(this->m_Database_Name)
				    .arg(Count++,  4, 10, QChar('0'));
	}
	this->m_Connection_String = Connection_String;

	return true;
}

bool Database_GTFPrivate::Open_SQLite_Database()
{
	Q_Q(Database_GTF);

	//檢查資料庫是否已經((被其他線程所)開啟
	this->m_Connected = this->m_Database.isOpen();

	if(!this->m_Connected)
	{
		//資料庫尚未開啟，開啟它
		this->m_Database = QSqlDatabase::addDatabase("QSQLITE", this->m_Connection_String);
		if(this->m_Database.isValid()==false)
		{
			QString Error_Message = QString(tr("無法建立資料庫，無效資料庫"));

			emit q->Last_Error(Error_Message);
			qDebug() << Error_Message;

			return false;
		}


		QString	Database_Name = this->m_Database_Name;
		if(!Database_Name.endsWith(".db"))
		{
			Database_Name.append(".db");
		}

		QString Database_full_name = Database_Name;

		if(!this->m_Database_Path.isEmpty())
		{
			Database_full_name = QString("%1/%2")
					.arg(this->m_Database_Path)
					.arg(Database_Name);
		}

		this->m_Database.setDatabaseName(Database_full_name);

		if((this->m_Connected = this->m_Database.open())==false)
		{
			if((this->m_Connected = this->m_Database.open())==false)
			{
				QString Error_Message = QString(tr("無法開啟資料庫<%1>，錯誤：%2"))
							.arg(this->m_Database.databaseName())
							.arg(this->m_Database.lastError().text());

				emit q->Last_Error(Error_Message);
				qDebug() << Error_Message;

				return false;
			}
		}
	}

	return true;
}


// SQL query function
bool Database_GTFPrivate::SQL_Exec(QSqlQuery& Query, const bool Show_Error)
{
	Q_Q(Database_GTF);

	bool Result = false;

	if(!this->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit q->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return false;
	}

	if((Result=Query.exec())==false)
	{

		QString Error_Message = QString(tr("資料庫執行動作<%1>，錯誤：%2"))
						.arg(Query.executedQuery())
						.arg(Query.lastError().text());

		if(Show_Error)
		{
			emit q->Last_Error(Error_Message);
			qDebug() << Error_Message;
		}
	}

	return Result;
}

bool Database_GTFPrivate::SQL_Exec_MutexLock(QSqlQuery& Query, const bool Show_Error)
{
	bool Result = false;

	QMutexLocker	Locker(&m_Mutex_Lock);

	Result = this->SQL_Exec(Query, Show_Error);

	return Result;
}


// functions
QStringList Database_GTFPrivate::Get_Gene_name_SQLite(QString Chome, qint64 Pos)
{
	Q_Q(Database_GTF);
	QStringList Result;
	Result.clear();

	QString Prepare;

	Prepare.append("SELECT `gene_name` FROM `refGene`"
		       " WHERE `seqname` = :Chome"
		       " AND `start` <= :POS AND `end` >= :POS");

	QSqlQuery Query =  QSqlQuery(this->m_Database);

	Query.prepare(Prepare);

	Query.bindValue(":Chome", Chome);
	Query.bindValue(":POS", Pos);

	if(this->SQL_Exec(Query)==false)
	{
		QString Error_Message = QString(tr("資料庫查詢動作<%1>，錯誤：%2"))
					.arg(Query.executedQuery())
					.arg(Query.lastError().text());

		emit q->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	while(Query.next())
	{
		Result.append(Query.value("gene_name").toString());
	}

	return Result;
}
