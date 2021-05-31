# Headless-Droid-Engine
Prototyping rendering framework served as a playground for my experiments

## Disclamer
* HDE will be in a development state (aka Not Completed) for a long time.
* HDE will be frequently refactored with API breaking changes here and there.

## Goals
* Layered architecture
* Custom allocators for memory management
* No STL
* Windows only
* Vulkan + DX12
* Heterogeneous Multi-GPU
* Render graph
* Minimal dependencies
* C++ latest

## External tools and libraries
* Premake (https://premake.github.io/)
* ImGUI (https://github.com/ocornut/imgui/)
* DXC (https://github.com/microsoft/DirectXShaderCompiler)
* fast_obj (https://github.com/thisistherk/fast_obj)
* meshoptimizer (https://github.com/zeux/meshoptimizer)

## Quick Guide
### Prerequisites
1. Visual Studio 2019
2. Windows SDK 10.0.17763.0 (or put any version you have into systemversion variable in Tools/HeadlessDroidSolutionDescription.lua)

### How to Build
1. Run GenerateProject.bat from repository root.
2. Open Development/HeadlessDroidEngine.sln
3. Press F5 or click Run.