#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

#include "config_recorder/uiconfigrecorder.h"

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
    void on_readRBtn_toggled(bool checked);

    void on_opPBtn_clicked();

    void on_clrOutputBtn_clicked();

    void on_prepPBtn_clicked();

    void read_sn_data_ready_sig_hdlr();

private:
    Ui::MainWindow *ui;

    bool m_prepared = false;
    QProcess * m_read_proc, *m_write_proc;
    QString m_scpt_err_str;


    UiConfigRecorder m_cfg_recorder;
    qobj_ptr_set_t m_cfg_filter_in, m_cfg_filter_out;

private:
    void sync_ui_items_display();

};
#endif // MAINWINDOW_H
