/*  @file
 *  @brief �����ݳ�����
 *  @author MSZ
 *  @date 2016/05/13
 *  @version 0.1
 */
#ifndef SEGMENTVIEW_H
#define SEGMENTVIEW_H

#include <QWidget>
#include "XenaManager/src/dialog/ImportData/ImportDataDialog.h"
#include "XenaManager/src/dialog/ImportData/utilTools.h"


class QTreeView;
class QStandardItemModel;
class SegmentView : public QWidget
{
	Q_OBJECT

public:
	explicit SegmentView(QWidget *parent = 0);
	~SegmentView();

signals:
	void updateStreamLength(quint16 length);							// �����������ý����еı��ĳ���

public slots:
	void parseFrame(const ImportData & frames);							// ���������SCD�ļ�����PCAP�ļ���ʾ�����ص�treeView
	void updateTreeView(ImportData *importData);						// ���stream����ͬ������

private:
	void initUI();
	void setMainLayout();
	void setSegmentData();
	void updateView();													// ����segmentView�е�����

private:
	QTreeView *m_pSegmentTreeView;
	QStandardItemModel *m_pSegmentModel;

	ImportData m_importData;
};

#endif // SEGMENTVIEW_H
