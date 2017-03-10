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
    ///     获取指定索引地址的SMV控制块所控制的DataSet的所有通道（FCDA）信息
    ///
    /// @param[in]
    ///     iedName        - SMV控制块所在的IED的名称
    /// @param[in]
    ///     ldInst         - SMV控制块所在的LDevice的Inst号
    /// @param[in]
    ///     cbName         - SMV控制块的名称
    /// @param[in]
    ///     dataSetInfoArr - 保存SMV的通道信息的数组
    /// @param[in]
    ///     arrSize        - 数组的最大容量
    ///
    /// @return
    ///     数组保存通道信息的数量：操作成功；  -1：失败
    ///
    int smvChnlInfo(const std::wstring &iedName, 
                    const std::wstring &ldInst,
                    const std::wstring &cbName, 
                    SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfoArr,
                    const int arrSize);

    ///
    /// @brief
    ///     获取指定索引地址的GOOSE控制块所控制的DataSet的所有通道（FCDA）信息
    ///
    /// @param[in]
    ///     iedName        - GOOSE控制块所在的IED的名称
    /// @param[in]
    ///     ldInst         - GOOSE控制块所在的LDevice的Inst号
    /// @param[in]
    ///     cbName         - GOOSE控制块的名称
    /// @param[in]
    ///     dataSetInfoArr - 保存GOOSE的通道信息的数组
    /// @param[in]
    ///     arrSize        - 数组的最大容量
    ///
    /// @return
    ///     数组保存通道信息的数量：操作成功；  -1：失败
    ///
    int gseChnlInfo(const std::wstring &iedName,
                    const std::wstring &ldInst,
                    const std::wstring &cbName,
                    SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfoArr,
                    const int arrSize);

    ///
    /// @brief
    ///     获取指定索引地址的GOOSE控制块的控制的某一父通道的子通道的数目
    ///
    /// @param[in]
    ///     iedName        - 该子通道所在的IED的名称
    /// @param[in]
    ///     ldInst         - 该子通道所在的LDevice的Inst号
    // @param[in]
    ///     dataSetInfo    - 父通道信息
    ///
    /// @return
    ///     子通道数目 - 操作成功； -1 - 操作失败
    ///
    int gseOneChnlSubChnlNum(
                            const std::wstring &iedName,
                            const std::wstring &ldInst,
                            const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo);

    ///
    /// @brief
    ///     获取指定索引地址的GOOSE控制块的控制的某一父通道的全部子通道的信息
    ///
    /// @note
    ///     SCL文件中某通道信息定义出错则该通道信息不予返回
    ///
    /// @param[in]
    ///     iedName           - 该子通道所在的IED的名称
    /// @param[in]
    ///     ldInst            - 该子通道所在的LDevice的Inst号
    // @param[in]
    ///     dataSetInfo       - 父通道信息
    // @param[in]
    ///     subChannelInfoArr - 保存子通道信息的数组
    // @param[in]
    ///     arrSize           - 数组大小
    ///
    /// @return
    ///     实际的子通道数 - 操作成功； -1 - 操作失败
    ///
    int gseOneChnlSubChnlInfo(
                            const std::wstring &iedName,
                            const std::wstring &ldInst,
                            const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &dataSetInfo,
                            SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *subChannelInfoArr,
                            const int arrSize );

    ///
    /// @brief
    ///     根据MAC地址、appId生产对应的SMV报文
    ///
    /// @param[in]
    ///     mac               - 该SMV控制块的MAC地址字符串，格式 xx-xx-xx-xx-xx-xx
    /// @param[in]
    ///     appId             - 该SMV控制块的APPID号
    // @param[in]
    ///     data              - 存放报文的数据缓冲区
    // @param[in]
    ///     size              - data的缓冲区大小
    ///
    /// @return
    ///      0：              - 生成报文出现错误，可能是没有找到对应的SMV控制块
    ///     >0：              - 生产报文成功，并返回报文的有效长度
    ///
    int smvGenerateFrame(const QString& mac, quint16 appId, quint8 *data, quint32 size);

    ///
    /// @brief
    ///     根据MAC地址、appId生产对应的GOOSE报文
    ///
    /// @param[in]
    ///     mac               - 该GOOSE控制块的MAC地址字符串，格式 xx-xx-xx-xx-xx-xx
    /// @param[in]
    ///     appId             - 该GOOSE控制块的APPID号
    // @param[in]
    ///     data              - 存放报文的数据缓冲区
    // @param[in]
    ///     size              - data的缓冲区大小
    ///
    /// @return
    ///      0：              - 生成报文出现错误，可能是没有找到对应的SMV控制块
    ///     >0：              - 生产报文成功，并返回报文的有效长度
    ///
    int gseGenerateFrame(const QString& mac, quint16 appId, quint8 *data, quint32 size);

    static CScdAnalysis* getInstance();
protected:
    CScdAnalysis();
    ~CScdAnalysis();

private:
    ///
    /// @brief
    ///    获取通道的类型信息
    ///
    /// @param[in] stSmvChannelInfo 采样值数据集中的数据通道信息
    ///
    /// @return
    ///     返回对应的通道类型信息 时间/电压/电流...
    ///
    enumChannelType __GetChannelType(const SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO& stSmvChannelInfo );

    ///
    /// @brief
    ///    获取通道的类型信息
    ///
    /// @param[in] stGooseChannelInfo goose数据集中的数据通道信息
    ///
    /// @return
    ///     返回对应的通道类型信息 单电/双点/时间...
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