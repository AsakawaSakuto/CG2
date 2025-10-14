# Particle Emitter Shapes Usage Guide

This guide demonstrates how to use the newly implemented particle emitter shapes in your particle system.

## Available Emitter Shapes

1. **Point Emitter** - Spawns particles at an exact position
2. **Line Emitter** - Spawns particles along a line
3. **Sphere Volume Emitter** - Spawns particles inside a sphere
4. **Sphere Surface Emitter** - Spawns particles only on the sphere surface
5. **Box Emitter** - Spawns particles inside a box
6. **Ring Emitter** - Spawns particles on a ring/circle (XZ plane - horizontal)
7. **Box Surface Emitter** - Spawns particles only on box surfaces
8. **Ring XY Emitter** - Spawns particles on a ring in XY plane (vertical facing forward)
9. **Ring YZ Emitter** - Spawns particles on a ring in YZ plane (vertical facing right)
10. **Cone Emitter** - Spawns particles inside a cone volume (adjustable angle)
11. **Cone Surface Emitter** - Spawns particles only on cone surface (adjustable angle)
12. **Hemisphere Emitter** - Spawns particles inside a hemisphere volume (adjustable angle)
13. **Hemisphere Surface Emitter** - Spawns particles only on hemisphere surface (adjustable angle)

## Usage Examples

### C++ Code Examples

```cpp
// Initialize a particle system
Particles particles;
particles.Initialize(dxCommon, "circle", 2); // texture name, max particle multiplier

// Get the emitter state
EmitterState emitter = {};

// Example 1: Point Emitter
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::POINT);
emitter.translate = {0.0f, 0.0f, 0.0f}; // Spawn point
particles.SetEmitterValue(emitter);

// Example 2: Line Emitter
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::LINE);
emitter.lineStart = {-2.0f, 0.0f, 0.0f};    // Start point of line
emitter.size = {1.0f, 0.0f, 0.0f};          // Direction vector
emitter.lineLength = 4.0f;                  // Length of line
particles.SetEmitterValue(emitter);

// Example 3: Sphere Volume Emitter
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::SPHERE_VOLUME);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Center of sphere
emitter.radius = 2.0f;                      // Radius
particles.SetEmitterValue(emitter);

// Example 4: Sphere Surface Emitter
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::SPHERE_SURFACE);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Center of sphere
emitter.radius = 2.0f;                      // Radius
particles.SetEmitterValue(emitter);

// Example 5: Box Emitter
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::BOX);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Center of box
emitter.size = {4.0f, 2.0f, 1.0f};         // Width, Height, Depth
particles.SetEmitterValue(emitter);

// Example 6: Ring Emitter (XZ plane - horizontal)
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::RING);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Center of ring
emitter.ringInnerRadius = 1.0f;             // Inner radius
emitter.ringOuterRadius = 3.0f;             // Outer radius
particles.SetEmitterValue(emitter);

// Example 7: Box Surface Emitter
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::BOX_SURFACE);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Center of box
emitter.size = {4.0f, 2.0f, 1.0f};         // Width, Height, Depth
particles.SetEmitterValue(emitter);

// Example 8: Ring XY Emitter (vertical facing forward)
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::RING_XY);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Center of ring
emitter.ringInnerRadius = 1.0f;             // Inner radius
emitter.ringOuterRadius = 3.0f;             // Outer radius
particles.SetEmitterValue(emitter);

// Example 9: Ring YZ Emitter (vertical facing right)
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::RING_YZ);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Center of ring
emitter.ringInnerRadius = 1.0f;             // Inner radius
emitter.ringOuterRadius = 3.0f;             // Outer radius
particles.SetEmitterValue(emitter);

// Example 10: Cone Emitter
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::CONE);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Base center of cone
emitter.coneAngle = 45.0f;                  // Cone angle in degrees (0-180)
emitter.coneHeight = 3.0f;                  // Cone height
emitter.coneDirection = {0.0f, 1.0f, 0.0f}; // Direction (normalized)
particles.SetEmitterValue(emitter);

// Example 11: Cone Surface Emitter
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::CONE_SURFACE);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Base center of cone
emitter.coneAngle = 30.0f;                  // Cone angle in degrees (0-180)
emitter.coneHeight = 2.0f;                  // Cone height
emitter.coneDirection = {0.0f, 1.0f, 0.0f}; // Direction (normalized)
particles.SetEmitterValue(emitter);

// Example 12: Hemisphere Emitter
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::HEMISPHERE);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Center of hemisphere
emitter.radius = 2.0f;                      // Hemisphere radius
emitter.hemisphereAngle = 90.0f;            // Hemisphere angle in degrees (0-180)
emitter.coneDirection = {0.0f, 1.0f, 0.0f}; // Direction (normalized)
particles.SetEmitterValue(emitter);

// Example 13: Hemisphere Surface Emitter
emitter.shapeType = static_cast<uint32_t>(EmitterShapeType::HEMISPHERE_SURFACE);
emitter.translate = {0.0f, 0.0f, 0.0f};     // Center of hemisphere
emitter.radius = 1.5f;                      // Hemisphere radius
emitter.hemisphereAngle = 120.0f;           // Hemisphere angle in degrees (0-180)
emitter.coneDirection = {0.0f, 1.0f, 0.0f}; // Direction (normalized)
particles.SetEmitterValue(emitter);
```

