"""Author the first-person harbor as an editable Blender scene and optimized glTF.
Run: blender --background --python tools/build-harbor-v2.py
Game coordinates are x, height, z. Helpers convert to Blender Z-up.
"""
import bpy, math, random, os
from mathutils import Vector
random.seed(120)
ROOT=os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
bpy.ops.wm.read_factory_settings(use_empty=True)
def coord(x,y,z): return (x,-z,y)
def ground(x,z): return .8+3/(1+math.exp((z+15)/3.5))+math.sin(x*.15)*.13
def pool(x,z,pad=0): return ((x+12)/(9+pad))**2+((z-5)/(10+pad))**2<1
def material(name,color,texture=None,metal=0):
    m=bpy.data.materials.new(name);m.diffuse_color=(*color,1);m.use_nodes=True
    shader=m.node_tree.nodes.get('Principled BSDF');shader.inputs['Base Color'].default_value=(*color,1);shader.inputs['Roughness'].default_value=.82;shader.inputs['Metallic'].default_value=metal
    if texture:
        t=m.node_tree.nodes.new('ShaderNodeTexImage');t.image=bpy.data.images.load(os.path.join(ROOT,'public','textures',texture));m.node_tree.links.new(t.outputs['Color'],shader.inputs['Base Color'])
    return m
wood=material('Oak • painted grain',(.37,.22,.10),'oak.png');dark=material('Deep walnut',(.19,.12,.065));stone=material('Warm limestone',(.68,.64,.49),'limestone.png');stone2=material('Stone shaded edges',(.42,.45,.32));plaster=material('Warm ivory plaster',(.79,.73,.55));teal=material('Weathered teal canvas',(.065,.27,.27));teal2=material('Teal roof highlights',(.10,.36,.34));brass=material('Aged brass',(.65,.42,.13),metal=.5);iron=material('Tempered iron',(.31,.36,.36),metal=.65);cream=material('Linen',(.88,.79,.56));amber=material('Lantern glass',(.98,.54,.11));shader=amber.node_tree.nodes.get('Principled BSDF');shader.inputs['Emission Color'].default_value=(1,.42,.08,1);shader.inputs['Emission Strength'].default_value=.6
grasses=[material('Meadow '+str(i),c) for i,c in enumerate([(.28,.38,.12),(.37,.46,.16),(.45,.51,.2),(.30,.42,.18),(.42,.48,.18)])]
leaves=[material('Leaf '+str(i),c) for i,c in enumerate([(.19,.29,.08),(.31,.39,.09),(.43,.47,.12),(.25,.37,.1),(.51,.5,.18)])]
paths=[material('Path '+str(i),c) for i,c in enumerate([(.66,.56,.35),(.7,.62,.42),(.74,.67,.46)])]
petal=material('Buttercream petals',(.98,.87,.56));pink=material('Wildflower blush',(.71,.38,.26));bark=material('Tree bark',(.31,.21,.095),'oak.png')
def collection(name):
    c=bpy.data.collections.new(name);bpy.context.scene.collection.children.link(c);return c
current=collection('Landscape')
def setup(o,name,mat):
    o.name=name
    for c in list(o.users_collection):c.objects.unlink(o)
    current.objects.link(o)
    if mat:o.data.materials.append(mat)
    return o
def box(name,x,y,z,w,h,d,mat,bevel=.04,rot=0):
    bpy.ops.mesh.primitive_cube_add(size=1,location=coord(x,y,z));o=bpy.context.object;o.dimensions=(w,d,h);bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);o.rotation_euler.z=-rot
    setup(o,name,mat)
    if bevel:
        mod=o.modifiers.new('Soft crafted edges','BEVEL');mod.width=bevel;mod.segments=2
        o.modifiers.new('Weighted corner normals','WEIGHTED_NORMAL')
    return o
def sphere(name,x,y,z,sx,sy,sz,mat,sub=1):
    bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=sub,radius=1,location=coord(x,y,z));o=bpy.context.object;o.scale=(sx,sz,sy);setup(o,name,mat)
    for p in o.data.polygons:p.use_smooth=True
    return o
