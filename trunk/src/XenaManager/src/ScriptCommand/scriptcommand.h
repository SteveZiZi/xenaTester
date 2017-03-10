/*  @file
 *  @brief ָ�����ʵ��
 *  @author MSZ
 *  @date 2016/05/04
 *  @version 0.1
 */
#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <QObject>
#include <QVector>
enum E_ORDER {
    C_LOGON,															// ��½
    C_LOGOFF,															// �ǳ�
    C_OWNER,															// �����û���
    C_KEEPALIVE,														// ��������
    C_MODEL,															// ��ȡģʽ 
    C_PORTCOUNTS,														// ��ȡ�˿�����
    C_NAME,																// ��ȡ��������
    C_SERIALNO,															// ��ȡ���к�
    C_RESERVATION_QUERY,												// ��ѯ����ռ��״̬
    C_RESERVATION_RESERVE,												// ռ�û���
    C_RESERVATION_RELEASE,												// �ͷŻ���
    C_RESERVATION_RELINQUISH,											// ��������
    C_RESERVEDBY,														// ��ȡռ�û�����û�
    C_INFO,                                                             // ������Ϣ
    C_CONFIG,                                                           // ����������Ϣ
    C_STATS,                                                            // ����״̬��Ϣ
    M_MODULE_COUNTS,													// ��ȡ�忨����
    M_RESERVATION_QUERY,												// ��ѯ�忨ռ��״̬
    M_RESERVATION_RESERVE,												// ռ�ð忨
    M_RESERVATION_RELEASE,												// �ͷŰ忨
    M_RESERVATION_RELINQUISH,											// �����忨
    M_RESERVEDBY,														// ��ȡռ�ð忨���û�
    M_MODEL,															// ��ȡ�忨����
    M_INFO,                                                             // �忨��Ϣ
    M_CONFIG,                                                           // �忨������Ϣ
    M_STATUS,                                                           // �忨״̬��Ϣ
    P_RESERVATION_QUERY,												// ��ѯ�˿�ռ��״̬
    P_RESERVATION_RESERVE,												// ռ�ö˿�
    P_RESERVATION_RELEASE,												// �ͷŶ˿�
    P_RESERVATION_RELINQUISH,											// �����˿�
    P_RESERVEDBY,														// ��ȡռ�ö˿ڵ��û�
    P_COMMENT,															// �˿�����
    P_INTERFACE,														// �˿�����
    P_RECEIVESYNC,														// ��ѯ�˿�ͬ��״̬
    P_TRAFFIC_QUERY,													// ��ѯ�˿ڷ���״̬
    P_TRAFFIC_ON,														// �˿ڷ�������
    P_TRAFFIC_OFF,														// �˿�ֹͣ��������
    P_TPLDMODE_QUERY,													// ��ѯ�˿�TPLDģʽ
    P_TPLDMODE_SET_NORMAL,												// ���ö˿�TPLDģʽΪNORMALģʽ
    P_TPLDMODE_SET_MIC,													// ���ö˿�TPLDģʽΪMICģʽ
    P_CHECKSUM_QUERY,													// �˿�У��Ͳ�ѯ
    P_CHECKSUM_SET,														// �˿�У�������
    P_MAXHEADERLENGTH_QUERY,											// �˿���ͷ�����Ȳ�ѯ
    P_MAXHEADERLENGTH_SET,												// �˿�����ͷ����������
    P_TXTIMELIMIT_QUERY,												// ��ѯ�˿ڷ���ʱ������
    P_TXTIMELIMIT_SET,													// ���ö˿ڷ���ʱ������
    P_SPEED,															// ��ѯ�˿ڵ�ʵ������
    P_INFO,                                                             // �˿���Ϣ
    P_CONFIG,                                                           // �˿�������Ϣ
    P_FULLCONIFG,                                                       // �˿�������Ϣ(��������ͳ����Ϣ)
    P_TXMODE_QUERY,                                                     // ��ѯ�˿ڷ���ģʽ
    P_TXMODE_NORMAL,                                                    // ���ö˿ڷ���ģʽΪ��ͨģʽ
    P_TXMODE_STRICUNIFORM,                                              // ���ö˿ڷ���ģʽΪStrict
    P_TXMODE_SEQUENTIAL,                                                // ���ö˿ڷ���ģʽΪ����ģʽ
    P_RATEFRACTION_QUERY,                                               // ��ѯ�˿ڵķ�������
    P_RATEFRACTION_SET,                                                 // ���ö˿ڵķ�������
    P_RATEPPS_QUERY,                                                    // ��ѯ�˿ڵķ�������
    P_RATEPPS_SET,                                                      // ���ö˿ڵķ�������
    P_RATEL2BPS_QUERY,                                                  // ��ѯ�˿ڵķ�������
    P_RATEL2BPS_SET,                                                    // ���ö˿ڵķ�������
    PS_CREATE,															// ����һ����������
    PS_DELETE,															// ɾ��һ��������
    PS_INDECES,															// ����ͬʱ�������������� ͬʱ֧�ֶ������Ķ�ȡ
    PS_DISENABLE,														// ��ʹ��������
    PS_ENABLE,															// ʹ��������
    PS_ENABLE_QUERY,													// ����ʹ��״̬��ѯ
    PS_TPLDID,															// ��������ID
    PS_COMMENT,															// ����������
    PS_PACKETLIMIT_SET,													// ��������������
    PS_PACKETLIMIT_QUERY,												// ��ȡ����������
    PS_PACKETHEADER,													// ��������Header
    PS_HEADERPROTOCOL,													// ��������Header��Լ
    PS_MODIFIER_INC,													// ����������Ϊ����
    PS_MODIFIER_DEC,													// ����������Ϊ����
    PS_MODIFIER_RAM,													// ����������Ϊ����䶯
    PS_MODIFIER_RANGE,                                                  // ����������ı仯��Χ
    PS_MODIFIERCOUNT,													// ��ȡ������ĸ���
    PS_MODIFIERCOUNT_SET,                                               // ����������ĸ���(ÿ������֮ǰҪ�Ƚ�������)
    PS_RATEFRACTION_SET,												// �������ķ�������(�ٷֱ�)
    PS_RATEFRACTION_QUERY,												// ��ȡ���ķ�������(�ٷֱ�)
    PS_RATEPPS_SET,														// �������ķ�������(֡ÿ��)
    PS_RATEPPS_QUERY,													// ��ȡ���ķ�������(֡ÿ��)
    PS_PACKETLENGTH_FIXED,												// ���ù̶��ı��ĳ���
    PS_CONFIG,                                                          // ��ȡ����������Ϣ
    PT_TOTAL                                                            // ͳ�ƶ˿ڷ�����֡
};

