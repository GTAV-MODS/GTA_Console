#include "stdafx.h"
#include "Commands.h"


#pragma region MAIN_LOGIC_COMMANDS_STUFF
void Commands::RegisterCommand(const char* command, const char* description, void (*ptr)(std::vector<std::string>&))
{
    if (!Commands::CommandList.empty())
    {
        auto cmd = Commands::CommandList.find(command);
        if (cmd != Commands::CommandList.end())
        {
            printf("This command %s has already been added. \n", command);
            return;
        }
        else
        {
            Commands::cmdDescription.push_back(description);
            Commands::CommandList.insert(std::pair<std::string_view, void (*)(std::vector<std::string>&)>(command, (*ptr)));
            printf("New command has been added ! %s %s\n", command, description);
            return;
        }
    }
    else
    {
        Commands::cmdDescription.push_back(description);
        Commands::CommandList.insert(std::pair<std::string_view, void (*)(std::vector<std::string>&)>(command, (*ptr)));
        printf("New command has been added ! %s %s\n", command, description);
        return;
    }
}

//DO NOT TOUCH THIS :
void Commands::ExecCommand()
{
    Commands::callExecCommand = false;

    Console::HistoryPos = -1;
    for (int i = Console::History.Size - 1; i >= 0; i--)
        if (Utils::Stricmp(Console::History[i], Commands::InputBuf) == 0)
        {
            free(Console::History[i]);
            Console::History.erase(Console::History.begin() + i);
            break;
        }
    Console::History.push_back(Utils::Strdup(Commands::InputBuf));


    if (!Commands::CommandList.empty())
    {
        auto cmd = Commands::CommandList.find(Commands::InputBuf);
        if (cmd != Commands::CommandList.end())
        {
            cmd->second(Commands::lastArgs);
            return;
        }
        else
        {
            Console::AddLog("[error] Command  %s not found  \n", Commands::InputBuf);
            return;
        }
    }

    // On command input, we scroll to bottom even if AutoScroll==false
    Console::ScrollToBottom = true;
    return;
}

#pragma endregion



#pragma region Your_Commands_Logic


//COMMANDS FOR SPAWNING A VEHICLE :
void Commands::SpawnVehicle(const char* model, int R, int G, int B)
{
    int Handle = PLAYER::PLAYER_PED_ID(), VehicleHash = GET_HASH_KEY(model);
    Vector3 MyCoords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(Handle, 0.0, 4.0, 0.0);


    if (STREAMING::IS_MODEL_IN_CDIMAGE(VehicleHash)) {
        STREAMING::REQUEST_MODEL(VehicleHash);
        while (!STREAMING::HAS_MODEL_LOADED(VehicleHash)) WAIT(0);

        Vehicle veh = VEHICLE::CREATE_VEHICLE(VehicleHash, MyCoords.x, MyCoords.y, MyCoords.z + 1.f, ENTITY::GET_ENTITY_HEADING(Handle), 1, 0, 0);
        VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, R, G, B);
        STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(VehicleHash);

        if (R == 255 && G == 255 && B == 255)
            Console::AddLog("[success] Vehicle %s successfully spawned with default color [R: %d G: %d B: %d].\n", model, R, G, B);
        else 
            Console::AddLog("[success] Vehicle %s successfully spawned with color : [R: %d G: %d B: %d].\n", model, R, G, B);
    }
    else
    {
        Console::AddLog("[error] Vehicle %s not found !\n", model);
    }
}



//COMMANDS FOR SPAWNING PEDS WITH PATTERNS :
void Commands::SpawnPedsPattern(const int MAX_PEDS)
{
    const Hash ped_model = GET_HASH_KEY("a_m_y_runner_01");
    int Handle = PLAYER::PLAYER_PED_ID();
    Vector3 MyCoords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(Handle, 0.0, 50.f, 0.0);


    if (STREAMING::IS_MODEL_IN_CDIMAGE(ped_model) && STREAMING::IS_MODEL_VALID(ped_model))
    {
        printf("\nPed : %ld found.\n", ped_model);

        STREAMING::REQUEST_MODEL(ped_model);
        while (!STREAMING::HAS_MODEL_LOADED(ped_model))
            WAIT(0);

        //Get the z ground coord ; 
        MISC::GET_GROUND_Z_FOR_3D_COORD(MyCoords.x, MyCoords.y, MyCoords.z + 1000.0f, &MyCoords.z, 0, 0);

        for (int col = 0; col < MAX_PEDS; col += 2)
        {
            for (int row = 0; row <= col; row += 2)
            {
                printf("%d", row);
                PED::CREATE_PED(0, ped_model, MyCoords.x + col, MyCoords.y + row, MyCoords.z, ENTITY::GET_ENTITY_HEADING(Handle) + -180.f, FALSE, FALSE);
                STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(ped_model);
            }

            printf("\n");
        }
    }
  
}



#pragma endregion


