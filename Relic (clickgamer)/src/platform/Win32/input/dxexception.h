#ifndef _DXEXCEPTION_H
#define _DXEXCEPTION_H

#include <string>

class DXException : public std::exception
{
public:

    DXException(const char *file, int line, HRESULT result = E_FAIL, const char *message = NULL, const char *text = NULL) :
        m_line(line), m_result(result), m_file(file ? file : ""), m_last(HRESULT_FROM_WIN32(::GetLastError())),
        m_message(message ? message : ""), m_text(text ? text : "")
    {
        int sz = static_cast<int>(m_file.length() + m_message.length() + m_text.length() + 1024);
        char *s = new char [sz];
        sprintf(s, "DESCRIPTION\r\n\r\n%s\r\n%s\r\n(in file '%s' at line %d; HRESULT = %d; LASTHR [may not be relevant] = %d)\r\n",
            m_message.c_str(), m_text.c_str(), m_file.c_str(), m_line, m_result, m_last);
        m_formatted = s;
        delete [] s;
    }
    ~DXException() { show(); }

    const char *file()      const { return m_file.c_str(); }
    int         line()      const { return m_line; }
    HRESULT     last()      const { return m_last; }
    HRESULT     result()    const { return m_result; }
    const char *message()   const { return m_message.c_str(); }
    const char *text()      const { return m_message.c_str(); }
    const char *formatted() const { return m_formatted.c_str(); }
    void        show()      const { ::MessageBox(0, formatted(), "Application Error", MB_OK); }

private:

    std::string  m_file,
                 m_message,
                 m_text,
                 m_formatted;
    int          m_line;
    HRESULT      m_result,
                 m_last;
};

#endif  // _DXEXCEPTION_H