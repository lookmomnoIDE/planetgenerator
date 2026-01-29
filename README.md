# Planet Generator

A real-time procedural planet generation tool with OpenGL rendering and Blender integration.

## System Requirements

- **OS**: Windows 10/11 (Linux untested)
- **Note**: Initial launch and parameter changes may cause performance slowdown on lower-end systems. Performance stabilizes during normal operation.

## Controls

### Camera Movement
- **W, A, S, D**: Move camera on the XY plane
- **Space**: Move camera up
- **Left Alt**: Move camera down
- **Left Ctrl**: Toggle between cursor camera control and GUI cursor control

### Interface
- **P**: Toggle ImGui demo window
- **ESC**: Exit application
- **Menu Bar**: File → Exit for alternative exit option

### Display Options
- Light/Dark mode toggle available in Edit dropdown menu

## Exporting Your Planet

Generated planets can be exported as `.ply` files for use in Blender and other 3D software that supports the Stanford PLY format.

### Blender Import Workflow

1. Open Blender and create a new project
2. Delete the default cube (optional)
3. Navigate to **File → Import → Stanford PLY (.ply)**
4. Select your exported `planet.ply` file
5. Click **Import PLY**

### Applying Colors in Blender

To utilize the exported color attributes:

1. Create a new material for the imported mesh
2. Open the Shader Editor
3. Add a **Color Attribute** node
4. Connect the Color Attribute output to your desired shader input

**Recommended Setup:**
- Route color attributes through a **Principled BSDF** shader for advanced control over:
  - Metallic properties
  - Roughness
  - Alpha channel
- Connect the shader output to the **Material Output** node

Once imported, you can export the planet in any format supported by Blender.

## Features

- Real-time procedural generation with customizable parameters
- Interactive camera system with unlocked world/view space navigation
- **Reset Camera** button to return to default position if disoriented
- Editable generation attributes (exploration encouraged)

## Support

For questions, comments, or concerns, please contact:  
**cameronbeckwith@gmail.com**

*Response time may vary*

---

*Procedural generation parameters are intentionally left for user experimentation*


