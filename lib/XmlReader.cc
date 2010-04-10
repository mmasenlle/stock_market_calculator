
#include <string.h>
#include <stdio.h>
#include "XmlReader.h"

enum
{
	XMLREADER_ST_NULL = 0,
	XMLREADER_ST_PARTIAL_NULL,
	XMLREADER_ST_LOOKING,
	XMLREADER_ST_PARTIAL_LOOKING,
	XMLREADER_ST_VALUE,
	XMLREADER_ST_TREE,
	XMLREADER_ST_DONE
};

	
XmlReader::XmlReader(const char *root) :
	root_label(root), state(XMLREADER_ST_NULL), cur_value(NULL), cur_tree(NULL)
{
	ign_deep = offset = 0;
}

XmlReader::~XmlReader()
{
	for (std::map<const char *, std::vector<XmlReader*> >::iterator i = trees.begin();
		i != trees.end(); i++)
		for (int j = 0; j < i->second.size(); j++)
			delete i->second[j];
}

bool XmlReader::match_string(const char *str)
{
	for (int i = 0; ; i++)
	{
		if (offset + i >= buffer.length())
		{
			state |= 1;
			return false;
		}
		if (!str[i])
		{
			if (buffer[offset + i] == '>')
			{
				offset += (i + 1);
				return true;
			}
			return false;
		}
		if (buffer[offset + i] != str[i])
		{
			return false;
		}
	}
}

/* Under syntactically incorrect xml the result is undefined */
/* xml inner comments and maybe other things are not supported */
int XmlReader::read(const char *buf, int len)
{
	int offs_shrink = (state == XMLREADER_ST_VALUE) ? 0 : -1;
	int used_len, old_len = buffer.length();
	buffer.append(buf, len);
	
	while (offset < buffer.length())
	{
		switch (state)
		{
		case XMLREADER_ST_DONE:
			len = (offset - old_len);
			offset = buffer.length();
			goto shrink;
		case XMLREADER_ST_PARTIAL_NULL:
		case XMLREADER_ST_PARTIAL_LOOKING:
			while (buffer[offset] != '<') offset--;
			state--;
			goto shrink;
		case XMLREADER_ST_TREE:
			used_len = cur_tree->read(buffer.c_str() + offset, buffer.length() - offset);		
			if (used_len == (buffer.length() - offset))
			{
				offset = buffer.length();
				goto shrink;
			}
			state = XMLREADER_ST_LOOKING;
			if (used_len > 0) offset += used_len;
			continue;
		default:
			while (offset < buffer.length() && buffer[offset] != '<') offset++;
			if (++offset >= buffer.length()) { offset--; goto shrink; }
			if (ign_deep)
			{		
				ign_deep++;
				if (buffer[offset] == '/') ign_deep -= 2;
				continue;
			}
			switch (state)
			{
			case XMLREADER_ST_NULL:
				if (match_string(root_label))
					state = XMLREADER_ST_LOOKING;
				continue;
			case XMLREADER_ST_LOOKING:
				if (buffer[offset] == '/')
				{
					if (++offset >= buffer.length()) { offset -= 2; goto shrink; }
					if (match_string(root_label))
					{
						state = XMLREADER_ST_DONE;
						done_cb();
					}
				}
				else
				{
					if ((cur_value = match_value(buffer[offset])))
					{
						state = XMLREADER_ST_VALUE;
						offs_shrink = offset;
					}
					else if ((cur_tree = match_tree(buffer[offset])))
					{
						state = XMLREADER_ST_TREE;
						cur_tree->state = XMLREADER_ST_LOOKING;
						trees[cur_tree->root_label].push_back(cur_tree);	
					}
					else if (state == XMLREADER_ST_LOOKING)
					{
						ign_deep = 1;
					}
				}
				continue;
			case XMLREADER_ST_VALUE:
				if (buffer[offset] == '/')
				{
					values[cur_value].push_back(buffer.substr(offs_shrink, (offset - 1) - offs_shrink));
					state = XMLREADER_ST_LOOKING;
					offs_shrink = -1;
				}
				else
				{
					ign_deep = 1;
				}
				continue;
			}
		}
	}

shrink:
	if (offs_shrink < 0)
		offs_shrink = offset;
	buffer.erase(0, offs_shrink);
	offset -= offs_shrink;
	return len;
}
