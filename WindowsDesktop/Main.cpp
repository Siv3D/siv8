# include <Siv3D.hpp> // OpenSiv3D v0.8.0
# include "../Test/Siv3DTest.hpp"
# include <Windows.h>
# include <iostream>
# include <cstdio>

//////////////////////////////////////////////////
//
//	ビルド設定 | Build Mode
//
//////////////////////////////////////////////////

# define SIV3D_BUILD(X) SIV3D_BUILD_PRIVATE_DEFINITION_##X()
# define SIV3D_BUILD_PRIVATE_DEFINITION_DEBUG()		0
# define SIV3D_BUILD_PRIVATE_DEFINITION_RELEASE()	0

# if defined(_DEBUG) || defined(DEBUG) // Debug Build

# undef SIV3D_BUILD_PRIVATE_DEFINITION_DEBUG
# define SIV3D_BUILD_PRIVATE_DEFINITION_DEBUG()		1

# else // Release Build

# undef SIV3D_BUILD_PRIVATE_DEFINITION_RELEASE
# define SIV3D_BUILD_PRIVATE_DEFINITION_RELEASE()	1

# endif

# if SIV3D_BUILD(DEBUG)
#	define SIV3D_DEBUG_LIB_POSTFIX(s) #s
# else
#	define SIV3D_DEBUG_LIB_POSTFIX(s)
# endif

# pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
# pragma comment (lib, "siv3d/Siv3D" SIV3D_DEBUG_LIB_POSTFIX(_d))

# undef SIV3D_DEBUG_LIB_POSTFIX

void Main()
{
	::AllocConsole();

	const UINT oldConsoleOutputCP = ::GetConsoleOutputCP();
	::SetConsoleOutputCP(65001);

	FILE* m_fp = nullptr;
	::freopen_s(&m_fp, "CONIN$", "r", stdin);
	::freopen_s(&m_fp, "CONOUT$", "w", stdout);
	::freopen_s(&m_fp, "CONOUT$", "w", stderr);

	std::ios_base::sync_with_stdio(false);




	const char s[] = "あ";

	std::cout << std::size(s) << '\n';
	std::cout << "日本語🐈日本語\n";

	std::system("pause");






	if (m_fp)
	{
		::fclose(m_fp);
		m_fp = nullptr;
	}

	::SetConsoleOutputCP(oldConsoleOutputCP);
	::FreeConsole();
}
