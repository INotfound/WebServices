#include <Magic/Magic>
#include <WebServices.h>
#include <Magic/DataBase/MySql.h>
#include <Magic/DataBase/ConnectionPool.h>

int main(int argc, char** argv){
    Magic::Thread::SetName("Main");
    const Safe<Magic::Container> ioc = WebServices::Initialize();
    ioc->resolve<Magic::NetWork::Http::HttpServer>()->run();
    return EXIT_SUCCESS;
}