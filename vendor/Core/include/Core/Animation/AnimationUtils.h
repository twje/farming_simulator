//#include <filesystem>
//
//#include <SFML/Graphics.hpp>
//
//// Forward declarations
//class Animation;
//
//namespace fs = std::filesystem;
//
//class SpritesheetAnimationExporter
//{
//	struct State;
//
//public:
//	void Export(const Animation& animation, const std::string name, const fs::path& outputDirectory,
//				uint16_t margin, uint16_t spacing);
//
//private:
//	// Setters
//	void SetFilePathsExportState(const std::string name, const fs::path& outputDirectory, State& outState);
//	void SetGraphicsExportState(const Animation& animation, State& outState);
//
//	// Export
//	void ExportSpritesheetConfig(const State& state);
//	void ExportSpritesheetTexture(const State& state);
//	void ExportAnimationConfig(const Animation& animation, const State& state);
//
//private:
//	struct State
//	{
//		State(uint16_t margin, uint16_t spacing)
//			: mMargin(margin),
//			  mSpacing(spacing)
//		{ }
//
//		uint16_t mMargin;
//		uint16_t mSpacing;
//		std::string mTextureId;
//		std::string mSpritesheetId;
//		std::vector<sf::Sprite> mSprites;
//		uint32_t mTextureWidth{ 0 };
//		uint32_t mTextureHeight{ 0 };
//		uint16_t mRows{ 0 };
//		uint16_t mCols{ 0 };
//		fs::path mSpritesheetConfigFilePath;
//		fs::path mSpritesheetTextureFilePath;
//		fs::path mAnimationFilePath;
//	};
//};