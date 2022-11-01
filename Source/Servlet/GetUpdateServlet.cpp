#include "Servlet/GetUpdateServlet.h"
#include <RapidJson/writer.h>
#include <RapidJson/document.h>
#include <RapidJson/stringbuffer.h>

GetUpdateServlet::GetUpdateServlet(const Safe<DataBaseManager>& dataBaseManager)
    :m_DataBaseManager(dataBaseManager){
}

void GetUpdateServlet::handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket){
    rapidjson::Document doc;
    httpSocket->getResponse()->setStatus(Magic::NetWork::Http::HttpStatus::OK);
    if(doc.Parse(httpSocket->getRequest()->getBody().c_str()).HasParseError()){
        httpSocket->getResponse()->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed!\",\"data\":{}}");
        httpSocket->sendResponse(httpSocket->getResponse());
        return;
    }
    Safe<DeviceInfo> deviceInfo = std::make_shared<DeviceInfo>();

    if(doc.HasMember("sn") && doc["sn"].IsString()){
        deviceInfo->m_Sn = doc["sn"].GetString();
    }

    if(doc.HasMember("mac") && doc["mac"].IsString()){
        deviceInfo->m_Mac = doc["mac"].GetString();
    }

    if(doc.HasMember("room_id") && doc["room_id"].IsString()){
        deviceInfo->m_RoomId = doc["room_id"].GetString();
    }

    if(doc.HasMember("version") && doc["version"].IsString()){
        deviceInfo->m_Version = doc["version"].GetString();
    }

    if(doc.HasMember("store_id") && doc["store_id"].IsString()){
        deviceInfo->m_StoreId = doc["store_id"].GetString();
    }

    if(doc.HasMember("room_name") && doc["room_name"].IsString()){
        deviceInfo->m_RoomName = doc["room_name"].GetString();
    }

    if(deviceInfo->m_Mac.empty() || deviceInfo->m_Version.empty()){
        httpSocket->getResponse()->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed!\",\"data\":{}}");
        httpSocket->sendResponse(httpSocket->getResponse());
        return;
    }
    std::string updateVersion;

    Safe<DeviceInfo> deviceInfoResult = m_DataBaseManager->queryFromDeviceByMac(deviceInfo->m_Mac);
    if(deviceInfoResult){
        if(deviceInfo->m_Sn.empty() && !deviceInfoResult->m_Sn.empty()){
            deviceInfo->m_Sn = deviceInfoResult->m_Sn;
        }
        if(deviceInfo->m_Mac.empty() && !deviceInfoResult->m_Mac.empty()){
            deviceInfo->m_Mac = deviceInfoResult->m_Mac;
        }
        if(deviceInfo->m_RoomId.empty() && !deviceInfoResult->m_RoomId.empty()){
            deviceInfo->m_RoomId = deviceInfoResult->m_RoomId;
        }
        if(deviceInfo->m_StoreId.empty() && !deviceInfoResult->m_StoreId.empty()){
            deviceInfo->m_StoreId = deviceInfoResult->m_StoreId;
        }
        if(deviceInfo->m_Version.empty() && !deviceInfoResult->m_Version.empty()){
            deviceInfo->m_Version = deviceInfoResult->m_Version;
        }
        if(deviceInfo->m_RoomName.empty() && !deviceInfoResult->m_RoomName.empty()){
            deviceInfo->m_RoomName = deviceInfoResult->m_RoomName;
        }

        /// 数据库指定升级的目标版本
        updateVersion = deviceInfoResult->m_UpdateVersion;

        m_DataBaseManager->flushFromDeviceByMac(deviceInfo->m_Mac,deviceInfo);
    }else{
        m_DataBaseManager->insertFromDeviceByMac(deviceInfo->m_Mac,deviceInfo);
    }


    Safe<VersionInfo> versionInfo;
    if(updateVersion.empty()){
        Safe<VersionInfo> normalVersionInfo = m_DataBaseManager->queryFromVersionByNewNormal();
        if(normalVersionInfo && deviceInfo->m_Version != normalVersionInfo->m_Version){
            versionInfo = std::move(normalVersionInfo);
        }else{
            httpSocket->getResponse()->setBody("{\"return_code\":1,\"return_msg\":\"Ok\",\"data\":{}}");
            httpSocket->sendResponse(httpSocket->getResponse());
            return;
        }
    }else{
        if(updateVersion != deviceInfo->m_Version){
            versionInfo = m_DataBaseManager->queryFromVersionByVersion(updateVersion);
        }else{
            httpSocket->getResponse()->setBody("{\"return_code\":1,\"return_msg\":\"Ok\",\"data\":{}}");
            httpSocket->sendResponse(httpSocket->getResponse());
            return;
        }
    }

    if(versionInfo){
        rapidjson::StringBuffer stringBuffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
        writer.StartObject();
        writer.Key("return_code");
        writer.Int(1);
        writer.Key("return_msg");
        writer.String("OK");
        writer.Key("data");
        writer.StartObject();
        writer.Key("force_update");
        writer.Bool(true);
        writer.Key("url");
        writer.String(versionInfo->m_Url.c_str());
        writer.Key("md5");
        writer.String(versionInfo->m_Md5.c_str());
        writer.Key("version");
        writer.String(versionInfo->m_Version.c_str());
        writer.EndObject();
        writer.EndObject();
        httpSocket->getResponse()->setBody(stringBuffer.GetString());
    }else{
        httpSocket->getResponse()->setBody("{\"return_code\":1,\"return_msg\":\"Ok\",\"data\":{}}");
    }
    httpSocket->sendResponse(httpSocket->getResponse());
    return;
}

