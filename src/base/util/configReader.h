#pragma once

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <string>
#include <map>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <list>
#include <set>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cctype>

#include "log/log.h"
#include "mutex.h"
// #include "util.h"

/**
 * @brief 配置变量的基类
 */
class ConfigVarBase
{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    /**
     * @brief 构造函数
     * @param[in] name 配置参数名称[0-9a-z_.]
     * @param[in] description 配置参数描述
     */
    ConfigVarBase(const std::string &name, const std::string &description = "")
        : m_name(name),
          m_description(description)
    {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    virtual ~ConfigVarBase() {}

    const std::string &getName() const { return m_name; }
    const std::string &getDescription() { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string &val) = 0;

    virtual std::string getTypeName() const = 0;

protected:
    std::string m_name;
    std::string m_description;

private:
};

/**
 * @brief 类型转换模板类(F 原类型， T目标类型)
 */
template <class F, class T>
class LexicalCast
{
public:
    /**
     * @brief 类型转换
     * @param[in] v 原类型值
     * @return 返回v转换后的目标类型
     * @exception 当类型不可转换时抛出异常
     */
    T operator()(const F &v)
    {
        return boost::lexical_cast<T>(v);
    }
};

/**
 * @brief 类型转换模板类偏特化(YAML String 转换成std::vector<T>)
 */
template <class T>
class LexicalCast<std::string, std::vector<T>>
{
public:
    std::vector<T> operator()(const std::string &v)
    {
        YAML::Node node = YAML::Load(v);
        typename std::vector<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++)
        {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

/**
 * @brief 类型转换模板偏特化(std::vector<T> 转换成 YAML String)
 */
template <class T>
class LexicalCast<std::vector<T>, std::string>
{
public:
    std::string operator()(const std::vector<T> &v)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 类型转换模板偏特化(YAML String  转换成  std::list<T>)
 */
template <class T>
class LexicalCast<std::string, std::list<T>>
{
public:
    std::list<T> operator()(const std::string &v)
    {
        YAML::Node node = YAML::Load(v);
        typename std::list<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++)
        {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

/**
 * @brief 类型转换模板偏特化(std::list<T> 转换成 YAML String)
 */
template <class T>
class LexicalCast<std::list<T>, std::string>
{
public:
    std::string operator()(const std::list<T> &v)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 类型转换模板偏特化(YAML String  转换成  std::list<T>)
 */
template <class T>
class LexicalCast<std::string, std::set<T>>
{
public:
    std::list<T> operator()(const std::string &v)
    {
        YAML::Node node = YAML::Load(v);
        typename std::set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); i++)
        {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

/**
 * @brief 类型转换模板偏特化(std::set<T> 转换成 YAML String)
 */
template <class T>
class LexicalCast<std::set<T>, std::string>
{
public:
    std::string operator()(const std::set<T> &v)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto &i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 类型转换模板类片特化(YAML String 转换成 std::map<std::string, T>)
 */
template <class T>
class LexicalCast<std::string, std::map<std::string, T>>
{
public:
    std::map<std::string, T> operator()(const std::string &v)
    {
        YAML::Node node = YAML::Load(v);
        typename std::map<std::string, T> vec;
        std::stringstream ss;
        for (auto it = node.begin();
             it != node.end(); ++it)
        {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(),
                                      LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

/**
 * @brief 类型转换模板类片特化(std::map<std::string, T> 转换成 YAML String)
 */
template <class T>
class LexicalCast<std::map<std::string, T>, std::string>
{
public:
    std::string operator()(const std::map<std::string, T> &v)
    {
        YAML::Node node(YAML::NodeType::Map);
        for (auto &i : v)
        {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template <class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
class ConfigVar : public ConfigVarBase
{
public:
    typedef RWMutex RWMutexType;
    // typedef std::shared_ptr<ConfigVar> ptr;
    using ptr = std::shared_ptr<ConfigVar>;
    typedef std::function<void(const T &old_value, const T &new_value)> on_change_cb;

    /**
     * @brief 通过参数名，参数值，描述构造ConfigVar
     * @param[in] name 参数名称有效字符为[0-9a-z_.]
     * @param[in] default_value 参数的默认值
     * @param[in] description 参数的描述
     */
    ConfigVar(const std::string &name, const T &default_value, const std::string &description = "")
        : ConfigVarBase(name, description),
          m_val(default_value)
    {
    }

    std::string toString() override
    {
        try
        {
            RWMutexType::ReadLock lock(m_mutex);
            ////boost::lexical_cast<std::string>(m_val);
            return ToStr()(m_val);
        }
        catch (std::exception &e)
        {
            LOG_ERROR(LOG_ROOT()) << "ConfigVar::toString exception"
                                  << e.what() << "convert: " << typeid(m_val).name() << " to string"
                                  << "name = " << m_name;
        }

        return "";
    }

    bool fromString(const std::string &val) override
    {
        try
        {
            ////m_val = boost::lexical_cast<T>(val);
            setValue(FromStr()(val));
            return true;
        }
        catch (std::exception &e)
        {
            LOG_ERROR(LOG_ROOT()) << "ConfigVar::fromString exception"
                                  << e.what() << "convert: string to " << typeid(m_val).name()
                                  << "name = " << m_name
                                  << " - " << val;
        }

        return false;
    }

    const T getValue() const
    {
        RWMutexType::ReadLock lock(m_mutex);
        return m_val;
    }
    void setValue(const T &v)
    {
        {
            RWMutexType::ReadLock lock(m_mutex);
            if (v == m_val)
            {
                return;
            }
            for (auto &i : m_cbs)
            {
                i.second(m_val, v);
            }
        }
        RWMutexType::WriteLock lock(m_mutex);
        m_val = v;
    }

    std::string getTypeName() const override
    {
        // return TypeToName<T>();
        return typeid(T).name();
    }

    /**
     * @brief 添加变化回调函数
     * @return 返回该回调函数对应的唯一id， 用于删除回调
     */
    uint64_t addListener(on_change_cb cb)
    {
        static uint64_t s_fun_id = 0;
        RWMutexType::WriteLock lock(m_mutex);
        s_fun_id++;
        m_cbs[s_fun_id] = cb;
        return s_fun_id;
    }

    /**
     * @brief 删除回调函数
     * @param[in] key 回调函数的唯一id
     */
    void delListener(uint64_t key)
    {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.erase(key);
    }

    on_change_cb getListener(uint64_t key)
    {
        RWMutexType::ReadLock lock(m_mutex);
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    void clearListener()
    {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.clear();
    }

private:
    mutable RWMutexType m_mutex;
    T m_val;
    // 变更回调函数map，uint64_t key要求唯一，可以用hash
    std::map<uint64_t, on_change_cb> m_cbs;
};

class Config
{
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    typedef RWMutex RWMutexType;

    /**
     * @brief 获取/创建对应参数名的配置参数
     * @param[in] name 配置参数名称
     * @param[in] default_value 参数默认值
     * @param[in] description 参数描述
     * @details 获取参数名为name的配置参数，如果存在直接返回
     *          如果不存在，创建参数配置并用default_value赋值
     * @return 返回对应的配置参数，如果参数名存在但是类型不匹配则返回nullptr
     * @exception 如果参数名包含非法字符[^0-9a-z._] 抛出异常 std::invalid_argument
     */
    template <class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string &name,
                                             const T &default_value, const std::string &description = "")
    {
        // RWMutexType::WriteLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if (it != GetDatas().end())
        {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if (tmp)
            {
                LOG_INFO(LOG_ROOT()) << "Lookup name = " << name << " exists";
                return tmp;
            }
            else
            {
                LOG_ERROR(LOG_ROOT()) << "Lookup name = " << name << " exists, but type "
                                      << /*TypeToName<T>()*/ typeid(T).name() << " not equals real_type = " << it->second->getTypeName()
                                      << " " << it->second->toString();
                return nullptr;
            }
        }

        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") != std::string::npos)
        {
            LOG_ERROR(LOG_ROOT()) << "Lookup name invalid: " << name;
            throw std::invalid_argument(name);
        }

        // typename 告诉编译器ConfigVar<T>::ptr 是类型别名而不是静态成员变量
        // typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        typename ConfigVar<T>::ptr v = std::make_shared<ConfigVar<T>>(name, default_value, description);
        GetDatas()[name] = v;

        return v;
    }

    /**
     * @brief 查找配置参数
     * @param[in] name 配置参数名称
     * @return 返回配置参数名为name的配置参数
     */
    template <class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string &name)
    {
        RWMutexType::ReadLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if (it == GetDatas().end())
        {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }

    static void LoadFromYaml(const YAML::Node &root);

    static void LoadFromConfDir(const std::string &path, bool force = false);

    // 不清楚类型时找到基类指针
    static ConfigVarBase::ptr LookupBase(const std::string &name);

    /**
     * @brief 遍历配置模块里面所有项
     * @param[in] cb 配置项回调函数
     */
    static void Visit(std::function<void(ConfigVarBase::ptr)> cb);

private:
    static ConfigVarMap &GetDatas()
    {
        static ConfigVarMap s_datas;
        return s_datas;
    }

    static RWMutexType &GetMutex()
    {
        static RWMutexType s_mutex;
        return s_mutex;
    }
};