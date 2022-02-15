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
    :Magic::NetWork::Http::IHttpServlet("^/?(.*)$",Magic::NetWork::Http::HttpServletType::Global)
    ,m_Directory(configuration->at<std::string>("WebServices.ResourceServlet.Directory","./www")){
}

bool ResourceServlet::handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket,
                              const Safe<Magic::NetWork::Http::HttpRequest>& request,
                              const Safe<Magic::NetWork::Http::HttpResponse>& response){
    std::string path = request->getPath();
    if(path == "/"){
        path += "index.html";
    }
    std::string resource = m_Directory + path;
    if(IsFileExists(resource)){
        response->setResource(resource);
        response->setStatus(Magic::NetWork::Http::HttpStatus::OK);
        response->setContentType(Magic::NetWork::Http::FileTypeToHttpContentType(resource));
        httpSocket->sendResponse(response);
        return true;
    }else{
        return false;
    }
}

