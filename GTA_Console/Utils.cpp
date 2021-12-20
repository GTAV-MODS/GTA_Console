#include "Utils.h"
#include "stdafx.h"




void  Utils::Strtrim(char* s)
{
	char* str_end = s + strlen(s);
	while (str_end > s && str_end[-1] == ' ')
		str_end--;
	*str_end = 0; 
}

int  Utils::Stricmp(const char* s1, const char* s2)
{	int d; 
	while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
	{
		s1++;
		s2++; 
	} 
	return d; 
}

int Utils::STRCMP(const char* s1, const char* s2)
{
	int result = strcmp(s1, s2);
	printf("strcmp(%s, %s) =  %d\n", s1, s2, result);
	return result;
}

int Utils::Strnicmp(const char* s1, const char* s2, int n)
{ 
	int d = 0; 
	while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
	{ 
		s1++; 
		s2++; 
		n--; 
	}
	return d; 
}

char* Utils::Strdup(const char* s)
{ 
	IM_ASSERT(s);
	size_t len = strlen(s) + 1;
	void* buf = malloc(len); 
	IM_ASSERT(buf);
	return (char*) memcpy(buf, (const void*)s, len); 
}