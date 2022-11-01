#include <Magic/Magic>
#include <WebServices.h>
#include <Magic/DataBase/MySql.h>
#include <Magic/DataBase/ConnectionPool.h>

int main(int argc, char** argv){
    Magic::Thread::SetName("Main");
    Safe<Magic::Application> application = std::make_shared<Magic::Application>();
    application->initialize();
    return EXIT_SUCCESS;
}