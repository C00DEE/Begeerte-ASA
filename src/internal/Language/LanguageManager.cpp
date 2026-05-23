#include "LanguageManager.h"

// U8 宏：将 GBK 源文件中的字符串字面量转为 UTF-8 const char*
#if defined(__cpp_char8_t)
#define U8(str) reinterpret_cast<const char*>(u8##str)
#else
#define U8(str) u8##str
#endif

#include <shellapi.h>
#include <thread>
#include <chrono>

// =============================================================================
// 静态成员默认值定义（GBK 源文件 + U8 宏 = 运行时 UTF-8 指针）
// =============================================================================

// -- Configs_Menu --
const char* LanguageManager::Configs_Menu::TabLabel = U8("配置");
const char* LanguageManager::Configs_Menu::SectionTitle = U8("配置管理系统");
const char* LanguageManager::Configs_Menu::RefreshFileList = U8("刷新列表");
const char* LanguageManager::Configs_Menu::OpenDirectory = U8("打开目录");
const char* LanguageManager::Configs_Menu::Create = U8("创建");
const char* LanguageManager::Configs_Menu::LoadConfig = U8("加载配置");
const char* LanguageManager::Configs_Menu::SaveConfig = U8("保存配置");
const char* LanguageManager::Configs_Menu::InputHint = U8("输入参数名称...");
const char* LanguageManager::Configs_Menu::NoConfigs = U8("暂无配置文件");

// -- Language_Menu --
const char* LanguageManager::Language_Menu::TabLabel = U8("语言");
const char* LanguageManager::Language_Menu::SectionTitle = U8("语言管理系统");
const char* LanguageManager::Language_Menu::RefreshFileList = U8("刷新列表");
const char* LanguageManager::Language_Menu::OpenDirectory = U8("打开目录");
const char* LanguageManager::Language_Menu::Create = U8("创建模板");
const char* LanguageManager::Language_Menu::LoadLanguage = U8("加载语言");
const char* LanguageManager::Language_Menu::InputHint = U8("输入语言名称...");
const char* LanguageManager::Language_Menu::NoLanguages = U8("暂无语言文件");
const char* LanguageManager::Language_Menu::WorkshopSection = U8("Workshop 语言包");

// -- Aimbot_Menu --
const char* LanguageManager::Aimbot_Menu::TabLabel = U8("自瞄");
const char* LanguageManager::Aimbot_Menu::SectionTitle = U8("自动瞄准");
const char* LanguageManager::Aimbot_Menu::AimbotEnabled = U8("自动瞄准");
const char* LanguageManager::Aimbot_Menu::AimbotFOV = U8("瞄准范围");
const char* LanguageManager::Aimbot_Menu::AimbotSmooth = U8("瞄准速度");
const char* LanguageManager::Aimbot_Menu::AimbotRCX = U8("水平后坐力补偿");
const char* LanguageManager::Aimbot_Menu::AimbotRCY = U8("垂直后坐力补偿");
const char* LanguageManager::Aimbot_Menu::TriggerbotEnabled = U8("自动射击");
const char* LanguageManager::Aimbot_Menu::TriggerHitChance = U8("最低命中率");

// -- EntityList_Menu --
const char* LanguageManager::EntityList_Menu::TabLabel = U8("生物列表");
const char* LanguageManager::EntityList_Menu::SectionTitle = U8("生物列表");
const char* LanguageManager::EntityList_Menu::EnableFilter = U8("应用筛选到全局视觉");
const char* LanguageManager::EntityList_Menu::SearchHint = U8("输入生物名称进行过滤，多个用逗号分隔 (如: 南巨,霸王龙,迅猛龙)...");
const char* LanguageManager::EntityList_Menu::FilterActive = U8("当前筛选 (%zu 项):");

// -- ItemList_Menu --
const char* LanguageManager::ItemList_Menu::TabLabel = U8("背包管理");
const char* LanguageManager::ItemList_Menu::SearchHint = U8("输入物品名称搜索...");
const char* LanguageManager::ItemList_Menu::TooltipItem = U8("物品: %s");
const char* LanguageManager::ItemList_Menu::TooltipTotal = U8("总数量: %d");
const char* LanguageManager::ItemList_Menu::TooltipDesc = U8("说明: %s");
const char* LanguageManager::ItemList_Menu::TooltipCrafter = U8("制造人: %s");
const char* LanguageManager::ItemList_Menu::TooltipTribe = U8("部落: %s");
const char* LanguageManager::ItemList_Menu::ActionUseOne = U8("使用一个 (从当前堆叠)");
const char* LanguageManager::ItemList_Menu::ActionDrop = U8("丢弃该物品");
const char* LanguageManager::ItemList_Menu::NoItems = U8("背包中没有匹配的物品");

// -- Lua_Menu --
const char* LanguageManager::Lua_Menu::TabLabel = U8("脚本");
const char* LanguageManager::Lua_Menu::EnableManager = U8("启用 Lua 管理器");
const char* LanguageManager::Lua_Menu::SectionTitle = U8("脚本管理系统");
const char* LanguageManager::Lua_Menu::RefreshFileList = U8("刷新列表");
const char* LanguageManager::Lua_Menu::OpenDirectory = U8("打开目录");
const char* LanguageManager::Lua_Menu::ErrorHint = U8("右键点击图标以复制错误详情");
const char* LanguageManager::Lua_Menu::CopyError = U8("复制错误信息");

