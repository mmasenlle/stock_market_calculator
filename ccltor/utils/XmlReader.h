#ifndef _XMLREADER_H_
#define _XMLREADER_H_

#include <map>
#include <vector>
#include <string>

class XmlReader
{
	const char *root_label;
	int state;
	int ign_deep;
	int offset;
	std::string buffer;
	const char *cur_value;
	XmlReader *cur_tree;

protected:
	std::map<const char *, std::vector<std::string> > values;
	std::map<const char *, std::vector<XmlReader *> > trees;
	
	bool match_string(const char *str);

	virtual const char * match_value(int hint) = 0;
	virtual XmlReader * match_tree(int hint) = 0;
	virtual void done() {};
	
	XmlReader(const char *root);

public:
	~XmlReader();
	int read(const char *buf, int len);
};

#endif /*_XMLREADER_H_*/
