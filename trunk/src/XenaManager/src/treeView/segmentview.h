/*  @file
 *  @brief 流数据呈现类
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
	void updateStreamLength(quint16 length);							// 更新流的配置界面中的报文长度

public slots:
	void parseFrame(const ImportData & frames);							// 将解析后的SCD文件或者PCAP文件显示到本地的treeView
	void updateTreeView(ImportData *importData);						// 点击stream流，同步更新

private:
	void initUI();
	void setMainLayout();
	void setSegmentData();
	void updateView();													// 更新segmentView中的内容

private:
	QTreeView *m_pSegmentTreeView;
	QStandardItemModel *m_pSegmentModel;

	ImportData m_importData;
};

#endif // SEGMENTVIEW_H
