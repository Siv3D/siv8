//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CEngineResource.hpp"
# include <Siv3D/Resource.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Compression.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		struct EngineResourceDetail
		{
			/// @brief リソースのディレクトリ
			StringView resourceDirectory;

			/// @brief リソースのパス
			StringView resourcePath;
			
			/// @brief リソースが必須であるか
			/// @remark リソースが存在しない場合、エンジンの初期化に失敗します。
			bool isMandatory;

			/// @brief リソースが圧縮されているか
			bool isCompressed;

			[[nodiscard]]
			String getFriendlyName() const
			{
				return (resourceDirectory + resourcePath);
			}

			[[nodiscard]]
			FilePath getResourceFilePath() const
			{
				FilePath path = (U"engine/" + resourceDirectory + resourcePath);

				if (isCompressed)
				{
					path += U".zstdcmp";
				}

				return Resource(path);
			}

			[[nodiscard]]
			FilePath getCachedFilePath(const FilePathView cacheDirectory) const
			{
				if (not isCompressed)
				{
					return{};
				}
				
				return (cacheDirectory + resourceDirectory + resourcePath);
			}
		};

		/// @brief エンジンで使用するリソースの一覧
		static constexpr std::array<EngineResourceDetail, 3> EngineResources
		{{
			{ U"font/", U"Noto-COLRv1.ttf", false, true },
			{ U"font/", U"MPLUS2.ttf", false, true },
			{ U"font/", U"materialdesignicons-webfont.ttf", false, true },
		}};
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CEngineResource::init()
	{
		LOG_SCOPED_DEBUG("CEngineResource::init()");

		const FilePath cacheDirectory = CacheDirectory::Engine();

		for (const auto& resource : EngineResources)
		{
			if (resource.isCompressed)
			{
				const String friendlyName = resource.getFriendlyName();
				const FilePath cachePath = resource.getCachedFilePath(cacheDirectory);
				const FilePath resourcePath = resource.getResourceFilePath();

				if (FileSystem::Exists(cachePath))
				{
					LOG_DEBUG(fmt::format("Found `{}`", friendlyName));
					continue;
				}

				if (not FileSystem::Exists(resourcePath))
				{
					if (resource.isMandatory)
					{
						throw InternalEngineError{ fmt::format("Resource `{}` not found", friendlyName) };
					}

					continue;
				}

				if (not Compression::DecompressFileToFile(resourcePath, cachePath))
				{
					throw InternalEngineError{ fmt::format("Failed to decompress `{}`", friendlyName) };
				}

				LOG_DEBUG(fmt::format("Successfully decompressed `{}`", friendlyName));
			}
		}
	}
}
