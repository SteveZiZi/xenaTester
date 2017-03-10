#include "segmentview.h"
#include <QTreeView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QLayout>


#define SEGMENTMINWIDTH 200
#define VALUEMINWIDTH 400

SegmentView::SegmentView(QWidget *parent)
	: QWidget(parent)
{
	initUI();
}

SegmentView::~SegmentView()
{

}

void SegmentView::parseFrame(const ImportData & frames)
{
	m_importData = frames;
	updateView();
}

void SegmentView::updateTreeView(ImportData *importData)
{
	m_importData = *importData;
	updateView();
}

void SegmentView::initUI()
{
	m_pSegmentTreeView = new QTreeView(this);
	//m_pSegmentTreeView->header()->setStretchLastSection(true);
	m_pSegmentModel = new QStandardItemModel(this);
	m_pSegmentTreeView->setModel(m_pSegmentModel);

// 	QStringList segmentStringList;
// 	segmentStringList<< tr("×Ö¶Î") << tr("Êý¾Ý");
// 	m_pSegmentModel->setHorizontalHeaderLabels(segmentStringList);

	setMainLayout();
}

void SegmentView::setMainLayout()
{
	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addWidget(m_pSegmentTreeView);
	mainLayout->setMargin(1);
	this->setLayout(mainLayout);
}

void SegmentView::setSegmentData()
{
	QStandardItem *item = new QStandardItem("");
	m_pSegmentModel->appendRow(item);
	m_pSegmentModel->setItem(0,1,new QStandardItem("Null"));
}

void SegmentView::updateView()
{
	m_pSegmentModel->clear();
	ethFrameToItemModel(m_importData.frame, m_importData.frameLength, m_pSegmentModel);

	emit updateStreamLength(m_importData.frameLength);
	m_pSegmentTreeView->setColumnWidth(0, SEGMENTMINWIDTH);
	m_pSegmentTreeView->setColumnWidth(1, VALUEMINWIDTH);
	m_pSegmentTreeView->expandAll();
}
