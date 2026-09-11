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

# A: connected terrain split into editable 20 m patches, with the tested path heights retained.
outline=[(-18,-96),(15,-94),(34,-83),(40,-62),(54,-42),(65,-10),(63,25),(48,55),(28,82),(15,101),(-16,100),(-34,79),(-47,57),(-64,30),(-66,-7),(-59,-34),(-48,-51),(-22,-66)]
main=[(0,-86,0),(0,-65,3),(0,-54,7),(0,-49,7),(17,-35,7),(27,-21,7),(29,5,7),(16,18,14),(12,22,14),(0,30,14),(-10,44,20),(-14,53,20),(-14,63,20),(-3,65,26),(0,65,26),(0,76,26)]
side=[(-10,-43,7),(-29,-36,7),(-47,-24,6),(-53,-2,9),(-51,17,11),(-38,5,10),(-38,17,10),(-24,18,14),(-24,24,14),(0,24,14),(27,24,14),(35,24,14),(38,3,10),(46,3,10)]
segments=[(a,b,w) for seq,w in [(main,3),(side,1.8),([(-14,53,20),(-20,53,20)],3),([(0,76,26),(0,92,30)],3)] for a,b in zip(seq,seq[1:])]
regions=[(0,-46,25,7.5,7),(30,-18,7,25,7),(-32,-18,9,25,7),(0,31,33.5,11.5,14),(-34,5,11.5,12,10),(46,10,8.5,9.5,10),(-20,58,10,9,20),(0,76,11,10,26),(0,92,8,5,30),(26,-11,11,9,7),(-47,-24,5,6,6),(0,-86,4.5,7.5,0)]
def inside(x,y):
 result=False;j=len(outline)-1
 for i,(xi,yi) in enumerate(outline):
  xj,yj=outline[j]
  if (yi>y)!=(yj>y) and x<(xj-xi)*(y-yi)/(yj-yi)+xi:result=not result
  j=i
 return result
def nearpath(x,y):
 best=(1e9,0,0)
 for a,b,w in segments:
  dx=b[0]-a[0];dy=b[1]-a[1];t=max(0,min(1,((x-a[0])*dx+(y-a[1])*dy)/(dx*dx+dy*dy)))
  d=math.hypot(x-a[0]-t*dx,y-a[1]-t*dy)
  if d<best[0]:best=(d,a[2]+t*(b[2]-a[2]),w)
 return best
def height(x,y):
 values=[]
 for cx,cy,rx,ry,z in regions:
  d=math.hypot(max(0,abs(x-cx)-rx),max(0,abs(y-cy)-ry))
  if d==0:values=[(0,z)];break
  values.append((d,z))
 values.sort(); h=sum(z/(d+.2)**3 for d,z in values[:3])/sum(1/(d+.2)**3 for d,z in values[:3])
 pond=(x/23)**2+((y+20)/17)**2
 if pond<1:h=4.85+max(0,pond-.6)*1.5
 if -54<x<-20 and abs(y+24)<2.1:h=min(h,5.45)
 d,z,w=nearpath(x,y)
 edge=w*.5+.8
 if d<edge:h=z-.045
 elif d<edge+3:
  t=(d-edge)/3;t=t*t*(3-2*t);h=(z-.045)*(1-t)+h*t
 if y<-77:h=min(h,max(0,(y+86)*.18))
 return h-.025
for ix in range(-70,70,20):
 for iy in range(-100,100,20):
  reset();v=[];f=[];N=21
  for j in range(N):
   for i in range(N):x=ix+i;y=iy+j;v.append((x,y,height(x,y)))
  for j in range(N-1):
   for i in range(N-1):
    if inside(ix+i+.5,iy+j+.5):a=j*N+i;f.extend([(a,a+1,a+N+1),(a,a+N+1,a+N)])
  if not f:continue
  o=mesh('Meadow surface',v,f,'Grass')
  for poly in o.data.polygons:poly.use_smooth=True
  name=f'SM_BH_Terrain_{ix+70:03}_{iy+100:03}';export(name,'Terrain','BH-ISLAND-TOPDOWN; BH-ISLAND-ELEVATION',True);place(name)
