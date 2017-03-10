#include "scriptcommand.h"
#include "XenaManager/src/XenaManageSettings/xenamanagesettings.h"

ScriptCommand::ScriptCommand(QObject *parent) :
QObject(parent),
m_userName("XT"),
m_password("xena")
{

}

QByteArray ScriptCommand::cmdToByteArray(E_ORDER type,S_ORDER & order)
{
	QString command;
	switch (type)
	{
	case C_LOGON:
		command = loginCommand();
		break;
	case C_LOGOFF:
		command = logoffCommand();
		break;
	case C_OWNER:
		command = ownerCommand();
		break;
	case C_KEEPALIVE:
		command = keepAlive();
		break;
	case C_MODEL:
		command = getModelCommand();
		break;
	case C_PORTCOUNTS:
		command = getPortCounts();
		break;
	case C_NAME:
		command = getChassisName();
		break;
	case C_SERIALNO:
		command = getChassisSerialNo();
		break;
	case C_RESERVATION_QUERY:
		command = queryChassisReservation();
		break;
	case C_RESERVATION_RESERVE:
		command = reserveChassis();
		break;
	case C_RESERVATION_RELEASE:
		command = releaseChassis();
		break;
	case C_RESERVATION_RELINQUISH:
		command = relinquishChassis();
		break;
	case C_RESERVEDBY:
		command = getChassisReservedBy();
		break;
	case M_MODULE_COUNTS:
		command = getModuleCounts();
		break;
	case M_RESERVATION_QUERY:
		command = queryModuleReservation(order);
		break;
	case M_RESERVATION_RESERVE:
		command = reserveModule(order);
		break;
	case M_RESERVATION_RELEASE:
		command = releaseModule(order);
		break;
	case M_RESERVATION_RELINQUISH:
		command = relinquishModule(order);
		break;
	case M_RESERVEDBY:
		command = getModuleReservedBy(order);
		break;
	case M_MODEL:
		command = getModuleModel(order);
		break;
	case P_RESERVATION_QUERY:
		command = queryPortReservation(order);
		break;
	case P_RESERVATION_RESERVE:
		command = reservePort(order);
		break;
	case P_RESERVATION_RELEASE:
		command = releasePort(order);
		break;
	case P_RESERVATION_RELINQUISH:
		command = relinquishPort(order);
		break;
	case P_RESERVEDBY:
		command = getPortReservedBy(order);
		break;
	case P_COMMENT:
		command = getPortDiscription(order);
		break;
	case P_INTERFACE:
		command = getPortName(order);
		break;
	case P_RECEIVESYNC:
		command = getPortSync(order);
		break;
	case P_TRAFFIC_QUERY:
		command = getPortTrafficStatu(order);
		break;
	case P_TRAFFIC_ON:
		command = onPortTraffic(order);
		break;
	case P_TRAFFIC_OFF:
		command = offPortTraffic(order);
		break;
	case P_TPLDMODE_QUERY:
		command = getPortTpldMode(order);
		break;
	case P_TPLDMODE_SET_NORMAL:
		command = setPortTpldModeNormal(order);
		break;
	case P_TPLDMODE_SET_MIC:
		command = setPortTpldModeMic(order);
		break;
	case P_CHECKSUM_QUERY:
		command = getPortCheckSum(order);
		break;
	case P_CHECKSUM_SET:
		command = setPortCheckSum(order);
		break;
	case P_MAXHEADERLENGTH_QUERY:
		command = getPortHeaderLegth(order);
		break;
	case P_MAXHEADERLENGTH_SET:
		command = setPortHeaderLegth(order);
		break;
	case P_TXTIMELIMIT_QUERY:
		command = getTxTimeLimit(order);
		break;
	case P_TXTIMELIMIT_SET:
		command = setTxTimeLimit(order);
		break;
	case P_SPEED:
		command = getPortSpeed(order);
		break;
	case PS_CREATE:
		command = createStream(order);
		break;
	case PS_DELETE:
		command = deleteStream(order);
		break;
	case PS_DISENABLE:
		command = disenableStream(order);
		break;
	case PS_ENABLE:
		command = enableStream(order);
		break;
	case PS_ENABLE_QUERY:
		command = getStreamStatus(order);
		break;
	case PS_TPLDID:
		command = setTPLDID(order);
		break;
	case PS_PACKETLIMIT_SET:
		command = setStreamLimit(order);
		break;
	case PS_PACKETLIMIT_QUERY:
		command = getStreamLimit(order);
		break;
	case PS_PACKETHEADER:
		command = setStreamHeader(order);
		break;
	case PS_HEADERPROTOCOL:
		command = setStreamHeaderProtocol(order);
		break;
	case PS_MODIFIER_INC:
		command = setStreamIncModifier(order);
		break;
	case PS_MODIFIER_DEC:
		command = setStreamDecModifier(order);
		break;
	case PS_MODIFIER_RAM:
		command = setStreamRamModifier(order);
		break;
    case PS_MODIFIER_RANGE:
        command = setStreamModifierRanger(order);
        break;
	case PS_MODIFIERCOUNT:
		command = getStreamModifierCounts(order);
		break;
    case PS_MODIFIERCOUNT_SET:
        command = setStreamModifierCounts(order);
        break;
	case PS_RATEFRACTION_SET:
		command = setStreamRate(order);
		break;
	case PS_RATEFRACTION_QUERY:
		command = getStreamRate(order);
		break;
	case PS_RATEPPS_SET:
		command = setStreamRatePacketPerSec(order);
		break;
	case PS_RATEPPS_QUERY:
		command = getStreamRatePacketPerSec(order);
		break;
	case PS_PACKETLENGTH_FIXED:
		command = setStreamFixedPackedLegth(order);
		break;
    case PT_TOTAL:
        command = getPortTotal(order);
        break;
	default:
		break;
	}
    command += "\r\n";
	return QByteArray(command.toLatin1());
}


