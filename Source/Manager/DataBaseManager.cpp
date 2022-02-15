#include "Manager/DataBaseManager.h"

/* Device Table
CREATE TABLE IF NOT EXISTS `device`(
   `id` INT UNSIGNED AUTO_INCREMENT,
   `sn` VARCHAR(64) DEFAULT '',
   `mac` VARCHAR(64) DEFAULT '',
   `created_time` DATETIME NOT NULL,
   `updated_time` DATETIME NOT NULL,
   `room_id` VARCHAR(100) DEFAULT '',
   `store_id` VARCHAR(100) DEFAULT '',
   `room_name` VARCHAR(100) DEFAULT '',
   `last_version` VARCHAR(256) DEFAULT '',
   `update_version` VARCHAR(256) DEFAULT '',
   PRIMARY KEY (`id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
 */

/* version Table
CREATE TABLE IF NOT EXISTS `version`(
   `id` INT UNSIGNED AUTO_INCREMENT,
   `url` TEXT DEFAULT '',
   `type` INT(1) DEFAULT 0,
   `md5` VARCHAR(256) DEFAULT '',
   `ver` VARCHAR(256) DEFAULT '',
   `created_time` DATETIME NOT NULL,
   `updated_time` DATETIME NOT NULL,
   PRIMARY KEY (`id`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
 */

DataBaseManager::DataBaseManager(const Safe<Magic::Config> &config,
                                 const Safe<Magic::TimingWheel>& timingWheel)
    :m_Config(config)
    ,m_DeviceConnectionPool(std::make_shared<Magic::DataBase::ConnectionPool<Magic::DataBase::MySql>>(config,timingWheel))
    ,m_VersionConnectionPool(std::make_shared<Magic::DataBase::ConnectionPool<Magic::DataBase::MySql>>(config,timingWheel)){
    m_DeviceConnectionPool->initialize(std::bind(&DataBaseManager::initialize,this));
    m_VersionConnectionPool->initialize(std::bind(&DataBaseManager::initialize,this));
}

const Safe<VersionInfo> DataBaseManager::queryFromVersionByNewNormal(){
    auto sqlConnection = m_VersionConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt queryStmt(*sqlConnection);
        queryStmt.prepare("SELECT url,md5,ver,type FROM version WHERE type = 1");
        if(queryStmt.query()){
            if(queryStmt.fetch()){
                Safe<VersionInfo> versionInfo = std::make_shared<VersionInfo>();
                versionInfo->m_Url = queryStmt.getString(0);
                versionInfo->m_Md5 = queryStmt.getString(1);
                versionInfo->m_Version = queryStmt.getString(2);
                versionInfo->m_Type = queryStmt.getInt32(3) == 1;
                return versionInfo;
            }
        }
    }
    return Safe<VersionInfo>();
}

void DataBaseManager::queryFromDeviceByAll(std::vector<DeviceInfo>& deviceInfos){
    auto sqlConnection = m_DeviceConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt queryStmt(*sqlConnection);
        queryStmt.prepare("SELECT sn,mac,room_id,store_id,room_name,last_version,update_version FROM device");
        if(queryStmt.query()){
            while(queryStmt.fetch()){
                DeviceInfo info;
                info.m_Sn = queryStmt.getString(0);
                info.m_Mac = queryStmt.getString(1);
                info.m_RoomId = queryStmt.getString(2);
                info.m_StoreId = queryStmt.getString(3);
                info.m_RoomName = queryStmt.getString(4);
                info.m_Version = queryStmt.getString(5);
                info.m_UpdateVersion = queryStmt.getString(6);
                deviceInfos.emplace_back(info);
            }
        }
    }
}