### Advanced Usage Examples

```cpp
// In your scene class (e.g., GameScene.cpp)
class GameScene {
private:
    std::unique_ptr<Particles> fireParticles_;
    std::unique_ptr<Particles> smokeParticles_;
    std::unique_ptr<Particles> explosionParticles_;
    std::unique_ptr<Particles> torchParticles_;
    std::unique_ptr<Particles> fountainParticles_;
    std::unique_ptr<Particles> flashlightParticles_;

public:
    void Initialize() {
        // Torch fire effect using cone emitter
        torchParticles_ = std::make_unique<Particles>();
        torchParticles_->Initialize(dxCommon_, "flame", 1);
        
        EmitterState torchEmitter = {};
        torchEmitter.shapeType = static_cast<uint32_t>(EmitterShapeType::CONE);
        torchEmitter.translate = {0.0f, 2.0f, 0.0f};
        torchEmitter.coneAngle = 20.0f;  // Narrow flame
        torchEmitter.coneHeight = 1.0f;
        torchEmitter.coneDirection = {0.0f, 1.0f, 0.0f};  // Upward
        torchEmitter.count = 8;
        torchEmitter.frequency = 0.1f;
        torchEmitter.startVelocity = {0.0f, 1.0f, 0.0f};
        torchEmitter.velocityRandom = 1;
        torchEmitter.minVelocity = {-0.2f, 0.5f, -0.2f};
        torchEmitter.maxVelocity = {0.2f, 1.5f, 0.2f};
        torchEmitter.startColor = {1.0f, 0.8f, 0.0f};
        torchEmitter.endColor = {1.0f, 0.2f, 0.0f};
        torchEmitter.colorFade = 1;
        torchEmitter.alphaFade = 1;
        torchEmitter.lifeTime = 2.0f;
        torchParticles_->SetEmitterValue(torchEmitter);
        
        // Water fountain using hemisphere emitter
        fountainParticles_ = std::make_unique<Particles>();
        fountainParticles_->Initialize(dxCommon_, "water", 2);
        
        EmitterState fountainEmitter = {};
        fountainEmitter.shapeType = static_cast<uint32_t>(EmitterShapeType::HEMISPHERE);
        fountainEmitter.translate = {0.0f, 0.0f, 0.0f};
        fountainEmitter.radius = 0.5f;
        fountainEmitter.hemisphereAngle = 60.0f;  // Narrow fountain spray
        fountainEmitter.coneDirection = {0.0f, 1.0f, 0.0f};  // Upward
        fountainEmitter.count = 15;
        fountainEmitter.frequency = 0.05f;
        fountainEmitter.startVelocity = {0.0f, 3.0f, 0.0f};
        fountainEmitter.velocityRandom = 1;
        fountainEmitter.minVelocity = {-1.0f, 2.0f, -1.0f};
        fountainEmitter.maxVelocity = {1.0f, 4.0f, 1.0f};
        fountainEmitter.startColor = {0.3f, 0.5f, 1.0f};
        fountainEmitter.endColor = {0.1f, 0.3f, 0.8f};
        fountainEmitter.colorFade = 1;
        fountainEmitter.alphaFade = 1;
        fountainEmitter.lifeTime = 3.0f;
        fountainParticles_->SetEmitterValue(fountainEmitter);
        
        // Flashlight beam using cone surface emitter
        flashlightParticles_ = std::make_unique<Particles>();
        flashlightParticles_->Initialize(dxCommon_, "dust", 1);
        
        EmitterState flashlightEmitter = {};
        flashlightEmitter.shapeType = static_cast<uint32_t>(EmitterShapeType::CONE_SURFACE);
        flashlightEmitter.translate = {0.0f, 1.0f, 0.0f};
        flashlightEmitter.coneAngle = 30.0f;  // Flashlight beam angle
        flashlightEmitter.coneHeight = 5.0f;  // Beam distance
        flashlightEmitter.coneDirection = {0.0f, 0.0f, 1.0f};  // Forward
        flashlightEmitter.count = 5;
        flashlightEmitter.frequency = 0.2f;
        flashlightEmitter.startVelocity = {0.0f, 0.0f, 0.0f};  // Static particles
        flashlightEmitter.startColor = {1.0f, 1.0f, 0.8f};
        flashlightEmitter.endColor = {0.8f, 0.8f, 0.6f};
        flashlightEmitter.colorFade = 1;
        flashlightEmitter.alphaFade = 1;
        flashlightEmitter.lifeTime = 1.5f;
        flashlightParticles_->SetEmitterValue(flashlightEmitter);
    }
    
    void Update() {
        torchParticles_->Update();
        fountainParticles_->Update();
        flashlightParticles_->Update();
    }
    
    void Draw(Camera& camera) {
        torchParticles_->Draw(camera);
        fountainParticles_->Draw(camera);
        flashlightParticles_->Draw(camera);
    }
    
    void DrawImGui() {
        torchParticles_->DrawImGui("Torch Particles");
        fountainParticles_->DrawImGui("Fountain Particles");
        flashlightParticles_->DrawImGui("Flashlight Particles");
    }
};
```

