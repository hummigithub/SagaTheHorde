class CfgPatches
{
    class SagaTheHorde
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data","DZ_Scripts"};
    };
};

class CfgMods
{
    class SagaTheHorde
    {
        dir = "SagaTheHorde";
        picture = "";
        action = "";
        hideName = 0;
        hidePicture = 0;
        name = "SagaTheHorde";
        credits = "";
        author = "hummigummi";
        authorID = "";
        version = "1.0.0";
        type = "mod";
        dependencies[] = {"Game","World","Mission"};
        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"SagaTheHorde/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"SagaTheHorde/scripts/5_Mission"};
            };
        };
    };
};

class CfgSoundShaders
{
    class STH_Teleport_SoundShader
    {
        samples[] = { {"\SagaTheHorde\data\teleport.ogg", 1} };
        range = 60;
        volume = 1.2;
    };
};

class CfgSoundSets
{
    class STH_Teleport_SoundSet
    {
        soundShaders[] = {"STH_Teleport_SoundShader"};
        volumeFactor = 1.2;
        spatial = 1;
        doppler = 0;
        loop = 0;
    };
};
