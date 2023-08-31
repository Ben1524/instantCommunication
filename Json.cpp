//
// Created by a1767 on 2023/8/20.
//

#include <stdexcept>
#include <sstream>
#include "Json.h"

byjson::Json::Json(bool val):m_type(json_bool)
{
    m_value.m_bool=val;
}

byjson::Json::Json(int val):m_type(json_int)
{
    m_value.m_int=val;
}

byjson::Json::Json(double val):m_type(json_double)
{
    m_value.m_double=val;
}

byjson::Json::Json(const char *val):m_type(json_string)
{
    m_value.m_string=new std::string (val);
}

byjson::Json::Json(const std::string &val):m_type(json_string)
{
    m_value.m_string=new std::string(val);
}

byjson::Json::Json(byjson::Json::valueType type):m_type(type)
{
    switch (type)
    {
        case json_null:
            break;
        case json_bool:
            m_value.m_bool=false;
            break;
        case json_int:
            m_value.m_int=0;
            break;
        case json_double:
            m_value.m_double=0.0;
            break;
        case json_string:
            m_value.m_string=new std::string();
            break;
        case json_array:
            m_value.m_array=new std::vector<Json>();
            break;
        case json_object:
            m_value.m_object=new std::map<std::string,Json>();
            break;
        default:
            break;
    }

}

byjson::Json::Json(const byjson::Json &other):m_type(other.m_type)
{
    copy(other);
}

byjson::Json::Json():m_type(json_null)
{
}

byjson::Json::operator bool() const
{
    if(m_type==json_bool)
        return m_value.m_bool;
    else
        throw  std::invalid_argument("Json type error not bool");
}

byjson::Json::operator int() const
{
    if(m_type==json_int)
        return m_value.m_int;
    else
        throw   std::invalid_argument("Json type error not int");
}

byjson::Json::operator double() const
{
    if(m_type==json_double)
        return m_value.m_double;
    else
        throw  std::invalid_argument("Json type error not double");
}

byjson::Json::operator std::string() const
{
    if(m_type==json_string)
        return *m_value.m_string;
    else
        throw  std::invalid_argument("Json type error not string");
}

byjson::Json &byjson::Json::operator[](int index)
{
    if(m_type!=json_array) // 当第一次使用[]运算符时自动创建Json数组
    {
        m_type=json_array;
        m_value.m_array=new std::vector<byjson::Json>();
    }
    if(index<0||index>=m_value.m_array->size())
    {
        throw  std::out_of_range("Json index out of range");
    }
    return (*m_value.m_array)[index];
}

void byjson::Json::append(const byjson::Json &val)
{
    if(m_type!=json_array) // 当第一次使用append时自动创建Json数组
    {
        clear();
        m_type=json_array;
        m_value.m_array=new std::vector<byjson::Json>();
    }
    m_value.m_array->push_back(val);
}

std::string byjson::Json::getValue() const
{
    std::stringstream ss;
    switch (m_type)
    {
    case json_int:
        ss<<m_value.m_int;
        break;
    case json_double:
        ss<<m_value.m_double;
        break;
    case json_string:
        ss<<'\"'<<*m_value.m_string<<'\"';
        break;
    case json_array:
        ss<<"[";
        for(int i=0;i<m_value.m_array->size();i++)
        {
            ss<<m_value.m_array->at(i).getValue();
            if(i!=m_value.m_array->size()-1) // 最后一个不加逗号
                ss<<",";
        }
        ss<<"]";
        break;
    case json_object:
        ss<<"{";
        auto end=m_value.m_object->end();
        end--;
        for(auto  it=m_value.m_object->begin();it!=m_value.m_object->end();it++)
        {
            ss<<"\""<<it->first<<"\":"<<it->second.getValue();
            if(it!=end)
                ss<<",";
        }
        ss<<"}";
        break;
    }
    return ss.str();
}

std::string byjson::Json::getType() const
{
    switch (m_type)
    {
    case json_int:
        return "int";
    case json_double:
        return "double";
    case json_string:
        return "string";
    case json_array:
        return "array";
    case json_object:
        return "object";
    default:
        return "null";
    }
}

byjson::Json &byjson::Json::operator[](const std::string &key)
{
    if(m_type!=json_object) // 转换成对象
    {
        clear();
        m_type=json_object;
        m_value.m_object=new std::map<std::string,byjson::Json>();
    }
    return (*m_value.m_object)[key];
}

byjson::Json &byjson::Json::operator[](const char *key)
{
    std::string ss(key);
    return (*this)[ss];
}

void byjson::Json::operator=(const byjson::Json &other)
{
    copy(other);
}

void byjson::Json::copy(const byjson::Json &other)
{
    clear();
    m_type=other.m_type;
    switch (m_type)
    {
        case json_null:
            break;
        case json_bool:
            m_value.m_bool=other.m_value.m_bool;
            break;
        case json_int:
            m_value.m_int=other.m_value.m_int;
            break;
        case json_double:
            m_value.m_double=other.m_value.m_double; //值允许共用,减少内存开销
            break;
        case json_string:
            m_value.m_string=other.m_value.m_string;
            break;
        case json_array:
            m_value.m_array=other.m_value.m_array;
            break;
        case json_object:
            m_value.m_object=other.m_value.m_object;
            break;
        default:
            break;
    }
}

