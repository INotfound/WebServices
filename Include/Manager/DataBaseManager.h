#pragma once
#include <vector>
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
    bool m_Type;
    std::string m_Url;
    std::string m_Md5;
    std::string m_Version;
};

class DataBaseManager{
public:
    DataBaseManager(const Safe<Magic::Config>& config,const Safe<Magic::TimingWheel>& timingWheel);
    const Safe<VersionInfo> queryFromVersionByNewNormal();
    void queryFromDeviceByAll(std::vector<DeviceInfo>& deviceInfos);
    void queryFromVersionByAll(std::vector<VersionInfo>& versionInfos);
    const Safe<DeviceInfo> queryFromDeviceByMac(const std::string& mac);
    const Safe<VersionInfo> queryFromVersionByVersion(const std::string& version);
    bool updateFromDeviceByMac(const std::string& mac,const std::string& updateVersion);
    void flushFromDeviceByMac(const std::string& mac,const Safe<DeviceInfo>& deviceInfo);
    void insertFromDeviceByMac(const std::string& mac,const Safe<DeviceInfo>& deviceInfo);
    bool updateFromVersionByType(const std::string& oldVersion,const std::string& newVersion);
    bool updateFromDeviceByStoreId(const std::string& storeId,const std::string& updateVersion);
    bool updateFromDeviceByStoreIdAndSn(const std::string& sn,const std::string& storeId,const std::string& updateVersion);
private:
    const Safe<Magic::DataBase::MySql> initialize();
private:
    Safe<Magic::Config> m_Config;
    Safe<Magic::DataBase::ConnectionPool<Magic::DataBase::MySql>> m_DeviceConnectionPool;
    Safe<Magic::DataBase::ConnectionPool<Magic::DataBase::MySql>> m_VersionConnectionPool;
};