# Cliff wedge sections with beveled layered fracture masses, not a flat disk.
for i,(a,b) in enumerate(zip(outline,outline[1:]+outline[:1])):
 reset();N=5; verts=[]
 for level,(factor,zoffset) in enumerate([(1,0),(1.02,-9),(.78,-26),(.36,-43)]):
  for j in range(N):
   t=j/(N-1);x=a[0]*(1-t)+b[0]*t;y=a[1]*(1-t)+b[1]*t
   verts.append((x*factor+rng.uniform(-.7,.7),y*factor+rng.uniform(-.7,.7),height(x,y)+zoffset if level==0 else zoffset+rng.uniform(-2,2)))
 faces=[]
 for k in range(3):
  for j in range(N-1):q=k*N+j;faces.append((q,q+1,q+N+1,q+N))
 faces.extend([(0,N,2*N,3*N),(N-1,2*N-1,3*N-1,4*N-1)])
 bevel(mesh('Layered limestone cliff',verts,faces,'Rock'),.3)
 length=math.dist(a,b)
 for j in range(max(2,int(length/4))):
  t=(j+.5)/max(2,int(length/4));x=a[0]*(1-t)+b[0]*t;y=a[1]*(1-t)+b[1]*t;z=height(x,y)
  ico('Broken upper ledge',(x,y,z-1.5),(rng.uniform(2.3,3.5),rng.uniform(2,3),rng.uniform(1.5,3.2)),'Stone',2)
 name=f'SM_BH_Cliff_{i:02}';export(name,'Terrain','BH-ISLAND-MASTER-AERIAL');place(name)

# B: three hand-directed primary boughs, root buttresses and layered leaf sprays.
reset()
tube('Twisted trunk',[(0,0,0),(.25,.1,2),(-.4,.1,4),(.1,.2,7),(.3,.1,10)],[1.55,1.35,1.2,.95,.55],'DarkWood',16)
for k,(tx,ty,tz) in enumerate([(-6,-1,10),(5,-3,11),(1,5,13)]):
 tube('Primary bough '+str(k),[(0,0,3),(.3*tx,.3*ty,6),(.7*tx,.7*ty,tz-1),(tx,ty,tz)],[1.05,.8,.5,.18],'Wood',14)
 for j in range(4):
  ang=j*1.6+k;end=(tx+math.cos(ang)*3,ty+math.sin(ang)*3,tz+rng.uniform(.5,2))
  tube('Fine spreading branch',[(tx*.65,ty*.65,tz-2),(tx,ty,tz),end],[.4,.2,.04],'DarkWood')
for j in range(10):
 a=j*2*math.pi/10;tube('Root buttress',[(math.cos(a)*.5,math.sin(a)*.5,2.2),(math.cos(a)*2,math.sin(a)*2,.65),(math.cos(a)*4,math.sin(a)*4,.04)],[.55,.42,.07],'Wood')
leafv=[];leaff=[]
for cx,cy,cz,s in [(-5,-1,11,3.8),(5,-2,12,4.2),(0,4,13.5,4),(-1,-4,13,3.3),(0,0,15,3),(6,3,13,3),(-6,3,12,3.2)]:
 ico('Canopy shaded mass',(cx,cy,cz),(s,s*.8,s*.52),'LeafDark',3)
 for j in range(550):
  u=rng.uniform(0,2*math.pi);v=rng.uniform(-1,1);r=math.sqrt(1-v*v);p=Vector((cx+math.cos(u)*r*s,cy+math.sin(u)*r*s*.85,cz+v*s*.55));l=rng.uniform(.25,.55);q=Vector((math.cos(u),math.sin(u),rng.uniform(-.2,.5))).to_track_quat('X','Z')
  n=len(leafv)
  for pt in [(-l,0,0),(-l*.3,l*.5,0),(l*.4,l*.4,0),(l,0,.08),(l*.3,-l*.4,0),(-l*.3,-l*.4,0),(0,0,l*.2)]:leafv.append(tuple(p+q@Vector(pt)))
  for k in range(6):leaff.append((n+k,n+(k+1)%6,n+6))
o=mesh('Individually shaped canopy leaves',leafv,leaff,'Leaf');o.data.materials.append(bpy.data.materials['LeafLight'])
for face in o.data.polygons:face.material_index=1 if rng.random()<.3 else 0
for x,y,z in [(-5,-2,8),(4,-4,8.7),(-2,4,9),(6,1,9),(-6,2,7.5)]:
 beam('Lantern chain',(x,y,z),(x,y,z-1.8),.035,'Iron');lantern((x,y,z-2.6),1.3)
