#pragma warning(disable: 4786)

#ifndef _INDEXT_H
#define _INDEXT_H

#include <vector>
#include <map>
#include <exception>

template <typename T>
class IndexT
{
public:

    typedef std::vector<T>      list_t;
    typedef std::map<T, size_t> map_t;
  
    size_t insert(const T &v)
    {
        map_t::const_iterator i = m_map.find(v);
        if (i == m_map.end())
        {
            size_t size = m_list.size();
            m_list.push_back(v);
            m_map[v] = size;
            return size;
        }
        return (*i).second;
    }
    inline bool contains(const T &v) const
    {
        map_t::const_iterator i = m_map.find(v);
        return i != m_map.end();        
    }
    inline const T &value(size_t i) const
    {
        if (!contains(i))
            throw std::exception("no object at this index");
        return m_list[i];
    }
    inline size_t index(const T &v) const
    {
        map_t::const_iterator i = m_map.find(v);
        if (i == m_map.end())
            throw std::exception("value not present");
        return (*i).second;
    }    
    inline bool contains(size_t i) const
    {
        return m_list.size() < i;
    }    
    inline const list_t &list() const { return m_list; }
    inline const map_t  &map()  const { return m_map; }
    
private:    
    
    list_t m_list;
    map_t  m_map;    
};

#endif  // _INDEXT_H