QString ScriptCommand::loginCommand()
{
	QString command("C_LOGON \"");
	command += m_password;
	command += "\"";
	return command;
}

QString ScriptCommand::logoffCommand()
{
	QString command("C_LOGOFF");
	return command;
}

QString ScriptCommand::ownerCommand()
{
	QString command("C_OWNER \"");
	command += m_userName;
	command += "\"";
	return command;
}

QString ScriptCommand::keepAlive()
{
	QString command("C_KEEPALIVE ?");
	return command;
}

QString ScriptCommand::getModelCommand()
{
	QString command("C_MODEL ?");
	return command;
}

QString ScriptCommand::getPortCounts()
{
	QString command("C_PORTCOUNTS ?");
	return command;
}

QString ScriptCommand::getChassisName()
{
	QString command("C_NAME ?");
	return command;
}

QString ScriptCommand::getChassisSerialNo()
{
	QString command("C_SERIALNO ?");
	return command;
}

QString ScriptCommand::queryChassisReservation()
{
	QString command("C_RESERVATION ?");
	return command;
}

QString ScriptCommand::reserveChassis()
{
	QString command("C_RESERVATION RESERVE");
	return command;
}

QString ScriptCommand::releaseChassis()
{
	QString command("C_RESERVATION RELEASE");
	return command;
}

QString ScriptCommand::relinquishChassis()
{
	QString command("C_RESERVATION RELINQUISH");
	return command;
}

QString ScriptCommand::getChassisReservedBy()
{
	QString command("C_RESERVEDBY ?");
	return command;
}

QString ScriptCommand::getModuleCounts()
{
	QString command("* M_MODEL ?");
	return command;
}

QString ScriptCommand::composeModuleCommandHead(const S_ORDER & order)
{
	QString command = QString::number(order.moduleIndex);
	return command;
}
QString ScriptCommand::queryModuleReservation(const S_ORDER & order)
{
	QString command = composeModuleCommandHead(order);
	command += " M_RESERVATION ?";
	return command;
}

QString ScriptCommand::reserveModule(const S_ORDER & order)
{
	QString command = composeModuleCommandHead(order);
	command += " M_RESERVATION RESERVE";
	return command;
}

QString ScriptCommand::releaseModule(const S_ORDER & order)
{
	QString command = composeModuleCommandHead(order);
	command += " M_RESERVATION RELEASE";
	return command;
}

