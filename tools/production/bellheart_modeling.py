"""Bellheart first art milestone. Run with Blender 4.2 --background --python.
All dimensions meters. Named meshes remain editable in per-asset .blend sources.
Reference authority: Docs/Art/Bellheart, inspected masters and measured layout.
"""
import bpy, math, random, json, sys
from pathlib import Path
from mathutils import Vector
R=Path('E:/Try'); rng=random.Random(4217)
OUT=R/'Content/Cloudwake/Art'; SRC=R/'Tools/Blender/Source'
manifest={'assets':[], 'instances':[], 'references':'Docs/Art/Bellheart/ASSET_REFERENCE_MAP.md'}
palette={'Wood':(.32,.16,.065),'DarkWood':(.095,.047,.022),'Stone':(.57,.51,.37),'Rock':(.29,.32,.29),'Teal':(.025,.19,.20),'TealLight':(.05,.28,.28),'Bronze':(.48,.28,.08),'Iron':(.11,.13,.13),'Rope':(.5,.36,.17),'Cream':(.83,.77,.57),'Grass':(.23,.31,.07),'Leaf':(.29,.38,.065),'LeafLight':(.43,.46,.10),'LeafDark':(.055,.15,.035),'Water':(.018,.43,.46),'Foam':(.72,.94,.89),'Glow':(1,.49,.08),'Dirt':(.42,.29,.13)}
def reset():
 bpy.ops.object.select_all(action='SELECT'); bpy.ops.object.delete(use_global=False)
 bpy.data.orphans_purge(do_recursive=True)
 for m in list(bpy.data.materials): bpy.data.materials.remove(m)
 for name,c in palette.items():
  m=bpy.data.materials.new(name); m.diffuse_color=(*c,1); m.use_nodes=True
  bs=m.node_tree.nodes.get('Principled BSDF'); bs.inputs['Base Color'].default_value=(*c,1); bs.inputs['Roughness'].default_value=.78
def mat(o,name): o.data.materials.append(bpy.data.materials[name]); return o
def mesh(name,v,f,m):
 data=bpy.data.meshes.new(name); data.from_pydata(v,[],f); data.update(); o=bpy.data.objects.new(name,data); bpy.context.collection.objects.link(o); mat(o,m); return o
def bevel(o,w=.05):
 mod=o.modifiers.new('Crafted edge highlights','BEVEL'); mod.width=w; mod.segments=2
 return o
def cube(name,p,size,m='Wood',b=.035):
 bpy.ops.mesh.primitive_cube_add(size=1,location=p); o=bpy.context.object; o.name=name; o.scale=size
 bpy.ops.object.transform_apply(location=False,rotation=False,scale=True); mat(o,m)
 if b: bevel(o,b)
 return o
def ico(name,p,size,m='Rock',sub=2):
 bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=sub,radius=1,location=p); o=bpy.context.object; o.name=name
 for v in o.data.vertices:v.co*=rng.uniform(.9,1.08)
 o.scale=size; mat(o,m)
 return o
def tube(name,points,radii,m='Wood',sides=12):
 v=[]; f=[]
 for i,p in enumerate(points):
  d=Vector(points[min(i+1,len(points)-1)])-Vector(points[max(0,i-1)])
  q=d.to_track_quat('Z','Y')
  for j in range(sides):
   a=j*2*math.pi/sides; v.append(tuple(Vector(p)+q@Vector((math.cos(a)*radii[i],math.sin(a)*radii[i],0))))
 for i in range(len(points)-1):
  for j in range(sides):a=i*sides+j; b=i*sides+(j+1)%sides; f.append((a,b,b+sides,a+sides))
 f.extend([tuple(range(sides-1,-1,-1)),tuple((len(points)-1)*sides+j for j in range(sides))])
 o=mesh(name,v,f,m)
 for face in o.data.polygons:face.use_smooth=True
 return o
def beam(name,a,b,width=.2,m='Wood'):
 o=cube(name,(Vector(a)+Vector(b))*.5,(width,width,(Vector(b)-Vector(a)).length),m,width*.12); o.rotation_euler=(Vector(b)-Vector(a)).to_track_quat('Z','Y').to_euler(); return o
def lathe(name,p,profile,m='Bronze',segments=32):
 v=[(p[0]+r*math.cos(j*2*math.pi/segments),p[1]+r*math.sin(j*2*math.pi/segments),p[2]+z) for r,z in profile for j in range(segments)]
 f=[]
 for i in range(len(profile)-1):
  for j in range(segments):a=i*segments+j;b=i*segments+(j+1)%segments;f.append((a,b,b+segments,a+segments))
 o=mesh(name,v,f,m)
 for face in o.data.polygons:face.use_smooth=True
 return o