cube('Root resting bench',(0,-2.3,.65),(2.6,.8,.18));beam('Bench leg',(-1,-2.3,0),(-1,-2.3,.6),.18);beam('Bench leg',(1,-2.3,0),(1,-2.3,.6),.18)
export('SM_BH_GiantTree','GiantTree','BH-GIANTTREE-MASTER');place('SM_BH_GiantTree',(-34,5,10))

# Reusable construction modules and distinct assembled building sources.
def roof(w,d,h,eave,offset=0):
 for sign in [-1,1]:
  rows=7;cols=max(8,int(d/.5))
  for i in range(rows):
   t=(i+.5)/rows;x=sign*(w*.5+.55)*t+offset;z=h-(h-eave)*t+.22*t*t
   for j in range(cols):
    o=cube('Overlapping teal shingle',(x,-d*.5-.45+(j+.5)*(d+.9)/cols,z+rng.uniform(-.025,.025)),((w*.5+.55)/rows+.12,(d+.9)/cols-.025,.11),'TealLight' if rng.random()<.22 else 'Teal',.045)
    o.rotation_euler.y=sign*math.atan2(h-eave,w*.5+.55)
  for y in [-d*.5-.58,d*.5+.58]:
   tube('Swept gable trim',[(offset,y,h+.16),(offset+sign*w*.25,y,(h+eave)*.5),(offset+sign*(w*.5+.65),y,eave+.27)],[.16,.15,.2],'Wood',8)
 beam('Roof ridge',(offset,-d*.5-.7,h+.18),(offset,d*.5+.7,h+.18),.32)
def chimney(x,y,h):
 for z in range(int(h/.5)):
  for dx in [-.25,.25]:
   for dy in [-.25,.25]:cube('Chimney limestone',(x+dx,y+dy,z*.5+.25),(.48,.48,.47),'Stone',.055)
 cube('Chimney cap',(x,y,h),(1.2,1.2,.25),'Stone',.08)
