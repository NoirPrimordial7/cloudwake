"""Render actual editable Blender sources together for silhouette and scale review."""
import bpy, json, math
from pathlib import Path
from mathutils import Vector

ROOT = Path('E:/Try')
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete(use_global=False)
assets = json.loads((ROOT/'tools/production/architecture_manifest.json').read_text())['assets']
for i, asset in enumerate(assets):
    with bpy.data.libraries.load(asset['source'], link=False) as (src, dst):
        dst.objects = src.objects
    offset = Vector(((i % 4 - 1.5) * 8, (i // 4) * 9, 0))
    for obj in dst.objects:
        if obj is not None:
            bpy.context.collection.objects.link(obj)
            obj.location += offset
    bpy.ops.object.text_add(location=offset + Vector((-3.2, -3.7, .02)))
    label=bpy.context.object
    label.data.body=asset['name'].replace('SM_BH_', '')
    label.data.size=.36
    label.data.extrude=.001
    bpy.ops.mesh.primitive_cube_add(size=1, location=offset+Vector((3,-2, .9)))
    gauge=bpy.context.object; gauge.name='180cm scale marker'
    gauge.dimensions=(.3,.3,1.8)

bpy.ops.mesh.primitive_plane_add(size=100, location=(0,12,-.35))
ground=bpy.context.object
mat=bpy.data.materials.new('Review neutral warm grey'); mat.diffuse_color=(.27,.28,.28,1)
ground.data.materials.append(mat)
bpy.ops.object.light_add(type='AREA',location=(0,-8,30))
key=bpy.context.object; key.data.energy=18000; key.data.shape='DISK'; key.data.size=20
bpy.ops.object.light_add(type='SUN',location=(0,0,25))
sun=bpy.context.object; sun.rotation_euler=(.4,-.5,-.5); sun.data.energy=2; sun.data.angle=.15
bpy.ops.object.camera_add(location=(26,-40,48))
cam=bpy.context.object
cam.rotation_euler=(Vector((0,12,0))-cam.location).to_track_quat('-Z','Y').to_euler()
cam.data.type='ORTHO'; cam.data.ortho_scale=46
scene=bpy.context.scene; scene.camera=cam
scene.render.engine='BLENDER_EEVEE_NEXT'
scene.render.resolution_x=1800; scene.render.resolution_y=1600; scene.render.resolution_percentage=100
scene.world.color=(.3,.3,.3)
scene.view_settings.view_transform='AgX'
out=ROOT/'Docs/Art/Bellheart/Reviews/ArchitectureKit_ActualBlender_V001.png'
out.parent.mkdir(parents=True,exist_ok=True)
scene.render.filepath=str(out)
bpy.ops.wm.save_as_mainfile(filepath=str(ROOT/'tools/blender/Source/Bellheart_Architecture_Review.blend'))
bpy.ops.render.render(write_still=True)
print('BH_KIT_REVIEW_RENDER_COMPLETE',flush=True)
