class STH_ZombieEntry
{
    string ClassName;
    bool Always;
    float Chance;
    int CountMin;
    int CountMax;
    ref array<string> Loot;
    bool TeleportEnabled;
    float TeleportDetectRadius;
    int TeleportCooldownSeconds;
    float TeleportCombatSuppressRadius;
    float TeleportMinDistance;
    float HealthStrengthBonus;

    void STH_ZombieEntry()
    {
        Loot = new array<string>;
        Chance = 0.5;
        CountMin = 1;
        CountMax = 1;
        TeleportEnabled = false;
        TeleportDetectRadius = 20.0;
        TeleportCooldownSeconds = 20;
        TeleportCombatSuppressRadius = 10.0;
        TeleportMinDistance = 4.0;
    HealthStrengthBonus = 0.0;
    }
};

class STH_RewardItem
{
    string ClassName;
    int QuantityMin;
    int QuantityMax;
    float Chance; // 0.0 - 1.0

    void STH_RewardItem()
    {
        QuantityMin = 1;
        QuantityMax = 1;
        Chance = 1.0;
    }
};

class STH_HordeLocation
{
    string Name;
    ref array<float> Position;
    float Radius;
    int MinCount;
    int MaxCount;
    float ProgressPlayerRadius;
    ref array<ref STH_ZombieEntry> ZombieClasses;

    void STH_HordeLocation()
    {
        Position = new array<float>;
        ZombieClasses = new array<ref STH_ZombieEntry>;
        Radius = 25.0;
        MinCount = 10;
        MaxCount = 20;
    ProgressPlayerRadius = 0.0;
    }
};

class STH_GlobalSettings
{
    bool UseExpansionNotifications;
    int HordeIntervalSeconds;
    int HordeLifetimeSeconds;
    bool DespawnOnServerRestart;
    int MinPlayersForHordeSpawn;
    string SpawnMessageTemplate;
    string DespawnMessageTemplate;
    int MaxActiveHordes;
    bool TeleportSoundEnabled;
    bool EnableSmoke;
    bool EnableChestReward;
    string ChestClass;
    int ChestDespawnSeconds;
    int RewardPickMin;
    int RewardPickMax;
    ref array<ref STH_RewardItem> RewardItems;
    bool EnableChestSmoke;
    float ChestSmokeOffsetY;
    bool EnableHordeProgressDisplay;
    int HordeProgressIntervalSeconds;
    float HordeProgressPlayerRadius;

    void STH_GlobalSettings()
    {
        UseExpansionNotifications = true;
        HordeIntervalSeconds = 1200;
        HordeLifetimeSeconds = 1800;
    DespawnOnServerRestart = true;
    MinPlayersForHordeSpawn = 0;
    SpawnMessageTemplate = "A zombie horde has spawned near {location} ({count})";
    DespawnMessageTemplate = "The zombie horde near {location} has disappeared ({count})";
           MaxActiveHordes = 1;
        TeleportSoundEnabled = true;
        EnableSmoke = true;
    EnableChestReward = true;
    ChestClass = "SeaChest";
    ChestDespawnSeconds = 600;
    RewardPickMin = 1;
    RewardPickMax = 3;
    RewardItems = new array<ref STH_RewardItem>;
    EnableChestSmoke = true;
    ChestSmokeOffsetY = 0.6;
    EnableHordeProgressDisplay = true;
    HordeProgressIntervalSeconds = 10;
    HordeProgressPlayerRadius = 60.0;
        // Example rewards
        STH_RewardItem r1 = new STH_RewardItem();
        r1.ClassName = "Ammo_762x39";
        r1.QuantityMin = 20;
        r1.QuantityMax = 40;
        r1.Chance = 0.8;
        RewardItems.Insert(r1);
        STH_RewardItem r2 = new STH_RewardItem();
        r2.ClassName = "BandageDressing";
        r2.QuantityMin = 1;
        r2.QuantityMax = 3;
        r2.Chance = 1.0;
        RewardItems.Insert(r2);
    }
};

class STH_ConfigData
{
    string ConfigVersion;
    ref STH_GlobalSettings Global;
    ref array<ref STH_HordeLocation> Hordes;

    void STH_ConfigData()
    {
        Global = new STH_GlobalSettings;
        Hordes = new array<ref STH_HordeLocation>;
    ConfigVersion = "3.0";
    }
};

