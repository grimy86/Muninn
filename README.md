<table>
  <tr>
    <td>
        <img width="200"
             src="banner.svg"
             alt="Corvus Banner" />
    </td>
    <td>
      <h1>Muninn</h1>
      <img src="https://custom-icon-badges.demolab.com/badge/Platform-Windows%2011-000000.svg?&logo=windows11&logoColor=white&style=for-the-badge" />
      <img src="https://img.shields.io/badge/Language-C%2B%2B%2017-000000?logo=c%2B%2B&logoColor=white&style=for-the-badge" />
      <img src="https://img.shields.io/badge/Language-C11-000000?logo=c&logoColor=white&style=for-the-badge" />
      <img src="https://img.shields.io/badge/License-MIT-000000?style=for-the-badge" />
      <img src="https://img.shields.io/badge/Snyk-Security%20Monitored-000000?logo=snyk&logoColor=white&style=for-the-badge" />
    </td>
  </tr>
</table>

## Summary
Muninn is a Windows SDK (x86/x64) implemented in C11 and C++17.
It exposes a minimal, exceptionless, C11 API for a stable ABI,
enabling straightforward integration with languages such as C#, Rust, and Python.

The project emphasizes architectural clarity, deterministic behavior, explicity and support over convenience abstractions.

> **Disclaimer**: This SDK project is relatively new, the architecture is constantly evolving. Some commits may not compile at this time.

## Table of contents
- [Summary](#summary)
- [Table of contents](#table-of-contents)
- [Design characteristics](#design-characteristics)
- [Architecture](#architecture)
  - [DataAccessLayer](#dataaccesslayer)
  - [ModelLayer](#modellayer)
  - [ControllerLayer](#controllerlayer)
  - [ViewLayer](#viewlayer)
  - [Mermaid Diagram](#mermaid-diagram)
- [Documentation](#documentation)
- [Third-party Libraries](#third-party-libraries)
- [Build requirements](#build-requirements)
- [Contributors](#contributors)

## Design characteristics
- Exceptionless ISO C++17 & C11 written in Visual Studio
- Explicit resource ownership
- No hidden global state
- Minimal STL usage beyond containers and strings (C-style)
- Experimental structures are clearly marked `[[deprecated]]`, if any
- Verbose naming convention
- Visual C++ XML documentation integrated with Doxygen

## Architecture
Corvus follows a layered MVC-inspired structure:

<details>

<summary>Layers</summary>
  
### DataAccessLayer
C11 wrappers around Windows API's like:
- Win32
- Native NT (ntdll)

These functions:
- Avoid exceptions
- Prefer direct `NTSTATUS` returns

### ModelLayer
Pure, strongly-defined data structures that unify data acquisition across:
- ToolHelp32
- PSAPI
- Process Snapshot API
- Native NT structures

### ControllerLayer
Higher-level singleton controllers responsible for orchestrating model lifecycle, including initialization, population, and safe resource management.

Copy semantics are intentionally disabled to prevent unsafe resource duplication.

### ViewLayer
Contains raw user-interface-related utilities.
This layer is isolated from native process logic.

### Mermaid Diagram
```mermaid
%%{init: {
    "securityLevel": "sandbox",
    "defaultRenderer": "elk",
    "theme": "forest",
    "themeVariables": {
    "fontFamily": "Consolas, monospace",
    "fontSize": "20px"},
  "flowchart": {
    "nodeSpacing": 50,
    "rankSpacing": 75,
    "padding": 15,
    "curve": "linear"
  }
}}%%
flowchart TD
    subgraph WINAPI["Windows / PHNT"]
    K32["Kernel32.dll & KernelBase.dll"]@{ shape: lin-cyl }
    NT["ntdll.dll"]@{ shape: lin-cyl }
    end

    subgraph DAL["DataAccessLayer (C11)"]
    K32 <--> W32[WindowsProvider32.c]
    NT <--> WNT[WindowsProviderNt.c]
    W32 <--> DUTIL[DataUtilities.c]
    WNT <--> DUTIL
    end

    subgraph CORE["Core (c++17)"]
        subgraph ModelLayer
        MM[MathModels.h]
        WM[WindowsModels.h]
        end

        subgraph ControllerLayer
        SC[SystemController.cpp]
        PC[ProcessController.cpp]
        end

        ModelLayer --> ControllerLayer
    end
    DAL <--> ControllerLayer
```

</details>

## Documentation 
The project is documented using XML comments in Visual C++ within Visual Studio. The generated documentation is hosted online and accessible at: https://grimy86.github.io/Muninn/

## Third-party Libraries
Process Hacker Native Types:
  - [MIT License](https://github.com/winsiderss/phnt/blob/fc1f96ee976635f51faa89896d1d805eb0586350/LICENSE)  
  - [GitHub phnt repository](https://github.com/processhacker/phnt)

## Build requirements
C11 & C++17 require:
  - [Visual Studio](https://visualstudio.microsoft.com/)
  - [Desktop development with C++](https://learn.microsoft.com/en-us/cpp/build/vscpp-step-0-installation?view=msvc-170)

## Contributors
Thanks to these wonderful people for contributing:

<a href="https://github.com/grimy86/corvus/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Grimy86/corvus" />
</a>