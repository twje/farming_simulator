#include "Core/Tiled/TiledMapLoader.h"

#include "Core/Tiled/TiledMapAsset.h"
#include "Core/Tiled/TiledMapJsonParser.h"

//--------------------------------------------------------------------------------
/*virtual*/ std::unique_ptr<Asset> TiledMapLoader::Load(AssetFileDescriptor<TiledMapAsset> descriptor)
{
    auto data = TiledMapJsonParser::Load(descriptor.GetFilePath());    
    std::cout << descriptor.GetFilePath() << std::endl;
    return std::make_unique<TiledMapAsset>(std::move(data));
}