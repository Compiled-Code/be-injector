#pragma once
#include <windows.h>
#include <cstdint>

#include "../util/util.hpp"

namespace vdm
{
	inline HANDLE drv_handle;
	__forceinline auto load_drv() -> HANDLE
	{
		vdm::drv_handle = CreateFile(
			"\\\\.\\RwDrv",
			GENERIC_READ | GENERIC_WRITE,
			NULL,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		return vdm::drv_handle;
	}

	__forceinline auto unload_drv(HANDLE drv_handle) -> NTSTATUS
	{
		if (!CloseHandle(drv_handle))
			return STATUS_FAIL_CHECK;
	}

	struct data_packet_t
	{
		void* in;
		std::uint32_t size, unk = 0;
		void* out;
	};

	__forceinline bool read_phys(void* addr, void* buffer, std::uint32_t size)
	{
		if (!util::is_valid(reinterpret_cast<std::uintptr_t>(addr)))
			return false;

		auto packet = data_packet_t{ .in = addr, .size = size, .out = buffer };
		DeviceIoControl( vdm::drv_handle, 0x222808, &packet, sizeof( packet ), &packet, sizeof( packet ), nullptr, nullptr );
	}

	__forceinline bool write_phys(void* addr, void* buffer, std::uint32_t size)
	{
		if (!util::is_valid(reinterpret_cast<std::uintptr_t>(addr)))
			return false;

		auto packet = data_packet_t{ .in = addr, .size = size, .out = buffer };
		DeviceIoControl( vdm::drv_handle, 0x22280C, &packet, sizeof( packet ), &packet, sizeof( packet ), nullptr, nullptr );
	}
}