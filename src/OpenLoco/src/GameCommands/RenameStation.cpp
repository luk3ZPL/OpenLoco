#include "Economy/Expenditures.h"
#include "GameCommands.h"
#include "Graphics/Gfx.h"
#include "Localisation/FormatArguments.hpp"
#include "Localisation/StringIds.h"
#include "Localisation/StringManager.h"
#include "Types.hpp"
#include "World/Industry.h"
#include "World/StationManager.h"
#include "World/TownManager.h"
#include <OpenLoco/Interop/Interop.hpp>

using namespace OpenLoco::Interop;

namespace OpenLoco::GameCommands
{
    /**
     * 0x00490756
     * Renames a particular station.
     *
     * This command is called 3 times before the buffer is applied. Each time, 12 chars of the 36 char buffer are provided.
     * The resulting station name has a maximum length of 31 chars; the last bytes are not used.
     *
     * @param flags @<bl> - game command flags
     * @param stationId @<cx> - station id
     * @param index @<ax> - update index (in order of: 1, 2, 0)
     * @param buffer0 @<edx> - First part (4 chars) of the 12 update buffer
     * @param buffer1 @<dx> - Second part (4 chars) of the 12 update buffer
     * @param buffer2 @<bp> - Third part (4 chars) of the 12 update buffer
     * @return @<ebx> - returns 0 if rename is successful; otherwise GameCommands::FAILURE
     */
    static uint32_t renameStation(const uint8_t flags, StationId stationId, int16_t index, uint32_t buffer0, uint32_t buffer1, uint32_t buffer2)
    {
        GameCommands::setExpenditureType(ExpenditureType::Miscellaneous);

        // Keep track of the station id over several calls.
        static StationId _stationId{};
        if (index == 1)
            _stationId = stationId;

        static uint32_t renameBuffer[9];

        // Fill buffer over calls into the renameBuffer
        if ((flags & GameCommands::Flags::apply) != 0)
        {
            static constexpr std::array<int, 3> kTransformTable = { 2, 0, 1 };
            int arrayIndex = kTransformTable.at(index);
            renameBuffer[arrayIndex * 3] = buffer0;
            renameBuffer[arrayIndex * 3 + 1] = buffer1;
            renameBuffer[arrayIndex * 3 + 2] = buffer2;
        }

        // Applying the buffer?
        if (index != 0)
            return 0;

        char renameStringBuffer[37] = "";
        memcpy(renameStringBuffer, renameBuffer, sizeof(renameBuffer));
        renameStringBuffer[36] = '\0';

        // Figure out the current name for this station.
        char currentStationName[256] = "";
        auto station = StationManager::get(_stationId);
        auto args = FormatArguments::common(station->town);
        StringManager::formatString(currentStationName, station->name, &args);

        // Verify the new name actually differs from the old one.
        if (strcmp(currentStationName, renameStringBuffer) == 0)
            return 0;

        string_id oldStringId = station->name;

        // If an empty string is given, generate one instead.
        if (strlen(renameStringBuffer) == 0)
        {
            // Are we bailing out early?
            if ((flags & GameCommands::Flags::apply) == 0)
                return 0;

            station->name = StationManager::generateNewStationName(_stationId, station->town, World::Pos3(station->x, station->y, station->z), 0);
        }
        else
        {
            // Allocate a string id for the new name.
            string_id allocatedStringId = StringManager::userStringAllocate(renameStringBuffer, 0);
            if (allocatedStringId == StringIds::empty)
                return GameCommands::FAILURE;

            // Are we bailing out early?
            if ((flags & GameCommands::Flags::apply) == 0)
            {
                StringManager::emptyUserString(allocatedStringId);
                return 0;
            }

            // Apply the new name to the station.
            station->name = allocatedStringId;
        }

        StringManager::emptyUserString(oldStringId);
        station->updateLabel();
        Gfx::invalidateScreen();
        return 0;
    }

    void renameStation(registers& regs)
    {
        regs.ebx = renameStation(regs.bl, StationId(regs.cx), regs.ax, regs.edx, regs.ebp, regs.edi);
    }
}
