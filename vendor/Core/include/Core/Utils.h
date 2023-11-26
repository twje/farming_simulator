#pragma once

class NonCopyableNonMovableMarker
{
protected:
    NonCopyableNonMovableMarker() = default;
    ~NonCopyableNonMovableMarker() = default;

    NonCopyableNonMovableMarker(const NonCopyableNonMovableMarker&) = delete;
    NonCopyableNonMovableMarker& operator=(const NonCopyableNonMovableMarker&) = delete;
    NonCopyableNonMovableMarker(NonCopyableNonMovableMarker&&) = delete;
    NonCopyableNonMovableMarker& operator=(NonCopyableNonMovableMarker&&) = delete;
};