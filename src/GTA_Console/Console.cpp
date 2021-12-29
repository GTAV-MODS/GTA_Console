#include "stdafx.h"

Console::Console()
{
    memset(InputBuf, 0, sizeof(InputBuf));
    memset(Commands::InputBuf, 0, sizeof(Commands::InputBuf));
    HistoryPos = -1;
    AutoScroll = true;
    ScrollToBottom = false;
}

void Console::ShowConsole(bool* showConsole)
{
    Console::DrawConsole("Console", showConsole);
}

void Console::AddLog(const char* fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    Items.push_back(Utils::Strdup(buf));
}

void Console::ClearLog()
{
    for (int i = 0; i < Items.Size; i++)
        free(Items[i]);

    Items.clear();
}


int Console::TextEditCallback(ImGuiInputTextCallbackData* data)
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

char** Console::str_split(char* a_str, const char a_delim)
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
        assert(idx == count - 1 && ".");
        *(result + idx) = 0;
    }

    return result;
}



void Console::DrawConsole(const char* title, bool* showConsole)
{
    //Get our viewport : 
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    //Set the IMGUI ON TOP POSITION X : 0 Y : 0 of the actual screen resolution.
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    //Change the Y viewport size to 200 and let the X to take the full screen : 
    viewport->Size.y = 200;
    ImGui::SetNextWindowSize(viewport->Size);

    //Some property of the console : 
    const static bool NO_TITLEBAR = false;
    const static bool NO_MOVE = true;
    const static bool NO_RESIZE = true;
    ImGuiWindowFlags window_flags = 0;
    if (NO_TITLEBAR)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (NO_MOVE)            window_flags |= ImGuiWindowFlags_NoMove;
    if (NO_RESIZE)          window_flags |= ImGuiWindowFlags_NoResize;

    if (!ImGui::Begin(title, showConsole, window_flags))
    {
        ImGui::End();
        return;
    }

    ImGui::PushItemWidth(ImGui::GetWindowWidth());


    //Some buttons : 
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close Console"))
            *showConsole = false;
        ImGui::EndPopup();
    }

    if (ImGui::SmallButton("Clear"))
        Console::ClearLog();

    ImGui::SameLine();

    if (ImGui::SmallButton("Enable/Disable the external debug console"))
    {
        if (isExternConsoleOpen)
            ShowWindow(GetConsoleWindow(), SW_SHOW);
        else
            ShowWindow(GetConsoleWindow(), SW_HIDE);

        isExternConsoleOpen = !isExternConsoleOpen;
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

    for (int i = 0; i < Items.Size; i++)
    {
        const char* item = Items[i];
        ImVec4 color;
        bool has_color = false;

        if (strstr(item, "[error]")) { color = ImColor(255, 0, 0); has_color = true; }
        else if (strstr(item, "[success]")) { color = ImColor(0, 255, 0); has_color = true; }
        else if (strstr(item, "[warning]")) { color = ImColor(255, 69, 0); has_color = true; }
        else if (strstr(item, "[info]")) { color = ImColor(112, 128, 144); has_color = true; }
        else if (strncmp(item, "#", 2) == 0) { color = ImColor(220, 220, 220); has_color = true; }
        if (has_color)
            ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(item);
        if (has_color)
            ImGui::PopStyleColor();
    }

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
        while (input_end > InputBuf && input_end[-1] == ' ')
            input_end--; *input_end = 0;

        //Fix avoid having empty space before having enter an command :
        if (Utils::StartsWith(InputBuf, " "))
            strcpy(InputBuf, "");

       if (!Commands::lastArgs.empty())
            Commands::lastArgs.erase(Commands::lastArgs.begin(), Commands::lastArgs.end());

       if (InputBuf[0])
       {
           args = str_split(InputBuf, ' ');

           //Check if the args exist : 
           if (args)
           {
               //Loop though all arguments :
               for (int i = 1; *(args + i); i++)
               {
                   //printf("commande loop %s |args  = [%s]\n\n\n", InputBuf, *(args + i));
                   if (*(args + i) != 0)
                   {
                       Commands::lastArgs.push_back(*(args + i));
                       free(*(args + i));
                   }
               }

               //Only free the memory when needed :
               free(args);
           }
           strcpy(Commands::InputBuf, InputBuf);
           Commands::callExecCommand = true;
       }
        strcpy(InputBuf, "");
    }

    ImGui::SetItemDefaultFocus();


    if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}
