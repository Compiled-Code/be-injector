#include <Windows.h>
#include <atomic>
#include "dependencies/lazy_importer/lazy_importer.hpp"


void dll_core( )
{
	constexpr char hello[] = { 'H', 'e', 'l', 'l', 'o', '\0' };

	LI_FN( MessageBoxA )( nullptr, hello, hello, MB_OK );
}