// -- Misc_Menu --
const char* LanguageManager::Misc_Menu::TabLabel = U8("其他功能");
const char* LanguageManager::Misc_Menu::SectionMenu = U8("菜单设置");
const char* LanguageManager::Misc_Menu::MenuColor = U8("菜单颜色");
const char* LanguageManager::Misc_Menu::LockLayout = U8("锁定布局");
const char* LanguageManager::Misc_Menu::ResetLayout = U8("重置布局");
const char* LanguageManager::Misc_Menu::SectionMisc = U8("其他功能");
const char* LanguageManager::Misc_Menu::Suicide = U8("自杀");
const char* LanguageManager::Misc_Menu::Unload = U8("卸载");
const char* LanguageManager::Misc_Menu::DumpServerInfo = U8("获取服务器信息");
const char* LanguageManager::Misc_Menu::UnlockExplorerNotes = U8("一键满级");
const char* LanguageManager::Misc_Menu::AutoFeed = U8("自动喂肉");
const char* LanguageManager::Misc_Menu::SuperFlyer = U8("飞行龙加强");
const char* LanguageManager::Misc_Menu::ForceTurn = U8("秒转头");
const char* LanguageManager::Misc_Menu::LogDamage = U8("伤害日志");
const char* LanguageManager::Misc_Menu::ArmorRange = U8("换甲阈值");
const char* LanguageManager::Misc_Menu::AutomaticChecked = U8("自动换甲");
const char* LanguageManager::Misc_Menu::OutBodyChecked = U8("灵魂出窍");

// -- StructureList_Menu --
const char* LanguageManager::StructureList_Menu::TabLabel = U8("建筑列表");
const char* LanguageManager::StructureList_Menu::SectionTitle = U8("建筑列表");
const char* LanguageManager::StructureList_Menu::EnableFilter = U8("应用筛选到全局视觉");
const char* LanguageManager::StructureList_Menu::SearchHint = U8("输入建筑名称进行过滤，多个用逗号分隔 (如: 大门,墙,地板)...");
const char* LanguageManager::StructureList_Menu::FilterActive = U8("当前筛选 (%zu 项):");

// -- Visuals_Menu --
const char* LanguageManager::Visuals_Menu::TabLabelVisuals = U8("视觉");
const char* LanguageManager::Visuals_Menu::TabLabelTeam = U8("队友");
const char* LanguageManager::Visuals_Menu::SectionGlobal = U8("全局设置");
const char* LanguageManager::Visuals_Menu::SectionExtra = U8("额外信息");
const char* LanguageManager::Visuals_Menu::SectionWorld = U8("世界信息");
const char* LanguageManager::Visuals_Menu::SectionTeam = U8("队友设置");
const char* LanguageManager::Visuals_Menu::SectionTeamExtra = U8("额外信息");

const char* LanguageManager::Visuals_Menu::ESPEnabled = U8("启用透视");

