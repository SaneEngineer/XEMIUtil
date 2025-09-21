#include <Hooks.h>
namespace MPL::Hooks
{
    struct InitOBJ
    {
        using Target = RE::TESObjectREFR;
        static inline void thunk(Target* a_ref)
        {
            func(a_ref);
            if (a_ref != nullptr)
            {
                auto ref = a_ref->GetObjectReference();
                if (ref != nullptr && (ref->Is(RE::FormType::MovableStatic) || ref->Is(RE::FormType::Static)))
                {
                    auto* std = MPL::Config::StatData::GetSingleton();
                    std->LoadConfig();

                    MPL::Config::ConfigEntry* entry = nullptr;
                    for (auto& ent : std->entries | std::views::reverse)
                    {
                        auto itm = std::ranges::find(ent.forms, a_ref->GetFormID());
                        if(itm != ent.forms.end())
                        {
                            entry = &ent;
                            break;
						}
					}

                    if (entry != nullptr && entry->xemi != 0x0 && !(a_ref->sourceFiles.array->back()->GetFilename().starts_with("WSU") || a_ref->sourceFiles.array->back()->GetFilename() == "Synthesis.esp"))
                    {
                        if (a_ref->extraList.HasType<RE::ExtraEmittanceSource>())
                        {
                            auto* edr = a_ref->extraList.GetByType<RE::ExtraEmittanceSource>();
                            auto* frm = RE::TESForm::LookupByID(entry->xemi);
#ifdef DEBUG
                            logger::info("(INIT)(REP): {:X}:{} -> {:X}:{} W/ {:X}:{}", a_ref->GetLocalFormID(), a_ref->sourceFiles.array->front()->GetFilename(), edr->source->GetLocalFormID(), edr->source->sourceFiles.array->front()->GetFilename(), frm->GetLocalFormID(), frm->sourceFiles.array->front()->GetFilename());
#endif
                            edr->source = frm;
                        }
                        else
                        {
                            auto* frm = RE::TESForm::LookupByID(entry->xemi);
#ifdef DEBUG
                            logger::info("(INIT)(CRE): {:X}:{} -> {:X}:{}", a_ref->GetLocalFormID(), a_ref->sourceFiles.array->front()->GetFilename(), frm->GetLocalFormID(), frm->sourceFiles.array->front()->GetFilename());
#endif
                            auto* ext = RE::BSExtraData::Create<RE::ExtraEmittanceSource>();
                            ext->source = frm;
                            a_ref->extraList.Add(ext);
                        }
                    }
                }
            }
        }
        static inline REL::Relocation<decltype(thunk)> func;
        static inline constexpr std::size_t index{ 0x13 };
    };
    void Install()
    {
        stl::install_hook<InitOBJ>();
    };
}  // namespace MPL::Hooks