class STH_Config
{
    static const float CURRENT_VERSION = 3.0;
    static const string CURRENT_VERSION_STR = "3.0";
    static string PROFILE_DIR = "$profile:\\SagaTheHorde";
    static string CONFIG_PATH = "$profile:\\SagaTheHorde\\config.json";
    static string BACKUP_DIR = "$profile:\\SagaTheHorde\\backup";

    ref STH_ConfigData Data;
    // ensure we only create a single backup per run even if multiple saves happen
    protected bool m_BackupDoneThisSession = false;

    // Create a timestamped backup of the existing config.json (once per session)
    void BackupExistingConfig()
    {
        if (m_BackupDoneThisSession)
        {
            return; // already backed up once this session
        }
        if (!FileExist(CONFIG_PATH))
        {
            return; // nothing to back up
        }
        if (!FileExist(BACKUP_DIR))
        {
            MakeDirectory(BACKUP_DIR);
        }

        // Use mission time in ms for a simple unique suffix
        int ms = GetGame().GetTime();
        string ts = ms.ToString();
        string backupPath = BACKUP_DIR + "\\config." + ts + ".json.bak";

        FileHandle fin = OpenFile(CONFIG_PATH, FileMode.READ);
        if (!fin)
        {
            return; // cannot open for read
        }
        FileHandle fout = OpenFile(backupPath, FileMode.WRITE);
        if (!fout)
        {
            CloseFile(fin);
            return; // cannot open backup for write
        }

        string line;
        while (FGets(fin, line) >= 0)
        {
            FPrintln(fout, line);
        }
        CloseFile(fin);
        CloseFile(fout);
        m_BackupDoneThisSession = true;
        Print("[SagaTheHorde] Backup created: " + backupPath);
    }

    void STH_Config() {}

    void Init()
    {
        if (!FileExist(PROFILE_DIR))
        {
            MakeDirectory(PROFILE_DIR);
        }

        if (FileExist(CONFIG_PATH))
        {
            Load();
            UpgradeIfNeeded();
            // Even if versions match, ensure any newly added fields are present and saved
            EnsureConsistencyAndSaveIfNeeded();
            // Do not save unconditionally here to avoid overwriting without changes
        }
        else
        {
            CreateDefault();
            Save();
        }
    }
                // example values are set in EnsureConsistency when needed

    void Load()
    {
        Data = new STH_ConfigData();
        JsonFileLoader<STH_ConfigData>.JsonLoadFile(CONFIG_PATH, Data);
        if (!Data)
        {
            Data = new STH_ConfigData();
        }
    }

    void Save()
    {
        if (!Data)
        {
            Data = new STH_ConfigData();
        }
        JsonFileLoader<STH_ConfigData>.JsonSaveFile(CONFIG_PATH, Data);
    }

    void CreateDefault()
    {
        Data = new STH_ConfigData();

        STH_HordeLocation loc = new STH_HordeLocation();
        loc.Name = "Chernogorsk";
        loc.Position.Insert(6740.0);
        loc.Position.Insert(0.0);
        loc.Position.Insert(2450.0);
    loc.Radius = 60.0;
        loc.MinCount = 8;
        loc.MaxCount = 15;

        STH_ZombieEntry z1 = new STH_ZombieEntry();
        z1.ClassName = "ZmbM_HermitSkinny_Beige";
        z1.Always = false;
        z1.Chance = 0.6;
        z1.CountMin = 1;
        z1.CountMax = 2;
        z1.Loot.Insert("BandageDressing");
    z1.TeleportEnabled = true;
    z1.TeleportDetectRadius = 25.0;
    z1.TeleportCooldownSeconds = 15;
    z1.TeleportCombatSuppressRadius = 12.0;
    z1.TeleportMinDistance = 4.0;
        z1.HealthStrengthBonus = 0.5; // +50% Stärke (1.5x)

        STH_ZombieEntry z2 = new STH_ZombieEntry();
        z2.ClassName = "ZmbF_CitizenANormal_Beige";
        z2.Always = true;
        z2.Chance = 1.0;
        z2.CountMin = 1;
        z2.CountMax = 3;
    z2.TeleportEnabled = false;
    z2.TeleportCooldownSeconds = 20;
    z2.TeleportCombatSuppressRadius = 10.0;
    z2.TeleportMinDistance = 4.0;

        loc.ZombieClasses.Insert(z1);
        loc.ZombieClasses.Insert(z2);

        Data.Hordes.Insert(loc);

        Data.Global.UseExpansionNotifications = true;
        Data.Global.HordeIntervalSeconds = 1200;
        Data.Global.HordeLifetimeSeconds = 1800;
    Data.Global.SpawnMessageTemplate = "A zombie horde has spawned near {location} ({count})";
    Data.Global.DespawnMessageTemplate = "The zombie horde near {location} has disappeared ({count})";
        Data.Global.MaxActiveHordes = 1;
        Data.Global.TeleportSoundEnabled = true;
        // Smoke default
        Data.Global.EnableSmoke = true;
        // Reward defaults
        Data.Global.EnableChestReward = true;
        Data.Global.ChestClass = "SeaChest";
        Data.Global.RewardPickMin = 1;
        Data.Global.RewardPickMax = 3;
        Data.Global.MinPlayersForHordeSpawn = 0;
        Data.Global.EnableChestSmoke = true;
        Data.Global.ChestSmokeOffsetY = 0.6;
        Data.Global.EnableHordeProgressDisplay = true;
        Data.Global.HordeProgressIntervalSeconds = 10;
        Data.Global.HordeProgressPlayerRadius = 60.0;
        // RewardItems pre-populated in STH_GlobalSettings constructor
    Data.ConfigVersion = CURRENT_VERSION_STR;
    }