void DataBaseManager::queryFromVersionByAll(std::vector<VersionInfo>& versionInfos){
    auto sqlConnection = m_VersionConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt queryStmt(*sqlConnection);
        queryStmt.prepare("SELECT url,md5,ver,type FROM version");
        if(queryStmt.query()){
            while(queryStmt.fetch()){
                VersionInfo info;
                info.m_Url = queryStmt.getString(0);
                info.m_Md5 = queryStmt.getString(1);
                info.m_Version = queryStmt.getString(2);
                info.m_Type = queryStmt.getInt32(3) == 1;
                versionInfos.emplace_back(info);
            }
        }
    }
}

const Safe<DeviceInfo> DataBaseManager::queryFromDeviceByMac(const std::string& mac){
    auto sqlConnection = m_DeviceConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt queryStmt(*sqlConnection);
        queryStmt.prepare("SELECT sn,mac,room_id,store_id,room_name,last_version,update_version FROM device WHERE mac = ?");
        queryStmt.bind(0,mac);
        if(queryStmt.query()){
            if(queryStmt.fetch()){
                Safe<DeviceInfo> result = std::make_shared<DeviceInfo>();
                result->m_Sn = queryStmt.getString(0);
                result->m_Mac = queryStmt.getString(1);
                result->m_RoomId = queryStmt.getString(2);
                result->m_StoreId = queryStmt.getString(3);
                result->m_RoomName = queryStmt.getString(4);
                result->m_Version = queryStmt.getString(5);
                result->m_UpdateVersion = queryStmt.getString(6);
                return result;
            }
        }
    }
    return Safe<DeviceInfo>();
}

const Safe<VersionInfo> DataBaseManager::queryFromVersionByVersion(const std::string &version) {
    auto sqlConnection = m_VersionConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt queryStmt(*sqlConnection);
        queryStmt.prepare("SELECT url,md5,ver,type FROM version WHERE ver = ?");
        queryStmt.bind(0,version);
        if(queryStmt.query()){
            if(queryStmt.fetch()){
                Safe<VersionInfo> versionInfo = std::make_shared<VersionInfo>();
                versionInfo->m_Url = queryStmt.getString(0);
                versionInfo->m_Md5 = queryStmt.getString(1);
                versionInfo->m_Version = queryStmt.getString(2);
                versionInfo->m_Type = queryStmt.getInt32(3) == 1;
                return versionInfo;
            }
        }
    }
    return Safe<VersionInfo>();
}

bool DataBaseManager::updateFromDeviceByMac(const std::string &mac, const std::string &updateVersion) {
    auto sqlConnection = m_DeviceConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt updateStmt(*sqlConnection);
        updateStmt.prepare("UPDATE device SET update_version=? where mac = ?");
        updateStmt.bind(0,updateVersion);
        updateStmt.bind(1,mac);
        if(!updateStmt.execute()){
            MAGIC_DEBUG() << "Execute Failed!";
            return false;
        }
        if(updateStmt.rows() > 0){
            return true;
        }
    }
    return false;
}

void DataBaseManager::flushFromDeviceByMac(const std::string& mac,const Safe<DeviceInfo>& deviceInfo) {
    auto sqlConnection = m_DeviceConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt updateStmt(*sqlConnection);
        updateStmt.prepare("UPDATE device SET sn=?,mac=?,room_id=?,store_id=?,room_name=?,last_version=?,updated_time=? where mac = ?");
        updateStmt.bind(0,deviceInfo->m_Sn);
        updateStmt.bind(1,deviceInfo->m_Mac);
        updateStmt.bind(2,deviceInfo->m_RoomId);
        updateStmt.bind(3,deviceInfo->m_StoreId);
        updateStmt.bind(4,deviceInfo->m_RoomName);
        updateStmt.bind(5,deviceInfo->m_Version);
        updateStmt.bindTime(6,time(0));
        updateStmt.bind(7,deviceInfo->m_Mac);
        if(!updateStmt.execute()){
            MAGIC_DEBUG() << "Execute Failed!";
        }
    }
}

