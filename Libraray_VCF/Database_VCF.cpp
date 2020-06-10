#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutexLocker>

#include "Database_VCF.h"

//#define DB_ENCRYPT

void Database_VCF::Build_Schema()
{
	// 檔頭
	this->m_DB_Schema_Header.insert("類別", DB_Schema("類別", "VARCHAR(16)", true, "", false, "Class"));

	this->m_DB_Schema_Header.insert("識別碼", DB_Schema("識別碼", "VARCHAR(16)", true, "", false, "ID"));
	this->m_DB_Schema_Header.insert("號碼", DB_Schema("號碼", "VARCHAR(16)", false, "", false, "Number"));
	this->m_DB_Schema_Header.insert("型態", DB_Schema("型態", "VARCHAR(16)", false, "", false, "Type"));
	this->m_DB_Schema_Header.insert("描述", DB_Schema("描述", "VARCHAR(256)", true, "", false, "Description"));

	this->m_DB_Schema_Header.insert("來源", DB_Schema("來源", "VARCHAR(256)", false, "", false, "Source"));
	this->m_DB_Schema_Header.insert("版本", DB_Schema("版本", "VARCHAR(256)", false, "", false, "Version"));
	this->m_DB_Schema_Header.insert("其他", DB_Schema("其他", "VARCHAR(256)", false, "", false, "Others"));

	// 變異資料
	this->m_DB_Schema_Record.insert("起始位置", DB_Schema("起始位置", "integer", true, "", true, "Location"));
	this->m_DB_Schema_Record.insert("染色體", DB_Schema("染色體", "VARCHAR(32)", true, "", false, "CHROM"));
	this->m_DB_Schema_Record.insert("位置", DB_Schema("位置", "integer", true, "", false, "POS"));
	this->m_DB_Schema_Record.insert("品質", DB_Schema("品質", "integer", true, "", false, "QUAL"));
}


Database_VCF::Database_VCF(const QString Database_Name, const QString Database_Path, QObject* parent)
	: QObject(parent)
	, m_Database_Name(Database_Name)
	, m_Database_Path(Database_Path)
	, m_Database_Type("Unknow")
	, m_Connected(false)
{
	this->Build_Schema();

	this->Check_SQLite_Engine();
	this->Generete_Connection_String();
	this->Open_SQLite_Database();
}

Database_VCF::Database_VCF(const QString Database_Name, const QString Hostname, const quint16 Port, const QString Username, const QString Password, QObject* parent)
	: QObject(parent)
	, m_Database_Name(Database_Name)
	, m_Hostname(Hostname)
	, m_Port(Port)
	, m_Username(Username)
	, m_Password(Password)
	, m_Database_Type("Unknow")
	, m_Connected(false)
{
	this->Build_Schema();

	this->Check_MySQL_Engine();
	this->Generete_Connection_String();
	this->Open_MySQL_Database();
}

Database_VCF::~Database_VCF()
{
	this->CloseDatabase();
	this->m_Connected = false;
	this->m_Database_Type = "Deleted";
}





bool Database_VCF::getConnected() const
{
	return m_Connected;
}

QString Database_VCF::getDatabase_Filename() const
{
	return this->m_Database.databaseName();
}

void Database_VCF::CloseDatabase()
{
	{
		this->m_Database.close();

		qDebug() << "Database.isOpen()" << this->m_Database.isOpen();

		QSqlDatabase::removeDatabase(this->m_Connection_String);
	}
}

