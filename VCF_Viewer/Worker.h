#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "Variant_Call_Format.h"
#include "Database_VCF.h"
#include "Database_GTF.h"


class Worker : public QObject
{
	Q_OBJECT
public:
	explicit Worker(QObject *parent = nullptr);
	~Worker();

	QString getVCF_Filename() const;
	void setVCF_Filename(const QString& VCF_Filename);

	QString getGFT_Filename() const;
	void setGFT_Filename(const QString& GFT_Filename);

public slots:
	void run();
	void Stop();

signals:
	void Position(qint64 Count);
	void Finished();


private:
	bool		m_Stop;
	bool		m_Restart;

	QMutex		m_Mutex;
	QWaitCondition	m_Condition;

private:
	QString m_VCF_Filename;
	QString m_GFT_Filename;

	Variant_Call_Format* m_VCF;
	Database_VCF* m_DB_VCF;
	Database_GTF* m_DB_GFT;

private slots:
	void Header_Added(QString Meta_Key, VO_Field* New_Head_Field);
	void Record_Added(qint64 Position, VO_Record* New_Record);
};


#endif // WORKER_H
