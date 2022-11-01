#include "Servlet/SetVersionServlet.h"
#include <RapidJson/writer.h>
#include <RapidJson/document.h>
#include <RapidJson/stringbuffer.h>

SetVersionServlet::SetVersionServlet(const Safe<DataBaseManager>& dataBaseManager)
    :m_DataBaseManager(dataBaseManager){
}

void SetVersionServlet::handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket){
    rapidjson::Document doc;
    httpSocket->getResponse()->setStatus(Magic::NetWork::Http::HttpStatus::OK);
    if(doc.Parse(httpSocket->getRequest()->getBody().c_str()).HasParseError()){
        httpSocket->getResponse()->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed!\",\"data\":{}}");
        httpSocket->sendResponse(httpSocket->getResponse());
        return;
    }


    std::string updateVersion;

    if(doc.HasMember("updateVersion") && doc["updateVersion"].IsString()){
        updateVersion = doc["updateVersion"].GetString();
    }


    if(updateVersion.empty()){
        httpSocket->getResponse()->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed! Missing Key Value.\",\"data\":{}}");
        httpSocket->sendResponse(httpSocket->getResponse());
        return;
    }

    if(m_DataBaseManager->updateFromVersionByType(updateVersion)){
        httpSocket->getResponse()->setBody("{\"return_code\":1,\"return_msg\":\"Succeed\",\"data\":{}}");
    }else{
        httpSocket->getResponse()->setBody("{\"return_code\":0,\"return_msg\":\"Failure\",\"data\":{}}");
    }
    httpSocket->sendResponse(httpSocket->getResponse());
    return;
}