## ImGui Controls

When you call `DrawImGui()` on your particle system, you'll now see:

1. **Emitter Shape** dropdown - Select from:
   - Point
   - Line
   - Sphere (Volume)
   - Sphere (Surface)
   - Box
   - Ring
   - Box (Surface)
   - Ring (XY Plane)
   - Ring (YZ Plane)
   - Cone
   - Cone (Surface)
   - Hemisphere
   - Hemisphere (Surface)

2. **Shape-specific parameters** that appear based on your selection:
   - **Point**: No additional parameters
   - **Line**: Line Start, Line Direction, Line Length
   - **Sphere**: Radius (with surface/volume indicator)
   - **Box/Box Surface**: Box Size (width, height, depth)
   - **Ring/Ring XY/Ring YZ**: Inner Radius, Outer Radius (with plane indicator)
   - **Cone/Cone Surface**: Cone Angle (0-180Åã), Cone Height, Cone Direction
   - **Hemisphere/Hemisphere Surface**: Radius, Hemisphere Angle (0-180Åã), Direction

## Effect Ideas

### Point Emitter
- Magic spells casting from a wand tip
- Bullet hit effects  
- Small focused explosions

### Line Emitter
- Laser beam effects
- Lightning bolts
- Rain effects
- Sword trail particles

### Sphere Volume Emitter
- Magical aura effects
- Area of effect spells
- Ambient particle clouds

### Sphere Surface Emitter
- Bubble effects
- Force field boundaries
- Planet atmosphere effects

### Box Emitter
- Room-filling effects (dust, smoke)
- Building destruction debris
- Area denial effects

