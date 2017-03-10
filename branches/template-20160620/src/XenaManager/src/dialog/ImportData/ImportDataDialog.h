#pragma once

#include <QtGui/QDialog>

struct ImportData {
    QString frameStyle;         //  报文格式字符串  SMV92 GOOSE UNKNOWN
    quint16 appId;              //  IED设备的APPID号
    QString iedName;            //  IED设备的名字  从pcap中导入 iedName = UNKNOWN
    quint8* frame;              //  报文的缓冲区指针
    quint16 frameLength;        //  报文的长度

    ImportData() : frameStyle("UNKNOWN"), appId(0x00), iedName("UNKNOWN"), frameLength(0)
    {
        frame = new quint8[1536];
    }
    ImportData(const ImportData &d) {depth_copy(d);}
    ~ImportData() {
        if (frame) {
            delete[] frame;
            frame = NULL;
        }
    }
    ImportData &operator=(const ImportData &d) {depth_copy(d); return *this;}

    void depth_copy(const ImportData &d) {
        this->frameStyle = d.frameStyle;
        this->appId = d.appId;
        this->iedName = d.iedName;
        this->frameLength = d.frameLength;
        memcpy(this->frame, d.frame, d.frameLength);
    }
};

class ImportDataDialog : public QDialog
{
public:
    ImportDataDialog(QWidget *parent = 0, Qt::WindowFlags f = 0) {}
    virtual ~ImportDataDialog() {}

    enum {MAX_FRAME_SIZE = 1536};

    /*
     *  @Func:  解析文件
     *  
     *  @Param[in]:
     *      fileName:   需要解析的文件名，包含文件路径，支持相对路径
     *  
     *  @Param[out]:
     *      None
     *  
     *  @Return:
     *      true：      解析文件成功
     *      false：     解析文件失败
     */
    virtual bool parseFile(const QString& fileName) = 0;

    /*
     *  @Func:  获取选择控制块的报文
     *  
     *  @Param[in]:
     *      data：      存放报文的缓冲区指针
     *      arrSize：   data缓冲区的大小
     *  
     *  @Param[out]:
     *      None
     *  
     *  @Return:
     *      false:      获取报文失败，报文解析出错，参数传递出错，没有选中导出的控制块报文
     *      true:       返回报文的实际长度
     */
    virtual bool selectedFrame(ImportData &data) = 0;
};

enum IMPORT_FILE_STYLE {
    IMPORT_SCD_FILE,
    IMPORT_PCAP_FILE
};

ImportDataDialog* CreateImportDataDlg(IMPORT_FILE_STYLE fileType, QWidget* parent = 0);
void ReleaseImportDataDlg(ImportDataDialog* dlg);