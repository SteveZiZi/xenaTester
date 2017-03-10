#pragma once

#include <QtGui/QDialog>

struct ImportData {
    QString frameStyle;         //  ���ĸ�ʽ�ַ���  SMV92 GOOSE UNKNOWN
    quint16 appId;              //  IED�豸��APPID��
    QString iedName;            //  IED�豸������  ��pcap�е��� iedName = UNKNOWN
    quint8* frame;              //  ���ĵĻ�����ָ��
    quint16 frameLength;        //  ���ĵĳ���

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
     *  @Func:  �����ļ�
     *  
     *  @Param[in]:
     *      fileName:   ��Ҫ�������ļ����������ļ�·����֧�����·��
     *  
     *  @Param[out]:
     *      None
     *  
     *  @Return:
     *      true��      �����ļ��ɹ�
     *      false��     �����ļ�ʧ��
     */
    virtual bool parseFile(const QString& fileName) = 0;

    /*
     *  @Func:  ��ȡѡ����ƿ�ı���
     *  
     *  @Param[in]:
     *      data��      ��ű��ĵĻ�����ָ��
     *      arrSize��   data�������Ĵ�С
     *  
     *  @Param[out]:
     *      None
     *  
     *  @Return:
     *      false:      ��ȡ����ʧ�ܣ����Ľ��������������ݳ���û��ѡ�е����Ŀ��ƿ鱨��
     *      true:       ���ر��ĵ�ʵ�ʳ���
     */
    virtual bool selectedFrame(ImportData &data) = 0;
};

enum IMPORT_FILE_STYLE {
    IMPORT_SCD_FILE,
    IMPORT_PCAP_FILE
};

ImportDataDialog* CreateImportDataDlg(IMPORT_FILE_STYLE fileType, QWidget* parent = 0);
void ReleaseImportDataDlg(ImportDataDialog* dlg);