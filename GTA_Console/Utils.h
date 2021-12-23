#pragma once

#include "stdafx.h"

#define WINVER 0x0500

//FIXER LE BUG AVEC LE MODE FOCUS, 
//PROBLEME RENCONTRER : 
/*
    UNE FOIS SUR L'INPUT TEXT, TOUT FONCTIONNE BIEN JUSQU'AU MOMENT OU EN SORT DU MODE "FOCUS" PUIS QU'IL FOCUS SUR AUTRE CHOSE EXEMPLE CONSOLE EXTERNE
    DETECT EN LOOP NOTRE TEXT PUIS CASSE LE SYSTEM DE CHECK VALIDER ON ENTER.
    ENUM BROKEN UNE FOIS LE MODE FOCUS EXIT : ImGuiInputTextFlags_EnterReturnsTrue 
 */



//Set the text color to GREEN : 
#define LOG_SUCCESS()                         \
{                                          \
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN); \
}

//Set the text color to RED : 
#define LOG_ERROR()                         \
{                                          \
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED); \
}

//Set the text color to INTENSITY : 
#define LOG_INFO()                         \
{                                          \
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); \
	SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY); \
}




namespace Utils
{
   void  Strtrim(char* s);
   int	 Stricmp(const char* s1, const char* s2);
   int	 Strnicmp(const char* s1, const char* s2, int n);
   char* Strdup(const char* s);
   int   STRCMP(const char* s1, const char* s2);
   std::vector<std::string> split(std::string x, char delim);
}


struct Console
{
    ImVector<char*> Items;
    ImVector<char*> History;
    bool AutoScroll;
    bool ScrollToBottom;
    char InputBuf[256];
    int HistoryPos;

    std::map < std::string_view, void (*)(std::vector <std::string>)> UserCommandList;
    std::vector<std::string> lastArgs;

    #define IM_FMTARGS(FMT)

    void RegisterCommand(const char* command, void (*ptr)(std::vector<std::string>))
      {
        if (!UserCommandList.empty())
        {
            auto cmd = UserCommandList.find(command);
            if (cmd != UserCommandList.end())
            {
                printf("This command %s has already been added. \n", command);
                return;
            }
            else
            {
                UserCommandList.insert(std::pair<std::string_view, void (*)(std::vector<std::string>)>(command, (*ptr)));
                printf("New command has been added ! %s \n", command);
                return;
            }
        }
        else
        {
            UserCommandList.insert(std::pair<std::string_view, void (*)(std::vector<std::string>)>(command, (*ptr)));
            printf("New command has been added ! %s \n", command);
            return;
        }
     }

    Console()
    {
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;
        AutoScroll = true;
        ScrollToBottom = false;
    }

    void ShowConsole(bool* showConsole)
    {
        Console::DrawConsole("Console", showConsole);
    }

