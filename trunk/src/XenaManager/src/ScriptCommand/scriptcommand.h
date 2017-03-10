/*  @file
 *  @brief 指令类的实现
 *  @author MSZ
 *  @date 2016/05/04
 *  @version 0.1
 */
#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <QObject>
#include <QVector>
enum E_ORDER {
    C_LOGON,															// 登陆
    C_LOGOFF,															// 登出
    C_OWNER,															// 设置用户名
    C_KEEPALIVE,														// 保持在线
    C_MODEL,															// 获取模式 
    C_PORTCOUNTS,														// 获取端口数量
    C_NAME,																// 获取机箱名称
    C_SERIALNO,															// 获取序列号
    C_RESERVATION_QUERY,												// 查询机箱占用状态
    C_RESERVATION_RESERVE,												// 占用机箱
    C_RESERVATION_RELEASE,												// 释放机箱
    C_RESERVATION_RELINQUISH,											// 放弃机箱
    C_RESERVEDBY,														// 获取占用机箱的用户
    C_INFO,                                                             // 机箱信息
    C_CONFIG,                                                           // 机箱配置信息
    C_STATS,                                                            // 机箱状态信息
    M_MODULE_COUNTS,													// 获取板卡数量
    M_RESERVATION_QUERY,												// 查询板卡占用状态
    M_RESERVATION_RESERVE,												// 占用板卡
    M_RESERVATION_RELEASE,												// 释放板卡
    M_RESERVATION_RELINQUISH,											// 放弃板卡
    M_RESERVEDBY,														// 获取占用板卡的用户
    M_MODEL,															// 获取板卡名称
    M_INFO,                                                             // 板卡信息
    M_CONFIG,                                                           // 板卡配置信息
    M_STATUS,                                                           // 板卡状态信息
    P_RESERVATION_QUERY,												// 查询端口占用状态
    P_RESERVATION_RESERVE,												// 占用端口
    P_RESERVATION_RELEASE,												// 释放端口
    P_RESERVATION_RELINQUISH,											// 放弃端口
    P_RESERVEDBY,														// 获取占用端口的用户
    P_COMMENT,															// 端口描述
    P_INTERFACE,														// 端口名称
    P_RECEIVESYNC,														// 查询端口同步状态
    P_TRAFFIC_QUERY,													// 查询端口发送状态
    P_TRAFFIC_ON,														// 端口发送数据
    P_TRAFFIC_OFF,														// 端口停止发送数据
    P_TPLDMODE_QUERY,													// 查询端口TPLD模式
    P_TPLDMODE_SET_NORMAL,												// 设置端口TPLD模式为NORMAL模式
    P_TPLDMODE_SET_MIC,													// 设置端口TPLD模式为MIC模式
    P_CHECKSUM_QUERY,													// 端口校验和查询
    P_CHECKSUM_SET,														// 端口校验和设置
    P_MAXHEADERLENGTH_QUERY,											// 端口流头部长度查询
    P_MAXHEADERLENGTH_SET,												// 端口流的头部长度设置
    P_TXTIMELIMIT_QUERY,												// 查询端口发送时间限制
    P_TXTIMELIMIT_SET,													// 设置端口发送时间限制
    P_SPEED,															// 查询端口的实际速率
    P_INFO,                                                             // 端口信息
    P_CONFIG,                                                           // 端口配置信息
    P_FULLCONIFG,                                                       // 端口配置信息(包含流和统计信息)
    P_TXMODE_QUERY,                                                     // 查询端口发送模式
    P_TXMODE_NORMAL,                                                    // 设置端口发送模式为普通模式
    P_TXMODE_STRICUNIFORM,                                              // 设置端口发送模式为Strict
    P_TXMODE_SEQUENTIAL,                                                // 设置端口发送模式为连续模式
    P_RATEFRACTION_QUERY,                                               // 查询端口的发送速率
    P_RATEFRACTION_SET,                                                 // 设置端口的发送速率
    P_RATEPPS_QUERY,                                                    // 查询端口的发送速率
    P_RATEPPS_SET,                                                      // 设置端口的发送速率
    P_RATEL2BPS_QUERY,                                                  // 查询端口的发送速率
    P_RATEL2BPS_SET,                                                    // 设置端口的发送速率
    PS_CREATE,															// 创建一条新数据流
    PS_DELETE,															// 删除一条数据流
    PS_INDECES,															// 可以同时创建多条数据流 同时支持多条流的读取
    PS_DISENABLE,														// 不使能数据流
    PS_ENABLE,															// 使能数据流
    PS_ENABLE_QUERY,													// 流的使能状态查询
    PS_TPLDID,															// 设置流的ID
    PS_COMMENT,															// 数据流描述
    PS_PACKETLIMIT_SET,													// 设置流条数限制
    PS_PACKETLIMIT_QUERY,												// 获取流条数限制
    PS_PACKETHEADER,													// 设置流的Header
    PS_HEADERPROTOCOL,													// 设置流的Header规约
    PS_MODIFIER_INC,													// 设置跳变域为增长
    PS_MODIFIER_DEC,													// 设置跳变域为减少
    PS_MODIFIER_RAM,													// 设置跳变域为随机变动
    PS_MODIFIER_RANGE,                                                  // 设置跳变域的变化范围
    PS_MODIFIERCOUNT,													// 获取跳变域的个数
    PS_MODIFIERCOUNT_SET,                                               // 设置跳变域的个数(每次增加之前要先进行设置)
    PS_RATEFRACTION_SET,												// 设置流的发送速率(百分比)
    PS_RATEFRACTION_QUERY,												// 获取流的发送速率(百分比)
    PS_RATEPPS_SET,														// 设置流的发送速率(帧每秒)
    PS_RATEPPS_QUERY,													// 获取流的发送速率(帧每秒)
    PS_PACKETLENGTH_FIXED,												// 设置固定的报文长度
    PS_CONFIG,                                                          // 获取流的配置信息
    PT_TOTAL                                                            // 统计端口发出的帧
};

