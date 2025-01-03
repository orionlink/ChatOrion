#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <string>
#include <stdexcept>

namespace config
{
    class ValueWrapper
    {
    public:
        explicit ValueWrapper(const std::string& value = "") : _value(value) {}
        
        std::string toString() const { return _value; }
        
        int toInt(int default_value = 0) const {
            try {
                return std::stoi(_value);
            } catch (const std::exception&) {
                return default_value;
            }
        }
        
        double toDouble(double default_value = 0.0) const {
            try {
                return std::stod(_value);
            } catch (const std::exception&) {
                return default_value;
            }
        }
        
        bool toBool(bool default_value = false) const {
            if (_value == "true" || _value == "1") return true;
            if (_value == "false" || _value == "0") return false;
            return default_value;
        }
        
        std::string get() const { return _value; }
        
    private:
        std::string _value;
    };

    class SectionInfo
    {
    public:
        SectionInfo() = default;
        ~SectionInfo() = default;
        
        SectionInfo(const SectionInfo&) = default;
        SectionInfo& operator=(const SectionInfo&) = default;
        SectionInfo(SectionInfo&&) noexcept = default;
        SectionInfo& operator=(SectionInfo&&) noexcept = default;

        ValueWrapper operator[](const std::string& key) const {
            auto it = _section_datas.find(key);
            return it != _section_datas.end() ? it->second : ValueWrapper();
        }

        std::map<std::string, ValueWrapper> _section_datas;
    };

    class Settings
    {
    public:
        ~Settings() = default;
        
        Settings(const Settings&) = default;
        Settings& operator=(const Settings&) = default;
        Settings(Settings&&) noexcept = default;
        Settings& operator=(Settings&&) noexcept = default;

        static Settings& GetInstance()
        {
            static Settings settings;
            return settings;
        }

        void setFileName(const std::string &filename)
        {
            _filename = filename;
        }

        void load();

        template<typename T>
        ValueWrapper value(const std::string& key, const T& default_value = T()) const
        {
            auto val = value(key);
            return val.get().empty() ? ValueWrapper(std::to_string(default_value)) : val;
        }

        ValueWrapper value(const std::string& key, const std::string& default_value = "") const;
        int valueInt(const std::string& key, int default_value = 0) const;
        double valueDouble(const std::string& key, double default_value = 0.0) const;
        bool valueBool(const std::string& key, bool default_value = false) const;

        void setValue(const std::string& key, const std::string& value);

        SectionInfo operator[](const std::string& section) const {
            auto it = _config_map.find(section);
            return it != _config_map.end() ? it->second : SectionInfo();
        }

    private:
        explicit Settings();

        void saveToFile() const;
        
        std::map<std::string, SectionInfo> _config_map;
        std::string _current_file_path;
        std::string _filename;
    };
} // namespace config

#endif //SETTINGS_H