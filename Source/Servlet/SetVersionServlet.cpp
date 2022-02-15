#include "Servlet/SetVersionServlet.h"
#include <RapidJson/writer.h>
#include <RapidJson/document.h>
#include <RapidJson/stringbuffer.h>

SetVersionServlet::SetVersionServlet(const Safe<DataBaseManager>& dataBaseManager)
    :Magic::NetWork::Http::IHttpServlet("/api/version/setVersion")
    ,m_DataBaseManager(dataBaseManager){
}

bool SetVersionServlet::handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket,
                              const Safe<Magic::NetWork::Http::HttpRequest>& request,
                              const Safe<Magic::NetWork::Http::HttpResponse>& response){
    rapidjson::Document doc;
    response->setStatus(Magic::NetWork::Http::HttpStatus::OK);
    if(doc.Parse(request->getBody().c_str()).HasParseError()){
        response->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed!\",\"data\":{}}");
        httpSocket->sendResponse(response);
        return true;
    }


    std::string updateVersion;
    std::string currentVersion;

    if(doc.HasMember("updateVersion") && doc["updateVersion"].IsString()){
        updateVersion = doc["updateVersion"].GetString();
    }

    if(doc.HasMember("currentVersion") && doc["currentVersion"].IsString()){
        currentVersion = doc["currentVersion"].GetString();
    }

    if(updateVersion.empty() || currentVersion.empty()){
        response->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed! Missing Key Value.\",\"data\":{}}");
        httpSocket->sendResponse(response);
        return true;
    }

    if(m_DataBaseManager->updateFromVersionByType(currentVersion,updateVersion)){
        response->setBody("{\"return_code\":1,\"return_msg\":\"Succeed\",\"data\":{}}");
    }else{
        response->setBody("{\"return_code\":0,\"return_msg\":\"Failure\",\"data\":{}}");
    }
    httpSocket->sendResponse(response);
    return true;
}

