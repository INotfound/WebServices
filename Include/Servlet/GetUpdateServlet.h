#pragma once
#include <Magic/Magic>
#include <Magic/DataBase/MySql.h>
#include <Magic/DataBase/ConnectionPool.h>
#include <Magic/NetWork/Http/HttpServlet.h>

#include "Manager/DataBaseManager.h"

class GetUpdateServlet :public Magic::NetWork::Http::IHttpServlet{
public:
    GetUpdateServlet(const Safe<DataBaseManager>& dataBaseManager);
    void handle(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket);

private:
     Safe<DataBaseManager> m_DataBaseManager;
};