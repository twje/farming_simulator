#include "Core/Tiled/TiledLoader.h"

#include "Core/Tiled/TiledMap.h"
#include "Core/Tiled/TiledMapJsonParser.h"

//--------------------------------------------------------------------------------
/*virtual*/ std::unique_ptr<Asset> TiledMapLoader::Load(AssetFileDescriptor<TiledMap> descriptor)
{
    auto data = TiledMapJsonParser::Load(descriptor.GetFilePath());    
    return std::make_unique<TiledMap>(std::move(data));
}