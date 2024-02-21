using System;
using System.IO;
using System.Text;
using System.Collections;
using System.Xml;

namespace utility
{

class XmlUtil
{
    public static XmlElement GetFirstElement(XmlNode node)
    {
        IEnumerator en = node.GetEnumerator();
        while (en.MoveNext())
        {
            if (en.Current is XmlElement)
            {
                return (XmlElement)en.Current;
            }
        }
        return null;
    }
    public static XmlElement GetFirstElement(XmlNodeList node)
    {
        IEnumerator en = node.GetEnumerator();
        while (en.MoveNext())
        {
            if (en.Current is XmlElement)
            {
                return (XmlElement)en.Current;
            }
        }
        return null;
    }
    public static XmlElement GetFirstElement(XmlDocument doc, string sTagName)
    {
        return GetFirstElement(GetElementsByTagName(doc, sTagName));
    }
    public static XmlNodeList GetElementsByTagName(XmlDocument doc, string sTagName)
    {
        return GetFirstElement(doc).GetElementsByTagName(sTagName);
    }
    public static XmlNodeList GetElementsByTagName(string sFileName, string sTagName)
    {
        XmlDocument doc = new XmlDocument();
        doc.Load(sFileName);
        return GetElementsByTagName(doc, sTagName);
    }
    public static string GetText(XmlNode node)
    {
        return GetText(node, false);
    }
    public static string GetText(XmlNode node, bool bRecurse)
    {
        return GetText(node, null, null, bRecurse);
    }
    public static string GetText(XmlNode node, string sFileAttribute, string sTextNodeAttribute, bool bRecurse)
    {
        string s = null;
        if (!(node is XmlComment) && node is XmlCharacterData)
        {
            s = ((XmlCharacterData)node).Data;
        }
        else if (node is XmlElement)
        {
            StringBuilder sb    = new StringBuilder();
            string        sFile = GetAttributeAsFile(node, sFileAttribute);
            sb.Append(sFile == null ? "" : sFile);

            XmlNodeList elements = node.ChildNodes;
            if (elements != null)
            {
                IEnumerator   en = elements.GetEnumerator();
                object u = null;
                while (en.MoveNext())
                {
                    u = en.Current;
                    sFile = GetTextFileNode((XmlNode)u, sTextNodeAttribute, sFileAttribute);
                    if (sFile != null)
                    {
                        sb.Append(sFile);
                    }
                    else if (u is XmlCharacterData || (bRecurse && (u is XmlElement)))
                    {
                        sb.Append(GetText((XmlNode)u));
                    }
                }
                s = sb.ToString();
                if (s.Length == 0)
                {
                    s = null;
                }
            }
        }
        return s;
    }
    public static IDictionary SetText(XmlNode node, string sPropertyName, IDictionary map)
    {
        string sText = XmlUtil.GetText(node);
        if (sText != null)
        {
            map[sPropertyName] = sText;
        }
        return map;
    }
    public static IDictionary CopyAttributes(XmlNode node, IDictionary map)
    {
        if (node != null)
        {
            XmlAttributeCollection attrs = node.Attributes;
            if (attrs != null)
            {
                IEnumerator i = attrs.GetEnumerator();
                XmlAttribute attr;
                while (i.MoveNext())
                {
                    attr = (XmlAttribute)i.Current;
                    map[attr.Name] = attr.Value;
                }
            }
        }
        return map;
    }
    public static IDictionary CopyTextProperties(XmlNodeList aNodes, IDictionary map)
    {
        if (aNodes != null)
        {
            IEnumerator i = aNodes.GetEnumerator();
            XmlNode node  = null;
            string  sText = null;
            while (i.MoveNext())
            {
                node = (XmlNode)i.Current;
                sText = GetText(node);
                if (node.Name != null && sText != null)
                {
                    map[node.Name] = sText;
                }
            }
        }
        return map;
    }
	public static string GetExpectedAttribute(XmlNode node, string sName, string sTagName)
	{
		string sAttr = GetAttribute(node, sName);
		if (sAttr == null)
		{
			throw new ArgumentException(Utility.Format(
				                         sTagName == null ? 
                                          "attribute '{0}' expected in element" :
				                          "attribute '{0}' expected in element <{1} ../>",
				                          sName, sTagName));
		}
		return sAttr;
	}
    public static string GetTextFileNode(XmlNode node, string sName, string sPathAttributeName)
    {
        if (node != null && sName != null && sPathAttributeName != null)
        {
            XmlElement el    = null;
            if (node is XmlElement)
            {
                el = (XmlElement)node;
                if (el.Name != null && el.Name.Equals(sName))
                {
                    return GetExpectedAttributeAsFile(node, sPathAttributeName, sName);
                }
            }
        }
        return null;
    }
    public static string GetAttributeAsFile(XmlNode node, string sName)
    {
        return ReadText(GetAttribute(node, sName));
    }
    public static string GetExpectedAttributeAsFile(XmlNode node, string sName, string sTagName)
    {
        return ReadText(GetExpectedAttribute(node, sName, sTagName));
    }
	public static string GetAttribute(XmlNode node, string sName)
	{
		if (node != null && sName != null && sName.Trim().Length > 0)
		{
			XmlAttributeCollection attrs = node.Attributes;
			if (attrs == null)
				return null;
			XmlAttribute attr = attrs[sName];
			return attr == null ? null : attr.Value;
		}
        return null;
	}
	public static bool GetBool(XmlNode node, string sName, bool bNVal)
	{
		try { return GetBool(node, sName); }
		catch {}
		return bNVal;
	}
	public static bool GetBool(XmlNode node, string sName)
	{
		string s = GetAttribute(node, sName);
		if (s != null)
		{
			if (s.Equals("true"))
				return true;
			else if (s.Equals("false"))
				return false;
		}
		throw new ArgumentException(Utility.Format("expected true/false for attribute '{0}'", sName));
	}
	public static int GetInt(XmlNode node, string sName)
	{
		try { return Int32.Parse(GetAttribute(node, sName)); }
		catch { throw new ArgumentException(Utility.Format("expected integer value for attribute '{0}'", sName)); }
	}
	public static int GetInt(XmlNode node, string sName, int nNVal)
	{
		try { return GetInt(node, sName); }
		catch {}
		return nNVal;
	}

    // private..
    private static string ReadText(string sPath)
    {
        if (sPath != null)
        {
            TextReader r = new StreamReader(sPath);
            return r.ReadToEnd();
        }
        return null;
    }
}

}
