//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "Array.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	FileDialogFilter
	//
	////////////////////////////////////////////////////////////////

	/// @brief ファイル選択ダイアログのフィルタを表す構造体
	struct FileDialogFilter
	{
		/// @brief ダイアログのドロップダウンに表示される説明文（例: "すべての画像ファイル"）
		String description;

		/// @brief 拡張子のリスト
		Array<String> extensions;

		////////////////////////////////////////////////////////////////
		//
		//	画像ファイル
		//	BMP, GIF, JPEG, PNG, PPM, SVG, TGA, TIFF, WebP, AllImageFiles
		//
		////////////////////////////////////////////////////////////////

		/// @brief BMP ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"BMP",{ U"bmp" } }` 
		[[nodiscard]]
		static constexpr FileDialogFilter BMP();

		/// @brief GIF ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"GIF",{ U"gif" } }` 
		[[nodiscard]]
		static constexpr FileDialogFilter GIF();

		/// @brief JPEG ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"JPEG",{ U"jpg", U"jpeg" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter JPEG();

		/// @brief PNG ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"PNG",{ U"png" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter PNG();

		/// @brief PPM ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"PPM",{ U"ppm", U"pgm", U"pbm", U"pnm" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter PPM();

		/// @brief SVG ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"SVG",{ U"svg" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter SVG();

		/// @brief TGA ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"TGA",{ U"tga" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter TGA();

		/// @brief TIFF ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"TIFF",{ U"tiff", U"tif" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter TIFF();

		/// @brief WebP ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"WebP",{ U"webp" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter WebP();

		/// @brief すべての画像ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"All Picture Files",{ U"png", U"jpg", U"jpeg", U"bmp", U"gif", U"tga", U"svg", U"webp", U"tiff", U"tif", U"ppm", U"pgm", U"pbm", U"pnm" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter AllImageFiles();

		////////////////////////////////////////////////////////////////
		//
		//	音声ファイル
		//	AAC, AIFF, FLAC, MIDI, MP3, Ogg Vorbis, Opus, WAVE, WMA, AllAudioFiles
		//
		////////////////////////////////////////////////////////////////

		/// @brief AAC ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"AAC",{ U"m4a", U"aac" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter AAC();

		/// @brief AIFF ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"AIFF",{ U"aif", U"aiff", U"aifc" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter AIFF();

		/// @brief FLAC ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"FLAC",{ U"flac" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter FLAC();

		/// @brief MIDI ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"MIDI",{ U"mid" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter MIDI();

		/// @brief MP3 ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"MP3",{ U"mp3" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter MP3();

		/// @brief Ogg Vorbis ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"Ogg Vorbis",{ U"ogg" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter OggVorbis();

		/// @brief Opus ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"Opus",{ U"opus" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter Opus();

		/// @brief WAVE ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"WAVE",{ U"wav" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter WAVE();

		/// @brief WMA ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"WMA",{ U"wma" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter WMA();

		/// @brief すべてのオーディオファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"All Audio Files",{ U"wav", U"mp3", U"m4a", U"aac", U"ogg", U"opus", U"mid", (+プラットフォームに応じた追加のフォーマット) } }`
		[[nodiscard]]
		static constexpr FileDialogFilter AllAudioFiles();

		////////////////////////////////////////////////////////////////
		//
		//	動画ファイル
		//	AVI, MP4, WebM, AllVideoFiles
		//
		////////////////////////////////////////////////////////////////

		/// @brief AVI ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"AVI",{ U"avi" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter AVI();

		/// @brief MP4 ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"MP4",{ U"mp4", U"m4v" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter MP4();

		/// @brief WebM ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"WebM",{ U"webm" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter WebM();

		/// @brief すべての動画ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"All Video Files",{ U"avi", U"mp4", U"m4v", U"webm" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter AllVideoFiles();

		////////////////////////////////////////////////////////////////
		//
		//	3D モデルファイル
		//	FBX, GLTF, OBJ, All3DModelFiles
		//
		////////////////////////////////////////////////////////////////

		/// @brief FBX ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"FBX",{ U"fbx" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter FBX();

		/// @brief glTF ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"glTF",{ U"gltf", U"glb" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter GLTF();

		/// @brief OBJ ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"OBJ",{ U"obj" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter OBJ();

		/// @brief すべての 3D モデルファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"All 3D Model Files",{ U"obj", U"gltf", U"glb", U"fbx" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter All3DModelFiles();

		////////////////////////////////////////////////////////////////
		//
		//	テキスト・データファイル
		//	CSV, INI, JSON, Markdown, Text, TOML, XML, PDF
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"CSV",{ U"csv" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter CSV();

		/// @brief INI ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"INI",{ U"ini" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter INI();

		/// @brief JSON ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"JSON",{ U"json" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter JSON();

		/// @brief Markdown ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"Markdown",{ U"md" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter Markdown();

		/// @brief プレーンテキストファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"Text",{ U"txt" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter Text();

		/// @brief TOML ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"TOML",{ U"toml" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter TOML();

		/// @brief XML ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"XML",{ U"xml" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter XML();

		/// @brief PDF ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"PDF",{ U"pdf" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter PDF();

		////////////////////////////////////////////////////////////////
		//
		//	フォントファイル
		//	AllFontFiles
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべてのフォントファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"All Font Files",{ U"ttf", U"otf", U"ttc", U"otc", U"woff", U"woff2" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter AllFontFiles();

		////////////////////////////////////////////////////////////////
		//
		//	アーカイブファイル
		//	ZIP
		//
		////////////////////////////////////////////////////////////////

		/// @brief ZIP ファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"ZIP",{ U"zip" } }`
		[[nodiscard]]
		static constexpr FileDialogFilter ZIP();

		////////////////////////////////////////////////////////////////
		//
		//	すべてのファイル
		//	AllFiles
		//
		////////////////////////////////////////////////////////////////

		/// @brief すべてのファイル用の拡張子フィルタを返します。
		/// @return `FileDialogFilter{ U"All Files", {} }`
		[[nodiscard]]
		static constexpr FileDialogFilter AllFiles();
	};
}

# include "detail/FileDialogFilter.ipp"
