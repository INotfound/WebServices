#pragma once
#include <Magic/Magic>
#include <Magic/Utilty/Config.h>
#include <Magic/DataBase/MySql.h>
#include <Magic/Utilty/TimingWheel.h>
#include <Magic/DataBase/ConnectionPool.h>

struct DeviceInfo{
    std::string m_Sn;
    std::string m_Mac;
    std::string m_RoomId;
    std::string m_StoreId;
    std::string m_Version;
    std::string m_RoomName;
    std::string m_UpdateVersion;
};

struct VersionInfo{
    std::string m_Url;
    std::string m_Md5;
    std::string m_Version;
};

class DataBaseManager{
public:
    DataBaseManager(const Safe<Magic::Config>& config,const Safe<Magic::TimingWheel>& timingWheel);

    const Safe<VersionInfo> queryFromVersionByNewNormal();
    const Safe<DeviceInfo> queryFromDeviceByMac(const std::string& mac);
    const Safe<VersionInfo> queryFromVersionByVersion(const std::string& version);
    void updateFromDeviceByMac(const std::string& mac,const Safe<DeviceInfo>& deviceInfo);
    void insertFromDeviceByMac(const std::string& mac,const Safe<DeviceInfo>& deviceInfo);
private:
    const Safe<Magic::DataBase::MySql> initialize();
private:
    Safe<Magic::Config> m_Config;
    Safe<Magic::DataBase::ConnectionPool<Magic::DataBase::MySql>> m_DeviceConnectionPool;
    Safe<Magic::DataBase::ConnectionPool<Magic::DataBase::MySql>> m_VersionConnectionPool;
};