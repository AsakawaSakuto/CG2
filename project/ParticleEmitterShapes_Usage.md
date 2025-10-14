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
```

### Using in Game Scenes

```cpp
// In your scene class (e.g., GameScene.cpp)
class GameScene {
private:
    std::unique_ptr<Particles> fireParticles_;
    std::unique_ptr<Particles> smokeParticles_;
    std::unique_ptr<Particles> explosionParticles_;
    std::unique_ptr<Particles> portalParticles_;
    std::unique_ptr<Particles> shieldParticles_;

public:
    void Initialize() {
        // Fire effect using sphere surface emitter
        fireParticles_ = std::make_unique<Particles>();
        fireParticles_->Initialize(dxCommon_, "fire", 1);
        
        EmitterState fireEmitter = {};
        fireEmitter.shapeType = static_cast<uint32_t>(EmitterShapeType::SPHERE_SURFACE);
        fireEmitter.translate = {0.0f, 0.0f, 0.0f};
        fireEmitter.radius = 0.5f;
        fireEmitter.count = 5;
        fireEmitter.frequency = 0.1f;
        fireEmitter.startVelocity = {0.0f, 2.0f, 0.0f};
        fireEmitter.startColor = {1.0f, 0.3f, 0.0f};
        fireEmitter.endColor = {1.0f, 0.0f, 0.0f};
        fireEmitter.colorFade = 1;
        fireEmitter.alphaFade = 1;
        fireEmitter.lifeTime = 2.0f;
        fireParticles_->SetEmitterValue(fireEmitter);
        
        // Smoke effect using box surface emitter
        smokeParticles_ = std::make_unique<Particles>();
        smokeParticles_->Initialize(dxCommon_, "smoke", 1);
        
        EmitterState smokeEmitter = {};
        smokeEmitter.shapeType = static_cast<uint32_t>(EmitterShapeType::BOX_SURFACE);
        smokeEmitter.translate = {0.0f, 1.0f, 0.0f};
        smokeEmitter.size = {1.0f, 0.5f, 1.0f};
        smokeEmitter.count = 3;
        smokeEmitter.frequency = 0.2f;
        smokeEmitter.startVelocity = {0.0f, 1.0f, 0.0f};
        smokeEmitter.velocityRandom = 1;
        smokeEmitter.minVelocity = {-0.5f, 0.5f, -0.5f};
        smokeEmitter.maxVelocity = {0.5f, 2.0f, 0.5f};
        smokeEmitter.startColor = {0.8f, 0.8f, 0.8f};
        smokeEmitter.endColor = {0.3f, 0.3f, 0.3f};
        smokeEmitter.colorFade = 1;
        smokeEmitter.alphaFade = 1;
        smokeEmitter.lifeTime = 4.0f;
        smokeParticles_->SetEmitterValue(smokeEmitter);
        
        // Explosion effect using ring emitter
        explosionParticles_ = std::make_unique<Particles>();
        explosionParticles_->Initialize(dxCommon_, "spark", 2);
        
        EmitterState explosionEmitter = {};
        explosionEmitter.shapeType = static_cast<uint32_t>(EmitterShapeType::RING);
        explosionEmitter.translate = {0.0f, 0.0f, 0.0f};
        explosionEmitter.ringInnerRadius = 0.1f;
        explosionEmitter.ringOuterRadius = 0.5f;
        explosionEmitter.count = 20;
        explosionEmitter.frequency = 10.0f; // Burst effect
        explosionEmitter.velocityRandom = 1;
        explosionEmitter.minVelocity = {-5.0f, -5.0f, -5.0f};
        explosionEmitter.maxVelocity = {5.0f, 5.0f, 5.0f};
        explosionEmitter.startColor = {1.0f, 1.0f, 0.0f};
        explosionEmitter.endColor = {1.0f, 0.2f, 0.0f};
        explosionEmitter.colorFade = 1;
        explosionEmitter.alphaFade = 1;
        explosionEmitter.lifeTime = 1.5f;
        explosionParticles_->SetEmitterValue(explosionEmitter);
        
        // Portal effect using ring XY emitter
        portalParticles_ = std::make_unique<Particles>();
        portalParticles_->Initialize(dxCommon_, "magic", 1);
        
        EmitterState portalEmitter = {};
        portalEmitter.shapeType = static_cast<uint32_t>(EmitterShapeType::RING_XY);
        portalEmitter.translate = {0.0f, 0.0f, 0.0f};
        portalEmitter.ringInnerRadius = 2.0f;
        portalEmitter.ringOuterRadius = 2.5f;
        portalEmitter.count = 10;
        portalEmitter.frequency = 0.1f;
        portalEmitter.startVelocity = {0.0f, 0.0f, 0.0f};
        portalEmitter.startColor = {0.0f, 0.5f, 1.0f};
        portalEmitter.endColor = {1.0f, 0.0f, 1.0f};
        portalEmitter.colorFade = 1;
        portalEmitter.alphaFade = 1;
        portalEmitter.rotateMove = 1;
        portalEmitter.startRotateVelocity = 1.0f;
        portalEmitter.lifeTime = 3.0f;
        portalParticles_->SetEmitterValue(portalEmitter);
        
        // Shield effect using ring YZ emitter
        shieldParticles_ = std::make_unique<Particles>();
        shieldParticles_->Initialize(dxCommon_, "energy", 1);
        
        EmitterState shieldEmitter = {};
        shieldEmitter.shapeType = static_cast<uint32_t>(EmitterShapeType::RING_YZ);
        shieldEmitter.translate = {0.0f, 0.0f, 0.0f};
        shieldEmitter.ringInnerRadius = 1.5f;
        shieldEmitter.ringOuterRadius = 2.0f;
        shieldEmitter.count = 8;
        shieldEmitter.frequency = 0.15f;
        shieldEmitter.startVelocity = {0.0f, 0.0f, 0.0f};
        shieldEmitter.startColor = {0.0f, 1.0f, 0.0f};
        shieldEmitter.endColor = {1.0f, 1.0f, 0.0f};
        shieldEmitter.colorFade = 1;
        shieldEmitter.alphaFade = 1;
        shieldEmitter.lifeTime = 2.5f;
        shieldParticles_->SetEmitterValue(shieldEmitter);
    }
    