def building(name,w,d,h,kind):
 reset();eave=h*.53
 # Plinth and back wall are individual reusable coursed blocks and planks.
 for x in [-w*.5,w*.5]:
  for j in range(int(d/.6)):
   for k in range(2):cube('Limestone footing',(x,-d*.5+(j+.5)*d/int(d/.6),.25+k*.48),(.5,d/int(d/.6)-.025,.45),'Stone',.045)
  for z in [1.2,eave]:beam('Wall ledger',(x,-d*.5,z),(x,d*.5,z),.22)
  for y in [-d*.5,0,d*.5]:beam('Pegged frame post',(x,y,.5),(x+rng.uniform(-.035,.035),y,eave+.15),.3)
  for j in range(int(d/.35)):cube('Side oak board',(x,-d*.5+(j+.5)*.35,(eave+1)*.5),(.16,.33,eave-1),'Wood',.02)
 for j in range(int(w/.35)):cube('Back board',(-w*.5+(j+.5)*.35,d*.5,eave*.5),(.33,.17,eave),'Wood',.02)
 # Open south-facing frontage keeps NPC/counter sight lines clear.
 for x in [-w*.5,w*.5]:beam('Front timber jamb',(x,-d*.5,.1),(x,-d*.5,eave),.36)
 beam('Front lintel',(-w*.5,-d*.5,eave),(w*.5,-d*.5,eave),.3)
 for j in range(int(w/.35)):
  x=-w*.5+(j+.5)*.35;top=h-(h-eave)*abs(x)/(w*.5)
  if top>eave:cube('Gable board',(x,-d*.5,(top+eave)*.5),(.32,.17,top-eave),'Wood',.015)
 roof(w,d,h,eave, -.4 if kind=='forge' else 0)
 chimney((w*.5-.4)*(1 if kind=='elder' else -1),d*.22,h+.4)
 banner((0,-d*.5-.23,h*.68),.7)
 for x in [-w*.5+.4,w*.5-.4]:lantern((x,-d*.5-.35,2),.75)
 if kind in ['fish','forge']:
  for x in [-w*.36,w*.36]:beam('Awning post',(x,-d*.5-1.0,0),(x,-d*.5-1.0,2.7),.19)
  for j in range(16):
   o=cube('Awning shingle',(-w*.45+(j+.5)*w*.9/16,-d*.5-.4,2.95),(w*.9/16-.02,1.6,.09),'Teal',.03);o.rotation_euler.x=.16
  cube('Open sales counter',(0,-d*.5+.1,.9),(w*.65,.75,.16),'Wood',.05)
  for x in [-w*.3,w*.3]:cube('Counter trestle',(x,-d*.5+.1,.4),(.18,.6,.8),'DarkWood',.025)
  cube('Shop name board',(0,-d*.5-.5,2.72),(w*.62,.08,.4),'DarkWood',.07);text('THE CLOUD CATCH' if kind=='fish' else "BRAM'S FORGE",(0,-d*.5-.56,2.61),.25)
 if kind=='fish':
  for j in range(6):beam('Display fishing rod',(-w*.35+j*.3,d*.5-.35,.5),(-w*.35+j*.3+.2,d*.5-.35,2.8),.035)
  for j in range(5):lathe('Bait jar',(1+j*.3,-d*.5+.1,1),[(.1,0),(.12,.05),(.12,.22),(.075,.25)],'Teal',12)
 if kind=='forge':
  cube('Forge hearth',(-w*.3,d*.35,.7),(1.6,1.1,1.4),'Stone',.1);cube('Forge coals',(-w*.3,d*.35-.58,.95),(1,.06,.3),'Glow',.08)
  cube('Anvil pedestal',(1,1,.4),(.65,.6,.8),'DarkWood',.05);cube('Anvil',(1,1,.95),(1,.4,.3),'Iron',.1)
  o=lathe('Grinding wheel',(0,0,0),[(.0,0),(.6,0),(.6,.22),(.0,.22)],'Stone');o.rotation_euler.x=math.pi/2;o.location=(w*.45,-d*.5,1)
  for j in range(7):beam('Tool haft',(-1+j*.35,d*.5-.2,1.2),(-1+j*.35,d*.5-.2,2.3),.055);cube('Hammer head',(-1+j*.35,d*.5-.2,2.3),(.22,.15,.12),'Iron',.02)
 if kind=='workshop':
  beam('Crane mast',(w*.5+1,0,0),(w*.5+1,0,6),.35);beam('Crane boom',(w*.5+1,0,5.7),(w*.5+3,-1,5.7),.3);beam('Crane brace',(w*.5+1,0,3.4),(w*.5+2.5,-.8,5.7),.23);beam('Hoist rope',(w*.5+3,-1,5.7),(w*.5+3,-1,2),.035,'Rope')
 export(name,{'fish':'FishingShop','forge':'Blacksmith','elder':'ElderHouse','workshop':'Workshop'}[kind],{'fish':'BH-FISHSHOP-MASTER','forge':'BH-BLACKSMITH-MASTER','elder':'BH-ELDERHOUSE-MASTER','workshop':'BH-WORKSHOP-MASTER'}[kind])
building('SM_BH_FishingShop',9,7,6.2,'fish');place('SM_BH_FishingShop',(-24,30,14))
building('SM_BH_Blacksmith',11,8,6.8,'forge');place('SM_BH_Blacksmith',(27,30,14))
building('SM_BH_ELDERHOUSE',8,7,6,'elder');place('SM_BH_ELDERHOUSE',(-20,58,20))
building('SM_BH_WORKSHOP',12,9,7,'workshop');place('SM_BH_WORKSHOP',(46,10,10))

# Tower: four piers, voussoir arch, bronze bell and east maintenance stair.
reset()
# Bellkeeper chamber and solid lower silhouette; a clear south doorway preserves quest access.
for k in range(18):
 for x in [-3.3,3.3]:
  for j in range(5):cube('Side chamber masonry',(x,-2.4+j*1.2,k+.5),(1.15,1.16,.96),'Stone',.09)
 for y in [-2.8,2.8]:
  for j in range(5):
   x=-2.4+j*1.2
   if y<0 and k<4 and abs(x)<1.3:continue
   cube('Chamber ashlar',(x,y,k+.5),(1.16,1.15,.96),'Stone',.085)
for z in [4.1,12,18]:cube('Tower cornice',(0,0,z),(8.2,7.2,.35),'Stone',.1)
for x in [-3.3,3.3]:
 for y in [-2.8,2.8]:
  for k in range(29):cube('Limestone pier course',(x+rng.uniform(-.03,.03),y,k+.5),(1.25,1.25,.96),'Stone',.1)
  for z in [1,10,21,28.5]:cube('Pier belt',(x,y,z),(1.65,1.65,.4),'Stone',.08)
