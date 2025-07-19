# C++ Math Library Demo with Jenkins Pipeline

This project demonstrates a simple C++ math library with a Jenkins pipeline for continuous integration. It includes basic mathematical operations and uses the spdlog library for logging.

## Project Structure

```
project_root/
├── CMakeLists.txt          # CMake build configuration
├── src/                    # Source code directory
│   ├── main.cpp           # Main application
│   ├── mathlib/           # Math library
│   │   ├── mathlib.h      # Library header
│   │   ├── mathlib.cpp    # Library implementation
├── tests/                 # Test directory (for future expansion)
├── build/                 # Build artifacts directory
├── Jenkinsfile           # Jenkins pipeline definition
```

## Prerequisites

- CMake (version 3.14 or higher)
- C++17 compatible compiler (e.g., GCC, Clang)
- Git
- Jenkins installation

## Building Locally

1. Clone the repository:
   ```bash
   git clone <your-repo-url>
   cd <repo-name>
   ```

2. Create and enter the build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure and build the project:
   ```bash
   cmake ..
   cmake --build .
   ```

4. Run the executable:
   ```bash
   ./mathdemo
   ```

## Setting Up Jenkins Pipeline

### Prerequisites Installation

1. Install Jenkins on your system:
   ```bash
   # For Ubuntu
   sudo apt-get update
   sudo apt-get install jenkins
   ```

2. Install required build tools:
   ```bash
   sudo apt-get install cmake g++ build-essential
   ```

### Jenkins Configuration

1. Open Jenkins in your web browser (typically http://localhost:8080)

2. Create a new Pipeline:
   - Click "New Item"
   - Enter a name for your pipeline
   - Select "Pipeline"
   - Click "OK"

3. Configure Pipeline:
   - Under "Pipeline", select "Pipeline script from SCM"
   - Select "Git" as SCM
   - Enter your repository URL
   - Specify branch to build (e.g., */main)
   - Save the configuration

4. The Jenkinsfile in the repository will automatically configure the build pipeline with the following stages:
   - Checkout: Clones the repository
   - Install Dependencies: Installs required packages
   - Build: Compiles the project using CMake
   - Run: Executes the compiled program

### Running the Pipeline

1. Push your code to GitHub:
   ```bash
   git add .
   git commit -m "Initial commit"
   git push origin main
   ```

2. In Jenkins:
   - Go to your pipeline
   - Click "Build Now"

## Features

- Basic mathematical operations (add, subtract, divide)
- Error handling for division by zero
- Logging using spdlog
- Automated build and test pipeline with Jenkins
- CMake-based build system
- C++17 features

## Adding New Features

1. Add new functions to `mathlib.h` and implement them in `mathlib.cpp`
2. Update `main.cpp` to test new functionality
3. Push changes to trigger the Jenkins pipeline

## Troubleshooting

If you encounter issues:

1. Check Jenkins console output for detailed error messages
2. Verify all prerequisites are installed
3. Ensure Jenkins has proper permissions to execute commands
4. Check system requirements (e.g., disk space, memory)

## License

This project is open source and available under the MIT License.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Support

For issues and questions, please open a GitHub issue in the repository.
