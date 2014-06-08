// Copyright 2014, Xiaojie Chen (swly@live.com). All rights reserved.
// https://github.com/vorfeed/naesala
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace naesala {

class ConfParser {
public:
    ConfParser(std::string const& conf_path) : _path(conf_path) {}

    /**
     * get conf value
     * @param option
     * @param key
     * @param value
     */
    template <typename Value>
    void get(std::string const& option, std::string const& key, Value& value) const {
        if (_conf.find(option) == _conf.end()) {
            throw std::exception(_path + "get|option[" + option + "] dosen't exist!");
        }
        item const& item = _conf.find(option)->second;
        if (item.find(key) == item.end()) {
            throw std::exception(_path + "get|option[" + option + "]|key("
                    + key + ") dosen't exist!");
        }
        std::string const& var = item.find(key)->second;
        try {
            value = boost::lexical_cast<Value>(var);
        } catch (boost::bad_lexical_cast const&) {
            throw std::exception(_path + "get|option[" + option + "]|key("
                    + key + ")|value(" + var + ") type dosen't match!");
        }
    }

    /**
     * get conf value, if not exist, use default value
     * @param option
     * @param key
     * @param value
     * @param default_value
     */
    template <typename Value>
    void get(std::string const& option, std::string const& key, Value& value,
            Value const& default_value) const {
        try {
            get(option, key, value);
        } catch (std::exception const&) {
            value = default_value;
        }
    }

    /**
     * set conf value
     * @param option
     * @param key
     * @param value
     */
    template <typename Value>
    void set(std::string const& option, std::string const& key, Value const& value) {
        std::string var;
        try {
            var = boost::lexical_cast<std::string>(value);
        } catch (boost::bad_lexical_cast const&) {
            throw std::exception(_path + "|set|option[" + option + "]|key("
                    + key + ")|value(" + value + ") unsupported value type!");
        }
        _conf[option][key] = var;
    }

    /**
     * check if confiure file has already been loaded
     * @return true if succeed
     */
    bool loaded() const { return _loaded; }

    /**
     * load configure info from file
     */
    void load() {
        std::fstream in(_path.c_str());
        if (!in) {
            throw std::exception(_path + "|load|Fail to open configure file!");
        }
        std::string line, cur_option;
        std::vector<std::string> pair;
        while (getline(in, line)) {
            boost::trim(line);
            if (line.empty()) {
                continue;
            }
            if (line.at(0) == '#') {
                /** comments */
                continue;
            }
            if (line.at(0) == '[') {
                auto pos = boost::find_first(line, "]");
                if (pos.empty()) {
                    /** error format */
                    throw std::exception(_path + "|load|Format error in option!");
                }
                cur_option = line.substr(1, pos.begin() - line.begin() - 1);
                continue;
            }
            boost::algorithm::split(pair, line, boost::is_any_of("="));
            if (pair.size() != 2) {
                throw std::exception(_path + "|load|option["
                        + cur_option + "]|Format error in item!");
            }
            boost::trim(pair[0]);
            boost::trim(pair[1]);
            _conf[cur_option][pair[0]] = pair[1];
        }
        _loaded = true;
    }

    friend std::ostream& operator<< (std::ostream& out, ConfParser const& conf) {
        out << "Configure information (" << conf._path << "): " << std::endl;
        if (conf._conf.empty()) {
            return out << "No elements.";
        }
        for (conf::const_iterator option = conf._conf.begin();
                option != conf._conf.end(); ++option) {
            out << "[" << option->first << "]" << std::endl;
            for (item::const_iterator pair = option->second.begin();
                    pair != option->second.end(); ++pair) {
                out << pair->first << " = " << pair->second << std::endl;
            }
        }
        return out;
    }

private:
    typedef std::unordered_map<std::string, std::string> item;
    typedef std::unordered_map<std::string, item> conf;

    bool _loaded { false };
    std::string _path;
    conf _conf;
};

} // namespace naesala
