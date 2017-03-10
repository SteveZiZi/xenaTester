#pragma once

#include <QString>
#include <QtGui/QStandardItemModel>
#include "sclManager/include/sclManager.h"
#include "inc\pub_datatypes.h"


class CScdAnalysis {
public:
    enum {MAC_ADDR_LENTH = 6};

    void setSrcMacAddr(unsigned char* mac, int length);
    void setSrcMacAddr(const QString &mac);

    bool parseFile(const QString& fileName);

    int smvIedNum() {return m_smvIedNum;}
    int gseIedNum() {return m_gseIedNum;}
    const SCDGOOSEANALYSIS_IEDGOOSE_INFO* gseIedInfo(int item) {
        Q_ASSERT( item < m_gseIedNum && item >= 0 );
        return m_gseIedInfo+item;
    }
    const SCDSMV92ANALYSIS_IEDSMV_INFO* smvIedInfo(int item) {
        Q_ASSERT ( item < m_smvIedNum && item>=0 );
        return m_smvIedInfo+item;
    }

    const SCDSMV92ANALYSIS_IEDSMV_INFO* smvIedInfo(const QString &mac, quint16 appId);
    const SCDGOOSEANALYSIS_IEDGOOSE_INFO* gseIedInfo(const QString &mac, quint16 appId);

    ///
    /// @brief
    ///     ��ȡָ��������ַ��SMV���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
    ///
    /// @param[in]
    ///     iedName        - SMV���ƿ����ڵ�IED������
    /// @param[in]
    ///     ldInst         - SMV���ƿ����ڵ�LDevice��Inst��
    /// @param[in]
    ///     cbName         - SMV���ƿ������
    /// @param[in]
    ///     dataSetInfoArr - ����SMV��ͨ����Ϣ������
    /// @param[in]
    ///     arrSize        - ������������
    ///
    /// @return
    ///     ���鱣��ͨ����Ϣ�������������ɹ���  -1��ʧ��
    ///
    int smvChnlInfo(const std::wstring &iedName, 
                    const std::wstring &ldInst,
                    const std::wstring &cbName, 
                    SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
                    const int arrSize);

    ///
    /// @brief
    ///     ��ȡָ��������ַ��GOOSE���ƿ������Ƶ�DataSet������ͨ����FCDA����Ϣ
    ///
    /// @param[in]
    ///     iedName        - GOOSE���ƿ����ڵ�IED������
    /// @param[in]
    ///     ldInst         - GOOSE���ƿ����ڵ�LDevice��Inst��
    /// @param[in]
    ///     cbName         - GOOSE���ƿ������
    /// @param[in]
    ///     dataSetInfoArr - ����GOOSE��ͨ����Ϣ������
    /// @param[in]
    ///     arrSize        - ������������
    ///
    /// @return
    ///     ���鱣��ͨ����Ϣ�������������ɹ���  -1��ʧ��
    ///
    int gseChnlInfo(const std::wstring &iedName,
                    const std::wstring &ldInst,
                    const std::wstring &cbName,
                    SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
                    const int arrSize);

    ///
    /// @brief
    ///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ������ͨ������Ŀ
    ///
    /// @param[in]
    ///     iedName        - ����ͨ�����ڵ�IED������
    /// @param[in]
    ///     ldInst         - ����ͨ�����ڵ�LDevice��Inst��
    // @param[in]
    ///     dataSetInfo    - ��ͨ����Ϣ
    ///
    /// @return
    ///     ��ͨ����Ŀ - �����ɹ��� -1 - ����ʧ��
    ///
    int gseOneChnlSubChnlNum(
                            const std::wstring &iedName,
                            const std::wstring &ldInst,
                            const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo);