    void UpgradeIfNeeded()
    {
        if (!Data)
        {
            return;
        }

    // If version is missing or different from current, run upgrade/backfill
    if (!Data.ConfigVersion || Data.ConfigVersion.Length() == 0 || Data.ConfigVersion != CURRENT_VERSION_STR)
        {
            // Ensure new fields have sensible defaults for existing configs
            int i;
            for (i = 0; i < Data.Hordes.Count(); i++)
            {
                STH_HordeLocation loc = Data.Hordes.Get(i);
                if (!loc)
                {
                    continue;
                }
                // Backfill per-location progress radius override
                if (loc.ProgressPlayerRadius <= 0)
                {
                    loc.ProgressPlayerRadius = 0.0; // 0 = use global
                }
                int j;
                for (j = 0; j < loc.ZombieClasses.Count(); j++)
                {
                    STH_ZombieEntry z = loc.ZombieClasses.Get(j);
                    if (!z)
                    {
                        continue;
                    }
                    if (z.TeleportDetectRadius <= 0)
                    {
                        z.TeleportDetectRadius = 20.0;
                    }
                    if (z.TeleportCooldownSeconds <= 0)
                    {
                        z.TeleportCooldownSeconds = 20;
                    }
                    if (z.TeleportCombatSuppressRadius <= 0)
                    {
                        z.TeleportCombatSuppressRadius = 10.0;
                    }
                    if (z.TeleportMinDistance <= 0)
                    {
                        z.TeleportMinDistance = 4.0;
                    }
                }
            }
            if (!Data.Global)
            {
                Data.Global = new STH_GlobalSettings();
            }
            if (Data.Global.HordeIntervalSeconds <= 0)
            {
                Data.Global.HordeIntervalSeconds = 1200;
            }
            if (Data.Global.ChestDespawnSeconds <= 0)
            {
                Data.Global.ChestDespawnSeconds = 600;
            }
            if (!Data.Global.SpawnMessageTemplate || Data.Global.SpawnMessageTemplate.Length() == 0)
            {
                Data.Global.SpawnMessageTemplate = "A zombie horde has spawned near {location} ({count})";
            }
            if (!Data.Global.DespawnMessageTemplate || Data.Global.DespawnMessageTemplate.Length() == 0)
            {
                Data.Global.DespawnMessageTemplate = "The zombie horde near {location} has disappeared ({count})";
            }
            if (Data.Global.MaxActiveHordes <= 0)
            {
                Data.Global.MaxActiveHordes = 1;
            }
            // Backfill teleport sound settings
            // TeleportSoundSet removed from config; always use default in code
            // if not explicitly set, default to disabled
            // (bool default is false if field missing, so no action needed)
            // Backfill smoke enable flag (new simplified field)
            // Do not force-change existing user value; rely on existing or defaults when missing
            if (!Data.Global)
            {
                Data.Global = new STH_GlobalSettings();
            }
            // Backfill reward chest settings
            if (!Data.Global.ChestClass || Data.Global.ChestClass.Length() == 0)
            {
                Data.Global.ChestClass = "SeaChest";
            }
            // Enable by default for new versions
            Data.Global.EnableChestReward = true;
            if (!Data.Global.RewardItems)
            {
                Data.Global.RewardItems = new array<ref STH_RewardItem>;
            }
            // Backfill chest smoke toggle
            if (Data.Global.ChestSmokeOffsetY == 0)
            {
                Data.Global.ChestSmokeOffsetY = 0.6;
            }
            // default to enabled
            Data.Global.EnableChestSmoke = true;
            if (Data.Global.RewardPickMax <= 0)
            {
                Data.Global.RewardPickMax = 3;
            }
            if (Data.Global.RewardPickMin <= 0 || Data.Global.RewardPickMin > Data.Global.RewardPickMax)
            {
                Data.Global.RewardPickMin = 1;
            }
            if (Data.Global.MinPlayersForHordeSpawn < 0)
            {
                Data.Global.MinPlayersForHordeSpawn = 0;
            }
            // Backfill progress display settings
            if (Data.Global.HordeProgressIntervalSeconds <= 0)
            {
                Data.Global.HordeProgressIntervalSeconds = 10;
            }
            // default to enabled
            Data.Global.EnableHordeProgressDisplay = true;
            if (Data.Global.HordeProgressPlayerRadius <= 0)
            {
                Data.Global.HordeProgressPlayerRadius = 60.0;
            }
            Data.ConfigVersion = CURRENT_VERSION_STR;
            // Create a backup of the current file before saving upgraded config
            BackupExistingConfig();
            Save();
        }
        else
        {
            // Config version matches current; nothing to upgrade
        }
    }
    
