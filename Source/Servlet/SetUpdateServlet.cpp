#include "Servlet/SetUpdateServlet.h"
#include <RapidJson/writer.h>
#include <RapidJson/document.h>
#include <RapidJson/stringbuffer.h>

SetUpdateServlet::SetUpdateServlet(const Safe<DataBaseManager>& dataBaseManager)
    :m_DataBaseManager(dataBaseManager){
}

void SetUpdateServlet::handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket){
    rapidjson::Document doc;
    httpSocket->getResponse()->setStatus(Magic::NetWork::Http::HttpStatus::OK);
    if(doc.Parse(httpSocket->getRequest()->getBody().c_str()).HasParseError()){
        httpSocket->getResponse()->setBody("{\"return_code\":0,\"return_msg\":\"Request Json Parse Failed!\",\"data\":{}}");
        httpSocket->sendResponse(httpSocket->getResponse());
        return;
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

    if(!mac.empty()){
        if(m_DataBaseManager->updateFromDeviceByMac(mac,updateVersion)){
            httpSocket->getResponse()->setBody("{\"return_code\":1,\"return_msg\":\"Succeed\",\"data\":{}}");
            httpSocket->sendResponse(httpSocket->getResponse());
            return;
        }
    }else if(!storeId.empty()){
        if(sn.empty()){
            if(m_DataBaseManager->updateFromDeviceByStoreId(storeId,updateVersion)){
                httpSocket->getResponse()->setBody("{\"return_code\":1,\"return_msg\":\"Succeed\",\"data\":{}}");
                httpSocket->sendResponse(httpSocket->getResponse());
                return;
            }
        }else{
            if(m_DataBaseManager->updateFromDeviceByStoreIdAndSn(sn,storeId,updateVersion)){
                httpSocket->getResponse()->setBody("{\"return_code\":1,\"return_msg\":\"Succeed\",\"data\":{}}");
                httpSocket->sendResponse(httpSocket->getResponse());
                return;
            }
        }
    }
    httpSocket->getResponse()->setBody("{\"return_code\":0,\"return_msg\":\"Failure\",\"data\":{}}");
    httpSocket->sendResponse(httpSocket->getResponse());
}

