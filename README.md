# GTA_Console
BEFORE EVERYTHING, (Thanks to pongo1231 for having release his source code with Imgui hooked.)
This mod has been push to help Developper, you clearly need to got some basic with C++ before using this mod. (no need to be expert).
This is a simple modern console using IMGUI.
only for Grand Theft Auto V Solo.

## How can i build it ? 
Before to compile the mod, you will need to change the Output directory.
Go to the project/settings make sure all configurations are selected then select general as on the screen.
Now change the Output directory to your game directory. (Make sure to have your MSVC Open in Administrator mode else you can't build it).

## Configuration : 
![Configuration](https://cdn.discordapp.com/attachments/869316172384960552/925150831114870824/unknown.png)



## I can't build it. I have an error like the screen : 
![Screen](https://cdn.discordapp.com/attachments/554479498721099787/948204878902202408/Capture_decran_2022-03-01_124327.png)
Here you can find the solution : https://www.youtube.com/watch?v=kZYuVxO5sHw



## Why did you make that console ?
A made it to help people debug them own code, and demonstrate how to communicate between multiple thread and using Native.
And of course because it was at first a challenge for me as (i'm a beginner with C++).


## Which version should i use to use this mod :
For now, It doesn't work with the last update, since i will not play cat and mouse to update each future update,
I will suggest you to play with an older version for e.g : 1.0.2245.0 work fine with.
If you do have the last version you can easily downgrade your, just type on google/youtube : how to Downgrade GTA V to 1.0.2245.0



## How the console work ? :
Simply add the .asi file into your directory game with of course ScriptHook V and once you will run your game you will see a console open.
So that's mean the mod is correctly injected next just press F8 and enjoy.

## Usage :
### To register a command your callback should always be of type void. : 
```c++
 //Parameter(String_view nameOfYourCommands, Void Function CallBack which will be called once your commands enter, vector of string which will contain your args)
 Commands::RegisterCommand("/yourCommands", "description or your command", [](std::vector<std::string>& args) { Your logic when you enter your command });

//For the exemple, i did use a lambda function, but you can use instead a void function name that you will call outside exemple : .

void myFunctionCBOutside(std::vector<std::string>& args)
{
 //Your logic
}

Commands::RegisterCommand("/yourCommands", "description of your command", myFunctionCBOutside);

```
### To print log in console : 
```c++
 //AddLog can take args as printf do : 
 Console::AddLog("[error] Red message  \n");
 Console::AddLog("[success] Green message  \n");
 Console::AddLog("[warning] Orange message  \n");
 
```

### New Colors available : 
```c++
    Console::AddLog("[error] Red Color.");
    Console::AddLog("[success] Green Color.");
    Console::AddLog("[warning] Orange Color.");
    Console::AddLog("[info] Gray Color.");
    Console::AddLog("[#] Gray bold Color.");
```

## Screen :
![Screen](https://cdn.discordapp.com/attachments/869316172384960552/925147702709325905/unknown.png)


## Video : 
https://www.youtube.com/watch?v=EK4ZSIKgT1Y


--------------||by Super.Cool.Ninja||--------------