typedef QVector<E_ORDER> E_ORDERS;

struct S_MODIFY{
    quint16 modifyIndex;    //����������
    quint16 pos;            //���ĸ�λ�õݱ䣬0->���ĵĵ�һ���ֽ�
    quint16 minVal;         //�仯����Сֵ
    quint16 maxVal;         //�仯�����ֵ
    quint16 step;           //�仯�Ĳ���
    quint16 repeat;         //�仯ѭ������
};

struct S_ORDER{
    int moduleIndex;
    int portIndex;
    int streamindex;
    QString data;
    S_MODIFY modify;
};

class ScriptCommand : public QObject
{
	Q_OBJECT
public:
	explicit ScriptCommand(QObject *parent = 0);
	/*
		brief: ָ�QByteArray��ת��
		param [in] int type,ORDERSTRUCT order
		param [out]None
		return QByteArray: ָ��ת���ɵ�����
		author:MSZ
		date: 2016-05-04
	*/
    QByteArray cmdToByteArray(E_ORDER type,S_ORDER  & order);

    void setPassword(const QString &passwd) { m_password = passwd; }
    void setUserName(const QString &userName) { m_userName = userName; }

public slots:

signals:

private:

    // ��������
    QString loginCommand();												// ��½
    QString logoffCommand();											// �ǳ�
    QString ownerCommand();												// �趨�û���
    QString keepAlive();												// ��������״̬
    QString getModelCommand();											// ��ȡģʽ
    QString getPortCounts();											// ��ȡ�˿������Լ��忨����
    QString getChassisName();											// ��ȡ��������
    QString getChassisSerialNo();										// ��ȡ�������к�
    QString queryChassisReservation();									// ��ȡ����ռ�����
    QString reserveChassis();											// ռ�û���
    QString releaseChassis();											// �ͷŻ���
    QString relinquishChassis();										// ��������
    QString getChassisReservedBy();										// ��ȡ����ռ��������
    QString getChassisInfo();                                           // ��ȡ������Ϣ
    QString getChassisConifg();                                         // ��ȡ����������Ϣ
    QString getChassisStats();                                          // ��ȡ�����״̬��Ϣ