### Box Surface Emitter
- Building facade effects
- Window breaking effects
- Container surface corrosion
- Wall-mounted effects

### Ring Emitter (XZ - Horizontal)
- Ground-based shockwaves
- Magic circles on the floor
- Explosion rings
- Power-up collection effects

### Ring XY Emitter (Vertical - Forward Facing)
- Portal effects
- Vertical magic circles
- Doorway effects
- Force field barriers

### Ring YZ Emitter (Vertical - Side Facing)
- Side portal effects
- Spinning shield effects
- Vertical barrier effects
- Wheel-like magical effects

### Cone Emitter
- Torch flames
- Rocket exhaust
- Spray effects
- Directional explosions
- Volcanic eruptions
- Breath attacks

### Cone Surface Emitter
- Flashlight beams
- Searchlight effects
- Laser cone boundaries
- Spotlight dust particles
- Sound wave visualizations

### Hemisphere Emitter
- Water fountains
- Dome shields
- Umbrella-like effects
- Directional area spells
- Underground explosions

### Hemisphere Surface Emitter
- Dome force fields
- Radar sweep effects
- Hemisphere barriers
- Protective domes
- Half-sphere magical effects

## Advanced Usage Tips

### Directional Effects with Cones
```cpp
// Create a directed flame thrower effect
EmitterState flameThrower = {};
flameThrower.shapeType = static_cast<uint32_t>(EmitterShapeType::CONE);
flameThrower.coneAngle = 15.0f;  // Narrow stream
flameThrower.coneHeight = 4.0f;  // Long range
flameThrower.coneDirection = {1.0f, 0.0f, 0.0f};  // Right direction

// Create a wide spray effect
EmitterState spray = {};
spray.shapeType = static_cast<uint32_t>(EmitterShapeType::CONE);
spray.coneAngle = 90.0f;  // Wide spray
spray.coneHeight = 1.0f;  // Short range
```

### Hemisphere Variations
```cpp
// Full hemisphere (180 degrees)
EmitterState fullDome = {};
fullDome.hemisphereAngle = 180.0f;

// Quarter hemisphere (90 degrees)
EmitterState quarterDome = {};
quarterDome.hemisphereAngle = 90.0f;

// Narrow directional spray (30 degrees)
EmitterState directionalSpray = {};
directionalSpray.hemisphereAngle = 30.0f;
```

### Combining Cone and Hemisphere
```cpp
// Rocket engine: Cone for main exhaust + Hemisphere for heat distortion
EmitterState mainExhaust = {};
mainExhaust.shapeType = static_cast<uint32_t>(EmitterShapeType::CONE);
mainExhaust.coneAngle = 20.0f;

EmitterState heatDistortion = {};  
heatDistortion.shapeType = static_cast<uint32_t>(EmitterShapeType::HEMISPHERE);
heatDistortion.hemisphereAngle = 60.0f;
```

## Performance Considerations

- **Point emitters** are the most performance-friendly
- **Cone and Hemisphere emitters** require trigonometric calculations (moderate cost)
- **Surface variants** are slightly more expensive than volume variants due to additional surface selection logic
- **Hemisphere emitters** are similar in cost to sphere emitters
- **Cone emitters** have additional calculations for height-based radius scaling
- Consider using fewer particles with larger textures for distant effects
- Use appropriate max particle counts based on the effect type

## Tips

1. **Combine multiple emitters** for complex effects (e.g., torch = cone flame + hemisphere heat shimmer)
2. **Use different blend modes** for different visual effects
3. **Animate emitter properties** over time for dynamic effects (rotating cone direction, changing hemisphere angle)
4. **Use velocity randomization** with directional emitters for more natural-looking effects
5. **Save and load configurations** using the JSON system for reusable effects
6. **Experiment with angles** - small cone angles create focused beams, large angles create wide sprays
7. **Use direction vectors** creatively - point cones sideways, downward, or at angles for varied effects
8. **Combine surface and volume emitters** of the same shape for layered effects