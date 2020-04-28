#include "log_formatter.h"

namespace zh
{
    

LogFormatter::LogFormatter(std::string& pattern) 
    : m_pattern(pattern)
{
    init();
}

void LogFormatter::init()
{
#define XX(str, C) m_item_ptr_map[str] = FormatterItem::ptr(new C())

    XX('m', MessageFormatItem);
    XX('t', ThreadIdFormatterItem);
    XX('F', CoroutineIdFormatItem);
    XX('p', LevelFormatItem);
    XX('l', LineFormatterItem);
    XX('D', DataTimeFormatterItem);

#undef XX

    int start = 0;
    int i = 0;
    for (; i < m_pattern.size(); i++)
    {
        while (i < m_pattern.size() && m_pattern[i] != '%') { i++; }

        //最后一位为'%' 或 不存在的转义字符 跳过
        if (i == m_pattern.size() - 1 || m_item_ptr_map.find(m_pattern[i + 1]) == m_item_ptr_map.end()) 
        {
            i++;
            continue;
        }
        //转义字符
        else if (i < m_pattern.size() - 1)
        {
            //添加非转义字符
            m_item_ptr_vec.push_back(FormatterItem::ptr(
                    new StringFormatterItem(m_pattern.substr(start, i - start))));
            //添加转义字符
            m_item_ptr_vec.push_back(m_item_ptr_map[m_pattern[i + 1]]);
            i += 2;
            start = i;
        }
    }

    if (start < i)
    {
        m_item_ptr_vec.push_back(StringFormatterItem::ptr(
                new StringFormatterItem(m_pattern.substr(start, i - start))));
    }
}

std::string LogFormatter::format(LogEvent& event)
{
    for (auto& i : m_item_ptr_vec)
        i->format(event);
    return event.get_content();
}


}