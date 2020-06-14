#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>

#include "Dialog_Analisys.h"
#include "Variant_Call_Format.h"
#include "Database_VCF.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_actionOpen_triggered();

	void on_lineEdit_Filter_textEdited(const QString &arg1);

	void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
private:
	Ui::MainWindow *ui;
	Variant_Call_Format* m_VCF;
	Database_VCF* m_DB;
	QSqlQueryModel *m_Model;
	Dialog_Analisys* m_Analisys;

	QSortFilterProxyModel *m_Filter;
};
#endif // MAINWINDOW_H
