#include <QRegExp>
#include "plugin_removesig.h"
#include "bunny.h"
#include "log.h"
#include "messagepacket.h"
#include "packet.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_removesig, PluginRemoveSig)

PluginRemoveSig::PluginRemoveSig():PluginInterface("removesig", "Removes signature from ambient message")
{
}

bool PluginRemoveSig::XmppVioletPacketMessage(Bunny * b, Packet const& p)
{
	QRegExp rx1("MC broadcast/[a-z_/]*signature[a-z_/]*.mp3\nCL \\d+\nCH broadcast/[a-z_/]*\\.chor\nMW\n");
	bool signature = b->GetPluginSetting(GetName(), "Remove/Signature", true).toBool();

	QRegExp rx2("MC broadcast/[a-z_/]*/respiration/\\d+Respiration\\d+\\.mp3\nMW\n");
	bool respiration = b->GetPluginSetting(GetName(), "Remove/Respiration", true).toBool();

	if(
		(QString(p.GetPrintableData()).indexOf(rx1) != -1 && signature) ||
		(QString(p.GetPrintableData()).indexOf(rx2) != -1 && respiration)
		)
	{
		QString packet = p.GetPrintableData();
		LogDebug(QString("Before removing signature :\n%1").arg(packet));
		if(signature)
			packet = packet.remove(rx1);
		if(respiration)
			packet = packet.remove(rx2);
		LogDebug(QString("After removing signature :\n%1").arg(packet));
		b->SendPacket(MessagePacket(packet.toAscii()));
		return true;
	}
	return false;
}

PluginRemoveSig::~PluginRemoveSig() {}

void PluginRemoveSig::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("signature", PluginRemoveSig, Api_Signature);
	DECLARE_PLUGIN_BUNNY_API_CALL("respiration", PluginRemoveSig, Api_Respiration);
}

PLUGIN_BUNNY_API_CALL(PluginRemoveSig::Api_Signature)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("remove"))
		return new ApiManager::ApiError(QString("Missing argument 'remove' for plugin Remove signature"));
	bunny->SetPluginSetting(GetName(), "Remove/Signature", QVariant(hRequest.GetArg("remove")));
	return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}

PLUGIN_BUNNY_API_CALL(PluginRemoveSig::Api_Respiration)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("remove"))
		return new ApiManager::ApiError(QString("Missing argument 'remove' for plugin Remove signature"));
	bunny->SetPluginSetting(GetName(), "Remove/Respiration", QVariant(hRequest.GetArg("remove")));
	return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}