def cylinder(name,x,y,z,r,depth,mat,vertices=12):
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices,radius=r,depth=depth,location=coord(x,y,z));o=bpy.context.object;setup(o,name,mat);m=o.modifiers.new('Edge bevel','BEVEL');m.width=.035;m.segments=2;return o
def beam(name,a,b,r,mat):
    av=Vector(coord(*a));bv=Vector(coord(*b));bpy.ops.mesh.primitive_cone_add(vertices=9,radius1=r*1.07,radius2=r*.85,depth=(bv-av).length,location=(av+bv)/2);o=bpy.context.object;o.rotation_euler=(bv-av).to_track_quat('Z','Y').to_euler();return setup(o,name,mat)
def mesh(name,verts,faces,mat):
    data=bpy.data.meshes.new(name);data.from_pydata([coord(*v) for v in verts],[],faces);data.update();o=bpy.data.objects.new(name,data);current.objects.link(o)
    if mat:data.materials.append(mat)
    return o
def lantern(x,y,z):
    cylinder('Lantern glass',x,y,z,.16,.48,amber,6)
    for yy in [-.27,.27]:cylinder('Lantern metal cap',x,y+yy,z,.22,.07,dark,6)
    for a in range(4):beam('Lantern strut',(x+math.cos(a*math.pi/2)*.17,y-.24,z+math.sin(a*math.pi/2)*.17),(x+math.cos(a*math.pi/2)*.17,y+.24,z+math.sin(a*math.pi/2)*.17),.02,brass)
def text(label,x,y,z,size=.25,mat=cream):
    bpy.ops.object.text_add(location=coord(x,y,z),rotation=(math.pi/2,0,0));o=bpy.context.object;setup(o,'Sign • '+label,mat);o.data.body=label;o.data.align_x='CENTER';o.data.size=size;o.data.extrude=.003
    return o
def sign(label,x,y,z,w=3):
    box('Carved sign',x,y,z,w,.57,.11,dark,.055);text(label,x,y-.09,z+.07,.21)

# Terrain is a broad rolling surface, with an actual pond opening.
verts=[];faces=[];N=52;step=1.4
for j in range(N+1):
    z=-35+j*step
    for i in range(N+1):
        x=-36+i*step;verts.append((x,ground(x,z),z))
for j in range(N):
    for i in range(N):
        idx=j*(N+1)+i;x=-36+(i+.5)*step;z=-35+(j+.5)*step
        if (x/37)**2+((z+2)/33)**2>1 or pool(x,z,-.25):continue
        faces.extend([(idx,idx+N+1,idx+1),(idx+1,idx+N+1,idx+N+2)])
o=mesh('Rolling island meadow',verts,faces,None)
for m in grasses+paths:o.data.materials.append(m)
for face in o.data.polygons:
    center=face.center;x=center.x;z=-center.y
    ispath=abs(x-1.5)<2.3 or (abs(z+2)<2 and -5<x<17) or (abs(z-15.8)<1.4 and -12<x<3)
    face.material_index=6 if ispath else 1
    face.use_smooth=True
# Layered cliff rings rather than giant free-standing rock primitives.
for band in range(4):
    top=-band*2.4+.2;bottom=top-2.8;outer=1-band*.12;inner=outer-.12;v=[];f=[];segments=65
    for k in range(segments):
        a=k/segments*math.tau;noise=1+.025*math.sin(k*2.6);v.extend([(math.cos(a)*36*outer*noise,top,math.sin(a)*32*outer*noise-2),(math.cos(a)*36*inner*noise,bottom,math.sin(a)*32*inner*noise-2)])
    for k in range(segments):f.append((k*2,k*2+1,((k+1)%segments)*2+1,((k+1)%segments)*2))
    mesh('Cliff stratum '+str(band),v,f,stone if band%2==0 else stone2)
# Pond bed and individual shoreline blocks.
for k in range(64):
    a=k/64*math.tau;x=-12+math.cos(a)*9.1;z=5+math.sin(a)*10.1
    o=box('Pond bank stone',x,ground(x,z)-.14,z,.7+random.random()*.6,.45,.55+random.random()*.35,stone,.15,rot=a);o.rotation_euler.y=random.uniform(-.08,.08)
for k in range(45):
    a=random.random()*math.tau;r=random.random()*.9;x=-12+math.cos(a)*9*r;z=5+math.sin(a)*10*r;sphere('Submerged pebble',x,.12,z,.2+random.random()*.3,.13,.2+random.random()*.2,stone2)