    void AddLog(const char* fmt, ...)
    {
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf) - 1] = 0;
        va_end(args);
        Items.push_back(Utils::Strdup(buf));
    }

    void ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);

        Items.clear();
    }


    int TextEditCallback(ImGuiInputTextCallbackData* data)
    {
        switch (data->EventFlag)
        {
            case ImGuiInputTextFlags_CallbackHistory:
            {
                // Example of HISTORY
                const int prev_history_pos = HistoryPos;
                if (data->EventKey == ImGuiKey_UpArrow)
                {
                    if (HistoryPos == -1)
                        HistoryPos = History.Size - 1;
                    else if (HistoryPos > 0)
                        HistoryPos--;
                }
                else if (data->EventKey == ImGuiKey_DownArrow)
                {
                    if (HistoryPos != -1)
                        if (++HistoryPos >= History.Size)
                            HistoryPos = -1;
                }

                // A better implementation would preserve the data on the current input line along with cursor position.
                if (prev_history_pos != HistoryPos)
                {
                    const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, history_str);
                }
            }
        }
        return 0;
    }

    void ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Utils::Stricmp(History[i], command_line) == 0)
            {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Utils::Strdup(command_line));


        if (!UserCommandList.empty())
        {
            auto cmd = UserCommandList.find(command_line);
            if (cmd != UserCommandList.end())
            {
                cmd->second(lastArgs);
                return;
            }
            else
            {
                AddLog("[error] Command  %s not found  \n", command_line);
                return;
            }
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    char** str_split(char* a_str, const char a_delim)
    {
        char** result = 0;
        size_t count = 0;
        char* tmp = a_str;
        char* last_comma = 0;
        char delim[2];
        delim[0] = a_delim;
        delim[1] = 0;

        /* Count how many elements will be extracted. */
        while (*tmp)
        {
            if (a_delim == *tmp)
            {
                count++;
                last_comma = tmp;
            }
            tmp++;
        }

        /* Add space for trailing token. */
        count += last_comma < (a_str + strlen(a_str) - 1);

        /* Add space for terminating null string so caller
           knows where the list of returned strings ends. */
        count++;

        result = (char**)malloc(sizeof(char*) * count);

        if (result)
        {
            size_t idx = 0;
            char* token = strtok(a_str, delim);

            while (token)
            {
                assert(idx < count && "No Character found on sending input.");
                *(result + idx++) = strdup(token);
                token = strtok(0, delim);
            }

            //printf("IDX : %d  COUNT : %d\n", idx, count);
            assert(idx == count - 1 && "No Character found on sending input.");
            *(result + idx) = 0;
        }

        return result;
    }

    void DrawConsole(const char* title, bool* showConsole)
    {
        //We get your actual screen resolution used on GTA V SETTINGS and play with to get a responsive render for the console :
        int x{ 0 }, y{ 0 };
        _GET_ACTIVE_SCREEN_RESOLUTION(&x, &y);

        //Set the IMGUI ON TOP POSITION X : 0 Y : 0 of your screen.
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        //Some responsive stuff : 
        if (x == 1920 and y == 1080)
            ImGui::SetNextWindowSize(ImVec2(1920, 200), ImGuiCond_FirstUseEver);
        else if (x == 1366 and y == 768)
            ImGui::SetNextWindowSize(ImVec2(1366, 200), ImGuiCond_FirstUseEver);
        else if (x == 1360 and y == 768)
            ImGui::SetNextWindowSize(ImVec2(1360, 200), ImGuiCond_FirstUseEver);
        else if (x == 1600 and y == 900)
            ImGui::SetNextWindowSize(ImVec2(1600, 200), ImGuiCond_FirstUseEver);
        else if (x == 1400 and y == 1050)
            ImGui::SetNextWindowSize(ImVec2(1400, 200), ImGuiCond_FirstUseEver);
        else if (x == 1440 and y == 900)
            ImGui::SetNextWindowSize(ImVec2(1440, 200), ImGuiCond_FirstUseEver);
        else if (x == 1680 and y == 1050)
            ImGui::SetNextWindowSize(ImVec2(1680, 200), ImGuiCond_FirstUseEver);
        else if (x == 1280 and y == 720)
            ImGui::SetNextWindowSize(ImVec2(1280, 200), ImGuiCond_FirstUseEver);
        else if (x == 1280 and y == 768)
            ImGui::SetNextWindowSize(ImVec2(1280, 200), ImGuiCond_FirstUseEver);
        else if (x == 1280 and y == 800)
            ImGui::SetNextWindowSize(ImVec2(1280, 200), ImGuiCond_FirstUseEver);
        else if (x == 1280 and y == 960)
            ImGui::SetNextWindowSize(ImVec2(1280, 200), ImGuiCond_FirstUseEver);
        else if (x == 1280 and y == 1024)
            ImGui::SetNextWindowSize(ImVec2(1280, 200), ImGuiCond_FirstUseEver);
        else if (x == 1024 and y == 768)
            ImGui::SetNextWindowSize(ImVec2(1024, 200), ImGuiCond_FirstUseEver);
        else if (x == 800 and y == 600)
            ImGui::SetNextWindowSize(ImVec2(800, 200), ImGuiCond_FirstUseEver);
        else if (x == 1152 and y == 864)
            ImGui::SetNextWindowSize(ImVec2(1152, 200), ImGuiCond_FirstUseEver);
        else if (x == 1280 and y == 600)
            ImGui::SetNextWindowSize(ImVec2(1280, 200), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin(title, showConsole))
        {
            ImGui::End();
            return;
        }


        ImGui::PushItemWidth(ImGui::GetWindowWidth());

        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close Console"))
                *showConsole = false;
            ImGui::EndPopup();
        }

        if (ImGui::SmallButton("Clear"))
            Console::ClearLog();       
        
        ImGui::SameLine();

        bool copy_to_clipboard = ImGui::SmallButton("Copy");
        bool isExternConsoleOpen = false;


        ImGui::SameLine();

        if (ImGui::SmallButton("Enable/Disable the external debug console"))
        {
            isExternConsoleOpen = !isExternConsoleOpen;
            if (isExternConsoleOpen)
                ShowWindow(GetConsoleWindow(), SW_SHOW);
            else
                ShowWindow(GetConsoleWindow(), SW_HIDE);
        }

        ImGui::Separator();

        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear"))
                Console::ClearLog();
            ImGui::EndPopup();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            ImGui::LogToClipboard();

        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            ImVec4 color;
            bool has_color = false;

            if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item);
            if (has_color)
                ImGui::PopStyleColor();
        }


        if (copy_to_clipboard)
            ImGui::LogFinish();
            

        if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);

        ScrollToBottom = false;

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        // Command-line
        if (ImGui::InputTextWithHint("", "enter the command here e.g /help", InputBuf, _countof(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory, [](ImGuiInputTextCallbackData* data) { Console* console = (Console*)data->UserData; return console->TextEditCallback(data); }, (void*)this))
        {
            char** args;
            char* input_end = InputBuf + strlen(InputBuf);
            while (input_end > InputBuf && input_end[-1] == ' ') input_end--; *input_end = 0;

            if (!lastArgs.empty())
                lastArgs.erase(lastArgs.begin(), lastArgs.end()); 

                if (InputBuf[0])
                {
                    args = str_split(InputBuf, ' ');

                    //Check if the args exist : 
                    if (args)
                    {
                        if (*(args + 1) != 0)
                        {
                            //Loop though all arguments :
                            for (int i = 1; *(args + i); i++)
                            {
                                printf("commande loop %s |args  = [%s]\n\n\n", InputBuf, *(args + i));
                                if (*(args + i) != 0)
                                {
                                    lastArgs.push_back(*(args + i));
                                    free(*(args + i));
                                }
                            }
                        }
                    }

                    Console::ExecCommand(InputBuf);
                    free(args);
                }    
           strcpy(InputBuf, "");
        }
        
        ImGui::SetItemDefaultFocus();


        if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
            
       

        ImGui::End();
    }
};