    void EnsureConsistencyAndSaveIfNeeded()
    {
        if (!Data)
        {
            return;
        }
        bool needSave = false;
        
        // Ensure the hordes array exists
        if (!Data.Hordes)
        {
            Data.Hordes = new array<ref STH_HordeLocation>;
            needSave = true;
        }
        // If no horde is defined, add a clear example entry for users to edit
        if (Data.Hordes.Count() == 0)
        {
            STH_HordeLocation exLoc = new STH_HordeLocation();
            exLoc.Name = "Chernogorsk";
            exLoc.Position.Insert(6740.0);
            exLoc.Position.Insert(0.0);
            exLoc.Position.Insert(2450.0);
            exLoc.Radius = 60.0;
            exLoc.MinCount = 8;
            exLoc.MaxCount = 15;
            exLoc.ProgressPlayerRadius = 0.0; // use global by default

            STH_ZombieEntry exZ1 = new STH_ZombieEntry();
            exZ1.ClassName = "ZmbM_HermitSkinny_Beige";
            exZ1.Always = false;
            exZ1.Chance = 0.6;
            exZ1.CountMin = 1;
            exZ1.CountMax = 2;
            exZ1.Loot.Insert("BandageDressing");
            exZ1.TeleportEnabled = true;
            exZ1.TeleportDetectRadius = 25.0;
            exZ1.TeleportCooldownSeconds = 15;
            exZ1.TeleportCombatSuppressRadius = 12.0;
            exZ1.TeleportMinDistance = 4.0;
                exZ1.HealthStrengthBonus = 0.5; // Beispiel: +50% Stärke (1.5x)

            STH_ZombieEntry exZ2 = new STH_ZombieEntry();
            exZ2.ClassName = "ZmbF_CitizenANormal_Beige";
            exZ2.Always = true;
            exZ2.Chance = 1.0;
            exZ2.CountMin = 1;
            exZ2.CountMax = 3;
            exZ2.TeleportEnabled = false;
            exZ2.TeleportCooldownSeconds = 20;
            exZ2.TeleportCombatSuppressRadius = 10.0;
            exZ2.TeleportMinDistance = 4.0;

            exLoc.ZombieClasses.Insert(exZ1);
            exLoc.ZombieClasses.Insert(exZ2);
            Data.Hordes.Insert(exLoc);
            needSave = true;
        }

        if (!Data.Global)
        {
            Data.Global = new STH_GlobalSettings();
            needSave = true;
        }
        if (Data.Global.ChestDespawnSeconds < 0)
        {
            Data.Global.ChestDespawnSeconds = 0; // allow 0 = disabled
            needSave = true;
        }
        if (Data.Global.HordeIntervalSeconds <= 0)
        {
            Data.Global.HordeIntervalSeconds = 1200;
            needSave = true;
        }
        if (Data.Global.HordeLifetimeSeconds <= 0)
        {
            Data.Global.HordeLifetimeSeconds = 1800;
            needSave = true;
        }
        if (!Data.Global.SpawnMessageTemplate || Data.Global.SpawnMessageTemplate.Length() == 0)
        {
            Data.Global.SpawnMessageTemplate = "A zombie horde has spawned near {location} ({count})";
            needSave = true;
        }
        if (!Data.Global.DespawnMessageTemplate || Data.Global.DespawnMessageTemplate.Length() == 0)
        {
            Data.Global.DespawnMessageTemplate = "The zombie horde near {location} has disappeared ({count})";
            needSave = true;
        }
        if (Data.Global.MaxActiveHordes <= 0)
        {
            Data.Global.MaxActiveHordes = 1;
            needSave = true;
        }
        // Smoke effect simple toggle consistency
        // Ensure field exists; default true
        if (!Data.Global)
        {
            Data.Global = new STH_GlobalSettings();
            needSave = true;
        }
        // Reward chest consistency
        if (!Data.Global.ChestClass || Data.Global.ChestClass.Length() == 0)
        {
            Data.Global.ChestClass = "SeaChest";
            needSave = true;
        }
        if (!Data.Global.RewardItems)
        {
            Data.Global.RewardItems = new array<ref STH_RewardItem>;
            needSave = true;
        }
        // Ensure chest smoke settings exist with sane defaults
        if (Data.Global.ChestSmokeOffsetY <= 0)
        {
            Data.Global.ChestSmokeOffsetY = 0.6;
            needSave = true;
        }
        // Default to enabled if missing (bool missing defaults to false); opt the feature in by default
        if (!Data.Global.EnableChestSmoke)
        {
            Data.Global.EnableChestSmoke = true;
            needSave = true;
        }
        // Progress display consistency
        if (Data.Global.HordeProgressIntervalSeconds <= 0)
        {
            Data.Global.HordeProgressIntervalSeconds = 10;
            needSave = true;
        }
        if (!Data.Global.EnableHordeProgressDisplay)
        {
            Data.Global.EnableHordeProgressDisplay = true;
            needSave = true;
        }
        if (Data.Global.HordeProgressPlayerRadius <= 0)
        {
            Data.Global.HordeProgressPlayerRadius = 60.0;
            needSave = true;
        }
        // If RewardItems exists but is empty, add an example set for the default SeaChest
        if (Data.Global.RewardItems.Count() == 0)
        {
            STH_RewardItem exR1 = new STH_RewardItem();
            exR1.ClassName = "Ammo_762x39";
            exR1.QuantityMin = 20;
            exR1.QuantityMax = 40;
            exR1.Chance = 0.8;
            Data.Global.RewardItems.Insert(exR1);

            STH_RewardItem exR2 = new STH_RewardItem();
            exR2.ClassName = "BandageDressing";
            exR2.QuantityMin = 1;
            exR2.QuantityMax = 3;
            exR2.Chance = 1.0;
            Data.Global.RewardItems.Insert(exR2);

            needSave = true;
        }
    // ConfigVersion kept as string to avoid float serialization artifacts
        
        // Ensure per-zombie new fields are initialized so teleport logic works safely
        int i;
        for (i = 0; i < Data.Hordes.Count(); i++)
        {
            STH_HordeLocation loc = Data.Hordes.Get(i);
            if (!loc)
            {
                continue;
            }
            if (loc.ProgressPlayerRadius < 0)
            {
                loc.ProgressPlayerRadius = 0.0;
                needSave = true;
            }
            int j;
            for (j = 0; j < loc.ZombieClasses.Count(); j++)
            {
                STH_ZombieEntry z = loc.ZombieClasses.Get(j);
                if (!z)
                {
                    continue;
                }
                if (z.TeleportDetectRadius <= 0)
                {
                    z.TeleportDetectRadius = 20.0;
                    needSave = true;
                }
                if (z.TeleportCooldownSeconds <= 0)
                {
                    z.TeleportCooldownSeconds = 20;
                    needSave = true;
                }
                if (z.TeleportCombatSuppressRadius <= 0)
                {
                    z.TeleportCombatSuppressRadius = 10.0;
                    needSave = true;
                }
                if (z.TeleportMinDistance <= 0)
                {
                    z.TeleportMinDistance = 4.0;
                    needSave = true;
                }
                // HealthStrengthBonus is the only toughness control
            }
        }
        
        if (needSave)
        {
            Print("[SagaTheHorde] Updating config.json with new default fields (same version)");
            // Create a backup of the current file before saving adjusted config
            BackupExistingConfig();
            Save();
        }
    }
};

