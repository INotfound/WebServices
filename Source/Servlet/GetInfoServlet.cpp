#include <future>
#include "Servlet/GetInfoServlet.h"
#include <RapidJson/writer.h>
#include <RapidJson/stringbuffer.h>

GetInfoServlet::GetInfoServlet(const Safe<DataBaseManager>& dataBaseManager)
    :m_DataBaseManager(dataBaseManager){
}

void GetInfoServlet::handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket){
    httpSocket->getResponse()->setStatus(Magic::NetWork::Http::HttpStatus::OK);
    std::vector<DeviceInfo> deviceInfos;
    std::vector<VersionInfo> versionInfos;
    m_DataBaseManager->queryFromDeviceByAll(deviceInfos);
    m_DataBaseManager->queryFromVersionByAll(versionInfos);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.Key("return_code");
    writer.Int(1);

    writer.Key("Versions");
    writer.StartArray();
    for(auto&v : versionInfos){
        writer.StartObject();
        writer.Key("Type");
        writer.Bool(v.m_Type);
        writer.Key("Url");
        writer.String(v.m_Url.c_str());
        writer.Key("Md5");
        writer.String(v.m_Md5.c_str());
        writer.Key("Version");
        writer.String(v.m_Version.c_str());
        writer.EndObject();
    }
    writer.EndArray();


    writer.Key("Devices");
    writer.StartArray();
    for(auto&v : deviceInfos){
        writer.StartObject();
        writer.Key("Sn");
        writer.String(v.m_Sn.c_str());
        writer.Key("Mac");
        writer.String(v.m_Mac.c_str());
        writer.Key("RoomId");
        writer.String(v.m_RoomId.c_str());
        writer.Key("StoreId");
        writer.String(v.m_StoreId.c_str());
        writer.Key("Version");
        writer.String(v.m_Version.c_str());
        writer.Key("RoomName");
        writer.String(v.m_RoomName.c_str());
        writer.Key("UpdateVersion");
        writer.String(v.m_UpdateVersion.c_str());
        writer.EndObject();
    }
    writer.EndArray();


    writer.EndObject();
    httpSocket->getResponse()->setBody(buffer.GetString());
    httpSocket->sendResponse(httpSocket->getResponse());
}