#pragma region INIT_Your_Commands_Here
//Add your commands and interaction here : 
void Commands::InitCommands()
{
    //COMMAND : /help that allow you to watch all the commands added : 
    Commands::RegisterCommand("/help", "Usage : /help (Get the list of all available commands)", [](std::vector<std::string>& args) {
        if (!Commands::CommandList.empty())
        {
            Console::AddLog("List of colors available :");
            Console::AddLog("[error] Red Color.");
            Console::AddLog("[success] Green Color.");
            Console::AddLog("[warning] Orange Color.");
            Console::AddLog("[info] Gray Color.");
            Console::AddLog("[#] Gray bold Color.");
            Console::AddLog("\n \n");


            Console::AddLog("List of available commands :");
            int index = 0;
            std::sort(Commands::cmdDescription.begin(), Commands::cmdDescription.end());
            for (std::map<std::string_view, void (*)(std::vector <std::string>&)>::iterator i = Commands::CommandList.begin(); i != Commands::CommandList.end(); i++)
            {
                Console::AddLog("[info] %s : [%s]", (const char*)i->first.data(), (const char*)Commands::cmdDescription[index].data());
                index++;
            }
            index = 0;
        }
    });


    //COMMAND : /veh [255 255 255] let you spawn a vehicle with colors of your choice using RGB args [for e.g : /veh adder 255 0 0] will spawn you an red adder.
    Commands::RegisterCommand("/veh", "Usage : /veh [model] [number R number G number B] (Spawn a vehicle with customs colors)", [](std::vector<std::string>& args)
    {
            if (!args.empty())
            {
                int R{ 0 }, G{ 0 }, B{ 0 };

                //Check if the args contain 3 or more then we call the car with the colors args else we only spawn a car with white default color : 
                if (args.size() >= 3)
                {
                    try
                    {
                        if (args[1].length() > 0 && args[2].length() > 0 && args[3].length() > 0)
                        {
                            R = std::stoi(args[1].data());
                            G = std::stoi(args[2].data());
                            B = std::stoi(args[3].data());
                        }

                        Commands::SpawnVehicle((const char*)args[0].data(), R, G, B);
                    }
                    catch (...)
                    {
                        Console::AddLog("[error] Please set a correct color to your car using RGB : e.g /veh adder 255 255 255\n");
                    }
                }
                else
                {
                    Commands::SpawnVehicle((const char*)args[0].data());
                }
            }
    });
    
    
    //COMMAND : /peds [number of peds] let you spawn the number of peds with a triangle pattern [for e.g : /peds 30] becareful with that you can crash if you expose to many peds in front you.
    Commands::RegisterCommand("/peds", "Usage : /peds [numbers of peds to spawn] (Spawn x numbers of peds with pyramid pattern).", [](std::vector<std::string>& args) {
        //Check if the args is not empty : 
        if (!args.empty())
        {
            try
            {
                if (std::stoi(args[0].data()) < 100)
                {
                    Console::AddLog("[success] %d peds spawned.\n", std::stoi(args[0].data()));
                    Commands::SpawnPedsPattern(std::stoi(args[0].data()));
                }
                else
                {
                    Console::AddLog("[warning] By spawning more than 100 peds, You must spawn them far from you by risk of having fps drop or even worst a crash.\n");
                    Commands::SpawnPedsPattern(std::stoi(args[0].data()));
                }
            }
            catch (...)
            {
                Console::AddLog("[error] Please set a correct number of peds to spawn.\n");
            }
        }
        else
        {
            Console::AddLog("[error] Please set a correct number of peds to spawn.\n");
        }
    });


    //COMMAND : /god [1/0] let you be in godmode [for e.g : /god 1].
    Commands::RegisterCommand("/god", "Usage : /god [1/0] (Let you be in GodMode)", [](std::vector<std::string>& args) {
        //Check if the args is not empty : 
        if (!args.empty())
        {
            try
            {
                if (std::stoi(args[0].data()) == 1)
                {
                    ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::PLAYER_PED_ID(), TRUE);
                    Console::AddLog("[success] GodMode successfully enabled.\n");
                }
                else
                {
                    ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::PLAYER_PED_ID(), FALSE);
                    Console::AddLog("[success] GodMode successfully disabled.\n");
                }
            }
            catch (...)
            {
                Console::AddLog("[error] Please set a correct number between 1 and 0 (1 to true, 0 to false).\n");
            }
        }
        else
        {
            Console::AddLog("[error] Please set a correct number between 1 and 0 (1 to true, 0 to false).\n");
        }
    });



    //COMMAND : /clear let you clear all the previous commands.
    Commands::RegisterCommand("/clear", "Usage : /clear (Clear all the previous command)", [](std::vector<std::string>& args) {Console::ClearLog(); });


    //COMMAND : /quit simply terminate the actual process(GTA V).
    Commands::RegisterCommand("/quit", "Usage : /quit (Let you exit the game)", [](std::vector<std::string>& args) {TerminateProcess(GetCurrentProcess(), 0); });
}

#pragma endregion