void byjson::Json::clear()
{
    switch (m_type)
    {
        case  json_null:
            break;
        case json_bool:
            m_value.m_bool=false;
            break;
        case json_int:
            m_value.m_int=0;
            break;
        case json_double:
            m_value.m_double=0.0;
            break;
        case json_string:
            if(m_value.m_string!=nullptr)
            {
                delete m_value.m_string;
                m_value.m_string = nullptr;
            }
            break;
        case json_array:
        {
            if(m_value.m_array!=nullptr)
            {
                for (auto it = m_value.m_array->begin(); it != m_value.m_array->end(); it++)
                {
                    it->clear(); //递归调用
                }
                delete m_value.m_array;
                m_value.m_array = nullptr;
            }
            break;
        }
        case json_object:
            if(m_value.m_object!=nullptr)
            {
                delete m_value.m_object;
                m_value.m_object = nullptr;
            }
            break;
        default:
            break;
    }
    m_type=json_null;
}

bool byjson::Json::operator==(const byjson::Json &other) const
{
    if(m_type!=other.m_type)
        return false;
    switch (m_type)
    {
        case json_null:
            return true;
        case json_bool:
            return m_value.m_bool==other.m_value.m_bool;
        case json_int:
            return m_value.m_int==other.m_value.m_int;
        case json_double:
            return m_value.m_double==other.m_value.m_double;
        case json_string:
            return *m_value.m_string==*other.m_value.m_string;
        case json_array:
            return m_value.m_array==other.m_value.m_array; // 数组指针共用,使得内存减少浪费
        case json_object:
            return m_value.m_object==other.m_value.m_object;
        default:
            return false;
    }
}

bool byjson::Json::operator!=(const byjson::Json &other) const
{
    if(m_type!=other.m_type)
        return true;
    switch (m_type)
    {
        case json_null:
            return false;
        case json_bool:
            return m_value.m_bool!=other.m_value.m_bool;
        case json_int:
            return m_value.m_int!=other.m_value.m_int;
        case json_double:
            return m_value.m_double!=other.m_value.m_double;
        case json_string:
            return  *m_value.m_string!=*other.m_value.m_string;
        case json_array:
            return m_value.m_array!=other.m_value.m_array;
        case json_object:
            return m_value.m_object!=other.m_value.m_object;
        default:
            return false;
    }
}

bool byjson::Json::isNull() const
{
    return m_type==json_null;
}

bool byjson::Json::isBool() const
{
    return m_type==json_bool;
}

bool byjson::Json::isInt() const
{
    return m_type==json_int;
}

bool byjson::Json::isDouble() const
{
    return m_type==json_double;
}

bool byjson::Json::isString() const
{
    return m_type==json_string;
}

bool byjson::Json::isArray() const
{
    return m_type==json_array;
}

bool byjson::Json::isObject() const
{
    return m_type==json_object;
}

bool byjson::Json::toBool() const
{
    if(m_type==json_bool)
        return m_value.m_bool;
    else
        throw std::invalid_argument("Json type error not bool");
}

int byjson::Json::toInt() const
{
    if(m_type==json_int)
        return m_value.m_int;
    else
        throw std::invalid_argument("Json type error not int");
}

double byjson::Json::toDouble() const
{
    if(m_type==json_double)
        return m_value.m_double;
    else
        throw std::invalid_argument("Json type error not double");
}

std::string byjson::Json::toString() const
{
    if(m_type==json_string)
        return *m_value.m_string;
    else
        throw std::invalid_argument("Json type error not string");
}

std::vector<byjson::Json> byjson::Json::toArray() const
{
    if(m_type==json_array)
        return *m_value.m_array;
    else
        throw std::invalid_argument("Json type error not array");
}

std::map<std::string,byjson::Json> byjson::Json::toObject() const
{
    if(m_type==json_object)
        return *m_value.m_object;
    else
        throw std::invalid_argument("Json type error not object");
}

bool byjson::Json::has(int index) const
{
    if(m_type==json_array)
        return index<m_value.m_array->size()&&index>=0;
    else
        throw std::invalid_argument("Json type error not array");
}

bool byjson::Json::has(const std::string &key) const
{
    if(m_type==json_object)
        return m_value.m_object->find(key)!=m_value.m_object->end();
    else
        throw std::invalid_argument("Json type error not object");
}
bool byjson::Json::has(const char *key) const
{
    return has(std::string(key));
}

void byjson::Json::remove(int index)
{
    if(m_type==json_array)
    {
        if(index<m_value.m_array->size()&&index>=0)
            m_value.m_array->erase(m_value.m_array->begin()+index);
        else
            throw std::invalid_argument("Json index error");
    }
    else
        throw std::invalid_argument("Json type error not array");
}

void byjson::Json::remove(const std::string &key)
{
    if(m_type==json_object)
    {
        if(m_value.m_object->find(key)!=m_value.m_object->end())
            m_value.m_object->erase(key);
        else
            throw std::invalid_argument("Json key error");
    }
    else
        throw std::invalid_argument("Json type error not object");
}

void byjson::Json::remove(const char *key)
{
    remove(std::string(key));
}





byjson::Json::operator std::vector<Json>() const
{
    if(m_type==json_array)
        return *m_value.m_array;
    else
        throw  std::invalid_argument("Json type error not array");
}

byjson::Json::operator std::map<std::string,Json>() const
{
    if(m_type==json_object)
        return *m_value.m_object;
    else
        throw  std::invalid_argument("Json type error not object");
}