void DataBaseManager::insertFromDeviceByMac(const std::string& mac, const Safe<DeviceInfo>& deviceInfo) {
    auto sqlConnection = m_DeviceConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt insertStmt(*sqlConnection);
        insertStmt.prepare("INSERT INTO device(sn,mac,room_id,store_id,room_name,last_version,created_time,updated_time) VALUES (?,?,?,?,?,?,?,?)");
        insertStmt.bind(0,deviceInfo->m_Sn);
        insertStmt.bind(1,deviceInfo->m_Mac);
        insertStmt.bind(2,deviceInfo->m_RoomId);
        insertStmt.bind(3,deviceInfo->m_StoreId);
        insertStmt.bind(4,deviceInfo->m_RoomName);
        insertStmt.bind(5,deviceInfo->m_Version);
        insertStmt.bindTime(6,time(0));
        insertStmt.bindTime(7,time(0));
        if(!insertStmt.execute()){
            MAGIC_DEBUG() << "Execute Failed!";
        }
    }
}

bool DataBaseManager::updateFromVersionByType(const std::string& oldVersion,const std::string& newVersion){
    auto sqlConnection = m_VersionConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt oldVersionStmt(*sqlConnection);
        oldVersionStmt.prepare("UPDATE version SET type=? WHERE ver = ?");
        oldVersionStmt.bind(0,0);
        oldVersionStmt.bind(1,oldVersion);
        if(oldVersionStmt.execute()){
            Magic::DataBase::MySqlStmt newVersionStmt(*sqlConnection);
            oldVersionStmt.prepare("UPDATE version SET type=? WHERE ver = ?");
            oldVersionStmt.bind(0,1);
            oldVersionStmt.bind(1,newVersion);
            if(oldVersionStmt.execute()){
                return true;
            }
        }
        return false;
    }
    return false;
}
bool DataBaseManager::updateFromDeviceByStoreId(const std::string &storeId, const std::string &updateVersion) {
    auto sqlConnection = m_DeviceConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt updateStmt(*sqlConnection);
        updateStmt.prepare("UPDATE device SET update_version=? where store_id = ?");
        updateStmt.bind(0,updateVersion);
        updateStmt.bind(1,storeId);
        if(!updateStmt.execute()){
            MAGIC_DEBUG() << "Execute Failed!";
            return false;
        }
        if(updateStmt.rows() > 0){
            return true;
        }
        return true;
    }
    return false;
}

bool DataBaseManager::updateFromDeviceByStoreIdAndSn(const std::string& sn,const std::string& storeId,const std::string& updateVersion){
    auto sqlConnection = m_DeviceConnectionPool->getConnection();
    if(sqlConnection){
        Magic::DataBase::MySqlStmt updateStmt(*sqlConnection);
        updateStmt.prepare("UPDATE device SET update_version=? where store_id = ? And sn = ?");
        updateStmt.bind(0,updateVersion);
        updateStmt.bind(1,storeId);
        updateStmt.bind(2,sn);
        if(!updateStmt.execute()){
            MAGIC_DEBUG() << "Execute Failed!";
            return false;
        }
        if(updateStmt.rows() > 0){
            return true;
        }
    }
    return false;
}

const Safe<Magic::DataBase::MySql> DataBaseManager::initialize(){
    Safe<Magic::DataBase::MySql> sql = std::make_shared<Magic::DataBase::MySql>();
    std::string dbIp = m_Config->at<std::string>("DataBaseManager.DataBaseIp","");
    std::string dbUser = m_Config->at<std::string>("DataBaseManager.DataBaseUser","");
    std::string dbName = m_Config->at<std::string>("DataBaseManager.DataBaseName","");
    std::string dbPassword = m_Config->at<std::string>("DataBaseManager.DataBasePassword","");
    uint32_t dbPort = m_Config->at<uint32_t>("DataBaseManager.DataBasePort",3306);
    if(sql->connnetDB(dbName,dbIp,dbUser,dbPassword,dbPort)){
        return sql;
    }
    return Safe<Magic::DataBase::MySql>();
}

