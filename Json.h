//
// Created by a1767 on 2023/8/20.
//

#ifndef REACTOR_CHATROOM_JSON_H
#define REACTOR_CHATROOM_JSON_H

#include <string>
#include <map>
#include <vector>

namespace byjson
{
    class Json
    {
    public:
        enum valueType
        {
            json_null=0,
            json_bool,
            json_int,
            json_double,
            json_string,
            json_array,
            json_object
        };

        Json();
        Json(bool val);
        Json(int val);
        Json(double val);
        Json(const char* val);
        Json(const std::string& val);
        Json(valueType type);// 使用类型的默认值来初始化
        Json(const Json&other); //  拷贝构造函数

        void operator=(const Json& other); // 赋值运算符
        void copy(const Json& other); // 拷贝函数,提高代码复用
        void clear(); // 清空函数

        bool operator==(const Json& other) const; // 判断是否相等
        bool operator!=(const Json& other) const; // 判断是否不相等

        Json&  operator[](int  index); //获取json数组值
        Json&  operator[](const std::string& key); //获取json对象值
        Json&  operator[](const char* key); //获取json对象值
        void append(const Json& val); // 添加json数组值
        void remove(int index); // 删除json数组值
        void remove(const std::string& key); // 删除json对象值
        void remove(const char* key); // 删除json对象值

        std::string getValue() const; //以字符串形式返回json值
        std::string getType() const; // /返回json值的类型

        bool  isNull() const;
        bool  isBool() const;
        bool  isInt() const;
        bool  isDouble() const;
        bool  isString() const;
        bool  isArray() const;
        bool  isObject() const;

        bool has(int  index) const; // 判断是不是有那个下标
        bool has(const std::string& key) const; // 判断是不是有那个key
        bool has(const char* key) const; // 判断是不是有那个key

        bool toBool() const;
        int  toInt() const;
        double toDouble() const;
        std::string toString() const;
        std::vector<Json> toArray() const;
        std::map<std::string,Json> toObject() const;



        operator bool() const;
        operator int() const;
        operator double() const;
        operator std::string() const;
        operator std::vector<Json>() const;
        operator std::map<std::string,Json>() const;

    private:
        union value
        {
            bool m_bool;
            int m_int;
            double m_double;
            std::string* m_string; // 如果使用std::string会导致union占用过大
            std::vector<Json>* m_array;
            std::map<std::string,Json>*m_object; // 键值对
        };
        valueType m_type;
        value m_value;
    };
}


#endif //REACTOR_CHATROOM_JSON_H