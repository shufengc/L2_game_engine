# L2-Game-Engine

L2-Game-Engine is a 2D game engine written in C++ with Lua as the scripting language. It incorporates various gaming elements, including physics middleware using Box2D and support for joystick inputs. It is designed for cross-platform use, supporting Linux, OSX, and Windows. The engine allows game developers to easily integrate physics, input systems, and custom game logic written in Lua.

## Features

- **C++ Core Engine**: The core of the engine is written in C++ to ensure optimal performance.
- **Lua Scripting**: Lua is used as the guest language, allowing for high flexibility in game logic and modding.
- **Physics Engine**: Integrates Box2D for realistic 2D physics simulations.
- **Joystick Input**: Supports up to two joysticks for enhanced input capabilities.
- **Cross-Platform**: Supports Linux, OSX, and Windows.

## Build Process

### Prerequisites

Before building the engine, ensure that you have the following installed:

- C++17 compatible compiler
- CMake (minimum version 3.5)
- Box2D (for physics engine)
- Lua 5.3+

### Building on Linux

#### Clone the repository
git clone https://github.com/shufengc/L2-game-engine.git

#### Navigate to the directory
cd L2-game-engine

#### Create a build directory
mkdir build && cd build

#### Generate build files using CMake
cmake ..

#### Build the project
make

### Building on OSX

#### Clone the repository
```bash
git clone https://github.com/shufengc/L2-game-engine.git

#### Navigate to the directory
cd L2-game-engine

#### Create a build directory
mkdir build && cd build

#### Generate Xcode project files using CMake
cmake -G Xcode ..

#### Open the project in Xcode and build
open L2-game-engine.xcodeproj

### Building on Windows

1. Clone the repository and open the `.vcxproj` file in Visual Studio.
2. Build the project using the solution explorer.

## Usage

Once the engine is built, you can start integrating Lua scripts to define your game logic, control actors, and handle interactions in the game world. Additionally, the engine can handle physics simulations using Box2D, and you can configure custom joystick inputs for player control.

### Example Lua Script

-- Example to create a player and enable joystick input
local player = CreateActor("Player")
player:SetPosition(0, 0)
player:EnableJoystickInput(1) -- 1 for first joystick

## Development Process Summary

The L2-game-engine was developed incrementally through a series of homework assignments, each focusing on a different aspect of game engine design:

1. **Homework 3**: Game Loops & Text Adventures  
   - **Pitch:**  
     This homework introduced the core mechanics of game engines by building a simple text-based adventure. It focused on the game loop concept, where the engine continuously processes input, updates the game state, and renders output.
   - **Purpose:**  
     Establish a foundational understanding of the game loop and its three stages: Input, Update, and Render.
   - **Key Tasks:**  
     - Implement a basic text-based game engine.
     - Create an Input -> Update -> Render loop.
     - Process user input and update the game state accordingly.
     - Render the game state as text in the console.

2. **Homework 4**: Modding & Customizability  
   - **Pitch:**  
     This homework focused on adding modding capabilities to the engine, allowing users to customize game logic and content using the Lua scripting language. By externalizing game logic, the engine became more flexible and extensible.
   - **Purpose:**  
     Enable game developers to modify and extend game behavior without altering the engine’s core code.
   - **Key Tasks:**  
     - Integrate Lua scripting for external gameplay logic.
     - Allow users to define custom game content through scripts.
     - Provide APIs for Lua to interact with core engine features.

3. **Homework 5**: From ASCII to 2D  
   - **Pitch:**  
     Converted the text-based engine into a 2D engine using SDL middleware. The assignment transitioned the engine from console-based ASCII rendering to 2D graphics, while keeping core gameplay intact.
   - **Purpose:**  
     Introduce 2D rendering and basic graphical concepts through SDL, converting the existing engine from a textual to a visual experience.
   - **Key Tasks:**  
     - Implement bitmap and audio rendering using SDL.
     - Convert the game from turn-based to real-time gameplay.
     - Render 2D graphics for actors, scenes, and UI elements.
     - Handle inputs through SDL’s event queue system.

4. **Homework 6**: Continuous Worlds  
   - **Pitch:**  
     Expanded the engine to support continuous worlds with smooth camera transitions, collision detection, and seamless movement. This homework introduced larger game environments and realistic navigation within them.
   - **Purpose:**  
     Enable continuous world exploration with fluid camera movement and basic collision detection.
   - **Key Tasks:**  
     - Implement continuous camera movement across large game worlds.
     - Integrate collision detection for actors and environment objects.
     - Smoothly transition between different areas of the game world.

5. **Homework 7**: Language Hosting  
   - **Pitch:**  
     Introduced language hosting via Lua, enabling dynamic gameplay logic externalized from the engine. By integrating Lua, the engine could load scripts at runtime, providing unprecedented flexibility in game design.
   - **Purpose:**  
     Allow game designers to script custom logic in Lua, decoupling gameplay logic from the core engine and enabling dynamic updates.
   - **Key Tasks:**  
     - Integrate Lua scripting into the engine using LuaBridge.
     - Enable dynamic loading and execution of Lua scripts for game logic.
     - Allow game designers to control actor behaviors and game rules via scripts.

6. **Homework 8**: Physics Middleware (Box2D Integration)  
   - **Pitch:**  
     This homework focused on integrating the Box2D physics engine to add realistic physics interactions to the engine. With this integration, the engine supported collision detection, gravity, friction, and other advanced physical simulations.
   - **Purpose:**  
     Incorporate real-time physics into the engine, providing more dynamic and realistic gameplay experiences.
   - **Key Tasks:**  
     - Integrate Box2D for advanced physics simulation.
     - Implement collision handling, gravity, and friction.
     - Apply physical properties to actors for realistic interactions.

## Technologies Used

- **C++**: The core of the engine is written in C++ for performance.
- **Lua**: Used as the scripting language for game logic and modding.
- **Box2D**: Integrated for physics simulations like gravity, collision detection, and rigid body dynamics.
- **SDL**: Used for rendering and handling input across different platforms.