# Stone path accents stay low and passable.
for z in range(-18,27,2):
    for xx in [-.7,.9,2.5]:
        x=xx+random.uniform(-.2,.2);box('Worn path flagstone',x,ground(x,z)+.035,z,1.1,.075,.8,stone,.1,random.uniform(-.15,.15))
def shop(name,x,z,width,roofmat):
    global current
    current=collection(name);y=ground(x,z)
    box('Foundation',x,y+.15,z,width+.5,.3,6.5,stone,.12)
    box('Rear plaster wall',x,y+2,z-2.7,width,4,.3,plaster,.07)
    for side in [-1,1]:
        box('Side plaster wall',x+side*(width/2-.12),y+1.8,z,.24,3.6,5.8,plaster,.06)
        for zz in [-2.8,2.7,5.1]:box('Oak upright',x+side*(width/2+.03),y+2, z+zz,.24,4.1,.24,wood,.055)
    for yy in [.5,3.6]:box('Cross beam',x,y+yy,z-2.45,width,.22,.22,wood)
    for xx in [-width/2,0,width/2]:box('Back timber',x+xx,y+2,z-2.43,.22,4,.2,wood)
    # Shingled gable, with individually softened and staggered tiles.
    for side in [-1,1]:
        for row in range(4):
            for col in range(9):
                xx=x+side*(.5+row*(width/8));zz=z-3+col*.78
                yy=y+5.8-(.5+row*(width/8))*math.tan(.52)
                tile=box('Teal shingle',xx,yy,zz,width/7+.35,.15,.86,roofmat if (row+col)%3 else teal2,.055)
                tile.rotation_euler.y=side*.52
    box('Ridge cap',x,y+6,z,.26,.2,7.5,wood)
    for zz in [-3.5,3.4]:
        beam('Gable rake',(x-width/2-.4,y+3.9,z+zz),(x,y+6,z+zz),.13,wood)
        beam('Gable rake',(x+width/2+.4,y+3.9,z+zz),(x,y+6,z+zz),.13,wood)
    # Curved canvas canopy, striped surfaces supported by posts.
    for i in range(8):
        xa=x-width/2+i*width/8;xb=xa+width/8;v=[]
        for j in range(6):
            zz=z+2+j*.63;yy=y+3.6-j*.09-math.sin(j/5*math.pi)*.22;v.extend([(xa,yy,zz),(xb,yy,zz)])
        mesh('Striped canvas',v,[(j*2,j*2+1,j*2+3,j*2+2) for j in range(5)],teal if i%2==0 else cream)
        box('Scalloped canvas edge',xa+width/16,y+3.1,z+5.2,width/8,.3,.07,teal if i%2==0 else cream,.07)
    box('Counter top',x,y+1.18,z+4.6,width-.6,.19,1.35,wood,.08)
    for xx in [-width/2+.8,width/2-.8]:box('Counter leg',x+xx,y+.57,z+4.6,.2,1.2,.8,dark)
    for i in range(int(width*2)):
        box('Counter face plank',x-width/2+.35+i*.46,y+.56,z+5.1,.43,1,.09,wood,.025)
    sign(name.upper(),x,y+4,z+3,width-.7)
    for side in [-1,1]:lantern(x+side*(width/2-.3),y+2.5,z+5.2)
    # Useful interior depth: shelving, jars, bundles, wall-mounted tools.
    for yy in [1.1,2.1,3.1]:
        box('Back shelf',x,y+yy,z-2.1,width-.6,.1,.65,wood)
        for i in range(7):
            xx=x-width*.37+i*width*.12
            cylinder('Glazed supply jar',xx,y+yy+.24,z-2.0,.12,.38,teal2 if i%2 else stone2,10)
    return y
