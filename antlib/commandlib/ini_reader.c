#include "ini_reader.h"
#include "alloc.h"

struct ini_node
{
	char _str[1000];
	char* _val;
	struct ini_node* _next;
};

struct ini_section
{
	char _sec_key[200];
	struct ini_node* _head;
	struct ini_node* _tail;
	struct ini_section* _next;
};

struct ini_config
{
	struct ini_section* _head;
	struct ini_section* _tail;
	struct ini_section* _cur;
};


int _read_ini_line(FILE* f, char* buf, int len)
{
	int i, j;
	char ch;
	for (i = 0, j = 0; i < len; j++)
	{
		if (fread(&ch, 1, 1, f) != 1)
		{
			if (feof(f) != 0 && j > 0)
				break;
			return -1;
		}
		else
		{
			if (ch == '\n' || ch == '\0')
				break;
			if (ch == '\r')
				continue;
			buf[i++] = ch;
		}
	}
	buf[i] = '\0';
	return i;
}

void _parse_ini_line(struct ini_config* config,char* buf, int len)
{
	int j = 0;
	int i;
	for (i = 0; i < len; i++)
	{
		if (buf[i] == '#')
			break;
		else if (buf[i] != ' ' && buf[i] != '\t')
			buf[j++] = buf[i];
	}
	buf[j] = 0;
	if (j == 0) { return; }
	
	if (buf[0] == '[')
	{
		if (j > 2 && buf[j - 1] == ']' && j < 200)
		{
			struct ini_section* s = (struct ini_section*)my_malloc(sizeof(struct ini_section));
			memset(s, 0, sizeof(*s));
			strncpy(s->_sec_key, buf + 1, j-2);
			if (config->_tail)
			{
				config->_tail->_next = s;
			}
			else
			{
				config->_head = s;
			}
			config->_tail = s;
			config->_cur = s;
		}
	}
	else if (config->_cur && buf[0] != '=')
	{
		struct ini_section* list = config->_cur;
		struct ini_node* node = (struct ini_node*)my_malloc(sizeof(struct ini_node));
		memset(node, 0, sizeof(*node));
		strncpy(node->_str, buf,j);
		for (i = 0; i < j; i++)
		{
			if (node->_str[i] == '=')
			{
				node->_str[i] = 0;
				node->_val = node->_str + i + 1;
				break;
			}
		}
		if (NULL == node->_val)
		{
			my_free(node);
			return;
		}
		if (list->_tail)
			list->_tail->_next = node;
		else
			list->_head = node;
		list->_tail = node;
	}
}

void* read_ini_file(const char* file_name)
{
	FILE* f = fopen(file_name, "r");
	if (NULL == f) {
		return NULL;
	}
	char buf[1000];
	int len;
	struct ini_config* config = (struct ini_config*)my_malloc(sizeof(struct ini_config));
	memset(config, 0, sizeof(*config));
	while (1)
	{
		len = _read_ini_line(f, buf, sizeof(buf));
		if (len > 0) 
		{
			_parse_ini_line(config, buf, len);
		} 
		else if (len == -1)
		{
			break;
		}
	}
	fclose(f);
	return config;
}


struct ini_section* get_section(void* f, const char* session)
{
	struct ini_config* config = (struct ini_config*)f;
	if (config->_cur == NULL)
	{
		return NULL;
	}

	if (strcmp(config->_cur->_sec_key, session) == 0)
	{
		return config->_cur;
	}
	else
	{
		struct ini_section* list = config->_head;
		while (list) {
			if (strcmp(list->_sec_key, session) == 0)
			{
				config->_cur = list;
				return list;
			}
			list = list->_next;
		}
	}
	return NULL;
}

int is_ini_session(void* f, const char* session)
{
	return get_section(f,session) == NULL ? -1:0;
}

int get_value_int(void* f, const char* session, const char* key,int def)
{
	struct ini_section* list = get_section(f,session);
	if (list == NULL)
		return def;
	struct ini_node* node = list->_head;
	while (node)
	{
		if (strcmp(node->_str, key) == 0)
			return atoi(node->_val);
		node = node->_next;
	}
	return def;
}
const char* get_value_string(void* f, const char* session, const char* key,const char* def)
{
	struct ini_section* list = get_section(f, session);
	if (list == NULL)
		return def;
	struct ini_node* node = list->_head;
	while (node)
	{
		if (strcmp(node->_str, key) == 0)
			return node->_val;
		node = node->_next;
	}
	return def;

}
void close_ini_file(void* f)
{
	struct ini_config* config = (struct ini_config*)f;
	struct ini_node* node;
	struct ini_node* nrm;
	struct ini_section* srm;
	struct ini_section* s = config->_head;
	while (s)
	{
		node = s->_head;
		while (node)
		{
			nrm = node;
			node = node->_next;
			my_free(nrm);
		}
		srm = s;
		s = s->_next;
		my_free(srm);
	}
	my_free(config);
}
