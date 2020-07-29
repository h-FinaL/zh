#include "ini_file.h"

namespace zh
{

IniFile::IniFile()
{}

IniFile::IniFile(std::string& file_name) :
    m_file_name(file_name)
{
    load_file(m_file_name);
}

IniFile::~IniFile()
{}

void IniFile::load_file(std::string& file_name)
{
    m_file_name = file_name;
    std::fstream file(file_name);
    if (!file.is_open())
    {
        throw "not find file " + file_name + "!";
        return;
    }
    
    //行
    std::string line;
    while(std::getline(file, line))
    {
        //注释
        std::string comment;
        //去掉注释
        std::string no_comment;

        //忽略头尾空格
        int left = line.find_first_not_of(' ');
        int right = line.find_last_not_of(' ');      
        
        //全是空格的行或者空行
        if (left == -1 || line == "")
        {
            continue;
        }

        //查找是否存在注释
        int comment_left = line.find_first_of('#');
        //存在注释
        if (comment_left != -1)
        {
            //注释内容
            comment = line.substr(comment_left, right - comment_left + 1);
            //当前整行为注释行
            if (comment_left == left)
            {
                add_comment_line(comment);
                continue;
            }

            //去掉注释，并忽略右边空格
            right = comment_left - 1;
            while (line[right] == ' ') { right--; }
            no_comment = line.substr(left, right - left + 1);
        }
        else
        {
            no_comment = line.substr(left, right - left + 1);
        } 

        //section行
        if (no_comment[0] == '[' && no_comment[no_comment.size() - 1] == ']')
        {
            //如果[ ]中存在空格或为空
            if (no_comment.size() == 2 || no_comment.find_first_of(' ') != -1)
            {
                throw_parse_error(line);
            }
            std::string section = no_comment.substr(1, no_comment.size() - 2);
            add_section_line(section, comment);
            continue;
        }

        //没有先出现首section
        if (m_section_lines.empty())
        {
            throw_no_section(line);
        }

        //查找等号位置
        int equal_p = no_comment.find_first_of('=');
        //如果没有等号
        if (equal_p == -1)
        {
            throw_parse_error(line);
        }

        //如果出现多个等号
        if (equal_p != no_comment.find_last_of('='))
        {
            throw_parse_error(line);
        }

        //key
        std::string key = no_comment.substr (left, equal_p - left);
        //去掉key右边空格
        key = key.substr(0, key.find_last_not_of(' ') + 1);
        //vaule
        std::string value = no_comment.substr(equal_p + 1, right - equal_p);
        //去掉value左边空格
        int value_left = value.find_first_not_of(' ');
        value = value.substr(value_left, value.size() - value_left);
        
        add_key_val_line(key, value, comment);
    }

}

void IniFile::write()
{
    std::fstream file(m_file_name, std::ios::out);
    for (auto i : m_lines)
    {
        i->write_line(file);
    }
    file.close();
}

void IniFile::add_comment_line(std::string& comment)
{
    m_lines.push_back(new IniCommentLine(comment));
}

void IniFile::add_section_line(std::string& section, std::string& comment)
{
    IniSecticonLine* section_line_p = new IniSecticonLine(section, comment);
    m_lines.push_back(section_line_p);
    m_section_lines.push_back(section_line_p);
}

void IniFile::add_key_val_line(std::string& key, std::string& val, std::string& comment)
{
    IniKeyValLine* key_value_line_p = new IniKeyValLine(key, val, comment);
    m_lines.push_back(key_value_line_p);
    //获取当前section
    auto section_it = --m_section_lines.end();
    //放入section中的键值对链表中
    ((*section_it)->m_key_val_lines).push_back(key_value_line_p);
}

std::string IniFile::get_string_value(const std::string& section, const std::string& key)
{
    auto section_it = m_section_lines.begin();
    while ( section_it != m_section_lines.end())
    {
        if ((*section_it)->m_section == section)
        {
            break;
        }
        ++section_it;
    }
    if (section_it == m_section_lines.end())
    {
        std::cout << "not section " << section << std::endl;
        return "";
    }
    auto key_val_it = (*section_it)->m_key_val_lines.begin();
    while (key_val_it != (*section_it)->m_key_val_lines.end())
    {
        if ((*key_val_it)->m_key == key)
        {
            break;
        }
        ++key_val_it;
    }
    if (key_val_it == (*section_it)->m_key_val_lines.end())
    {
        std::cout << "not key: " << key << " in section: " << section;
        return "";
    }
    return (*key_val_it)->m_value;
}

int IniFile::get_int_value(const std::string& section, const std::string& key)
{
    std::string value = get_string_value(section, key);
    return atoi(value.c_str());
}

double IniFile::get_double_value(const std::string& section, const std::string& key)
{
    std::string value = get_string_value(section, key);
    return atof(value.c_str());
}

void IniFile::set_value(const std::string& section, const std::string& key, const std::string& value)
{
    IniFile& ini = *this;
    ini[section][key] = value;
}

void IniFile::set_value(const std::string& section, const std::string& key, int value)
{
    IniFile& ini = *this;
    std::string value_str = std::to_string(value);
    ini[section][key] = value_str;
}

void IniFile::set_value(const std::string& section, const std::string& key, double value)
{
    IniFile& ini = *this;
    std::string value_str = std::to_string(value);
    ini[section][key] = value_str;
}

void IniFile::throw_parse_error(std::string& line)
{
    //当前行
    int n_line = m_lines.size() + 1;
    std::stringstream error;
    error << "Unable to parse " << n_line << " line : " << line;
    throw error.str();
}

void IniFile::throw_no_section(std::string& line)
{
    int n_line = m_lines.size() + 1;
    std::stringstream error;
    error << "File contains no section headers. : " << line;
    throw error.str();
}

void IniFile::IniCommentLine::write_line(std::fstream& file)
{
    file << m_comment << "\n";
}

void IniFile::IniSecticonLine::write_line(std::fstream& file)
{
    file << '[' << m_section << ']';
    if (m_comment != "")
    {
        for (int i = 0; i < ZH_INILINE_COMMENT_INTERVAL; i++)
        {
            file << ' ';
        }
        file << m_comment;
    }
    file << '\n';
}

void IniFile::IniKeyValLine::write_line(std::fstream& file)
{
    file << m_key << " = "<< m_value;
    if (m_comment != "")
    {
        for (int i = 0; i < ZH_INILINE_COMMENT_INTERVAL; i++)
        {
            file << ' ';
        }
        file << m_comment;
    }
    file << '\n';
}

IniFile::IniSecticonLine& IniFile::operator[](const std::string& section)
{
    auto section_it = m_section_lines.begin();
    while (section_it != m_section_lines.end())
    {
        if ((*section_it)->m_section == section)
        {
            break;
        }
        ++section_it;
    }
    if (section_it == m_section_lines.end())
    {
        std::cout << "not section " << section << std::endl;
        throw;
    }
    return *(*section_it);
}

std::string& IniFile::IniSecticonLine::operator[](const std::string& key)
{
    auto key_val_it = m_key_val_lines.begin();
    while (key_val_it != m_key_val_lines.end())
    {
        if ((*key_val_it)->m_key == key)
        {
            break;
        }
        ++key_val_it;
    }
    if (key_val_it == m_key_val_lines.end())
    {
        std::cout << "not key: " << key << " in section: " << m_section;
        throw ;
    }

    return (*key_val_it)->m_value;
}

}