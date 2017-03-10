/*  @file
 *  @brief 配置界面的虚基类
 *  @author MSZ
 *  @date 2016/05/13
 *  @version 0.1
 */
#ifndef BASECONFIGWIDGET_H
#define BASECONFIGWIDGET_H

#include <QWidget>

#define GAPBETWEENLAYOUT 100
#define MAXWIDGETHEIGHT 200
#define LINEEDITWIDTH 150

class QLabel;
class Communicate;
class BaseConfigWidget : public QWidget
{
	Q_OBJECT

public:
	BaseConfigWidget(QWidget *parent = 0);
	virtual ~BaseConfigWidget();

	void setTypeNameLabelContext(const QString & name);
	void setCurrentIndex(int module,int port,int stream);
	virtual void updateConfigWidget() = 0;
	
protected:
	virtual void initUI();
	
protected:
	QLabel		*m_pNameLabel;
	QLabel		*m_pTypeNameLabel;

	int			m_currentModuleIndex;
	int			m_currentPortIndex;
	int			m_currentStreamIndex;
};

#endif // BASECONFIGWIDGET_H
