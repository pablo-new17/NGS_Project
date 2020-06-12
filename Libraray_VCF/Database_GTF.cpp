#include <QFileInfo>

#include "Database_GTF.h"
#include "Database_GTFPrivate.h"

Database_GTF::Database_GTF(const QString Filename, QObject *parent)
	: QObject(parent)
	, m_Private(new Database_GTFPrivate(this))
{
	Q_D(Database_GTF);

	QFileInfo* info = new QFileInfo(Filename);
	d->m_Database_Name = info->fileName();
	d->m_Database_Path = info->path();
	d->m_Connected = false;

	d->Check_SQLite_Engine();
	d->Generete_Connection_String();
	d->Open_SQLite_Database();
}

Database_GTF::~Database_GTF()
{
	Q_D(Database_GTF);

	d->m_Database.close();

	QSqlDatabase::removeDatabase(d->m_Connection_String);
}

bool Database_GTF::getConnected() const
{
	Q_D(const Database_GTF);

	return d->m_Connected;
}

QStringList Database_GTF::Gene_name(const QString Chome, const qint64 Pos)
{
	Q_D(Database_GTF);

	QString gene = Chome;

	if(!gene.startsWith("chr"))
		gene.prepend("chr");

	return d->Get_Gene_name_SQLite(gene, Pos);
}

void Database_GTF::CloseDatabase()
{
	Q_D(Database_GTF);

	{
		d->m_Database.close();

		QSqlDatabase::removeDatabase(d->m_Connection_String);
	}
}
