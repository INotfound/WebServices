#include "Servlet/AddVersionServlet.h"
#include <RapidJson/writer.h>
#include <RapidJson/document.h>
#include <RapidJson/stringbuffer.h>

AddVersionServlet::AddVersionServlet(const Safe<DataBaseManager>& dataBaseManager)
    :m_DataBaseManager(dataBaseManager){
}

void AddVersionServlet::handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket){
    rapidjson::Document doc;
    httpSocket->getResponse()->setStatus(Magic::NetWork::Http::HttpStatus::OK);
    if(doc.Parse(httpSocket->getRequest()->getBody().c_str()).HasParseError()){
        httpSocket->getResponse()->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed!\",\"data\":{}}");
        httpSocket->sendResponse(httpSocket->getResponse());
        return;
    }

    Safe<VersionInfo> info = std::make_shared<VersionInfo>();
    info->m_Type = false;

    if(doc.HasMember("url") && doc["url"].IsString()){
        info->m_Url = doc["url"].GetString();
    }

    if(doc.HasMember("md5") && doc["md5"].IsString()){
        info->m_Md5 = doc["md5"].GetString();
    }

    if(doc.HasMember("type") && doc["type"].IsBool()){
        info->m_Type = doc["type"].GetBool();
    }

    if(doc.HasMember("version") && doc["version"].IsString()){
        info->m_Version = doc["version"].GetString();
    }

    if(info->m_Url.empty() || info->m_Md5.empty() || info->m_Version.empty()){
        httpSocket->getResponse()->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed! Missing Key Value.\",\"data\":{}}");
        httpSocket->sendResponse(httpSocket->getResponse());
        return;
    }

    if(m_DataBaseManager->insertFromVersionByNew(info)){
        httpSocket->getResponse()->setBody("{\"return_code\":1,\"return_msg\":\"Succeed\",\"data\":{}}");
    }else{
        httpSocket->getResponse()->setBody("{\"return_code\":0,\"return_msg\":\"Failure\",\"data\":{}}");
    }
    httpSocket->sendResponse(httpSocket->getResponse());
    return;
}

