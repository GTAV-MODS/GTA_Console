#pragma once

namespace Utils
{
   void  Strtrim(char* s);
   int	 Stricmp(const char* s1, const char* s2);
   int	 Strnicmp(const char* s1, const char* s2, int n);
   char* Strdup(const char* s);
   int   STRCMP(const char* s1, const char* s2);
   bool  StartsWith(const char* buffer, const char* prefix);
}