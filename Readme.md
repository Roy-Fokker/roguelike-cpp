# Roguelike Tutorials in C++

## VSCode
### Required Extensions
- MS Cpp Tools
- CMake Tools Helper
  - CMake
  - CMake Tools

### Project Configuration
- In `.vscode/c_cpp_properties.json` add
  ```json 
  "compilerArgs": [
                "/std:c++17",
                "/permissive-"
            ]
  ```
- In `launch.json` change 
  ```json
  "program": "${command:cmake.launchTargetPath}",
  "cwd": "${workspaceFolder}\\build\\bin",
  "externalConsole": true
  ```