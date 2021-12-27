#pragma once

#define IM_FMTARGS(FMT)


struct Console
{
    bool AutoScroll;
    char InputBuf[256];
    bool isExternConsoleOpen;
    static inline int HistoryPos;
    static inline bool ScrollToBottom;
    static inline ImVector<char*> History;
    static inline ImVector<char*> Items;


    Console();
    void ShowConsole(bool* showConsole);
    static void AddLog(const char* fmt, ...);
    static void ClearLog();
    int TextEditCallback(ImGuiInputTextCallbackData* data);
    char** str_split(char* a_str, const char a_delim);
    void DrawConsole(const char* title, bool* showConsole);
};