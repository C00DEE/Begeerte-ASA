#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <map>
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

namespace fs = std::filesystem;

struct LanguageFile {
    std::string name;
    fs::path    path;
    bool        isWorkshop = false;
    std::string downloadUrl;
    std::string content;
};

class LanguageManager {
public:
    static LanguageManager& Get() {
        static LanguageManager instance;
        return instance;
    }

    void Initialize(const std::string& languageDir);
    void RefreshFileList();
    void FetchWorkshopScripts();

    bool CreateLanguage(const std::string& name);
    bool LoadLanguage(const std::string& filename);

    std::vector<LanguageFile>& GetLanguages() { return m_languages; }
    const std::string& GetLanguageDir() const { return m_languageDir; }
    std::string HttpRequestPublic(const std::string& url) { return HttpRequest(url); }

    // =========================================================================
    // 本地化字符串命名空间
    // =========================================================================

    struct Configs_Menu {
        static const char* TabLabel;
        static const char* SectionTitle;
        static const char* RefreshFileList;
        static const char* OpenDirectory;
        static const char* Create;
        static const char* LoadConfig;
        static const char* SaveConfig;
        static const char* InputHint;
        static const char* NoConfigs;
    };

    struct Language_Menu {
        static const char* TabLabel;
        static const char* SectionTitle;
        static const char* RefreshFileList;
        static const char* OpenDirectory;
        static const char* Create;
        static const char* LoadLanguage;
        static const char* InputHint;
        static const char* NoLanguages;
        static const char* WorkshopSection;
    };

    struct Aimbot_Menu {
        static const char* TabLabel;
        static const char* SectionTitle;
        static const char* AimbotEnabled;
        static const char* AimbotFOV;
        static const char* AimbotSmooth;
        static const char* TriggerbotEnabled;
        static const char* OtherTitle;
        static const char* ArmorRange;
        static const char* AutomaticChecked;
    };

    struct EntityList_Menu {
        static const char* TabLabel;
        static const char* SectionTitle;
        static const char* EnableFilter;
        static const char* SearchHint;
        static const char* FilterActive;    // "当前筛选 (%zu 项):" — 含格式符，调用方自行 sprintf
    };

    struct ItemList_Menu {
        static const char* TabLabel;
        static const char* SearchHint;
        static const char* TooltipItem;
        static const char* TooltipTotal;
        static const char* TooltipDesc;
        static const char* TooltipCrafter;
        static const char* TooltipTribe;
        static const char* ActionUseOne;
        static const char* ActionDrop;
        static const char* NoItems;
    };

    struct Lua_Menu {
        static const char* TabLabel;
        static const char* EnableManager;
        static const char* SectionTitle;
        static const char* RefreshFileList;
        static const char* OpenDirectory;
        static const char* ErrorHint;
        static const char* CopyError;
    };

    struct Misc_Menu {
        static const char* TabLabel;
        static const char* SectionMenu;
        static const char* MenuColor;
        static const char* LockLayout;
        static const char* ResetLayout;
        static const char* SectionMisc;
        static const char* Suicide;
        static const char* Unload;
        static const char* DumpServerInfo;
        static const char* UnlockExplorerNotes;
        static const char* AutoFeed;
        static const char* SuperFlyer;
        static const char* ForceTurn;
        static const char* LogDamage;
    };

    struct StructureList_Menu {
        static const char* TabLabel;
        static const char* SectionTitle;
        static const char* EnableFilter;
        static const char* SearchHint;
        static const char* FilterActive;
    };

    struct Visuals_Menu {
        // Tab 标签
        static const char* TabLabelVisuals;
        static const char* TabLabelTeam;

        // 分区标题
        static const char* SectionGlobal;
        static const char* SectionExtra;
        static const char* SectionWorld;
        static const char* SectionTeam;
        static const char* SectionTeamExtra;

        // 全局
        static const char* Box;
        static const char* Name;
        static const char* HealthBar;
        static const char* Torpor;
        static const char* Ragdoll;

        // 额外信息
        static const char* Distance;
        static const char* ShowAimPoints;

        // 世界信息 - 掉落物
        static const char* DroppedItems;
        static const char* DroppedItemPiled;
        static const char* DroppedItemCryopod;
        static const char* DroppedItemEgg;
        static const char* DroppedItemMeat;
        static const char* DroppedItemSpoiledMeat;
        static const char* DroppedItemWood;
        static const char* DroppedItemThatch;
        static const char* DroppedItemMetal;
        static const char* DroppedItemStone;
        static const char* DroppedItemCrystal;
        static const char* DroppedItemGem;
        static const char* DroppedItemPearl;
        static const char* DroppedItemHide;
        static const char* DroppedItemPelt;
        static const char* DroppedItemKeratin;
        static const char* DroppedItemChitin;
        static const char* DroppedItemCorruptedPolymer;
        static const char* DroppedItemPolymer_Organic;
        static const char* DroppedItemPolymer;
        static const char* DroppedItemMaxDistance;

        // 世界信息 - 建筑
        static const char* ShowStructures;
        static const char* OnlyEnemyStructures;
        static const char* StructureMaxDistance;

        // 世界信息 - 水源
        static const char* ShowWater;
        static const char* WaterMaxCount;
        static const char* WaterCountUnit;

        // 队友
        static const char* TeamBox;
        static const char* TeamName;
        static const char* TeamHealthBar;
        static const char* TeamTorpor;
        static const char* TeamRagdoll;
        static const char* TeamDistance;
    };

private:
    LanguageManager() = default;
    ~LanguageManager() = default;

    std::string               m_languageDir;
    std::vector<LanguageFile> m_languages;
    std::mutex                m_mutex;

    std::unordered_map<std::string, std::string> m_data;
    std::unordered_map<std::string, std::string> m_storage;

    void RegisterDefaults(std::unordered_map<std::string, std::string>& defaults) const;
    void ApplyToStaticMembers();

    static std::string Trim(const std::string& str);
    std::string HttpRequest(const std::string& url);
};