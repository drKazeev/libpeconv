#include <peconv\find_base.h>
#include <peconv\pe_hdrs_helper.h>
#include <peconv\relocate.h>
#include <set>

namespace peconv {
    class CollectRelocs : public RelocBlockCallback
    {
    public:
        CollectRelocs(IN bool _is64bit, OUT std::set<ULONGLONG> &_relocs)
            : RelocBlockCallback(_is64bit), relocs(_relocs)
        {
        }

        virtual bool processRelocField(ULONG_PTR relocField)
        {
            if (is64bit) {
                ULONGLONG* relocateAddr = (ULONGLONG*)((ULONG_PTR)relocField);
                ULONGLONG rva = (*relocateAddr);
                relocs.insert(rva);
            }
            else {
                DWORD* relocateAddr = (DWORD*)((ULONG_PTR)relocField);
                ULONGLONG rva = ULONGLONG(*relocateAddr);
                relocs.insert(rva);
            }
            return true;
        }

    protected:
        std::set<ULONGLONG> &relocs;
    };
}

ULONGLONG peconv::find_base(IN BYTE* modulePtr, IN size_t moduleSize)
{
    if (moduleSize == 0) {
            moduleSize = peconv::get_image_size((const BYTE*)modulePtr);
    }
    if (moduleSize == 0) return 0;

    bool is64 = peconv::is64bit(modulePtr);
    std::set<ULONGLONG> relocs;
    CollectRelocs callback(is64, relocs);
    int count = peconv::process_relocation_table(modulePtr, moduleSize, &callback);
    if (count == 0) return 0;

    std::set<ULONGLONG>::iterator itr = relocs.begin();
    ULONGLONG first = *itr;
    ULONGLONG guessed_base = first & ~ULONGLONG(0xFFFF);
    return guessed_base;
}
