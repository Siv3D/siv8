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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	画像ファイル
	//	BMP, GIF, JPEG, PNG, PPM, SVG, TGA, TIFF, WebP, AllImageFiles
	//
	////////////////////////////////////////////////////////////////

	constexpr FileDialogFilter FileDialogFilter::BMP()
	{
		return{ U"BMP",{ U"bmp" } };
	}

	constexpr FileDialogFilter FileDialogFilter::GIF()
	{
		return{ U"GIF",{ U"gif" } };
	}

	constexpr FileDialogFilter FileDialogFilter::JPEG()
	{
		return{ U"JPEG", { U"jpg", U"jpeg" } };
	}

	constexpr FileDialogFilter FileDialogFilter::PNG()
	{
		return{ U"PNG", { U"png" } };
	}

	constexpr FileDialogFilter FileDialogFilter::PPM()
	{
		return{ U"PPM", { U"ppm", U"pgm", U"pbm", U"pnm" } };
	}

	constexpr FileDialogFilter FileDialogFilter::SVG()
	{
		return{ U"SVG", { U"svg" } };
	}

	constexpr FileDialogFilter FileDialogFilter::TGA()
	{
		return{ U"TGA", { U"tga" } };
	}

	constexpr FileDialogFilter FileDialogFilter::TIFF()
	{
		return{ U"TIFF", { U"tiff", U"tif" } };
	}

	constexpr FileDialogFilter FileDialogFilter::WebP()
	{
		return{ U"WebP", { U"webp" } };
	}

	constexpr FileDialogFilter FileDialogFilter::AllImageFiles()
	{
		return{ U"All Image Files", { U"png", U"jpg", U"jpeg", U"bmp", U"gif", U"tga", U"svg", U"webp", U"tiff", U"tif", U"ppm", U"pgm", U"pbm", U"pnm" } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	音声ファイル
	//	AAC, AIFF, FLAC, MIDI, MP3, Ogg Vorbis, Opus, WAVE, WMA, AllAudioFiles
	//
	////////////////////////////////////////////////////////////////

	constexpr FileDialogFilter FileDialogFilter::AAC()
	{
		return{ U"AAC", { U"m4a", U"aac" } };
	}

	constexpr FileDialogFilter FileDialogFilter::AIFF()
	{
		return{ U"AIFF", { U"aif", U"aiff", U"aifc" } };
	}

	constexpr FileDialogFilter FileDialogFilter::FLAC()
	{
		return{ U"FLAC", { U"flac" } };
	}

	constexpr FileDialogFilter FileDialogFilter::MIDI()
	{
		return{ U"MIDI", { U"mid" } };
	}

	constexpr FileDialogFilter FileDialogFilter::MP3()
	{
		return{ U"MP3", { U"mp3" } };
	}

	constexpr FileDialogFilter FileDialogFilter::OggVorbis()
	{
		return{ U"Ogg Vorbis", { U"ogg" } };
	}

	constexpr FileDialogFilter FileDialogFilter::Opus()
	{
		return{ U"Opus", { U"opus" } };
	}

	constexpr FileDialogFilter FileDialogFilter::WAV()
	{
		return{ U"WAV", { U"wav" } };
	}

	constexpr FileDialogFilter FileDialogFilter::WMA()
	{
		return{ U"WMA", { U"wma" } };
	}

	constexpr FileDialogFilter FileDialogFilter::AllAudioFiles()
	{
		return{ U"All Audio Files",{ U"wav", U"mp3", U"m4a", U"aac", U"ogg", U"opus", U"mid",
		# if SIV3D_PLATFORM(WINDOWS)
				U"wma",
				U"flac"
		# elif SIV3D_PLATFORM(MACOS)
				U"aiff",
				U"flac"
		# elif SIV3D_PLATFORM(LINUX)

		# else

		# endif
		} };
	}

	////////////////////////////////////////////////////////////////
	//
	//	動画ファイル
	//	AVI, MP4, WebM, AllVideoFiles
	//
	////////////////////////////////////////////////////////////////

	constexpr FileDialogFilter FileDialogFilter::AVI()
	{
		return{ U"AVI", { U"avi" } };
	}

	constexpr FileDialogFilter FileDialogFilter::MP4()
	{
		return{ U"MP4", { U"mp4", U"m4v" } };
	}

	constexpr FileDialogFilter FileDialogFilter::WebM()
	{
		return{ U"WebM", { U"webm" } };
	}

	constexpr FileDialogFilter FileDialogFilter::AllVideoFiles()
	{
		return{ U"All Video Files", { U"avi", U"mp4", U"m4v", U"webm", U"mov" } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	3D モデルファイル
	//	FBX, GLTF, OBJ, All3DModelFiles
	//
	////////////////////////////////////////////////////////////////

	constexpr FileDialogFilter FileDialogFilter::FBX()
	{
		return{ U"FBX", { U"fbx" } };
	}

	constexpr FileDialogFilter FileDialogFilter::GLTF()
	{
		return{ U"glTF", { U"gltf", U"glb" } };
	}

	constexpr FileDialogFilter FileDialogFilter::OBJ()
	{
		return{ U"OBJ", { U"obj" } };
	}

	constexpr FileDialogFilter FileDialogFilter::All3DModelFiles()
	{
		return{ U"All 3D Model Files", { U"obj", U"gltf", U"glb", U"fbx" } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	テキスト・データファイル
	//	CSV, INI, JSON, Markdown, Text, TOML, XML, PDF
	//
	////////////////////////////////////////////////////////////////

	constexpr FileDialogFilter FileDialogFilter::CSV()
	{
		return{ U"CSV", { U"csv" } };
	}

	constexpr FileDialogFilter FileDialogFilter::INI()
	{
		return{ U"INI", { U"ini" } };
	}

	constexpr FileDialogFilter FileDialogFilter::JSON()
	{
		return{ U"JSON", { U"json" } };
	}

	constexpr FileDialogFilter FileDialogFilter::Markdown()
	{
		return{ U"Markdown", { U"md" } };
	}

	constexpr FileDialogFilter FileDialogFilter::Text()
	{
		return{ U"Text", { U"txt" } };
	}

	constexpr FileDialogFilter FileDialogFilter::TOML()
	{
		return{ U"TOML", { U"toml" } };
	}

	constexpr FileDialogFilter FileDialogFilter::XML()
	{
		return{ U"XML", { U"xml" } };
	}

	constexpr FileDialogFilter FileDialogFilter::PDF()
	{
		return{ U"PDF", { U"pdf" } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	フォントファイル
	//	AllFontFiles
	//
	////////////////////////////////////////////////////////////////

	constexpr FileDialogFilter FileDialogFilter::AllFontFiles()
	{
		return{ U"All Font Files", { U"ttf", U"otf", U"ttc", U"otc", U"woff", U"woff2" } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	アーカイブファイル
	//	ZIP
	//
	////////////////////////////////////////////////////////////////

	constexpr FileDialogFilter FileDialogFilter::ZIP()
	{
		return{ U"ZIP", { U"zip" } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	すべてのファイル
	//	AllFiles
	//
	////////////////////////////////////////////////////////////////

	constexpr FileDialogFilter FileDialogFilter::AllFiles()
	{
		return{ U"All Files", {} };
	}
}