def lantern(p,s=1):
 x,y,z=p
 lathe('Lantern bronze cage',(x,y,z),[(.18*s,0),(.22*s,.06*s),(.17*s,.13*s),(.17*s,.52*s),(.25*s,.55*s),(.08*s,.76*s)],'Bronze',12)
 cube('Lantern warm glass',(x,y,z+.31*s),(.26*s,.26*s,.35*s),'Glow',.015)
 for a in range(4):
  t=a*math.pi/2+.785; beam('Lantern stile',(x+.17*s*math.cos(t),y+.17*s*math.sin(t),z+.1*s),(x+.17*s*math.cos(t),y+.17*s*math.sin(t),z+.55*s),.035*s,'Iron')
def emblem(x,y,z,s=1):
 pts=[(-.4,-.3),(-.27,-.1),(-.23,.3),(-.12,.48),(0,.53),(.12,.48),(.23,.3),(.27,-.1),(.4,-.3),(-.4,-.3)]
 tube('Split wind bell outline',[(x+a*s,y,z+b*s) for a,b in pts],[.025*s]*len(pts),'Cream',6)
 beam('Bell clapper',(x,y,z-.05*s),(x,y,z-.43*s),.045*s,'Cream')
 for side in [-1,1]:tube('Wind stroke',[(x+side*.48*s,y,z-.25*s),(x+side*.62*s,y,z-.19*s),(x+side*.73*s,y,z-.24*s)],[.02*s]*3,'Cream',6)
def banner(p,s=1):
 x,y,z=p; v=[(x-.52*s,y,z+.8*s),(x+.52*s,y,z+.8*s),(x+.5*s,y+.05*s,z-.8*s),(x,y+.08*s,z-.62*s),(x-.5*s,y+.05*s,z-.8*s)]
 mesh('Teal notched banner',v,[(0,1,2,3,4)],'Teal'); emblem(x,y-.03,z,s*.8); beam('Banner rod',(x-.67*s,y,z+.86*s),(x+.67*s,y,z+.86*s),.07*s,'Bronze')
def text(name,p,size=.3):
 bpy.ops.object.text_add(location=p,rotation=(math.pi/2,0,0));o=bpy.context.object;o.name='Sign lettering';o.scale.x=-1;o.data.body=name;o.data.align_x='CENTER';o.data.size=size;o.data.extrude=.005;mat(o,'Cream')
def export(name,category,ref,collision=False):
 dest=OUT/category/'Source';dest.mkdir(parents=True,exist_ok=True);SRC.mkdir(parents=True,exist_ok=True)
 # Explicit planar UVs on mesh faces; procedural material family adds world-scale breakup.
 for o in bpy.context.scene.objects:
  if o.type=='MESH' and not o.data.uv_layers:
   uv=o.data.uv_layers.new(name='UVMap')
   for poly in o.data.polygons:
    n=poly.normal; axes=(0,1) if abs(n.z)>.5 else (0,2) if abs(n.y)>.5 else (1,2)
    for li in poly.loop_indices:
     co=o.data.vertices[o.data.loops[li].vertex_index].co;uv.data[li].uv=(co[axes[0]],co[axes[1]])
 bpy.ops.wm.save_as_mainfile(filepath=str(SRC/(name+'.blend')))
 bpy.ops.object.select_all(action='SELECT');bpy.context.view_layer.objects.active=list(bpy.context.scene.objects)[0];bpy.ops.object.convert(target='MESH')
 bpy.ops.export_scene.fbx(filepath=str(dest/(name+'.fbx')),use_selection=True,global_scale=100,apply_unit_scale=False,axis_forward='X',axis_up='Z',use_space_transform=False,bake_anim=False,add_leaf_bones=False,object_types={'MESH'},mesh_smooth_type='FACE')
 manifest['assets'].append({'name':name,'category':category,'reference':ref,'source':str(SRC/(name+'.blend')),'fbx':str(dest/(name+'.fbx')),'collision':collision})
 print('BH_ART_EXPORTED',name,flush=True)
def place(name,p=(0,0,0),scale=(1,1,1),yaw=0):manifest['instances'].append({'name':name,'position':p,'scale':scale,'yaw':yaw})