for y in [-2.8,2.8]:
 for j in range(13):
  a=(j+.5)*math.pi/13;x=3.3*math.cos(a);z=28.2+3.3*math.sin(a);o=cube('Arch voussoir',(x,y,z),(.88,1.3,1.0),'Stone',.055);o.rotation_euler.y=a-math.pi/2
beam('Bell hanging beam',(-3.5,0,28),(3.5,0,28),.55)
lathe('Bronze wind bell',(0,0,24),[(1.55,0),(1.6,.15),(1.3,.3),(1,1.1),(.7,2.5),(.62,3),(.2,3.15)],'Bronze',48)
beam('Bell hanger',(0,0,27),(0,0,28),.15,'Iron');lathe('Bell clapper',(0,0,23.6),[(.2,0),(.28,.3),(.1,.6),(.1,2)],'Iron',16)
banner((0,-2.9,17),3.2)
for k in range(38):cube('East service stair',(4.1,-3+k*.15,k*.22),(.95,.3,.17),'Wood',.025)
for k in range(9):beam('Stair rail post',(4.65,-3+k*.7,k*.99),(4.65,-3+k*.7,k*.99+1),.1)
for j in range(9):cube('Broken west ruin',(-5,-2+j*.55,rng.uniform(.8,2)),(.85,.5,rng.uniform(1.2,3)),'Stone',.1)
export('SM_BH_BellTower','BellTower','BH-BELLTOWER-MASTER');place('SM_BH_BellTower',(0,76,26))

# Reusable small modules and instances. No thousands of independent Unreal actors.
for name,kind in [('SM_BH_WoodBeam','beam'),('SM_BH_StoneBlock','stone'),('SM_BH_RoofShingle','roof'),('SM_BH_Fence','fence'),('SM_BH_Lantern','lamp'),('SM_BH_GrassTuft','grass'),('SM_BH_Flowers','flower'),('SM_BH_ShoreRock','rock')]:
 reset()
 if kind=='beam':cube('Timber module',(0,0,1.5),(.25,.25,3),'Wood',.045)
 if kind=='stone':cube('Stone module',(0,0,.3),(1,.5,.6),'Stone',.07)
 if kind=='roof':cube('Shingle module',(0,0,0),(.6,.45,.09),'Teal',.04)
 if kind=='fence':
  for x in [-1,1]:beam('Oak post',(x,0,0),(x+.03,0,1.12),.16)
  for z in [.45,.92]:beam('Fence rail',(-1.1,0,z),(1.1,.03,z+.05),.1)
 if kind=='lamp':lantern((0,0,0))
 if kind=='rock':ico('Waterworn limestone',(0,0,.2),(1,.75,.65),'Stone',2)
 if kind in ['grass','flower']:
  v=[];f=[]
  for j in range(18):
   a=rng.uniform(0,6.28);x=rng.uniform(-.3,.3);y=rng.uniform(-.3,.3);h=rng.uniform(.18,.55);w=.045;n=len(v)
   v.extend([(x-w*math.cos(a),y-w*math.sin(a),0),(x+w*math.cos(a),y+w*math.sin(a),0),(x+.1*math.sin(a),y+.1*math.cos(a),h)]);f.append((n,n+1,n+2))
   if kind=='flower' and j<5:
    for k in range(6):
     ang=k*math.pi/3;ico('Cream petal',(x+math.cos(ang)*.045,y+math.sin(ang)*.045,h),(.05,.035,.015),'Cream',1)
  mesh('Meadow blades',v,f,'Grass')
 export(name,'Kit','BH-ISLAND-MASTER-AERIAL; BH-MATERIAL-BIBLE')
for j in range(16000):
 x=rng.uniform(-58,57);y=rng.uniform(-57,91)
 if not inside(x,y) or nearpath(x,y)[0]<3 or (x/25)**2+((y+20)/19)**2<1 or (abs(x-26)<13 and abs(y+11)<11):continue
 if any(abs(x-a)<w/2+1 and abs(y-b)<d/2+1 for a,b,w,d in [(-24,30,9,7),(27,30,11,8),(-20,58,8,7),(46,10,12,9),(0,76,8,7)]):continue
 z=height(x,y);place('SM_BH_GrassTuft',(x,y,z),(rng.uniform(1,2),)*3,rng.uniform(0,360))
 if rng.random()<.14:place('SM_BH_Flowers',(x+.4,y,z),(1.5,)*3,rng.uniform(0,360))