const char* LanguageManager::Visuals_Menu::Box = U8("方框");
const char* LanguageManager::Visuals_Menu::Name = U8("名称");
const char* LanguageManager::Visuals_Menu::HealthBar = U8("血量");
const char* LanguageManager::Visuals_Menu::Torpor = U8("眩晕");
const char* LanguageManager::Visuals_Menu::Ragdoll = U8("尸体");
const char* LanguageManager::Visuals_Menu::Distance = U8("距离");
const char* LanguageManager::Visuals_Menu::ShowAimPoints = U8("显示瞄准点");
const char* LanguageManager::Visuals_Menu::DroppedItems = U8("掉落的物品");
const char* LanguageManager::Visuals_Menu::DroppedItemPiled = U8("堆叠颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemCryopod = U8("低温仓颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemEgg = U8("蛋颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemMeat = U8("肉类颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemSpoiledMeat = U8("腐肉颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemWood = U8("木头颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemThatch = U8("茅草颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemMetal = U8("金属颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemStone = U8("石头颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemCrystal = U8("水晶颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemGem = U8("宝石颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemPearl = U8("珍珠颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemHide = U8("兽皮颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemPelt = U8("毛皮颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemKeratin = U8("角质颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemChitin = U8("甲壳素颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemCorruptedPolymer = U8("腐化瘤颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemPolymer_Organic = U8("有机聚合物颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemPolymer = U8("聚合物颜色");
const char* LanguageManager::Visuals_Menu::DroppedItemMaxDistance = U8("物品显示距离");
const char* LanguageManager::Visuals_Menu::ShowStructures = U8("显示建筑");
const char* LanguageManager::Visuals_Menu::OnlyEnemyStructures = U8("仅显示敌人建筑");
const char* LanguageManager::Visuals_Menu::StructureMaxDistance = U8("建筑显示距离");
const char* LanguageManager::Visuals_Menu::ShowWater = U8("显示水源");
const char* LanguageManager::Visuals_Menu::WaterMaxCount = U8("显示最近水源数量");
const char* LanguageManager::Visuals_Menu::WaterCountUnit = U8("个");

const char* LanguageManager::Visuals_Menu::ESPTeamEnabled = U8("启用透视##Team");

const char* LanguageManager::Visuals_Menu::TeamBox = U8("方框##Team");
const char* LanguageManager::Visuals_Menu::TeamName = U8("名称##Team");
const char* LanguageManager::Visuals_Menu::TeamHealthBar = U8("血量##Team");
const char* LanguageManager::Visuals_Menu::TeamTorpor = U8("眩晕##Team");
const char* LanguageManager::Visuals_Menu::TeamRagdoll = U8("尸体##Team");
const char* LanguageManager::Visuals_Menu::TeamDistance = U8("距离##Team");

// =============================================================================
// RegisterDefaults —— 生成模板 ini 使用，值为 UTF-8 字符串
// =============================================================================
void LanguageManager::RegisterDefaults(std::unordered_map<std::string, std::string>& d) const {
    // Configs_Menu
    d["Configs_Menu.TabLabel"] = U8("配置");
    d["Configs_Menu.SectionTitle"] = U8("配置管理系统");
    d["Configs_Menu.RefreshFileList"] = U8("刷新列表");
    d["Configs_Menu.OpenDirectory"] = U8("打开目录");
    d["Configs_Menu.Create"] = U8("创建");
    d["Configs_Menu.LoadConfig"] = U8("加载配置");
    d["Configs_Menu.SaveConfig"] = U8("保存配置");
    d["Configs_Menu.InputHint"] = U8("输入参数名称...");
    d["Configs_Menu.NoConfigs"] = U8("暂无配置文件");

    // Language_Menu
    d["Language_Menu.TabLabel"] = U8("语言");
    d["Language_Menu.SectionTitle"] = U8("语言管理系统");
    d["Language_Menu.RefreshFileList"] = U8("刷新列表");
    d["Language_Menu.OpenDirectory"] = U8("打开目录");
    d["Language_Menu.Create"] = U8("创建模板");
    d["Language_Menu.LoadLanguage"] = U8("加载语言");
    d["Language_Menu.InputHint"] = U8("输入语言名称...");
    d["Language_Menu.NoLanguages"] = U8("暂无语言文件");
    d["Language_Menu.WorkshopSection"] = U8("Workshop 语言包");

    // Aimbot_Menu
    d["Aimbot_Menu.TabLabel"] = U8("自瞄");
    d["Aimbot_Menu.SectionTitle"] = U8("自动瞄准");
    d["Aimbot_Menu.AimbotEnabled"] = U8("自动瞄准");
    d["Aimbot_Menu.AimbotFOV"] = U8("瞄准范围");
    d["Aimbot_Menu.AimbotSmooth"] = U8("瞄准速度");
    d["Aimbot_Menu.AimbotRCX"] = U8("水平后坐力补偿");
    d["Aimbot_Menu.AimbotRCY"] = U8("垂直后坐力补偿");
    d["Aimbot_Menu.TriggerbotEnabled"] = U8("自动射击");
    d["Aimbot_Menu.TriggerHitChance"] = U8("最低命中率");

    // EntityList_Menu
    d["EntityList_Menu.TabLabel"] = U8("生物列表");
    d["EntityList_Menu.SectionTitle"] = U8("生物列表");
    d["EntityList_Menu.EnableFilter"] = U8("应用筛选到全局视觉");
    d["EntityList_Menu.SearchHint"] = U8("输入生物名称进行过滤，多个用逗号分隔 (如: 南巨,霸王龙,迅猛龙)...");
    d["EntityList_Menu.FilterActive"] = U8("当前筛选 (%zu 项):");

    // ItemList_Menu
    d["ItemList_Menu.TabLabel"] = U8("背包管理");
    d["ItemList_Menu.SearchHint"] = U8("输入物品名称搜索...");
    d["ItemList_Menu.TooltipItem"] = U8("物品: %s");
    d["ItemList_Menu.TooltipTotal"] = U8("总数量: %d");
    d["ItemList_Menu.TooltipDesc"] = U8("说明: %s");
    d["ItemList_Menu.TooltipCrafter"] = U8("制造人: %s");
    d["ItemList_Menu.TooltipTribe"] = U8("部落: %s");
    d["ItemList_Menu.ActionUseOne"] = U8("使用一个 (从当前堆叠)");
    d["ItemList_Menu.ActionDrop"] = U8("丢弃该物品");
    d["ItemList_Menu.NoItems"] = U8("背包中没有匹配的物品");

    // Lua_Menu
    d["Lua_Menu.TabLabel"] = U8("脚本");
    d["Lua_Menu.EnableManager"] = U8("启用 Lua 管理器");
    d["Lua_Menu.SectionTitle"] = U8("脚本管理系统");
    d["Lua_Menu.RefreshFileList"] = U8("刷新列表");
    d["Lua_Menu.OpenDirectory"] = U8("打开目录");
    d["Lua_Menu.ErrorHint"] = U8("右键点击图标以复制错误详情");
    d["Lua_Menu.CopyError"] = U8("复制错误信息");

    // Misc_Menu
    d["Misc_Menu.TabLabel"] = U8("其他功能");
    d["Misc_Menu.SectionMenu"] = U8("菜单设置");
    d["Misc_Menu.MenuColor"] = U8("菜单颜色");
    d["Misc_Menu.LockLayout"] = U8("锁定布局");
    d["Misc_Menu.ResetLayout"] = U8("重置布局");
    d["Misc_Menu.SectionMisc"] = U8("其他功能");
    d["Misc_Menu.Suicide"] = U8("自杀");
    d["Misc_Menu.Unload"] = U8("卸载");
    d["Misc_Menu.DumpServerInfo"] = U8("获取服务器信息");
    d["Misc_Menu.UnlockExplorerNotes"] = U8("一键满级");
    d["Misc_Menu.AutoFeed"] = U8("自动喂肉");
    d["Misc_Menu.SuperFlyer"] = U8("飞行龙加强");
    d["Misc_Menu.ForceTurn"] = U8("秒转头");
    d["Misc_Menu.LogDamage"] = U8("伤害日志");
    d["Misc_Menu.ArmorRange"] = U8("换甲阈值");
    d["Misc_Menu.AutomaticChecked"] = U8("自动换甲");
    d["Misc_Menu.OutBodyChecked"] = U8("灵魂出窍");

    // StructureList_Menu
    d["StructureList_Menu.TabLabel"] = U8("建筑列表");
    d["StructureList_Menu.SectionTitle"] = U8("建筑列表");
    d["StructureList_Menu.EnableFilter"] = U8("应用筛选到全局视觉");
    d["StructureList_Menu.SearchHint"] = U8("输入建筑名称进行过滤，多个用逗号分隔 (如: 大门,墙,地板)...");
    d["StructureList_Menu.FilterActive"] = U8("当前筛选 (%zu 项):");

    // Visuals_Menu
    d["Visuals_Menu.TabLabelVisuals"] = U8("视觉");
    d["Visuals_Menu.TabLabelTeam"] = U8("队友");
    d["Visuals_Menu.SectionGlobal"] = U8("全局设置");
    d["Visuals_Menu.SectionExtra"] = U8("额外信息");
    d["Visuals_Menu.SectionWorld"] = U8("世界信息");
    d["Visuals_Menu.SectionTeam"] = U8("队友设置");
    d["Visuals_Menu.SectionTeamExtra"] = U8("额外信息");
    d["Visuals_Menu.ESPEnabled"] = U8("启用透视");
    d["Visuals_Menu.Box"] = U8("方框");
    d["Visuals_Menu.Name"] = U8("名称");
    d["Visuals_Menu.HealthBar"] = U8("血量");
    d["Visuals_Menu.Torpor"] = U8("眩晕");
    d["Visuals_Menu.Ragdoll"] = U8("尸体");
    d["Visuals_Menu.Distance"] = U8("距离");
    d["Visuals_Menu.ShowAimPoints"] = U8("显示瞄准点");
    d["Visuals_Menu.DroppedItems"] = U8("掉落的物品");
    d["Visuals_Menu.DroppedItemPiled"] = U8("堆叠颜色");
    d["Visuals_Menu.DroppedItemCryopod"] = U8("低温仓颜色");
    d["Visuals_Menu.DroppedItemEgg"] = U8("蛋颜色");
    d["Visuals_Menu.DroppedItemMeat"] = U8("肉类颜色");
    d["Visuals_Menu.DroppedItemSpoiledMeat"] = U8("腐肉颜色");
    d["Visuals_Menu.DroppedItemWood"] = U8("木头颜色");
    d["Visuals_Menu.DroppedItemThatch"] = U8("茅草颜色");
    d["Visuals_Menu.DroppedItemMetal"] = U8("金属颜色");
    d["Visuals_Menu.DroppedItemStone"] = U8("石头颜色");
    d["Visuals_Menu.DroppedItemCrystal"] = U8("水晶颜色");
    d["Visuals_Menu.DroppedItemGem"] = U8("宝石颜色");
    d["Visuals_Menu.DroppedItemPearl"] = U8("珍珠颜色");
    d["Visuals_Menu.DroppedItemHide"] = U8("兽皮颜色");
    d["Visuals_Menu.DroppedItemPelt"] = U8("毛皮颜色");
    d["Visuals_Menu.DroppedItemKeratin"] = U8("角质颜色");
    d["Visuals_Menu.DroppedItemChitin"] = U8("甲壳素颜色");
    d["Visuals_Menu.DroppedItemCorruptedPolymer"] = U8("腐化瘤颜色");
    d["Visuals_Menu.DroppedItemPolymer_Organic"] = U8("有机聚合物颜色");
    d["Visuals_Menu.DroppedItemPolymer"] = U8("聚合物颜色");
    d["Visuals_Menu.DroppedItemMaxDistance"] = U8("物品显示距离");
    d["Visuals_Menu.ShowStructures"] = U8("显示建筑");
    d["Visuals_Menu.OnlyEnemyStructures"] = U8("仅显示敌人建筑");
    d["Visuals_Menu.StructureMaxDistance"] = U8("建筑显示距离");
    d["Visuals_Menu.ShowWater"] = U8("显示水源");
    d["Visuals_Menu.WaterMaxCount"] = U8("显示最近水源数量");
    d["Visuals_Menu.WaterCountUnit"] = U8("个");
    d["Visuals_Menu.ESPTeamEnabled"] = U8("启用透视##Team");
    d["Visuals_Menu.TeamBox"] = U8("方框##Team");
    d["Visuals_Menu.TeamName"] = U8("名称##Team");
    d["Visuals_Menu.TeamHealthBar"] = U8("血量##Team");
    d["Visuals_Menu.TeamTorpor"] = U8("眩晕##Team");
    d["Visuals_Menu.TeamRagdoll"] = U8("尸体##Team");
    d["Visuals_Menu.TeamDistance"] = U8("距离##Team");
}

// =============================================================================
// ApplyToStaticMembers
// =============================================================================
void LanguageManager::ApplyToStaticMembers() {
    auto apply = [&](const std::string& key, const char*& target) {
        std::unordered_map<std::string, std::string>::iterator it = m_data.find(key);
        if (it != m_data.end()) {
            m_storage[key] = it->second;
            target = m_storage[key].c_str();
        }
        };

    // Configs_Menu
    apply("Configs_Menu.TabLabel", Configs_Menu::TabLabel);
    apply("Configs_Menu.SectionTitle", Configs_Menu::SectionTitle);
    apply("Configs_Menu.RefreshFileList", Configs_Menu::RefreshFileList);
    apply("Configs_Menu.OpenDirectory", Configs_Menu::OpenDirectory);
    apply("Configs_Menu.Create", Configs_Menu::Create);
    apply("Configs_Menu.LoadConfig", Configs_Menu::LoadConfig);
    apply("Configs_Menu.SaveConfig", Configs_Menu::SaveConfig);
    apply("Configs_Menu.InputHint", Configs_Menu::InputHint);
    apply("Configs_Menu.NoConfigs", Configs_Menu::NoConfigs);

    // Language_Menu
    apply("Language_Menu.TabLabel", Language_Menu::TabLabel);
    apply("Language_Menu.SectionTitle", Language_Menu::SectionTitle);
    apply("Language_Menu.RefreshFileList", Language_Menu::RefreshFileList);
    apply("Language_Menu.OpenDirectory", Language_Menu::OpenDirectory);
    apply("Language_Menu.Create", Language_Menu::Create);
    apply("Language_Menu.LoadLanguage", Language_Menu::LoadLanguage);
    apply("Language_Menu.InputHint", Language_Menu::InputHint);
    apply("Language_Menu.NoLanguages", Language_Menu::NoLanguages);
    apply("Language_Menu.WorkshopSection", Language_Menu::WorkshopSection);

    // Aimbot_Menu
    apply("Aimbot_Menu.TabLabel", Aimbot_Menu::TabLabel);
    apply("Aimbot_Menu.SectionTitle", Aimbot_Menu::SectionTitle);
    apply("Aimbot_Menu.AimbotEnabled", Aimbot_Menu::AimbotEnabled);
    apply("Aimbot_Menu.AimbotFOV", Aimbot_Menu::AimbotFOV);
    apply("Aimbot_Menu.AimbotSmooth", Aimbot_Menu::AimbotSmooth);
    apply("Aimbot_Menu.AimbotRCX", Aimbot_Menu::AimbotRCX);
    apply("Aimbot_Menu.AimbotRCY", Aimbot_Menu::AimbotRCY);
    apply("Aimbot_Menu.TriggerbotEnabled", Aimbot_Menu::TriggerbotEnabled);
    apply("Aimbot_Menu.TriggerHitChance", Aimbot_Menu::TriggerHitChance);

    // EntityList_Menu
    apply("EntityList_Menu.TabLabel", EntityList_Menu::TabLabel);
    apply("EntityList_Menu.SectionTitle", EntityList_Menu::SectionTitle);
    apply("EntityList_Menu.EnableFilter", EntityList_Menu::EnableFilter);
    apply("EntityList_Menu.SearchHint", EntityList_Menu::SearchHint);
    apply("EntityList_Menu.FilterActive", EntityList_Menu::FilterActive);

    // ItemList_Menu
    apply("ItemList_Menu.TabLabel", ItemList_Menu::TabLabel);
    apply("ItemList_Menu.SearchHint", ItemList_Menu::SearchHint);
    apply("ItemList_Menu.TooltipItem", ItemList_Menu::TooltipItem);
    apply("ItemList_Menu.TooltipTotal", ItemList_Menu::TooltipTotal);
    apply("ItemList_Menu.TooltipDesc", ItemList_Menu::TooltipDesc);
    apply("ItemList_Menu.TooltipCrafter", ItemList_Menu::TooltipCrafter);
    apply("ItemList_Menu.TooltipTribe", ItemList_Menu::TooltipTribe);
    apply("ItemList_Menu.ActionUseOne", ItemList_Menu::ActionUseOne);
    apply("ItemList_Menu.ActionDrop", ItemList_Menu::ActionDrop);
    apply("ItemList_Menu.NoItems", ItemList_Menu::NoItems);

    // Lua_Menu
    apply("Lua_Menu.TabLabel", Lua_Menu::TabLabel);
    apply("Lua_Menu.EnableManager", Lua_Menu::EnableManager);
    apply("Lua_Menu.SectionTitle", Lua_Menu::SectionTitle);
    apply("Lua_Menu.RefreshFileList", Lua_Menu::RefreshFileList);
    apply("Lua_Menu.OpenDirectory", Lua_Menu::OpenDirectory);
    apply("Lua_Menu.ErrorHint", Lua_Menu::ErrorHint);
    apply("Lua_Menu.CopyError", Lua_Menu::CopyError);

    // Misc_Menu
    apply("Misc_Menu.TabLabel", Misc_Menu::TabLabel);
    apply("Misc_Menu.SectionMenu", Misc_Menu::SectionMenu);
    apply("Misc_Menu.MenuColor", Misc_Menu::MenuColor);
    apply("Misc_Menu.LockLayout", Misc_Menu::LockLayout);
    apply("Misc_Menu.ResetLayout", Misc_Menu::ResetLayout);
    apply("Misc_Menu.SectionMisc", Misc_Menu::SectionMisc);
    apply("Misc_Menu.Suicide", Misc_Menu::Suicide);
    apply("Misc_Menu.Unload", Misc_Menu::Unload);
    apply("Misc_Menu.DumpServerInfo", Misc_Menu::DumpServerInfo);
    apply("Misc_Menu.UnlockExplorerNotes", Misc_Menu::UnlockExplorerNotes);
    apply("Misc_Menu.AutoFeed", Misc_Menu::AutoFeed);
    apply("Misc_Menu.SuperFlyer", Misc_Menu::SuperFlyer);
    apply("Misc_Menu.ForceTurn", Misc_Menu::ForceTurn);
    apply("Misc_Menu.LogDamage", Misc_Menu::LogDamage);
    apply("Misc_Menu.ArmorRange", Misc_Menu::ArmorRange);
    apply("Misc_Menu.AutomaticChecked", Misc_Menu::AutomaticChecked);
    apply("Misc_Menu.OutBodyChecked", Misc_Menu::OutBodyChecked);

    // StructureList_Menu
    apply("StructureList_Menu.TabLabel", StructureList_Menu::TabLabel);
    apply("StructureList_Menu.SectionTitle", StructureList_Menu::SectionTitle);
    apply("StructureList_Menu.EnableFilter", StructureList_Menu::EnableFilter);
    apply("StructureList_Menu.SearchHint", StructureList_Menu::SearchHint);
    apply("StructureList_Menu.FilterActive", StructureList_Menu::FilterActive);

    // Visuals_Menu
    apply("Visuals_Menu.TabLabelVisuals", Visuals_Menu::TabLabelVisuals);
    apply("Visuals_Menu.TabLabelTeam", Visuals_Menu::TabLabelTeam);
    apply("Visuals_Menu.SectionGlobal", Visuals_Menu::SectionGlobal);
    apply("Visuals_Menu.SectionExtra", Visuals_Menu::SectionExtra);
    apply("Visuals_Menu.SectionWorld", Visuals_Menu::SectionWorld);
    apply("Visuals_Menu.SectionTeam", Visuals_Menu::SectionTeam);
    apply("Visuals_Menu.SectionTeamExtra", Visuals_Menu::SectionTeamExtra);

    apply("Visuals_Menu.ESPEnabled", Visuals_Menu::ESPEnabled);
    apply("Visuals_Menu.Box", Visuals_Menu::Box);
    apply("Visuals_Menu.Name", Visuals_Menu::Name);
    apply("Visuals_Menu.HealthBar", Visuals_Menu::HealthBar);
    apply("Visuals_Menu.Torpor", Visuals_Menu::Torpor);
    apply("Visuals_Menu.Ragdoll", Visuals_Menu::Ragdoll);
    apply("Visuals_Menu.Distance", Visuals_Menu::Distance);
    apply("Visuals_Menu.ShowAimPoints", Visuals_Menu::ShowAimPoints);
    apply("Visuals_Menu.DroppedItems", Visuals_Menu::DroppedItems);
    apply("Visuals_Menu.DroppedItemPiled", Visuals_Menu::DroppedItemPiled);
    apply("Visuals_Menu.DroppedItemCryopod", Visuals_Menu::DroppedItemCryopod);
    apply("Visuals_Menu.DroppedItemEgg", Visuals_Menu::DroppedItemEgg);
    apply("Visuals_Menu.DroppedItemMeat", Visuals_Menu::DroppedItemMeat);
    apply("Visuals_Menu.DroppedItemSpoiledMeat", Visuals_Menu::DroppedItemSpoiledMeat);
    apply("Visuals_Menu.DroppedItemWood", Visuals_Menu::DroppedItemWood);
    apply("Visuals_Menu.DroppedItemThatch", Visuals_Menu::DroppedItemThatch);
    apply("Visuals_Menu.DroppedItemMetal", Visuals_Menu::DroppedItemMetal);
    apply("Visuals_Menu.DroppedItemStone", Visuals_Menu::DroppedItemStone);
    apply("Visuals_Menu.DroppedItemCrystal", Visuals_Menu::DroppedItemCrystal);
    apply("Visuals_Menu.DroppedItemGem", Visuals_Menu::DroppedItemGem);
    apply("Visuals_Menu.DroppedItemPearl", Visuals_Menu::DroppedItemPearl);
    apply("Visuals_Menu.DroppedItemHide", Visuals_Menu::DroppedItemHide);
    apply("Visuals_Menu.DroppedItemPelt", Visuals_Menu::DroppedItemPelt);
    apply("Visuals_Menu.DroppedItemKeratin", Visuals_Menu::DroppedItemKeratin);
    apply("Visuals_Menu.DroppedItemChitin", Visuals_Menu::DroppedItemChitin);
    apply("Visuals_Menu.DroppedItemCorruptedPolymer", Visuals_Menu::DroppedItemCorruptedPolymer);
    apply("Visuals_Menu.DroppedItemPolymer_Organic", Visuals_Menu::DroppedItemPolymer_Organic);
    apply("Visuals_Menu.DroppedItemPolymer", Visuals_Menu::DroppedItemPolymer);
    apply("Visuals_Menu.DroppedItemMaxDistance", Visuals_Menu::DroppedItemMaxDistance);
    apply("Visuals_Menu.ShowStructures", Visuals_Menu::ShowStructures);
    apply("Visuals_Menu.OnlyEnemyStructures", Visuals_Menu::OnlyEnemyStructures);
    apply("Visuals_Menu.StructureMaxDistance", Visuals_Menu::StructureMaxDistance);
    apply("Visuals_Menu.ShowWater", Visuals_Menu::ShowWater);
    apply("Visuals_Menu.WaterMaxCount", Visuals_Menu::WaterMaxCount);
    apply("Visuals_Menu.WaterCountUnit", Visuals_Menu::WaterCountUnit);

    apply("Visuals_Menu.ESPTeamEnabled", Visuals_Menu::ESPTeamEnabled);
    apply("Visuals_Menu.TeamBox", Visuals_Menu::TeamBox);
    apply("Visuals_Menu.TeamName", Visuals_Menu::TeamName);
    apply("Visuals_Menu.TeamHealthBar", Visuals_Menu::TeamHealthBar);
    apply("Visuals_Menu.TeamTorpor", Visuals_Menu::TeamTorpor);
    apply("Visuals_Menu.TeamRagdoll", Visuals_Menu::TeamRagdoll);
    apply("Visuals_Menu.TeamDistance", Visuals_Menu::TeamDistance);
}

// =============================================================================
// Initialize
// =============================================================================
void LanguageManager::Initialize(const std::string& languageDir) {
    if (languageDir.empty()) return;
    try {
        fs::path p = fs::absolute(languageDir);
        m_languageDir = p.string();
        if (!fs::exists(m_languageDir)) fs::create_directories(m_languageDir);
    }
    catch (const fs::filesystem_error&) { return; }
    RefreshFileList();
}

// =============================================================================
// RefreshFileList
// =============================================================================
void LanguageManager::RefreshFileList() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_languageDir.empty()) return;
    fs::path dirPath = fs::absolute(m_languageDir);
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) return;

    std::vector<fs::path> currentFiles;
    try {
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".ini")
                currentFiles.push_back(fs::absolute(entry.path()));
        }
    }
    catch (...) { return; }

    // --- 修复点：只删除 [非Workshop] 且 [磁盘已不存在] 的文件 ---
    m_languages.erase(
        std::remove_if(m_languages.begin(), m_languages.end(), [&](const LanguageFile& lf) {
            if (lf.isWorkshop) return false; // 绝对不要删除内存中的 Workshop 模板
            return std::find(currentFiles.begin(), currentFiles.end(), lf.path) == currentFiles.end();
            }),
        m_languages.end()
    );

    // 添加新发现的本地文件
    for (const auto& filePath : currentFiles) {
        bool found = false;
        for (const auto& lf : m_languages) {
            if (!lf.isWorkshop && lf.path == filePath) { found = true; break; }
        }
        if (!found) {
            LanguageFile lf;
            lf.name = filePath.filename().string();
            lf.path = filePath;
            lf.isWorkshop = false;
            m_languages.push_back(lf);
        }
    }
}

// =============================================================================
// CreateLanguage
// =============================================================================
bool LanguageManager::CreateLanguage(const std::string& name) {
    if (name.empty() || name.length() > 32) return false;
    try {
        fs::path langPath = fs::path(m_languageDir) / (name + ".ini");
        if (fs::exists(langPath)) return false;
        std::ofstream file(langPath);
        if (!file.is_open()) return false;

        file << "# Language File: " << name << "\n";
        file << "# Encoding: UTF-8\n";
        file << "# Edit values to localize the UI. Use a UTF-8 editor (e.g. VS Code).\n";
        file << "# Format: Key=Value (under [SectionName])\n\n";

        std::unordered_map<std::string, std::string> defaults;
        RegisterDefaults(defaults);

        std::map<std::string, std::vector<std::pair<std::string, std::string>>> sections;
        for (std::unordered_map<std::string, std::string>::iterator it = defaults.begin();
            it != defaults.end(); ++it)
        {
            size_t dot = it->first.find('.');
            if (dot == std::string::npos) continue;
            sections[it->first.substr(0, dot)].push_back(
                std::make_pair(it->first.substr(dot + 1), it->second));
        }

        for (std::map<std::string, std::vector<std::pair<std::string, std::string>>>::iterator sit = sections.begin();
            sit != sections.end(); ++sit)
        {
            file << "[" << sit->first << "]\n";
            for (size_t i = 0; i < sit->second.size(); i++)
                file << sit->second[i].first << "=" << sit->second[i].second << "\n";
            file << "\n";
        }

        file.close();
        RefreshFileList();
        return true;
    }
    catch (...) { return false; }
}

// =============================================================================
// LoadLanguage
// =============================================================================
bool LanguageManager::LoadLanguage(const std::string& filename) {
    std::string rawData;
    bool foundInMem = false;

    // 1. 优先检查是否是内存中的 Workshop 文件
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& lf : m_languages) {
            if (lf.isWorkshop && lf.name == filename) {
                rawData = lf.content;
                foundInMem = true;
                break;
            }
        }
    }

    // 2. 如果内存没找到，走磁盘读取
    if (!foundInMem) {
        try {
            fs::path langPath = fs::path(m_languageDir) / filename;
            if (!fs::exists(langPath)) return false; // 增强健壮性
            std::ifstream file(langPath);
            if (!file.is_open()) return false;
            rawData.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
        }
        catch (...) { return false; }
    }

    if (rawData.empty()) return false;

    // --- 关键修复：清理旧数据 ---
    m_data.clear();

    std::stringstream ss(rawData);
    std::string line, currentSection;
    while (std::getline(ss, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#') continue;
        if (line[0] == '[') {
            size_t end = line.find(']');
            if (end != std::string::npos) currentSection = line.substr(1, end - 1);
            continue;
        }
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string key = Trim(line.substr(0, pos));
        std::string value = Trim(line.substr(pos + 1));
        if (!key.empty() && !currentSection.empty())
            m_data[currentSection + "." + key] = value;
    }

    ApplyToStaticMembers();
    return true;
}

// =============================================================================
// FetchWorkshopScripts
// =============================================================================
// =============================================================================
// FetchWorkshopScripts - 完整修复版
// =============================================================================
void LanguageManager::FetchWorkshopScripts() {
    std::thread([this]() {
        // 1. 获取文件列表
        // 注意：GitHub API 建议带上 User-Agent，否则有时会返回空或 403
        std::string json;
        while (true) {
            json = HttpRequest("https://api.github.com/repos/zetsr/Begeerte-ASA/git/trees/main?recursive=1");
            if (!json.empty() && json.find("\"tree\":[") != std::string::npos) break;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // 获取清单失败，死循环重试
        }

        // 2. 定位到 "tree": [ 数组的开始，跳过外层 JSON 的第一个左大括号
        size_t treeArrayStart = json.find("\"tree\":[");
        if (treeArrayStart == std::string::npos) return;

        size_t entryStart = treeArrayStart;

        // 3. 开始解析每一个项目
        while ((entryStart = json.find("{", entryStart)) != std::string::npos) {
            size_t entryEnd = json.find("}", entryStart);
            if (entryEnd == std::string::npos) break;

            std::string entryBlock = json.substr(entryStart, entryEnd - entryStart);

            // 每次循环前必须步进，防止死循环
            entryStart = entryEnd + 1;

            std::string pathKey = "\"path\":\"";
            size_t pPos = entryBlock.find(pathKey);

            if (pPos != std::string::npos) {
                pPos += pathKey.length();
                size_t pEnd = entryBlock.find("\"", pPos);
                if (pEnd == std::string::npos) continue;

                std::string fullPath = entryBlock.substr(pPos, pEnd - pPos);

                // 筛选包含在 language 目录下且后缀为 .ini 的文件
                if (fullPath.find("language/") != std::string::npos && fullPath.find(".ini") != std::string::npos) {

                    LanguageFile lf;
                    lf.isWorkshop = true;
                    lf.downloadUrl = "https://raw.githubusercontent.com/zetsr/Begeerte-ASA/main/" + fullPath;

                    // 提取文件名 (例如 language/zh_CN.ini -> zh_CN.ini)
                    size_t lastSlash = fullPath.find_last_of('/');
                    lf.name = (lastSlash != std::string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;

                    // --- 关键点：下载内容 ---
                    lf.content = HttpRequest(lf.downloadUrl);

                    // 只要内容不为空，就立即插入列表
                    if (!lf.content.empty()) {
                        std::lock_guard<std::mutex> lock(m_mutex);

                        // 检查是否已经存在同名的 Workshop 项（防止重复添加）
                        bool exists = false;
                        for (const auto& existing : m_languages) {
                            if (existing.isWorkshop && existing.name == lf.name) {
                                exists = true;
                                break;
                            }
                        }

                        if (!exists) {
                            // 插入到列表开头，这样在 UI 上会优先显示
                            m_languages.insert(m_languages.begin(), lf);
                        }
                    }
                }
            }
        }
        }).detach();
}

// =============================================================================
// HttpRequest
// =============================================================================
std::string LanguageManager::HttpRequest(const std::string& url) {
    std::string response;
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64)",
        INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet) {
        DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
        HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, flags, 0);
        if (hConnect) {
            char buffer[4096]; DWORD bytesRead;
            while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
                response.append(buffer, bytesRead);
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hInternet);
    }
    return response;
}

// =============================================================================
// Trim
// =============================================================================
std::string LanguageManager::Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}