    ///
    /// @brief
    ///     ��ȡָ��������ַ��GOOSE���ƿ�Ŀ��Ƶ�ĳһ��ͨ����ȫ����ͨ������Ϣ
    ///
    /// @note
    ///     SCL�ļ���ĳͨ����Ϣ����������ͨ����Ϣ���践��
    ///
    /// @param[in]
    ///     iedName           - ����ͨ�����ڵ�IED������
    /// @param[in]
    ///     ldInst            - ����ͨ�����ڵ�LDevice��Inst��
    // @param[in]
    ///     dataSetInfo       - ��ͨ����Ϣ
    // @param[in]
    ///     subChannelInfoArr - ������ͨ����Ϣ������
    // @param[in]
    ///     arrSize           - �����С
    ///
    /// @return
    ///     ʵ�ʵ���ͨ���� - �����ɹ��� -1 - ����ʧ��
    ///
    int gseOneChnlSubChnlInfo(
                            const std::wstring &iedName,
                            const std::wstring &ldInst,
                            const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
                            SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
                            const int arrSize );

    ///
    /// @brief
    ///     ����MAC��ַ��appId������Ӧ��SMV����
    ///
    /// @param[in]
    ///     mac               - ��SMV���ƿ��MAC��ַ�ַ�������ʽ xx-xx-xx-xx-xx-xx
    /// @param[in]
    ///     appId             - ��SMV���ƿ��APPID��
    // @param[in]
    ///     data              - ��ű��ĵ����ݻ�����
    // @param[in]
    ///     size              - data�Ļ�������С
    ///
    /// @return
    ///      0��              - ���ɱ��ĳ��ִ��󣬿�����û���ҵ���Ӧ��SMV���ƿ�
    ///     >0��              - �������ĳɹ��������ر��ĵ���Ч����
    ///
    int smvGenerateFrame(const QString& mac, quint16 appId, quint8 *data, quint32 size);

    ///
    /// @brief
    ///     ����MAC��ַ��appId������Ӧ��GOOSE����
    ///
    /// @param[in]
    ///     mac               - ��GOOSE���ƿ��MAC��ַ�ַ�������ʽ xx-xx-xx-xx-xx-xx
    /// @param[in]
    ///     appId             - ��GOOSE���ƿ��APPID��
    // @param[in]
    ///     data              - ��ű��ĵ����ݻ�����
    // @param[in]
    ///     size              - data�Ļ�������С
    ///
    /// @return
    ///      0��              - ���ɱ��ĳ��ִ��󣬿�����û���ҵ���Ӧ��SMV���ƿ�
    ///     >0��              - �������ĳɹ��������ر��ĵ���Ч����
    ///
    int gseGenerateFrame(const QString& mac, quint16 appId, quint8 *data, quint32 size);

    static CScdAnalysis* getInstance();
protected:
    CScdAnalysis();
    ~CScdAnalysis();

private:
    ///
    /// @brief
    ///    ��ȡͨ����������Ϣ
    ///
    /// @param[in] stSmvChannelInfo ����ֵ���ݼ��е�����ͨ����Ϣ
    ///
    /// @return
    ///     ���ض�Ӧ��ͨ��������Ϣ ʱ��/��ѹ/����...
    ///
    enumChannelType __GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo );

    ///
    /// @brief
    ///    ��ȡͨ����������Ϣ
    ///
    /// @param[in] stGooseChannelInfo goose���ݼ��е�����ͨ����Ϣ
    ///
    /// @return
    ///     ���ض�Ӧ��ͨ��������Ϣ ����/˫��/ʱ��...
    ///
    enumChannelType __GetChannelType(const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO& stGooseChannelInfo );


private:
    CSCLManager* m_sclMgr;
    ISCDGOOSEAnalysis* m_gseAnalysis;
    ISCDSMV92Analysis* m_smvAnalysis;
    SCDGOOSEANALYSIS_IEDGOOSE_INFO* m_gseIedInfo;
    int m_gseIedNum;
    SCDSMV92ANALYSIS_IEDSMV_INFO* m_smvIedInfo;
    int m_smvIedNum;

    unsigned char m_srcMacAddr[MAC_ADDR_LENTH];
};