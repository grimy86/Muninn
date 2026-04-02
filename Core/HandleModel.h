#pragma once
#include <windows.h>
#include <string>

namespace Muninn::Model
{
	/// <summary>
	/// This is a specific structure for user object handles, not native handles.
	/// <para> Native object handles, UCHAR ObjectTypeIndex @ OBJECT_TYPE_INFORMATION. </para>
	/// <para> The ObjectTypeIndex may be subject to change accross Windows versions. </para>
	/// </summary>
	enum class UserHandleObjectType : uint8_t
	{
		Unknown = 0,
		Process = 1,
		Thread = 2,
		Mutant = 3,
		Event = 4,
		Section = 5,
		// BUGBUG WINBLUE 571662 2013-12-31 GenghisK: #ifdef this for OS after
		// WINBLUE
		Semaphore = 6
	};

	/// <summary>
	/// PSS_HANDLE_ENTRY @ processsnapshot.h data
	/// <para> SYSTEM_HANDLE_TABLE_ENTRY_INFO @ ntdll.h data </para>
	/// </summary>
	struct HandleModel
	{
		/// <summary>
		/// UNICODE_STRING TypeName @ OBJECT_TYPE_INFORMATION
		/// </summary>
		std::wstring typeName{};
		std::wstring objectName{};
		HANDLE handleValue{};
		ACCESS_MASK grantedAccess{};

		/// <summary>
		/// Based on <see cref="UserHandleObjectType"/>
		/// </summary>
		DWORD userTargetProcessId{};

		/// <summary>
		/// PSS_OBJECT_TYPE ObjectType field @ PSS_HANDLE_ENTRY
		/// </summary>
		UserHandleObjectType userHandleObjectType{};
	};
}