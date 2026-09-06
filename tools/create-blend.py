"""Convert the game-authored GLB into a named, editable Blender project."""
import bpy, os, sys, math
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.gltf(filepath=os.path.join(ROOT,'art','cloudwake-harbor.glb'))
world=bpy.data.worlds.new('Cloudwake Sky');bpy.context.scene.world=world;world.use_nodes=True
world.node_tree.nodes['Background'].inputs[0].default_value=(0.55,0.76,0.8,1)
world.node_tree.nodes['Background'].inputs[1].default_value=.5
bpy.ops.object.light_add(type='SUN',location=(-30,-20,50));sun=bpy.context.object;sun.name='Warm afternoon sun';sun.rotation_euler=(math.radians(25),math.radians(-20),math.radians(-35));sun.data.energy=3
bpy.ops.object.camera_add(location=(55,-75,57));camera=bpy.context.object;camera.name='Harbor Overview'
from mathutils import Vector
camera.rotation_euler=(Vector((0,0,1))-camera.location).to_track_quat('-Z','Y').to_euler();camera.data.type='ORTHO';camera.data.ortho_scale=90;bpy.context.scene.camera=camera
scene=bpy.context.scene;scene.render.engine='BLENDER_EEVEE_NEXT';scene.render.resolution_x=1440;scene.render.resolution_y=1080;scene.render.resolution_percentage=100
scene['README']='Cloudwake editable first island. Named parent objects group buildings, props, NPCs and boat. Browser game source: src/world.js. Export island visual changes with tools/export-island.py.'
for area in bpy.context.screen.areas:
    if area.type=='VIEW_3D':
        area.spaces.active.region_3d.view_distance=85
bpy.ops.file.pack_all()
bpy.ops.wm.save_as_mainfile(filepath=os.path.join(ROOT,'art','Cloudwake-Harbor.blend'))
print('Saved editable Cloudwake-Harbor.blend')