y=shop('Bram’s Forge',13,-1.5,8,teal)
current=collection('Forge goods and wheel')
# Physical knife, wheel and prices align to the server interaction coordinates.
box('Knife display cloth',12.2,y+1.31,3.5,2,.035,.85,teal)
box('Knife blade',12.2,y+1.43,3.5,.14,.055,.75,iron,.02,rot=-.4)
beam('Knife handle',(12.45,y+1.43,3.9),(12.6,y+1.43,4.25),.075,wood)
sign('IRON KNIFE   24',12.2,y+.9,3.67,2)
wheel=cylinder('Sharpening wheel',16.7,y+1.35,3.5,.6,.27,stone,32);wheel.rotation_euler.x=math.pi/2
for side in [-1,1]:beam('Wheel stand',(16.7+side*.45,y,3.3),(16.7+side*.12,y+1.4,3.5),.12,wood)
beam('Wheel axle',(16.7,y+1.35,3.15),(16.7,y+1.35,3.95),.07,iron);sign('SHARPEN   12',16.7,y+.45,4,1.8)
for i in range(5):beam('Wall tool',(10+i*.5,y+2.2,-3.45),(10+i*.5,y+2.85,-3.45),.035,iron)
y=shop('Mallow’s Catch & Tackle',-5,-10,7,teal)
current=collection('Weighing tray')
box('Sell tray',-4,y+1.36,-5.2,1.4,.15,1,brass,.035)
for xx in [-.65,.65]:box('Tray rim',-4+xx,y+1.47,-5.2,.08,.14,1,brass)
sign('WEIGH YOUR CATCH',-4,y+.75,-4.83,2.4)
# Bell tower: masonry layers, open bell arch, buttresses and crooked ruin crown.
current=collection('Wind Bell Tower');x=3;z=-24;y=ground(x,z)
box('Tower base',x,y+.2,z,7,.4,6,stone,.15)
for layer in range(8):
    for side in [-1,1]:
        box('Masonry pier',x+side*2,y+.65+layer*.92,z,1.25,.86,2.1,stone,.09)
        box('Buttress',x+side*2.75,y+.45+layer*.65,z-.6,.6,.63,2.2,stone,.09)
for i in range(7):
    a=i/6*math.pi;xx=x+math.cos(a)*2;yy=y+7+math.sin(a)*1.65
    o=box('Arch voussoir',xx,yy,z,1,.9,2.25,stone,.06);o.rotation_euler.y=a-math.pi/2
beam('Bell crossbar',(x-2,y+6.7,z),(x+2,y+6.7,z),.16,wood)
for i in range(3):box('Ruin crest',x+(i-1)*1.1,y+9.15+(.3 if i==1 else 0),z,1,.6,2,stone,.1)
sign('WIND BELL HARBOR',x,y+1,z+1.25,3.5)
# Living tree with curved branch networks, roots and overlapping leaf masses.
current=collection('Great lantern tree');tx=-19;tz=-17;ty=ground(tx,tz)
for i in range(7):
    a=i*math.tau/7;beam('Spreading root',(tx,ty+.8,tz),(tx+math.cos(a)*3.2,ground(tx+math.cos(a)*3.2,tz+math.sin(a)*3.2),tz+math.sin(a)*3.2),.32,bark)
nodes=[(tx,ty,tz),(tx+.6,ty+2.5,tz),(tx-.2,ty+5,tz+.2),(tx+1,ty+8,tz)]
for i in range(3):beam('Twisted trunk',nodes[i],nodes[i+1],1-i*.18,bark)
for j in range(9):
    a=j*2.4;base=nodes[1+j%2];end=(tx+math.cos(a)*5,ty+7+(j%3),tz+math.sin(a)*4)
    mid=((base[0]+end[0])/2,ty+5+j%3,(base[2]+end[2])/2);beam('Branch',base,mid,.35,bark);beam('Branch tip',mid,end,.22,bark)
    for k in range(5):sphere('Leaf crown',end[0]+random.uniform(-1.6,1.6),end[1]+random.uniform(-.5,1),end[2]+random.uniform(-1.6,1.6),2.2,1.3,1.8,random.choice(leaves),2)
for i in range(6):
    xx=tx-4+i*1.6;zz=tz+2;beam('Lantern cord',(xx,ty+7,zz),(xx,ty+4.5,zz),.015,dark);lantern(xx,ty+4.3,zz)