for j in range(62):
 a=j*2*math.pi/62;x=24*math.cos(a);y=-20+18*math.sin(a)
 if nearpath(x,y)[0]<2.6 or (x>15 and y>-23):continue
 place('SM_BH_ShoreRock',(x,y,6.5),(rng.uniform(.85,1.8),rng.uniform(.8,1.4),rng.uniform(.7,1.3)),rng.uniform(0,360))
for seq in [main,side]:
 for a,b in zip(seq,seq[1:]):
  dist=math.dist(a,b)
  if dist<5:continue
  dx=b[0]-a[0];dy=b[1]-a[1];L=math.hypot(dx,dy)
  for t in [.2,.55,.85]:
   if rng.random()<.5:continue
   x=a[0]+t*dx+dy/L*2.1;y=a[1]+t*dy-dx/L*2.1;z=a[2]+t*(b[2]-a[2]);place('SM_BH_Fence',(x,y,z),yaw=math.degrees(math.atan2(dy,dx)))
# Pond and waterfall are independently editable geometry; shader animates the surface.
reset();v=[(0,-20,6.02)]+[(23*math.cos(i*2*math.pi/96),-20+17*math.sin(i*2*math.pi/96),6.02) for i in range(96)];f=[(0,i+1,(i+1)%96+1) for i in range(96)];mesh('Turquoise pond surface',v,f,'Water');mesh('West stream',[(-22,-22,6.02),(-54,-22,6.02),(-54,-26,6.02),(-22,-26,6.02)],[(0,1,2,3)],'Water');export('SM_BH_PondWater','Terrain','BH-POND-MASTER');place('SM_BH_PondWater')
reset();v=[];f=[]
for j in range(25):
 for i in range(9):v.append((-54-.2*math.sin(j*.2),-26+i*.5,6-j))
for j in range(24):
 for i in range(8):a=j*9+i;f.append((a,a+1,a+10,a+9))
mesh('Falling water ribbon',v,f,'Water')
for j in range(13):tube('White foam streak',[(-54.08,-26+j*.33,6),(-54.15,-26+j*.33,2),(-54.22,-26+j*.33,-8),(-54.1,-26+j*.33,-18)],[.035,.045,.065,.1],'Foam',5)
export('SM_BH_Waterfall','Terrain','BH-POND-MASTER');place('SM_BH_Waterfall')
# Dock: plank deck, deep joists and rope moorings, preserving the original platform.
reset()
for j in range(30):cube('Dock plank',(0,-7.5+(j+.5)*.5,-.1),(9,.47,.2),'Wood',.035)
for x in [-4,4]:
 beam('Dock underbeam',(x,-7.5,-.5),(x,7.5,-.5),.5)
 for y in [-7,0,7]:
  tube('Mooring post',[(x,y,-3),(x,y,1.2)],[.28,.22],'DarkWood',12)
  for z in [.4,.5,.6]:lathe('Rope binding',(x,y,z),[(.245,0),(.245,.04)],'Rope',16)
export('SM_BH_Dock','Dock','BH-ISLAND-MASTER-AERIAL (dock views absent)');place('SM_BH_Dock',(0,-86,0));place('SM_BH_Dock',(22,-83,0),(.95,.8,1))
# Irregular path shoulders over the preserved, invisible walking ramps.
reset(); v=[];f=[]
for a,b,w in segments:
 L=math.dist(a,b);dx=b[0]-a[0];dy=b[1]-a[1];horizontal=math.hypot(dx,dy);n=max(2,int(L*2));start=len(v)
 for j in range(n+1):
  t=j/n;width=w*.5+rng.uniform(.04,.22)
  for sign in [-1,1]:v.append((a[0]+t*dx-sign*dy/horizontal*width,a[1]+t*dy+sign*dx/horizontal*width,a[2]+t*(b[2]-a[2])+.012))
 for j in range(n):k=start+j*2;f.extend([(k,k+1,k+3),(k,k+3,k+2)])
mesh('Packed-earth path network',v,f,'Dirt');export('SM_BH_PathNetwork','Terrain','BH-ISLAND-GAMEPLAY-FLOW');place('SM_BH_PathNetwork')
OUT.mkdir(parents=True,exist_ok=True);(R/'tools/production/art_manifest.json').write_text(json.dumps(manifest,indent=2))
print('BH_ART_BUILD_COMPLETE',len(manifest['assets']),len(manifest['instances']),flush=True)
