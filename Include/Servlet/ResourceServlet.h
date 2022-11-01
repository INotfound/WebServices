#pragma once
#include <Magic/Magic>
#include <Magic/Utilty/Config.h>
#include <Magic/NetWork/Http/HttpServlet.h>

class ResourceServlet :public Magic::NetWork::Http::IHttpServlet{
public:
    ResourceServlet(const Safe<Magic::Config>& configuration);
    void handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket);
private:
    std::string m_Directory;
};