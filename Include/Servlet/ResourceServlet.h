#pragma once
#include <Magic/Magic>
#include <Magic/Utilty/Config.h>
#include <Magic/NetWork/Http/HttpServlet.h>

class ResourceServlet :public Magic::NetWork::Http::IHttpServlet{
public:
    ResourceServlet(const Safe<Magic::Config>& configuration);
    bool handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket
                ,const Safe<Magic::NetWork::Http::HttpRequest>& request
                ,const Safe<Magic::NetWork::Http::HttpResponse>& response) override;

private:
    std::string m_Directory;
};