# Mystic Brawl

**Mystic Brawl** is a 2D game built using **C++**, **OpenGL**, and **GLFW**, showcasing basic game mechanics like textured rendering, player movement, and enemies. This is an educational project to explore low-level graphics programming using OpenGL.
---

## 🚀 Features

- Textured background rendering
- Player character with WASD and arrow key movement
- Multiple enemies rendered on screen
- Basic shader-based rendering using GLSL
- OpenGL context created via GLFW
- Texture loading using stb_image

---

## 🛠️ Technologies Used

- **C++**
- **OpenGL 3.3**
- **GLFW**
- **GLAD**
- **stb_image.h** for image loading

---

## 🖥️ Controls

| Key      | Action          |
|----------|-----------------|
| W/A/S/D  | Move player     |
| Space    | Attack          |
| ESC      | Exit game       |

---

## 🧩 How to Build

### Prerequisites

- C++17 or later
- OpenGL development libraries
- GLFW
- GLAD

### Build Instructions (Linux/macOS)

```bash
git clone https://github.com/yourusername/MysticBrawl.git
cd MysticBrawl
g++ main.cpp glad.c -I. -ldl -lglfw -o mythic
./MysticBrawl
