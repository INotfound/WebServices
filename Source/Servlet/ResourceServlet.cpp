#include "Servlet/ResourceServlet.h"

static bool IsFileExists(const std::string& path){
    if (FILE *file = fopen(path.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

ResourceServlet::ResourceServlet(const Safe<Magic::Config>& configuration)
    :m_Directory(configuration->at<std::string>("WebServices.ResourceServlet.Directory","./www")){
}

void ResourceServlet::handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket){
    std::string path = httpSocket->getRequest()->getPath();
    if(path == "/"){
        path += "index.html";
    }
    std::string resource = m_Directory + path;
    if(IsFileExists(resource)){
        httpSocket->getResponse()
        ->setResource(resource)
        ->setStatus(Magic::NetWork::Http::HttpStatus::OK)
        ->setContentType(Magic::NetWork::Http::FileTypeToHttpContentType(resource));
        httpSocket->sendResponse(httpSocket->getResponse());
    }
}

