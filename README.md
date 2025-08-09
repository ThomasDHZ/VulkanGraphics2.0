# VulkanGraphics2.0

An early prototype of a Vulkan-based game engine built in C++, serving as a precursor to the [Eclipse Game Engine](https://github.com/ThomasDHZ/EclipseEngine). VulkanGraphics2.0 explores fundamental Vulkan rendering techniques, including initialization and basic shader pipelines, as a learning exercise in graphics programming. This project demonstrates foundational C++ and Vulkan skills, laying the groundwork for advanced features like ray tracing and PBR lighting in Eclipse.

## Features

- **Vulkan Rendering**: Implements basic Vulkan rendering pipeline for simple 3D scenes, focusing on initialization and setup.
- **C++ Fundamentals**: Practices core C++ programming, including object-oriented design and memory management.
- **Shader Integration**: Uses GLSL shaders for basic rendering effects, exploring Vulkan’s graphics capabilities.
- **Learning Focus**: Serves as an educational project to master Vulkan API and graphics programming concepts.

## Technologies

- **Language**: C++
- **Graphics API**: Vulkan
- **Tools**: GLSL, Visual Studio 2022
- **Build System**: Visual Studio 2022 (v143 toolset)

## Setup Instructions

To build and run VulkanGraphics2.0:

1. **Install Prerequisites**:
   - [Vulkan SDK](https://vulkan.lunarg.com/) (set `VULKAN_SDK` environment variable).
   - Visual Studio 2022 with C++ Desktop Development workload.
   - Git for cloning dependencies (if any).

2. **Clone the Repository**:
   ```bash
   git clone https://github.com/ThomasDHZ/VulkanGraphics2.0.git
   cd VulkanGraphics2.0
   ```

3. **Build the Project**:
   - Open `VulkanGraphics2.0.sln` in Visual Studio 2022.
   - Set configuration to `Release|x64` or `Debug|x64`.
   - Build the solution, ensuring `$(VULKAN_SDK)` is set for Vulkan includes/libs.
   - Alternatively, use a build tool like CMake (if applicable):
     ```bash
     cmake -S . -B build
     cmake --build build --config Release
     ```

4. **Run the Project**:
   - Execute the binary (e.g., `VulkanGraphics2.0.exe`) to render a sample scene.

## Related Projects

VulkanGraphics2.0 is an early prototype that evolved into the [Eclipse Game Engine](https://github.com/ThomasDHZ/EclipseEngine), a mature 3D graphics engine with advanced features like ray tracing and PBR lighting.

## Contributing

Contributions are welcome! Fork the repository, create a feature branch, and submit a pull request. For major changes, open an issue to discuss ideas.

## License

[MIT License](LICENSE) - free to use and modify for personal or commercial projects.
