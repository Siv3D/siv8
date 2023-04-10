//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

# if defined(_WIN32)

# if (defined(_WINDOWS_) && !defined(S3D_MINIMAL_WINDOWS_INCLUDE))

	# pragma message("You have included <Windows.h> before MinWindows.h")

# endif

	// MinWindows.hpp をインクルードすると定義されるマクロ
	# define S3D_MINIMAL_WINDOWS_INCLUDE

	# ifndef STRICT
	#	define STRICT
	# endif

	# define WIN32_LEAN_AND_MEAN

	# define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
	# define NOVIRTUALKEYCODES // VK_*
	# define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
	# define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
	# define NOSYSMETRICS      // SM_*
	# define NOMENUS           // MF_*
	# define NOICONS           // IDI_*
	# define NOKEYSTATES       // MK_*
	# define NOSYSCOMMANDS     // SC_*
	# define NORASTEROPS       // Binary and Tertiary raster ops
	# define NOSHOWWINDOW      // SW_*
	# define OEMRESOURCE       // OEM Resource values
	# define NOATOM            // Atom Manager routines
	# define NOCLIPBOARD       // Clipboard routines
	# define NOCOLOR           // Screen colors
	# define NOCTLMGR          // Control and Dialog routines
	# define NODRAWTEXT        // DrawText() and DT_*
	# define NOGDI             // All GDI defines and routines
	# define NOKERNEL          // All KERNEL defines and routines
	//# define NOUSER            // All USER defines and routines
	# define NONLS             // All NLS defines and routines
	# define NOMB              // MB_* and MessageBox()
	# define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
	# define NOMETAFILE        // typedef METAFILEPICT
	# define NOMINMAX          // Macros min(a,b) and max(a,b)
	//# define NOMSG             // typedef MSG and associated routines
	# define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
	# define NOSCROLL          // SB_* and scrolling routines
	# define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
	# define NOSOUND           // Sound driver routines
	# define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
	# define NOWH              // SetWindowsHook and WH_*
	# define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
	# define NOCOMM            // COMM driver routines
	# define NOKANJI           // Kanji support stuff.
	# define NOHELP            // Help engine interface.
	# define NOPROFILER        // Profiler interface.
	# define NODEFERWINDOWPOS  // DeferWindowPos routines
	# define NOMCX             // Modem Configuration Extensions

	# ifndef _WIN32_WINNT
	#	define _WIN32_WINNT _WIN32_WINNT_WIN10
	# endif

	# ifndef NTDDI_VERSION
	#	define NTDDI_VERSION NTDDI_WIN10
	# endif

	# include <Windows.h>

# endif // defined(_WIN32)
