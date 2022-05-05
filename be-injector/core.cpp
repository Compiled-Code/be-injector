/*
	Author: CompiledCode

	Credits:
		- Xerox for VDM
*/

#include "dependencies/vdm/vdm_ctx/vdm_ctx.hpp"

int main( )
{
	std::unique_ptr< std::remove_pointer_t< HANDLE >, decltype( &vdm::unload_drv ) > driver{ vdm::load_drv( ), &vdm::unload_drv };
	vdm::vdm_ctx vdm{};

	const std::pair< std::uintptr_t, std::uintptr_t > modules
	{ 
		reinterpret_cast< std::uintptr_t >( LoadLibraryExA( "holder.dll", nullptr, DONT_RESOLVE_DLL_REFERENCES ) ),
		reinterpret_cast< std::uintptr_t >( LoadLibraryExA( "module.dll", nullptr, DONT_RESOLVE_DLL_REFERENCES ) )
	};

	const auto get_physical_address = [ & ]( const std::uintptr_t virtual_address ) -> void*
	{
		static const auto get_ntk_export = std::bind( &util::get_kmodule_export, "ntoskrnl.exe", std::placeholders::_1, false );

		static const auto mm_get_physical_address = get_ntk_export( "MmGetPhysicalAddress" );

		using mm_get_physical_address_t = PHYSICAL_ADDRESS( * )( const std::uintptr_t );
		const auto physical_address = vdm.syscall< mm_get_physical_address_t >( mm_get_physical_address, virtual_address );

		return reinterpret_cast< void* >( physical_address.QuadPart );
	};

	const auto get_nt_headers = []( const std::uintptr_t image_base ) -> IMAGE_NT_HEADERS*
	{
		const auto dos_header = reinterpret_cast< IMAGE_DOS_HEADER* >( image_base );

		return reinterpret_cast< IMAGE_NT_HEADERS* >( image_base + dos_header->e_lfanew );
	};

	const auto size_of_code = get_nt_headers( modules.second )->OptionalHeader.SizeOfCode;

	std::printf( "[+] size of code: %X\n", size_of_code );

	VirtualLock( reinterpret_cast< void* >( modules.first + 0x2000 ), size_of_code );

	const auto remainder = size_of_code % 0x1000;

	if ( ( size_of_code - remainder ) >= 0x1000 )
	{
		for ( auto page = 0x1000; page < size_of_code - remainder; ++page )
		{
			std::printf( "[+] writing region with size 0x1000 at holder+%X\n", page + 0x1000 );

			vdm::write_phys( get_physical_address( modules.first + page + 0x1000 ), reinterpret_cast< void* >( modules.second + page ), 0x1000 );
		}
	}

	if ( remainder )
	{
		std::printf( "[+] writing region with size %X at holder+%X\n", remainder, ( size_of_code - remainder ) + 0x2000 );

		vdm::write_phys( get_physical_address( modules.first + ( size_of_code - remainder ) + 0x2000 ), reinterpret_cast< void* >( modules.second + ( size_of_code - remainder ) + 0x1000 ), remainder );
	}

	std::printf( "[+] once game open press 'enter'\n" );

	std::getchar( );

	DWORD process_id;
	const auto thread_id = GetWindowThreadProcessId( FindWindowA( "UnityWndClass", nullptr ), &process_id );

	const auto hook = SetWindowsHookExA( WH_KEYBOARD, reinterpret_cast< HOOKPROC >( modules.first + 0x2000 ), reinterpret_cast< HMODULE >( modules.first ), thread_id );
	
	std::printf( "[+] injected" );

	return std::getchar( );
}