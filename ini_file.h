#ifndef __ZH_INIFILE_H__
#define __ZH_INIFILE_H__

#include <string>
#include <list>
#include <fstream>
#include <memory>
#include <vector>
#include <iostream>
#include <sstream>

namespace zh
{

class IniFile
{

//行内注释与内容之间的间隔空格数
#define ZH_INILINE_COMMENT_INTERVAL 4
//每个section组之间的间隔行数
#define ZH_COMMENT_INILINE_INTERVAL 1

class IniLine
{
public:
    typedef std::list<IniLine*>::iterator iterator;
    //写入一行
    virtual void write_line(std::fstream& file) = 0;
};

//注释行
class IniCommentLine : public IniLine
{
public:
    IniCommentLine(std::string& comment) : 
        m_comment(comment){};
    
    virtual void write_line(std::fstream& file);

    std::string m_comment;
};


//键值行
class IniKeyValLine : public IniLine
{
public:
    IniKeyValLine(std::string& key, std::string& value, std::string& comment) :
        m_key(key), 
        m_value(value), 
        m_comment(comment) {}
    
    virtual void write_line(std::fstream& file);

    //key名称
    std::string m_key;
    //value名称
    std::string m_value;
    //该行的注释
    std::string m_comment;
};

//section行
class IniSecticonLine : public IniLine
{
public:
    IniSecticonLine(std::string& section, std::string& comment) : 
        m_section(section), 
        m_comment(comment) {}

    virtual void write_line(std::fstream& file);

    //重载运算符[]
    std::string& operator[](const std::string& key);

    //section名称
    std::string m_section;
    //该行的注释
    std::string m_comment;
    //键值行链表
    std::list<IniKeyValLine*> m_key_val_lines;
};


public:
    IniFile(std::string& file_name);
    IniFile();
    ~IniFile();

    //加载ini文件
    void load_file(std::string& file_name);
    //写入文件
    void write();
    //获取字符串类型value值
    std::string get_string_value(const std::string& section, const std::string& key);
    //获取int类型value值
    int get_int_value(const std::string& section, const std::string& key);
    //获取double类型值
    double get_double_value(const std::string& section, const std::string& key);

    //设置字符串类型值
    void set_value(const std::string& section, const std::string& key, const std::string& value);
    //设置int类型value值
    void set_value(const std::string& section, const std::string& key, int value);
    //设置double类型value值
    void set_value(const std::string& section, const std::string& key, double value);

    IniSecticonLine& operator[](const std::string& section);

private:
    //添加注释行 空行
    void add_comment_line(std::string& comment);
    //添加section行
    void add_section_line(std::string& section, std::string& comment);
    //添加键值对行
    void add_key_val_line(std::string& key, std::string& val, std::string& comment);

    //抛出解析异常
    void throw_parse_error(std::string& line);
    //抛出无section异常
    void throw_no_section(std::string& line);

    void pasrse_assert(bool b, std::string& line);

private:
    //文件名称
    std::string m_file_name;
    //行列表
    std::list<IniLine*> m_lines;
    //section行链表
    std::list<IniSecticonLine*> m_section_lines; 
};


}
#endif