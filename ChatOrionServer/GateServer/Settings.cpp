#include "Settings.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace config
{
    ValueWrapper Settings::value(const std::string& key, const std::string& default_value) const
    {
        size_t separator_pos = key.find('/');
        if (separator_pos != std::string::npos)
        {
            std::string section = key.substr(0, separator_pos);
            std::string key_in_section = key.substr(separator_pos + 1);

            auto section_it = _config_map.find(section);
            if (section_it != _config_map.end())
            {
                auto key_it = section_it->second._section_datas.find(key_in_section);
                if (key_it != section_it->second._section_datas.end())
                {
                    return key_it->second;
                }
            }
        }
        return ValueWrapper(default_value);
    }

    int Settings::valueInt(const std::string& key, int default_value) const
    {
        return value(key).toInt(default_value);
    }

    double Settings::valueDouble(const std::string& key, double default_value) const
    {
        return value(key).toDouble(default_value);
    }

    bool Settings::valueBool(const std::string& key, bool default_value) const
    {
        return value(key).toBool(default_value);
    }

    void Settings::setValue(const std::string& key, const std::string& value)
    {
        size_t separator_pos = key.find('/');
        if (separator_pos != std::string::npos)
        {
            std::string section = key.substr(0, separator_pos);
            std::string key_in_section = key.substr(separator_pos + 1);
            _config_map[section]._section_datas[key_in_section] = ValueWrapper(value);
            saveToFile();
        }
    }

    Settings::Settings(const std::string& filename)
    {
        try {
            boost::filesystem::path current_path = boost::filesystem::current_path();
            std::string config_path = current_path.string();
            config_path += "/" + filename;
            _current_file_path = config_path;

            boost::property_tree::ptree ptree;
            boost::property_tree::read_ini(_current_file_path, ptree);

            for (const auto& section : ptree)
            {
                const std::string& section_name = section.first;
                const boost::property_tree::ptree& section_tree = section.second;

                SectionInfo section_info;
                for (const auto& item : section_tree)
                {
                    section_info._section_datas[item.first] =
                        ValueWrapper(item.second.get_value<std::string>());
                }
                _config_map[section_name] = std::move(section_info);
            }

            // Debug output
            for (const auto& section : _config_map) {
                std::cout << "[" << section.first << "]" << std::endl;
                for (const auto& item : section.second._section_datas) {
                    std::cout << item.first << "=" << item.second.get() << std::endl;
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading config file: " << e.what() << std::endl;
            throw;  // 重新抛出异常，让调用者处理
        }
    }

    void Settings::saveToFile() const
    {
        try {
            boost::property_tree::ptree ptree;

            for (const auto& section : _config_map)
            {
                for (const auto& item : section.second._section_datas)
                {
                    ptree.put(section.first + "." + item.first, item.second.get());
                }
            }

            boost::property_tree::write_ini(_current_file_path, ptree);
        }
        catch (const std::exception& e) {
            std::cerr << "Error saving config file: " << e.what() << std::endl;
            throw;
        }
    }
} // namespace config