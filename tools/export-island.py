"""Run in Blender's Scripting workspace after editing Cloudwake-Harbor.blend.
Exports only the island, keeping the game's controllable boat separate.
"""
import bpy, os
root=os.path.dirname(os.path.dirname(bpy.data.filepath))
destination=os.path.join(root,'public','models','harbor.glb')
island=bpy.data.objects.get('Wind_Bell_Harbor')
if island is None:
    raise RuntimeError('Keep the Wind_Bell_Harbor parent object in the scene.')
bpy.ops.object.select_all(action='DESELECT')
island.select_set(True)
for obj in island.children_recursive:
    obj.select_set(True)
os.makedirs(os.path.dirname(destination),exist_ok=True)
bpy.ops.export_scene.gltf(filepath=destination,export_format='GLB',use_selection=True,export_yup=True,export_extras=False)
print('Exported island to '+destination)