typedef QVector<E_ORDER> E_ORDERS;

struct S_MODIFY{
    quint16 modifyIndex;    //跳变域索引
    quint16 pos;            //在哪个位置递变，0->报文的第一个字节
    quint16 minVal;         //变化的最小值
    quint16 maxVal;         //变化的最大值
    quint16 step;           //变化的步长
    quint16 repeat;         //变化循环次数
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
		brief: 指令到QByteArray的转换
		param [in] int type,ORDERSTRUCT order
		param [out]None
		return QByteArray: 指令转换成的数据
		author:MSZ
		date: 2016-05-04
	*/
    QByteArray cmdToByteArray(E_ORDER type,S_ORDER  & order);

    void setPassword(const QString &passwd) { m_password = passwd; }
    void setUserName(const QString &userName) { m_userName = userName; }

public slots:

signals:

private:

    // 机箱命令
    QString loginCommand();												// 登陆
    QString logoffCommand();											// 登出
    QString ownerCommand();												// 设定用户名
    QString keepAlive();												// 保持在线状态
    QString getModelCommand();											// 获取模式
    QString getPortCounts();											// 获取端口数量以及板卡数量
    QString getChassisName();											// 获取机箱名称
    QString getChassisSerialNo();										// 获取机箱序列号
    QString queryChassisReservation();									// 获取机箱占用情况
    QString reserveChassis();											// 占用机箱
    QString releaseChassis();											// 释放机箱
    QString relinquishChassis();										// 放弃机箱
    QString getChassisReservedBy();										// 获取机箱占用者名称
    QString getChassisInfo();                                           // 获取机箱信息
    QString getChassisConifg();                                         // 获取机箱配置信息
    QString getChassisStats();                                          // 获取机箱的状态信息

    // 板卡命令
    QString getModuleCounts();											// 获取板卡数量
    QString composeModuleCommandHead(const S_ORDER & order);			// 组合板卡命令前面相同的部分
    QString queryModuleReservation(const S_ORDER & order);				// 获取板卡占用情况
    QString reserveModule(const S_ORDER & order);						// 占用板卡
    QString releaseModule(const S_ORDER & order);						// 释放板卡
    QString relinquishModule(const S_ORDER & order);					// 放弃板卡
    QString getModuleModel(const S_ORDER & order);						// 获取板卡名称
    QString getModuleReservedBy(const S_ORDER & order);					// 获取板卡占用者名称
    QString getModuleInfo(const S_ORDER & order);                       // 获取板卡信息
    QString getModuleConfig(const S_ORDER & order);                     // 获取板卡配置信息
    QString getModuleStatus(const S_ORDER & order);                     // 获取板卡状态信息