QString ScriptCommand::relinquishModule(const S_ORDER & order)
{
	QString command = composeModuleCommandHead(order);
	command += " M_RESERVATION RELINQUISH";
	return command;
}

QString ScriptCommand::getModuleModel(const S_ORDER & order)
{
	QString command = composeModuleCommandHead(order);
	command += " M_MODEL ?";
	return command;
}

QString ScriptCommand::getModuleReservedBy(const S_ORDER & order)
{
	QString command = composeModuleCommandHead(order);
	command += " M_RESERVEDBY ?";
	return command;
}

QString ScriptCommand::composePortCommandHead(const S_ORDER & order)
{
	QString command = QString::number(order.moduleIndex);
	command += "/";
	command += QString::number(order.portIndex);
	return command;
}

QString ScriptCommand::queryPortReservation(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_RESERVATION ?";
	return command;
}

QString ScriptCommand::reservePort(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_RESERVATION RESERVE";
	return command;
}

QString ScriptCommand::releasePort(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_RESERVATION RELEASE";
	return command;
}

QString ScriptCommand::relinquishPort(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_RESERVATION RELINQUISH";
	return command;
}

QString ScriptCommand::getPortReservedBy(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_RESERVEDBY ?";
	return command;
}

QString ScriptCommand::getPortDiscription(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_COMMENT ?";
	return command;
}

QString ScriptCommand::getPortName(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_INTERFACE ?";
	return command;
}

QString ScriptCommand::getPortSync(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_RECEIVESYNC ?";
	return command;
}

QString ScriptCommand::getPortTrafficStatu(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_TRAFFIC ?";
	return command;
}

QString ScriptCommand::onPortTraffic(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_TRAFFIC ON";
	return command;
}

QString ScriptCommand::offPortTraffic(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_TRAFFIC OFF";
	return command;
}

QString ScriptCommand::getPortTpldMode(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_TPLDMODE ?";
	return command;
}

QString ScriptCommand::setPortTpldModeNormal(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_TPLDMODE NORMAL";
	return command;
}

QString ScriptCommand::setPortTpldModeMic(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_TPLDMODE MICRO";
	return command;
}

QString ScriptCommand::getPortCheckSum(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_CHECKSUM ?";
	return command;
}

QString ScriptCommand::setPortCheckSum(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_CHECKSUM ";
	command += order.data;
	return command;
}

QString ScriptCommand::getPortHeaderLegth(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_MAXHEADERLENGTH ?";
	return command;
}

QString ScriptCommand::setPortHeaderLegth(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_MAXHEADERLENGTH ";
	command += order.data;
	return command;
}

QString ScriptCommand::getTxTimeLimit(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_TXTIMELIMIT ?";
	return command;
}

QString ScriptCommand::setTxTimeLimit(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_TXTIMELIMIT ";
	command += order.data;
	return command;
}

QString ScriptCommand::getPortSpeed(const S_ORDER & order)
{
	QString command = composePortCommandHead(order);
	command += " P_SPEED ?";
	return command;
}

QString ScriptCommand::composeStraeamCommandHead(const S_ORDER & order)
{
	QString command = QString::number(order.moduleIndex);
	command += "/";
	command += QString::number(order.portIndex);
	return command;
}

