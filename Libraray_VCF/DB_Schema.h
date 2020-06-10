#ifndef DB_SCHEMA_H
#define DB_SCHEMA_H

#include <QString>

class DB_Schema
{
public:
	DB_Schema();
	DB_Schema(QString Name, QString Type, bool NotNull, QString DefaultValue, bool PrimaryKey, QString KeyName);

	QString Name;
	QString Type;
	bool	NotNull;
	QString DefaultValue;
	bool	PrimaryKey;
	QString	KeyName;	//for English name
};

#endif // DB_SCHEMA_H
