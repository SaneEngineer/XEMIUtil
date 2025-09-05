#pragma once
namespace MPL::Config
{
    struct ConfigEntry
    {
        std::unordered_set<RE::FormID> forms;
        RE::FormID xemi;
    };
    class StatData : public REX::Singleton<StatData>
    {
    private:
        std::mutex load_lock;
        bool config_loaded = false;
        const static inline std::string config_path = "Data/SKSE/XEMIUtil";

    public:
        inline void LoadConfig()
        {
            std::lock_guard _guard(this->load_lock);
            if (!this->config_loaded)
            {
                this->config_loaded = true;
                if (std::filesystem::exists(this->config_path))
                {
                    for (auto file : std::filesystem::directory_iterator(this->config_path))
                    {
                        if (file.path().extension() == ".json")
                        {
                            auto cfg = rfl::json::load<std::vector<ConfigEntry>>(file.path().string());
                            if (cfg)
                            {
                                this->entries.insert(this->entries.end(), cfg->begin(), cfg->end());
                            }
                            else {
                                logger::error("Error {}, skipping", cfg.error().what());
                            }
                        }
                    }
                    logger::info("Loaded {} Entries", this->entries.size());
                }
                else {
                    logger::error("Config path does not exist, skipping the loading of records.");
                }
            }
        }
        std::vector<ConfigEntry> entries;
    };
}  // namespace MPL::Config

namespace rfl
{
    template <>
    struct Reflector<RE::FormID>
    {
        using ReflType = std::string;
        static ReflType from(const RE::FormID& v)
        {
            auto frm = RE::TESForm::LookupByID(v);
            return std::format("{:06X}:{}", frm->GetLocalFormID(), frm->sourceFiles.array->front()->GetFilename());
        }
        static RE::FormID to(const ReflType& v)
        {
            auto loc = v.find(":");
            if (loc != std::string::npos)
            {
                auto lfid = strtoul(v.substr(0, loc).c_str(), nullptr, 16);
                auto file = v.substr(loc + 1);
                auto dh = RE::TESDataHandler::GetSingleton();
                return dh->LookupFormID(lfid, file);
            }
            else {
                return RE::TESForm::LookupByEditorID(v)->GetFormID();
            }
        }
    };
}  // namespace rfl