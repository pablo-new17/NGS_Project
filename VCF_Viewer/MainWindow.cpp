#include <QDebug>
#include <QFileDialog>


#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->m_Model = new QSqlQueryModel(this);
	this->m_Model->setHeaderData(0, Qt::Horizontal, tr("染色體"));
	this->m_Model->setHeaderData(1, Qt::Horizontal, tr("位置"));
	this->m_Model->setHeaderData(2, Qt::Horizontal, tr("品質"));
	this->m_Model->setHeaderData(3, Qt::Horizontal, tr("起始位置"));
	this->m_Model->setHeaderData(4, Qt::Horizontal, tr("名稱"));

	this->m_Filter = new QSortFilterProxyModel(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
	QString VCF_Filename = QFileDialog::getOpenFileName(
				   this,
				   tr("開啟變異資料"), ".", tr("變異資料 (*.vcf)"));
	if(VCF_Filename.isEmpty())
	{
		qDebug() << "未指定任何檔案";
		return;
	}
	QFileInfo* VCF_Info = new QFileInfo(VCF_Filename);
	if(!VCF_Info->exists())
	{
		qDebug() << "指定的VCF檔案，不存在";
		return;
	}

	QFileInfo* DB_Info = new QFileInfo(QString(VCF_Filename).append(".db"));
	if(!DB_Info->exists())
	{
		QString GTF_Filename = QFileDialog::getOpenFileName(
					   this,
					   tr("開啟GTF資料庫"), ".", tr("GTF資料庫 (*.db)"));
		if(GTF_Filename.isEmpty())
		{
			qDebug() << "未指定任何 General Transfer Format(GTF) 資料庫";
			return;
		}

		this->m_Analisys = new Dialog_Analisys(VCF_Filename, GTF_Filename, this);
		this->m_Analisys->exec();
	}


	// VCF 資料檔
	this->m_VCF = new Variant_Call_Format(VCF_Filename);
	this->m_VCF->Open();
	this->m_VCF->SetPos(0);
	// VCF 資料庫
	this->m_DB = new Database_VCF(VCF_Info->fileName().append(".db"), VCF_Info->path(), this);

	this->m_Model->setQuery("SELECT `染色體`, `位置`, `品質`, `起始位置`, `名稱` FROM `變異資料`", this->m_DB->getDatabase());
	this->m_Filter->setSourceModel(this->m_Model);
	ui->tableView_Gene->setModel(this->m_Filter);
	ui->tableView_Gene->hideColumn(3);
	ui->tableView_Gene->show();

	QItemSelectionModel *Selection = ui->tableView_Gene->selectionModel();
	connect(Selection, &QItemSelectionModel::currentRowChanged,
		this, &MainWindow::currentRowChanged);
}

void MainWindow::on_lineEdit_Filter_textEdited(const QString &arg1)
{
	this->m_Filter->setFilterKeyColumn(-1);
	this->m_Filter->setFilterFixedString(arg1); //根據字串過濾
}

void MainWindow::currentRowChanged(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous)

	if (current.isValid())
	{
		int Row = current.row();
		int Position = current.model()->data(current.model()->index(Row, 3)).toInt();

		this->m_VCF->SetPos(Position);
		if(this->m_VCF->Next())
		{
			VO_Record* rec = this->m_VCF->getCurrent_Record();
			ui->lineEdit_CHROM->setText(rec->CHROM);
			ui->lineEdit_POS->setText(QString::number(rec->POS));
			ui->lineEdit_ID->setText(rec->ID);
			ui->lineEdit_REF->setText(rec->REF);
			ui->lineEdit_ALT->setText(rec->ALT.join("|"));
			ui->lineEdit_QUAL->setText(QString::number(rec->QUAL));
			ui->lineEdit_INFO->setText(rec->INFO.keys().join("|"));
			ui->lineEdit_FORMAT->setText(rec->FORMAT);
			QVariantMap INFO;

			return;
		}
	}

	ui->lineEdit_CHROM->setText("");
	ui->lineEdit_POS->setText("");
	ui->lineEdit_ID->setText("");
	ui->lineEdit_REF->setText("");
	ui->lineEdit_ALT->setText("");
	ui->lineEdit_QUAL->setText("0");
	ui->lineEdit_INFO->setText("");
	ui->lineEdit_FORMAT->setText("");

}
