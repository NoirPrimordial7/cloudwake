"""Export the saved editable source without changing its .blend file."""
import bpy, os
root=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
bpy.ops.wm.open_mainfile(filepath=os.path.join(root,'art','v2','Cloudwake-FirstPerson.blend'))
for col in list(bpy.data.collections):
    objects=[o for o in col.objects if o.type in {'MESH','FONT'}]
    if not objects: continue
    bpy.ops.object.select_all(action='DESELECT')
    for o in objects:o.select_set(True)
    bpy.context.view_layer.objects.active=objects[0]
    bpy.ops.object.convert(target='MESH');bpy.ops.object.join()
    bpy.context.object.name=col.name
bpy.ops.object.select_all(action='DESELECT')
for o in bpy.context.scene.objects:
    if o.type=='MESH':o.select_set(True)
bpy.ops.export_scene.gltf(filepath=os.path.join(root,'public','models','harbor-v2.glb'),export_format='GLB',use_selection=True,export_yup=True)
print('Exported harbor-v2.glb. Saved Blender source remains unchanged.')
