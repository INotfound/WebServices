#include "Servlet/AddVersionServlet.h"
#include <RapidJson/writer.h>
#include <RapidJson/document.h>
#include <RapidJson/stringbuffer.h>

AddVersionServlet::AddVersionServlet(const Safe<DataBaseManager>& dataBaseManager)
    :Magic::NetWork::Http::IHttpServlet("/api/version/addVersion")
    ,m_DataBaseManager(dataBaseManager){
}

bool AddVersionServlet::handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket,
                              const Safe<Magic::NetWork::Http::HttpRequest>& request,
                              const Safe<Magic::NetWork::Http::HttpResponse>& response){
    rapidjson::Document doc;
    response->setStatus(Magic::NetWork::Http::HttpStatus::OK);
    if(doc.Parse(request->getBody().c_str()).HasParseError()){
        response->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed!\",\"data\":{}}");
        httpSocket->sendResponse(response);
        return true;
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
        response->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed! Missing Key Value.\",\"data\":{}}");
        httpSocket->sendResponse(response);
        return true;
    }

    if(m_DataBaseManager->insertFromVersionByNew(info)){
        response->setBody("{\"return_code\":1,\"return_msg\":\"Succeed\",\"data\":{}}");
    }else{
        response->setBody("{\"return_code\":0,\"return_msg\":\"Failure\",\"data\":{}}");
    }
    httpSocket->sendResponse(response);
    return true;
}

