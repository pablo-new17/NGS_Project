#include "DB_Schema.h"

DB_Schema::DB_Schema()
{

}

DB_Schema::DB_Schema(QString Name, QString Type, bool NotNull, QString DefaultValue, bool PrimaryKey, QString KeyName)
	: Name(Name)
	, Type(Type)
	, NotNull(NotNull)
	, DefaultValue(DefaultValue)
	, PrimaryKey(PrimaryKey)
	, KeyName(KeyName)
{
	if(KeyName.isEmpty())
		KeyName = Name;
}