bool Database_VCF::Create_Table()
{
	bool Result = false;

	if(!this->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(this->m_Database_Type == "SQLITE")
	{
		return Create_Table_SQLite();
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

int Database_VCF::Read_Record_Count()
{
	int Result = 0;

	if(!this->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(this->m_Database_Type == "SQLITE")
	{
		return Read_Record_Count_SQLite();
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

QList<qint64> Database_VCF::Read_Record_IDs()
{
	QList<qint64> Result;
	Result.clear();

	if(!this->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(this->m_Database_Type == "SQLITE")
	{
		return Read_Record_Locations_SQLite();
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

bool Database_VCF::Insert_Header(QMap<QString, QVariant> Record_Data)
{
	bool Result = false;

	if(!this->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(this->m_Database_Type == "SQLITE")
	{
		return Inset_Header_SQLite(Record_Data);
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}
bool Database_VCF::Insert_Record(QMap<QString, QVariant> Record_Data)
{
	bool Result = false;

	if(!this->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(this->m_Database_Type == "SQLITE")
	{
		return Inset_Record_SQLite(Record_Data);
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

QMap<QString, QVariant> Database_VCF::Read_Header_By_ID(const QString Class, const QString ID)
{
	QMap<QString, QVariant> Result;
	Result.clear();

	if(!this->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(this->m_Database_Type == "SQLITE")
	{
		return Read_Header_SQLite(Class, ID);
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}
QMap<QString, QVariant> Database_VCF::Read_Record_By_Location(const int ID)
{
	QMap<QString, QVariant> Result;
	Result.clear();

	if(!this->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(this->m_Database_Type == "SQLITE")
	{
		return Read_Record_SQLite(ID);
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

QList<qint64> Database_VCF::Read_Record_By_Qual(qint64 Qual)
{
	QList<qint64> Result;
	Result.clear();

	if(!this->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(this->m_Database_Type == "SQLITE")
	{
		return Read_Record_By_Qual_SQLite(Qual);
	}

	QString Error_Message = QString(tr("資料庫型態不合"));

	emit this->Last_Error(Error_Message);
	qDebug() << Error_Message;

	return Result;
}

// SQL query function

bool Database_VCF::SQL_Exec(QSqlQuery& Query, const bool Show_Error)
{
	bool Result = false;

	if(!this->m_Connected)
	{
		QString Error_Message = QString(tr("資料庫尚未連線，請先連線"));

		emit this->Last_Error(Error_Message);
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
			emit this->Last_Error(Error_Message);
			qDebug() << Error_Message;
		}
	}

	return Result;
}

bool Database_VCF::SQL_Exec_MutexLock(QSqlQuery& Query, const bool Show_Error)
{
	bool Result = false;

	QMutexLocker	Locker(&m_Mutex_Lock);

	Result = this->SQL_Exec(Query, Show_Error);

	return Result;
}


//SQLite

bool Database_VCF::Check_SQLite_Engine()
{
	if(QSqlDatabase::isDriverAvailable("QSQLITE")==false)
	{
		QString Error_Message = QString(tr("所在的系統裡，找不到 '%1' 資料庫引擎")).arg("SQLite");

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return false;
	}

	this->m_Database_Type = "SQLITE";
	return true;
}

bool Database_VCF::Check_MySQL_Engine()
{
	if(QSqlDatabase::isDriverAvailable("QMYSQL")==false)
	{
		QString Error_Message = QString(tr("所在的系統裡，找不到 '%1' 資料庫引擎")).arg("MySQL");

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return false;
	}

	this->m_Database_Type = "MYSQL";
	return true;
}

bool Database_VCF::Generete_Connection_String()
{
	if(!(this->m_Database_Type == "SQLITE" ||
	     this->m_Database_Type == "MYSQL") )
	{
		QString Error_Message = QString(tr("請先設定資料庫型態，再來產生資料連結識別字串"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return false;
	}

	if(this->m_Database_Name.isEmpty())
	{
		QString Error_Message = QString(tr("尚未設定資料庫名稱"));

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return false;
	}

	QString Connection_String = QString("%1_%2")
				    .arg(this->m_Database_Type)
				    .arg(this->m_Database_Name);
	int Count = 1;
	while(QSqlDatabase::contains(Connection_String))
	{
		//若資料庫連接字串 已經存在，則後面補上四碼數字，以便區別。
		//最大至 9999
		Connection_String = QString("%1_%2_%3")
				    .arg(this->m_Database_Type)
				    .arg(this->m_Database_Name)
				    .arg(Count++,  4, 10, QChar('0'));
	}
	this->m_Connection_String = Connection_String;

//	if(this->m_Database_Type=="SQLITE")
//	{
//		this->m_Database = QSqlDatabase::addDatabase("QSQLITE", this->m_Connection_String);
//	}
//	else if (this->m_Database_Type=="MYSQL")
//	{
//		this->m_Database = QSqlDatabase::addDatabase("QMYSQL", this->m_Connection_String);
//	}

//	return this->m_Database.open();
	return true;
}

bool Database_VCF::Open_SQLite_Database()
{
	if(this->m_Database_Type != "SQLITE")
	{
		QString Error_Message = QString(tr("資料庫型態不合，須為 %1"))
					.arg("SQLite");

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return false;
	}


	//檢查資料庫是否已經((被其他線程所)開啟
	this->m_Connected = this->m_Database.isOpen();

	if(!this->m_Connected)
	{
		//資料庫尚未開啟，開啟它
//		this->m_Database = QSqlDatabase::addDatabase("QSQLITE", this->m_Connection_String);
		this->m_Database = QSqlDatabase::addDatabase("SQLITECIPHER", this->m_Connection_String);

		if(this->m_Database.isValid()==false)
		{
			QString Error_Message = QString(tr("無法建立資料庫，無效資料庫"));

			emit this->Last_Error(Error_Message);
			qDebug() << Error_Message;

			return false;
		}

		//set password
#ifdef	DB_ENCRYPT
		this->m_Database.setPassword("tzuchi-pablo");
		this->m_Database.setConnectOptions("QSQLITE_CREATE_KEY");
#endif

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
			this->m_Database.setConnectOptions("QSQLITE_USE_KEY");
			if((this->m_Connected = this->m_Database.open())==false)
			{
				QString Error_Message = QString(tr("無法開啟資料庫<%1>，錯誤：%2"))
							.arg(this->m_Database.databaseName())
							.arg(this->m_Database.lastError().text());

				emit this->Last_Error(Error_Message);
				qDebug() << Error_Message;

				return false;
			}
		}
	}

	return true;
}



bool Database_VCF::Create_Table_SQLite()
{
	bool Result = false;

	QString Prepare;

	Prepare.append("CREATE TABLE IF NOT EXISTS `檔頭` (");
	bool First = true;
	foreach(DB_Schema Schema, this->m_DB_Schema_Header.values())
	{
		if(!First)
			Prepare.append(", ");

		Prepare.append(QString("`%1` %2").arg(Schema.Name).arg(Schema.Type));
		if(Schema.NotNull)
			Prepare.append(" NOT NULL");
		if(Schema.PrimaryKey)
			Prepare.append(" PRIMARY KEY UNIQUE");
		if(!Schema.DefaultValue.isEmpty())
			Prepare.append(QString(" DEFAULT %1").arg(Schema.DefaultValue));

		First = false;
	}
	Prepare.append(" )");
	qDebug() << Prepare;

	QSqlQuery Query =  QSqlQuery(this->m_Database);
	Query.prepare(Prepare);

	if((Result=this->SQL_Exec_MutexLock(Query))==false)
	{
		return Result;
	}


	Prepare.clear();
	Prepare.append("CREATE TABLE IF NOT EXISTS `變異資料` (");
	First = true;
	foreach(DB_Schema Schema, this->m_DB_Schema_Record.values())
	{
		if(!First)
			Prepare.append(", ");

		Prepare.append(QString("`%1` %2").arg(Schema.Name).arg(Schema.Type));
		if(Schema.NotNull)
			Prepare.append(" NOT NULL");
		if(Schema.PrimaryKey)
			Prepare.append(" PRIMARY KEY UNIQUE");
		if(!Schema.DefaultValue.isEmpty())
			Prepare.append(QString(" DEFAULT %1").arg(Schema.DefaultValue));

		First = false;
	}
	Prepare.append(" )");
	qDebug() << Prepare;

	Query.prepare(Prepare);

	if((Result=this->SQL_Exec_MutexLock(Query))==false)
	{
		return Result;
	}

	return Result;
}

int Database_VCF::Read_Record_Count_SQLite()
{
	int Result = -1;

	QString Prepare;

	Prepare.append("SELECT COUNT(*) AS `Count` FROM `變異資料`");

	QSqlQuery Query =  QSqlQuery(this->m_Database);

	Query.prepare(Prepare);

	if(this->SQL_Exec(Query)==false)
	{
		QString Error_Message = QString(tr("資料庫查詢動作<%1>，錯誤：%2"))
					.arg(Query.executedQuery())
					.arg(Query.lastError().text());

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	if(Query.next())
	{
		Result = Query.value("Count").toInt();
	}

	return Result;
}

QList<qint64> Database_VCF::Read_Record_Locations_SQLite()
{
	QList<qint64> Result;
	Result.clear();

	QString Prepare;

	Prepare.append("SELECT `起始位置` FROM `變異資料`");

	QSqlQuery Query =  QSqlQuery(this->m_Database);

	Query.prepare(Prepare);

	if(this->SQL_Exec(Query)==false)
	{
		QString Error_Message = QString(tr("資料庫查詢動作<%1>，錯誤：%2"))
					.arg(Query.executedQuery())
					.arg(Query.lastError().text());

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	while(Query.next())
	{
		Result.append(Query.value("起始位置").toLongLong());
	}

	return Result;
}

bool Database_VCF::Inset_Header_SQLite(const QMap<QString, QVariant> Record_Data)
{
	bool Result = false;

	QString Prepare = QString("INSERT INTO `檔頭` ("
				  "  `類別`, `識別碼`, `號碼`, `型態`, `描述`"
				  ", `來源`, `版本`, `其他`"
				  ") VALUES ("
				  "  :Class, :ID, :Number, :Type, :Description"
				  ", :Source, :Version, :Others"
				  ")");

	QSqlQuery Query =  QSqlQuery(this->m_Database);

	Query.prepare(Prepare);
	Query.bindValue(":Class", Record_Data.value("類別").toString());
	Query.bindValue(":ID", Record_Data.value("識別碼").toString());
	Query.bindValue(":Number", Record_Data.value("號碼").toString());
	Query.bindValue(":Type", Record_Data.value("型態").toString());
	Query.bindValue(":Description", Record_Data.value("描述").toString());
	Query.bindValue(":Source", Record_Data.value("來源").toString());
	Query.bindValue(":Version", Record_Data.value("版本").toString());
	Query.bindValue(":Others", Record_Data.value("其他").toString());

	if((Result = this->SQL_Exec(Query)) == false)
	{
		QString Error_Message = QString(tr("資料庫查詢動作<%1>，錯誤：%2"))
					.arg(Query.executedQuery())
					.arg(Query.lastError().text());

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	return Result;
}
bool Database_VCF::Inset_Record_SQLite(const QMap<QString, QVariant> Record_Data)
{
	bool Result = false;

	QString Prepare = QString("INSERT INTO `變異資料` ("
				  "  `起始位置`, `染色體`, `位置`, `品質`"
				  ") VALUES ("
				  "  :Location, :CHROM, :POS, :QUAL"
				  ")");

	QSqlQuery Query =  QSqlQuery(this->m_Database);

	Query.prepare(Prepare);
	Query.bindValue(":Location", Record_Data.value("起始位置").toLongLong());
	Query.bindValue(":CHROM", Record_Data.value("染色體").toString());
	Query.bindValue(":POS", Record_Data.value("位置").toLongLong());
	Query.bindValue(":QUAL", Record_Data.value("品質").toInt());

	if((Result = this->SQL_Exec(Query)) == false)
	{
		QString Error_Message = QString(tr("資料庫查詢動作<%1>，錯誤：%2"))
					.arg(Query.executedQuery())
					.arg(Query.lastError().text());

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	return Result;
}

QMap<QString, QVariant> Database_VCF::Read_Header_SQLite(const QString Class, const QString ID)
{
	QMap<QString, QVariant> Result;
	Result.clear();

	QString Prepare;

	Prepare.append("SELECT * FROM `檔頭`"
		       "  WHERE `類別` = :Class AND `識別碼` = :ID "
		       );

	QSqlQuery Query =  QSqlQuery(this->m_Database);

	Query.prepare(Prepare);
	Query.bindValue(":Class", Class);
	Query.bindValue(":ID", ID);

	if(this->SQL_Exec(Query)==false)
	{
		QString Error_Message = QString(tr("資料庫查詢動作<%1>，錯誤：%2"))
					.arg(Query.executedQuery())
					.arg(Query.lastError().text());

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	while(Query.next())
	{
		Result.insert("類別", Query.value("類別"));

		Result.insert("識別碼", Query.value("識別碼"));
		Result.insert("號碼", Query.value("號碼"));
		Result.insert("型態", Query.value("型態"));
		Result.insert("描述", Query.value("描述"));

		Result.insert("來源", Query.value("來源"));
		Result.insert("版本", Query.value("版本"));
		Result.insert("其他", Query.value("其他"));

	}
	return Result;
}
QMap<QString, QVariant> Database_VCF::Read_Record_SQLite(const int Location)
{
	QMap<QString, QVariant> Result;
	Result.clear();

	QString Prepare;

	Prepare.append("SELECT * FROM `變異資料`"
		       "  WHERE `起始位置` = :Location"
		       );

	QSqlQuery Query =  QSqlQuery(this->m_Database);

	Query.prepare(Prepare);
	Query.bindValue(":Location", Location);

	if(this->SQL_Exec(Query)==false)
	{
		QString Error_Message = QString(tr("資料庫查詢動作<%1>，錯誤：%2"))
					.arg(Query.executedQuery())
					.arg(Query.lastError().text());

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	while(Query.next())
	{
		Result.insert("起始位置", Query.value("起始位置"));
		Result.insert("染色體", Query.value("染色體"));
		Result.insert("位置", Query.value("位置"));
		Result.insert("品質", Query.value("品質"));
	}

	return Result;
}

QList<qint64> Database_VCF::Read_Record_By_Qual_SQLite(qint64 Qual)
{
	QList<qint64> Result;
	Result.clear();

	QString Prepare;

	Prepare.append("SELECT `起始位置` FROM `變異資料`"
		       " WHERE `品質` >= :Qual");

	QSqlQuery Query =  QSqlQuery(this->m_Database);

	Query.prepare(Prepare);
	Query.bindValue(":Qual", Qual);


	if(this->SQL_Exec(Query)==false)
	{
		QString Error_Message = QString(tr("資料庫查詢動作<%1>，錯誤：%2"))
					.arg(Query.executedQuery())
					.arg(Query.lastError().text());

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return Result;
	}

	while(Query.next())
	{
		Result.append(Query.value("起始位置").toLongLong());
	}

	return Result;
}


bool Database_VCF::Open_MySQL_Database()
{
	if(this->m_Database_Type != "MYSQL")
	{
		QString Error_Message = QString(tr("資料庫型態不合，須為 %1"))
					.arg("MySQL");

		emit this->Last_Error(Error_Message);
		qDebug() << Error_Message;

		return false;
	}


	//檢查資料庫是否已經((被其他線程所)開啟
	this->m_Connected = this->m_Database.isOpen();

	if(!this->m_Connected)
	{
		//資料庫尚未開啟，開啟它
		this->m_Database = QSqlDatabase::addDatabase("QMYSQL", this->m_Connection_String);
		if(this->m_Database.isValid()==false)
		{
			QString Error_Message = QString(tr("無法建立資料庫，無效資料庫"));

			emit this->Last_Error(Error_Message);
			qDebug() << Error_Message;

			return false;
		}

		this->m_Database.setHostName(this->m_Hostname);
		this->m_Database.setPort(this->m_Port);
		this->m_Database.setDatabaseName(this->m_Database_Name);
		this->m_Database.setUserName(this->m_Username);
		this->m_Database.setPassword(this->m_Password);

		if((this->m_Connected = this->m_Database.open())==false)
		{
			QString Error_Message = QString(tr("無法連線資料庫<%1>，錯誤：%2"))
						.arg(this->m_Database.databaseName())
						.arg(this->m_Database.lastError().text());

			emit this->Last_Error(Error_Message);
			qDebug() << Error_Message;

			return false;
		}
	}

	return true;
}