    void Update() {
        fireParticles_->Update();
        smokeParticles_->Update();
        explosionParticles_->Update();
        portalParticles_->Update();
        shieldParticles_->Update();
    }
    
    void Draw(Camera& camera) {
        fireParticles_->Draw(camera);
        smokeParticles_->Draw(camera);
        explosionParticles_->Draw(camera);
        portalParticles_->Draw(camera);
        shieldParticles_->Draw(camera);
    }
    
    void DrawImGui() {
        fireParticles_->DrawImGui("Fire Particles");
        smokeParticles_->DrawImGui("Smoke Particles");
        explosionParticles_->DrawImGui("Explosion Particles");
        portalParticles_->DrawImGui("Portal Particles");
        shieldParticles_->DrawImGui("Shield Particles");
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

2. **Shape-specific parameters** that appear based on your selection:
   - **Point**: No additional parameters
   - **Line**: Line Start, Line Direction, Line Length
   - **Sphere**: Radius (with surface/volume indicator)
   - **Box/Box Surface**: Box Size (width, height, depth)
   - **Ring/Ring XY/Ring YZ**: Inner Radius, Outer Radius (with plane indicator)

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

## Advanced Usage Tips

### Combining Multiple Ring Orientations
```cpp
// Create a complex 3D magical effect using all three ring orientations
EmitterState ringXZ = {}; // Horizontal ring
ringXZ.shapeType = static_cast<uint32_t>(EmitterShapeType::RING);

EmitterState ringXY = {}; // Vertical ring facing forward
ringXY.shapeType = static_cast<uint32_t>(EmitterShapeType::RING_XY);

EmitterState ringYZ = {}; // Vertical ring facing right
ringYZ.shapeType = static_cast<uint32_t>(EmitterShapeType::RING_YZ);

// Configure all three with the same center but different colors
// This creates a complex 3D magical sphere effect
```

### Box vs Box Surface
```cpp
// Volume box for dense smoke clouds
EmitterState smokeVolume = {};
smokeVolume.shapeType = static_cast<uint32_t>(EmitterShapeType::BOX);

// Surface box for building facade effects
EmitterState buildingDamage = {};
buildingDamage.shapeType = static_cast<uint32_t>(EmitterShapeType::BOX_SURFACE);
```

## Performance Considerations

- **Point emitters** are the most performance-friendly
- **Box Surface and Ring emitters** require more calculations than their volume counterparts
- **Ring XY/YZ emitters** have similar performance to the standard Ring emitter
- Consider using fewer particles with larger textures for distant effects
- Use appropriate max particle counts based on the effect type

## Tips

1. **Combine multiple emitters** for complex effects (e.g., fire + smoke + sparks)
2. **Use different blend modes** for different visual effects
3. **Animate emitter properties** over time for dynamic effects
4. **Use velocity randomization** with shape emitters for more natural-looking effects
5. **Save and load configurations** using the JSON system for reusable effects
6. **Experiment with ring orientations** to create complex 3D effects
7. **Use box surface emitters** for architectural and structural effects