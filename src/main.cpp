#include <QtGui>
#include <QApplication>
#include <string>
#include <stdio.h>
#include <iostream>

#include "widget.h"
#include "ctpthread.h"

#ifdef __unix__                   /* __unix__ is usually defined by compilers targeting Unix systems */
#define OS_Windows 0
#include <unistd.h>
#elif defined(_WIN32) || defined(WIN32)     /* _Win32 is usually defined by compilers targeting 32 or   64 bit Windows systems */
#define OS_Windows 1
#include <windows.h>
#endif

QStandardItemModel *createQuoteModel(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(0, 14, parent);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("��Լ����"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("����������"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("��Լ����"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("�ǵ���"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("����"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("��߼�"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("��ͼ�"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("���¼�"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("������"));
    model->setHeaderData(9, Qt::Horizontal, QObject::tr("�ɽ�����"));
    model->setHeaderData(10, Qt::Horizontal, QObject::tr("�ɽ����"));
    model->setHeaderData(11, Qt::Horizontal, QObject::tr("��ͣ���"));
    model->setHeaderData(12, Qt::Horizontal, QObject::tr("��ͣ���"));
    model->setHeaderData(13, Qt::Horizontal, QObject::tr("����޸�ʱ��"));

    return model;
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    Widget w;
    ctpThread thrd_ctp;
    QStandardItemModel *quoteModel = createQuoteModel(&w);
    w.setQuoteModel(quoteModel);
    w.show();
    thrd_ctp.setQuoteModel(quoteModel);
    thrd_ctp.start();
    return a.exec();
}