# Meadow details, instanced-looking but fully editable objects in source.
current=collection('Meadow and flowers')
for i in range(1000):
    x=random.uniform(-32,32);z=random.uniform(-29,27)
    if (x/34)**2+((z+2)/30)**2>.97 or pool(x,z,1) or abs(x-1.5)<3 or abs(z+2)<2 or (8<x<19 and -6<z<6) or (-10<x<0 and -14<z<-3):continue
    yy=ground(x,z);v=[];f=[]
    for k in range(3):
        a=k*2.1+random.random();r=.13;h=random.uniform(.18,.48);n=len(v);v.extend([(x+math.cos(a)*r,yy,z+math.sin(a)*r),(x-math.cos(a)*r,yy,z-math.sin(a)*r),(x+math.sin(a)*.17,yy+h,z+math.cos(a)*.17)]);f.append((n,n+1,n+2))
    mesh('Grass clump',v,f,random.choice(grasses))
    if i%7==0:
        beam('Flower stem',(x,yy,z),(x,yy+.42,z),.012,grasses[0])
        for k in range(5):
            a=k*math.tau/5;sphere('Wildflower petal',x+math.cos(a)*.09,yy+.43,z+math.sin(a)*.09,.085,.028,.055,petal if i%3 else pink,1)
        sphere('Flower heart',x,yy+.46,z,.035,.03,.035,brass)
# A bank pier, safe walkable dock and resting bench.
current=collection('Dock and harbor furniture')
for i in range(25):box('Dock plank',0,.84,25+i*.45,5,.22,.42,wood,.04)
for side in [-1,1]:
    for zz in [25,28,31,34,36]:
        box('Mooring post',side*2.7,1,zz,.24,2.4,.24,wood)
        if zz<34:beam('Dock rope',(side*2.7,1.65,zz),(side*2.7,1.65,zz+3),.035,cream)
    lantern(side*2.7,2,34)
yy=ground(5,10)
for xx in [4.2,5.8]:box('Bench leg',xx,yy+.32,10,.2,.65,.65,wood)
box('Rest bench seat',5,yy+.65,10,2.3,.12,.9,wood);box('Rest bench back',5,yy+1.15,9.6,2.3,.5,.12,wood)
sign('TAKE A BREATH',5,yy+1.55,9.5,2)
# Low fences suggest routes without obstructing sightlines.
for zz in [17,21,24]:
    for xx in [-4,5]:
        yy=ground(xx,zz);box('Fence post',xx,yy+.6,zz,.16,1.2,.16,wood)
        if zz<24:beam('Fence rail',(xx,yy+.8,zz),(xx,ground(xx,zz+3)+.8,zz+3),.055,wood)
# Save editable objects before making a render-efficient export.
scene=bpy.context.scene;scene.world=bpy.data.worlds.new('Cloudwake sky');scene.world.color=(.4,.6,.65)
bpy.ops.object.camera_add(location=coord(2,3,21));cam=bpy.context.object;cam.name='First person reference';cam.rotation_euler=(Vector(coord(-10,1.4,3))-cam.location).to_track_quat('-Z','Y').to_euler();scene.camera=cam;cam.data.lens=26
bpy.ops.object.light_add(type='SUN',location=(0,0,30));bpy.context.object.rotation_euler=(.5,-.4,-.6);bpy.context.object.data.energy=3
scene['README']='Cloudwake v2 first-person harbor. Source objects remain editable. Runtime water, players, catches, bell and equipment are separate game objects. Re-run tools/build-harbor-v2.py to regenerate; export manually without regenerating to preserve hand edits.'
bpy.ops.file.pack_all();os.makedirs(os.path.join(ROOT,'art','v2'),exist_ok=True);bpy.ops.wm.save_as_mainfile(filepath=os.path.join(ROOT,'art','v2','Cloudwake-FirstPerson.blend'))
# Apply modifiers and join by collection for a compact runtime scene.
for col in list(bpy.data.collections):
    objs=[o for o in col.objects if o.type in {'MESH','FONT'}]
    if not objs:continue
    bpy.ops.object.select_all(action='DESELECT')
    for o in objs:o.select_set(True)
    bpy.context.view_layer.objects.active=objs[0];bpy.ops.object.convert(target='MESH');bpy.ops.object.join();bpy.context.object.name=col.name
bpy.ops.object.select_all(action='DESELECT')
for o in scene.objects:
    if o.type=='MESH':o.select_set(True)
bpy.ops.export_scene.gltf(filepath=os.path.join(ROOT,'public','models','harbor-v2.glb'),export_format='GLB',use_selection=True,export_yup=True,export_extras=False)
print('CLOUDWAKE_V2_ASSETS_READY')