    // 端口命令
    QString composePortCommandHead(const S_ORDER & order);				// 组合端口命令前面相同的部分
    QString queryPortReservation(const S_ORDER & order);				// 端口占用情况
    QString reservePort(const S_ORDER & order);							// 占用端口
    QString releasePort(const S_ORDER & order);							// 释放端口
    QString relinquishPort(const S_ORDER & order);						// 放弃端口
    QString getPortReservedBy(const S_ORDER & order);					// 获取端口占用者名称
    QString getPortDiscription(const S_ORDER & order);					// 获取端口的描述
    QString getPortName(const S_ORDER & order);							// 获取端口名称
    QString getPortSync(const S_ORDER & order);							// 获取端口同步状态
    QString getPortTrafficStatu(const S_ORDER & order);					// 获取端口发送状态
    QString onPortTraffic(const S_ORDER & order);						// 端口发送
    QString offPortTraffic(const S_ORDER & order);						// 端口停止发送
    QString getPortTpldMode(const S_ORDER & order);						// 获取端口TPLD的模式
    QString setPortTpldModeNormal(const S_ORDER & order);				// 设置端口TPLD为普通模式
    QString setPortTpldModeMic(const S_ORDER & order);					// 设置端口TPLD为MIC模式
    QString getPortCheckSum(const S_ORDER & order);						// 获取端口CheckSum值
    QString setPortCheckSum(const S_ORDER & order);						// 设置端口CheckSum值
    QString getPortHeaderLegth(const S_ORDER & order);					// 获取端口Header的长度
    QString setPortHeaderLegth(const S_ORDER & order);					// 设置流的最大报文长度（可编辑部分）
    QString getTxTimeLimit(const S_ORDER & order);						// 获取端口传送时间
    QString setTxTimeLimit(const S_ORDER & order);						// 设置端口传送时间
    QString getPortSpeed(const S_ORDER & order);						// 获取端口的发送速率
    QString getPortInfo(const S_ORDER & order);                         // 获取端口信息
    QString getPortConfig(const S_ORDER & order);                       // 获取端口配置信息
    QString getPortFullConfig(const S_ORDER & order);                   // 获取端口配置信息(包括流和统计的信息)
    QString getTxMode(const S_ORDER & order);                           // 获取端口发送模式
    QString setTxModeNormal(const S_ORDER & order);                     // 设置端口发送模式--normal
    QString setTxModeStrict(const S_ORDER & order);                     // 设置端口发送模式--strict
    QString setTxModeSequential(const S_ORDER & order);                 // 设置端口发送模式--sequential
    QString getPortRateFraction(const S_ORDER & order);                 // 获取端口发送速率--
    QString setPortRateFraction(const S_ORDER & order);                 // 设置端口发送速率
    QString getPortPacketRate(const S_ORDER & order);                   // 获取端口发送速率--帧速率
    QString setPortPacketRate(const S_ORDER & order);                   // 设置端口发送速率
    QString getPortBitRate(const S_ORDER & order);                      // 获取端口发送速率--位速率
    QString setPortBitRate(const S_ORDER & order);                      // 设置端口发送速率

    // 流命令
    QString composeStraeamCommandHead(const S_ORDER & order);			// 组合流命令前面相同的部分
    QString createStream(const S_ORDER & order);						// 创建一条流
    QString deleteStream(const S_ORDER & order);						// 删除一条流
    QString disenableStream(const S_ORDER & order);						// 不使能一条流
    QString enableStream(const S_ORDER & order);						// 使能一条流
    QString getStreamStatus(const S_ORDER & order);						// 获取流的使能状态
    QString setTPLDID(const S_ORDER & order);							// 设置流的ID
    QString setStreamComment(const S_ORDER & order);                    // 设置流的名称
    QString setStreamLimit(const S_ORDER & order);						// 设置流的传输帧长度限制
    QString getStreamLimit(const S_ORDER & order);						// 获取流的传输帧长度限制
    QString setStreamHeader(const S_ORDER & order);						// 设置流的Header
    QString setStreamHeaderProtocol(const S_ORDER & order);				// 设置流的Header的规约 (以太网规约和VLAN)
    QString setModifierPolicy(const QString  & act,const S_ORDER & order);// 设置更改Modify的策略
    QString setStreamIncModifier(const S_ORDER & order);				// Modify为增加
    QString setStreamDecModifier(const S_ORDER & order);				// Modify为减少
    QString setStreamRamModifier(const S_ORDER & order);				// Modify为随机
    QString setStreamModifierRanger(const S_ORDER & order);             // 设置流Modify的变化范围
    QString getStreamModifierCounts(const S_ORDER & order);				// 获取Modify的个数
    QString setStreamModifierCounts(const S_ORDER & order);             // 设置Modify的个数(创建前要先进行设置)
    QString setStreamRate(const S_ORDER & order);						// 设置流的速率(百分比)
    QString getStreamRate(const S_ORDER & order);						// 获取流的速率(百分比)
    QString setStreamRatePacketPerSec(const S_ORDER & order);			// 设置流的速率(帧每秒)
    QString getStreamRatePacketPerSec(const S_ORDER & order);			// 获取流的速率(帧每秒)
    QString setStreamFixedPackedLegth(const S_ORDER & order);			// 设置流的固定报文长度
    QString getStreamConfig(const S_ORDER & order);                     // 获取流的配置信息

    // 统计命令
    QString getPortTotal(const S_ORDER & order);                        // 统计端口发送的数据

private:
    QString m_password;
    QString m_userName;
};

#endif
