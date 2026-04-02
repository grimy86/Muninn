#pragma once
#include <Windows.h>
#include <vector>

namespace Muninn::Model
{
	/// <summary>
	/// TOKEN_PRIVILEGES @ winnt.h data
	/// <para> TOKEN_PRIVILEGES @ ntdll.h data </para>
	/// </summary>
	struct PrivilegeEntry
	{
		uint64_t TokenLuid{};
		DWORD TokenAttributes{};
	};

	/// <summary>
	/// Data for process access token information.
	/// <para> OpenProcessToken() @ Processthreadsapi.h </para>
	/// <para> GetTokenInformation() @ Securitybaseapi.h </para>
	/// <para> NtOpenProcessToken() @ ntdll.h </para>
	/// <para> NtQueryInformationToken() @ ntdll.h </para>
	/// </summary>
	struct AccessTokenModel
	{
		/// <summary>
		/// Returns token attributes and tokenLuid's for each privilege held by the token.
		/// <para>Arg: TOKEN_INFORMATION_CLAS::TOKEN_PRIVILEGES </para>
		/// </summary>
		std::vector<PrivilegeEntry> TokenPrivileges{};

		/// <summary>
		/// Arg: TOKEN_INFORMATION_CLAS::TOKEN_STATISTICS
		/// </summary>
		uint64_t TokenId{};

		/// <summary>
		/// Arg: TOKEN_INFORMATION_CLAS::TOKEN_STATISTICS
		/// </summary>
		uint64_t AuthenticationId{};

		/// <summary>
		/// Arg: TOKEN_INFORMATION_CLAS::TOKEN_STATISTICS
		/// </summary>
		DWORD SessionId{};
	};
}