#include "Servlet/SetUpdateServlet.h"
#include <RapidJson/writer.h>
#include <RapidJson/document.h>
#include <RapidJson/stringbuffer.h>

SetUpdateServlet::SetUpdateServlet(const Safe<DataBaseManager>& dataBaseManager)
    :Magic::NetWork::Http::IHttpServlet("/api/version/setUpdate")
    ,m_DataBaseManager(dataBaseManager){
}

bool SetUpdateServlet::handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket,
                              const Safe<Magic::NetWork::Http::HttpRequest>& request,
                              const Safe<Magic::NetWork::Http::HttpResponse>& response){
    rapidjson::Document doc;
    response->setStatus(Magic::NetWork::Http::HttpStatus::OK);
    if(doc.Parse(request->getBody().c_str()).HasParseError()){
        response->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed!\",\"data\":{}}");
        httpSocket->sendResponse(response);
        return true;
    }

    std::string sn;
    std::string mac;
    std::string storeId;
    std::string updateVersion;

    if(doc.HasMember("sn") && doc["sn"].IsString()){
        sn = doc["sn"].GetString();
    }

    if(doc.HasMember("mac") && doc["mac"].IsString()){
        mac = doc["mac"].GetString();
    }

    if(doc.HasMember("storeId") && doc["storeId"].IsString()){
        storeId = doc["storeId"].GetString();
    }

    if(doc.HasMember("updateVersion") && doc["updateVersion"].IsString()){
        updateVersion = doc["updateVersion"].GetString();
    }

    if(updateVersion.empty()){
        response->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed! Missing Key Value.\",\"data\":{}}");
        httpSocket->sendResponse(response);
        return true;
    }

    if(!mac.empty()){
        if(m_DataBaseManager->updateFromDeviceByMac(mac,updateVersion)){
            response->setBody("{\"return_code\":1,\"return_msg\":\"Succeed\",\"data\":{}}");
            httpSocket->sendResponse(response);
            return true;
        }
    }else if(!storeId.empty()){
        if(sn.empty()){
            if(m_DataBaseManager->updateFromDeviceByStoreId(storeId,updateVersion)){
                response->setBody("{\"return_code\":1,\"return_msg\":\"Succeed\",\"data\":{}}");
                httpSocket->sendResponse(response);
                return true;
            }
        }else{
            if(m_DataBaseManager->updateFromDeviceByStoreIdAndSn(sn,storeId,updateVersion)){
                response->setBody("{\"return_code\":1,\"return_msg\":\"Succeed\",\"data\":{}}");
                httpSocket->sendResponse(response);
                return true;
            }
        }
    }
    response->setBody("{\"return_code\":0,\"return_msg\":\"Failure\",\"data\":{}}");
    httpSocket->sendResponse(response);
    return true;
}