QString ScriptCommand::createStream(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_CREATE [";
	command += QString::number(order.streamindex);
	command += "]";
	return command;
}

QString ScriptCommand::deleteStream(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_DELETE [";
	command += QString::number(order.streamindex);
	command += "]";
	return command;
}

QString ScriptCommand::disenableStream(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_ENABLE [";
	command += QString::number(order.streamindex);
	command += "] OFF";
	return command;
}

QString ScriptCommand::enableStream(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_ENABLE [";
	command += QString::number(order.streamindex);
	command += "] ON";
	return command;
}

QString ScriptCommand::getStreamStatus(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_ENABLE [";
	command += QString::number(order.streamindex);
	command += "] ?";
	return command;
}

QString ScriptCommand::setTPLDID(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_TPLDID [";
	command += QString::number(order.streamindex);
	command += "] ";
	command += order.data;
	return command;
}

QString ScriptCommand::setStreamLimit(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_PACKETLIMIT [";
	command += QString::number(order.streamindex);
	command += "]";
    command += order.data;
	return command;
}

QString ScriptCommand::getStreamLimit(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_PACKETLIMIT [";
	command += QString::number(order.streamindex);
	command += "] ?";
	return command;
}

QString ScriptCommand::setStreamHeader(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_PACKETHEADER [";
	command += QString::number(order.streamindex);
	command += "] 0x";
	command += order.data;
	return command;
}

QString ScriptCommand::setStreamHeaderProtocol(const S_ORDER & order)
{
    int segmentNum = order.data.toUInt() / 64;
    int lastSegmentLength = 256 - order.data.toUInt() % 64;
    QString command = composeStraeamCommandHead(order);
    command += " PS_HEADERPROTOCOL [";
    command += QString::number(order.streamindex);
    command += "] ETHERNET VLAN";
    for (int i = 0;i < segmentNum;i++)
    {
        command += " 192 ";
    }
    if (lastSegmentLength > 0 && 256 != lastSegmentLength)
    {
        command += QString::number(lastSegmentLength);
    }
    return command;
}

QString ScriptCommand::setModifierPolicy(const QString & act,const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_MODIFIER [";
	command += QString::number(order.streamindex);
	command += ",";
	command += QString::number(order.modify.modifyIndex);
	command += "] ";
	command += QString::number(order.modify.pos);
	command += " 0xFFFF0000";                                                // mask设置为默认的FFFF
	command += act;
	command += QString::number(order.modify.repeat); 
	return command;
}

QString ScriptCommand::setStreamIncModifier(const S_ORDER & order)
{
	QString command = setModifierPolicy(" INC ",order);
	return command;
}

QString ScriptCommand::setStreamDecModifier(const S_ORDER & order)
{
	QString command = setModifierPolicy(" DEC ",order);
	return command;
}

QString ScriptCommand::setStreamRamModifier(const S_ORDER & order)
{
	QString command = setModifierPolicy(" RANDOM ",order);
	return command;
}

QString ScriptCommand::setStreamModifierRanger(const S_ORDER & order)
{
    QString command = composeStraeamCommandHead(order);
    command += " PS_MODIFIERRANGE [";
    command += QString::number(order.streamindex);
    command += ",";
    command += QString::number(order.modify.modifyIndex);
    command += "] ";
    command += QString::number(order.modify.minVal);
    command += " ";
    command += QString::number(order.modify.step);
    command += " ";
    command += QString::number(order.modify.maxVal);
    return command;
}

QString ScriptCommand::getStreamModifierCounts(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_MODIFIERCOUNT [";
	command += QString::number(order.streamindex);
	command += "] ?";
	return command;
}

QString ScriptCommand::setStreamModifierCounts(const S_ORDER & order)
{
    QString command = composeStraeamCommandHead(order);
    command += " PS_MODIFIERCOUNT [";
    command += QString::number(order.streamindex);
    command += "] ";
    command += order.data;
    return command;
}

QString ScriptCommand::setStreamRate(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_RATEFRACTION [";
	command += QString::number(order.streamindex);
	command += "] ";
	command += order.data;
	return command;
}

QString ScriptCommand::getStreamRate(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_RATEFRACTION [";
	command += QString::number(order.streamindex);
	command += "] ?";
	return command;
}

QString ScriptCommand::setStreamRatePacketPerSec(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_RATEPPS [";
	command += QString::number(order.streamindex);
	command += "] ";
	command += order.data;
	return command;
}

QString ScriptCommand::getStreamRatePacketPerSec(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_RATEPPS [";
	command += QString::number(order.streamindex);
	command += "] ";
	return command;
}

QString ScriptCommand::setStreamFixedPackedLegth(const S_ORDER & order)
{
	QString command = composeStraeamCommandHead(order);
	command += " PS_PACKETLENGTH [";
	command += QString::number(order.streamindex);
	command += "] FIXED ";
	command += order.data;
	command += " 1518";
	return command;
}

QString ScriptCommand::getPortTotal(const S_ORDER & order)
{
    QString command = composePortCommandHead(order);
    command += " PT_TOTAL ?";
    return command;
}