    // �忨����
    QString getModuleCounts();											// ��ȡ�忨����
    QString composeModuleCommandHead(const S_ORDER & order);			// ��ϰ忨����ǰ����ͬ�Ĳ���
    QString queryModuleReservation(const S_ORDER & order);				// ��ȡ�忨ռ�����
    QString reserveModule(const S_ORDER & order);						// ռ�ð忨
    QString releaseModule(const S_ORDER & order);						// �ͷŰ忨
    QString relinquishModule(const S_ORDER & order);					// �����忨
    QString getModuleModel(const S_ORDER & order);						// ��ȡ�忨����
    QString getModuleReservedBy(const S_ORDER & order);					// ��ȡ�忨ռ��������
    QString getModuleInfo(const S_ORDER & order);                       // ��ȡ�忨��Ϣ
    QString getModuleConfig(const S_ORDER & order);                     // ��ȡ�忨������Ϣ
    QString getModuleStatus(const S_ORDER & order);                     // ��ȡ�忨״̬��Ϣ

    // �˿�����
    QString composePortCommandHead(const S_ORDER & order);				// ��϶˿�����ǰ����ͬ�Ĳ���
    QString queryPortReservation(const S_ORDER & order);				// �˿�ռ�����
    QString reservePort(const S_ORDER & order);							// ռ�ö˿�
    QString releasePort(const S_ORDER & order);							// �ͷŶ˿�
    QString relinquishPort(const S_ORDER & order);						// �����˿�
    QString getPortReservedBy(const S_ORDER & order);					// ��ȡ�˿�ռ��������
    QString getPortDiscription(const S_ORDER & order);					// ��ȡ�˿ڵ�����
    QString getPortName(const S_ORDER & order);							// ��ȡ�˿�����
    QString getPortSync(const S_ORDER & order);							// ��ȡ�˿�ͬ��״̬
    QString getPortTrafficStatu(const S_ORDER & order);					// ��ȡ�˿ڷ���״̬
    QString onPortTraffic(const S_ORDER & order);						// �˿ڷ���
    QString offPortTraffic(const S_ORDER & order);						// �˿�ֹͣ����
    QString getPortTpldMode(const S_ORDER & order);						// ��ȡ�˿�TPLD��ģʽ
    QString setPortTpldModeNormal(const S_ORDER & order);				// ���ö˿�TPLDΪ��ͨģʽ
    QString setPortTpldModeMic(const S_ORDER & order);					// ���ö˿�TPLDΪMICģʽ
    QString getPortCheckSum(const S_ORDER & order);						// ��ȡ�˿�CheckSumֵ
    QString setPortCheckSum(const S_ORDER & order);						// ���ö˿�CheckSumֵ
    QString getPortHeaderLegth(const S_ORDER & order);					// ��ȡ�˿�Header�ĳ���
    QString setPortHeaderLegth(const S_ORDER & order);					// ������������ĳ��ȣ��ɱ༭���֣�
    QString getTxTimeLimit(const S_ORDER & order);						// ��ȡ�˿ڴ���ʱ��
    QString setTxTimeLimit(const S_ORDER & order);						// ���ö˿ڴ���ʱ��
    QString getPortSpeed(const S_ORDER & order);						// ��ȡ�˿ڵķ�������
    QString getPortInfo(const S_ORDER & order);                         // ��ȡ�˿���Ϣ
    QString getPortConfig(const S_ORDER & order);                       // ��ȡ�˿�������Ϣ
    QString getPortFullConfig(const S_ORDER & order);                   // ��ȡ�˿�������Ϣ(��������ͳ�Ƶ���Ϣ)
    QString getTxMode(const S_ORDER & order);                           // ��ȡ�˿ڷ���ģʽ
    QString setTxModeNormal(const S_ORDER & order);                     // ���ö˿ڷ���ģʽ--normal
    QString setTxModeStrict(const S_ORDER & order);                     // ���ö˿ڷ���ģʽ--strict
    QString setTxModeSequential(const S_ORDER & order);                 // ���ö˿ڷ���ģʽ--sequential
    QString getPortRateFraction(const S_ORDER & order);                 // ��ȡ�˿ڷ�������--
    QString setPortRateFraction(const S_ORDER & order);                 // ���ö˿ڷ�������
    QString getPortPacketRate(const S_ORDER & order);                   // ��ȡ�˿ڷ�������--֡����
    QString setPortPacketRate(const S_ORDER & order);                   // ���ö˿ڷ�������
    QString getPortBitRate(const S_ORDER & order);                      // ��ȡ�˿ڷ�������--λ����
    QString setPortBitRate(const S_ORDER & order);                      // ���ö˿ڷ�������

    // ������
    QString composeStraeamCommandHead(const S_ORDER & order);			// ���������ǰ����ͬ�Ĳ���
    QString createStream(const S_ORDER & order);						// ����һ����
    QString deleteStream(const S_ORDER & order);						// ɾ��һ����
    QString disenableStream(const S_ORDER & order);						// ��ʹ��һ����
    QString enableStream(const S_ORDER & order);						// ʹ��һ����
    QString getStreamStatus(const S_ORDER & order);						// ��ȡ����ʹ��״̬
    QString setTPLDID(const S_ORDER & order);							// ��������ID
    QString setStreamComment(const S_ORDER & order);                    // ������������
    QString setStreamLimit(const S_ORDER & order);						// �������Ĵ���֡��������
    QString getStreamLimit(const S_ORDER & order);						// ��ȡ���Ĵ���֡��������
    QString setStreamHeader(const S_ORDER & order);						// ��������Header
    QString setStreamHeaderProtocol(const S_ORDER & order);				// ��������Header�Ĺ�Լ (��̫����Լ��VLAN)
    QString setModifierPolicy(const QString  & act,const S_ORDER & order);// ���ø���Modify�Ĳ���
    QString setStreamIncModifier(const S_ORDER & order);				// ModifyΪ����
    QString setStreamDecModifier(const S_ORDER & order);				// ModifyΪ����
    QString setStreamRamModifier(const S_ORDER & order);				// ModifyΪ���
    QString setStreamModifierRanger(const S_ORDER & order);             // ������Modify�ı仯��Χ
    QString getStreamModifierCounts(const S_ORDER & order);				// ��ȡModify�ĸ���
    QString setStreamModifierCounts(const S_ORDER & order);             // ����Modify�ĸ���(����ǰҪ�Ƚ�������)
    QString setStreamRate(const S_ORDER & order);						// ������������(�ٷֱ�)
    QString getStreamRate(const S_ORDER & order);						// ��ȡ��������(�ٷֱ�)
    QString setStreamRatePacketPerSec(const S_ORDER & order);			// ������������(֡ÿ��)
    QString getStreamRatePacketPerSec(const S_ORDER & order);			// ��ȡ��������(֡ÿ��)
    QString setStreamFixedPackedLegth(const S_ORDER & order);			// �������Ĺ̶����ĳ���
    QString getStreamConfig(const S_ORDER & order);                     // ��ȡ����������Ϣ

    // ͳ������
    QString getPortTotal(const S_ORDER & order);                        // ͳ�ƶ˿ڷ��͵�����

private:
    QString m_password;
    QString m_userName;
};

#endif
