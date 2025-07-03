#include <QProcess>
#include <QButtonGroup>
#include <QDir>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logger/logger.h"

static const char* gs_local_scpt_folder = "op-scripts";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_cfg_recorder(this)
{
    ui->setupUi(this);

    QString curr_pth = QDir::currentPath();
    QString work_pth = curr_pth + "/" + gs_local_scpt_folder;

    m_read_proc = new QProcess(this);
    connect(m_read_proc, &QProcess::readyReadStandardOutput,
            this, &MainWindow::read_sn_data_ready_sig_hdlr);
    m_read_proc->setWorkingDirectory(work_pth);
    m_read_proc->setProgram(work_pth + "/" + "sn-operator.bat");
    m_read_proc->setArguments({"read-sn"});

    connect(m_read_proc, &QProcess::readyReadStandardError, [=]() {
        QByteArray err = m_read_proc->readAllStandardError();
        m_scpt_err_str = QString::fromLocal8Bit(err);
        ui->outputDispEdit->append(m_scpt_err_str);
    });

    m_write_proc = new QProcess(this);
    connect(m_write_proc, &QProcess::readyReadStandardError, [=]() {
        QByteArray err = m_write_proc->readAllStandardError();
        ui->outputDispEdit->append(QString::fromLocal8Bit(err));
    });
    m_write_proc->setWorkingDirectory(work_pth);
    m_write_proc->setProgram(work_pth + "/" + "sn-operator.bat");


    m_cfg_filter_in.clear();
    m_cfg_filter_in << ui->SNStrLEdit << ui->SNHexLEdit;

    QButtonGroup *rw_rbtn_grp = new QButtonGroup(this);
    rw_rbtn_grp->addButton(ui->readRBtn);
    rw_rbtn_grp->addButton(ui->writeRBtn);

    ui->SNHexLEdit->setReadOnly(true);

    /*set default ui items state.*/
    ui->writeRBtn->setChecked(true);

    /*load saved ui cfg-----------------------------------------------------------*/
    m_cfg_recorder.load_configs_to_ui(this, m_cfg_filter_in, m_cfg_filter_out);

    sync_ui_items_display();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sync_ui_items_display()
{
    ui->SNStrLEdit->setReadOnly(ui->readRBtn->isChecked());
}

void MainWindow::on_readRBtn_toggled(bool checked)
{
    ui->SNStrLEdit->setReadOnly(checked);
}


void MainWindow::on_opPBtn_clicked()
{
    bool proc_ret;
    QString ret_str;
    QProcess * proc;

    m_scpt_err_str.clear();
    ui->SNHexLEdit->clear();

    if(ui->readRBtn->isChecked())
    {
        ui->SNStrLEdit->clear();
        proc = m_read_proc;
    }
    else
    {
        QString sn_str = ui->SNStrLEdit->text();
        if(sn_str.isEmpty())
        {
            QMessageBox::critical(this, "", "SN不能为空");
            return;
        }
        m_write_proc->setArguments({"write-sn", sn_str});
        proc = m_write_proc;
    }
    proc->start();
    proc_ret = proc->waitForFinished(-1);
    ret_str = proc_ret ? "ok" : "error";
    ui->outputDispEdit->append(ret_str);
}


void MainWindow::on_clrOutputBtn_clicked()
{
    ui->outputDispEdit->clear();
}


void MainWindow::on_prepPBtn_clicked()
{
    QString app_curr_pth = QDir::currentPath();
    QString bat_path = QDir::toNativeSeparators(app_curr_pth + "/" + gs_local_scpt_folder
                                                + "/01.gen-keys.bat");

    QString bat_dir = QFileInfo(bat_path).absolutePath();
    QString command = QString("cmd.exe");
    QStringList args = {
        "/c", "start", "cmd.exe", "/k",
        bat_path
    };

    QProcess::startDetached(command, args, bat_dir);

    m_prepared = true;
}

void MainWindow::read_sn_data_ready_sig_hdlr()
{
    if(m_scpt_err_str.isEmpty())
    {
        ui->SNHexLEdit->clear();
        QByteArray output = m_read_proc->readAllStandardOutput();
        QString sn_hex_str = QString::fromUtf8(output);
        ui->SNHexLEdit->setText(sn_hex_str);

        ui->SNStrLEdit->clear();
        QByteArray raw_data = QByteArray::fromHex(sn_hex_str.remove(QChar(' ')).toUtf8());
        QString sn_ascii_str = QString::fromUtf8(raw_data);
        ui->SNStrLEdit->setText(sn_ascii_str);
    }
}
