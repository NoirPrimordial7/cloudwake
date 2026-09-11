"""Blender-first Bellheart construction kit. Dimensions meters; individual editable sources."""
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
from bellheart_modeling import *

REF='BH-FISHSHOP-MASTER; BH-BLACKSMITH-MASTER; BH-MATERIAL-BIBLE'
def finish(name): export(name,'ArchitectureKit',REF)
for name,kind in [('SM_BH_WoodBeam_A','beam'),('SM_BH_WoodBeam_B','brace'),('SM_BH_Wall_Wood_A','woodwall'),('SM_BH_Wall_Stone_A','stonewall'),('SM_BH_Foundation_Stone_A','foundation'),('SM_BH_Roof_Teal_A','roof'),('SM_BH_Roof_Teal_Corner','roofcorner'),('SM_BH_Window_A','window'),('SM_BH_Door_A','door'),('SM_BH_Stairs_A','stairs'),('SM_BH_Railing_A','railing'),('SM_BH_Fence_A','fence'),('SM_BH_Awning_A','awning'),('SM_BH_Banner_A','banner'),('SM_BH_DockModule_A','dock'),('SM_BH_Bridge_A','bridge')]:
 reset()
 if kind=='beam':
  beam('Slightly bowed oak post',(0,0,0),(.035,0,3),.25)
  for z in [.15,2.85]:cube('Joinery collar',(0,0,z),(.29,.29,.12),'DarkWood',.02)
 if kind=='brace': beam('Knee brace',(0,0,0),(1.25,0,1.25),.2)
 if kind=='woodwall':
  for j in range(10):cube('Oak board',(-1.35+j*.3,0,1.4),(.28,.12,2.8),'Wood',.025)
  for x in [-1.5,1.5]:beam('Frame post',(x,0,0),(x+.025,0,3),.23)
  for z in [.12,2.9]:beam('Frame rail',(-1.5,-.03,z),(1.5,-.03,z),.22,'DarkWood')
 if kind in ['stonewall','foundation']:
  rows=3 if kind=='foundation' else 6
  for k in range(rows):
   edges=[-1.5,-.75,0,.75,1.5] if k%2==0 else [-1.5,-1.125,-.375,.375,1.125,1.5]
   for left,right in zip(edges,edges[1:]):
    cube('Warm limestone staggered course',((left+right)*.5,0,k*.48+.24),(right-left-.03,.6 if kind=='foundation' else .35,.45),'Stone',.045)
 if kind in ['roof','roofcorner']:
  if kind=='roof':
   for j in range(6):
    for k in range(6):
     x=-1.25+k*.5;y=j*.43;z=.32*j+.025*math.sin(k)
     o=cube('Overlapping teal shingle',(x,y,z),(.48,.66,.10),'TealLight' if (j+k)%7==0 else 'Teal',.045);o.rotation_euler.x=.63
   beam('Swept eave',(-1.6,-.22,-.08),(1.6,-.22,-.08),.18,'DarkWood')
  else:
   # Two triangular roof planes meet at a diagonal hip; closed extruded tile wedges.
   for side in range(2):
    for row in range(6):
     v0=row*.5;v1=min(3,(row+1)*.5+.08)
     for col in range(row,6):
      u0=col*.5+.01;u1=(col+1)*.5-.01
      points=[(u0,v0),(u1,v0),(u1,min(v1,u1)),(max(u0,v1),v1)] if col>row else [(u0,v0),(u1,v0),(u1,u1),(u0,u0)]
      verts=[]
      for dz in [0,-.09]:
       for a,b in points:
        x,y=(a,b) if side==0 else (b,a)
        verts.append((x-1.5,y,.64*b+dz))
      mesh('Hip roof shingle',verts,[(0,1,2,3),(7,6,5,4),(0,4,5,1),(1,5,6,2),(2,6,7,3),(3,7,4,0)],'TealLight' if (row+col)%7==0 else 'Teal')
   beam('Diagonal hip ridge',(-1.5,0,.09),(1.5,3,2.01),.18,'DarkWood')
   beam('Corner eave X',(-1.5,0,-.08),(1.5,0,-.08),.18,'DarkWood')
   beam('Corner eave Y',(-1.5,0,-.08),(-1.5,3,-.08),.18,'DarkWood')
 if kind in ['window','door']:
  width=1.1 if kind=='window' else 1.35;height=1.4 if kind=='window' else 2.2
  for j in range(6):cube('Inset panel',(-width*.5+(j+.5)*width/6,0,height*.5),(width/6-.012,.09,height),'Teal' if kind=='window' else 'Wood',.02)
  for x in [-width*.5-.09,width*.5+.09]:beam('Jamb',(x,0,-.1),(x,0,height+.13),.18,'DarkWood')
  for z in [-.03,height+.07]:beam('Lintel/sill',(-width*.5-.16,-.02,z),(width*.5+.16,-.02,z),.19,'DarkWood')
  if kind=='window':
   beam('Mullion',(0,-.07,0),(0,-.07,height),.075,'Cream');beam('Transom',(-width*.5,-.07,height*.52),(width*.5,-.07,height*.52),.075,'Cream')
  else:
   for z in [.45,1.65]:cube('Iron strap',(0,-.07,z),(width,.055,.09),'Iron',.02)
   ico('Bronze latch',(width*.3,-.13,1.0),(.05,.05,.07),'Bronze',2)
 if kind=='stairs':
  for j in range(6):cube('Limestone tread',(0,j*.3,j*.16+.08),(2,.32,.16),'Stone',.04)
 if kind in ['fence','railing']:
  for x in [-1.5,1.5]:beam('Oak post',(x,0,0),(x+.025,0,1.05),.17)
  for z in [.4,.9]:beam('Hand rail',(-1.55,0,z),(1.55,.015,z),.12,'DarkWood')
  if kind=='railing':
   for x in [-1,-.5,0,.5,1]:beam('Baluster',(x,0,.1),(x,0,.9),.075)
 if kind=='awning':
  for x in [-1.5,1.5]:beam('Awning post',(x,-1.4,0),(x,-1.4,2.65),.15)
  for j in range(12):cube('Teal awning strip',(-1.375+j*.25,-.7,2.75),(.24,1.6,.07),'Teal',.03)
  beam('Front valance',(-1.55,-1.45,2.65),(1.55,-1.45,2.65),.15,'DarkWood')
 if kind=='banner':banner((0,0,1.2),1)
 if kind in ['dock','bridge']:
  length=3 if kind=='dock' else 6
  for j in range(int(length/.25)):cube('Deck plank',(0,-length*.5+(j+.5)*.25,0),(3,.23,.14),'Wood',.025)
  for x in [-1.3,1.3]:
   beam('Deck joist',(x,-length*.5,-.25),(x,length*.5,-.25),.24,'DarkWood')
   for y in [-length*.5,length*.5]:beam('Rail post',(x,y,-.3),(x,y,1),.18)
   beam('Bridge rail',(x,-length*.5,.9),(x,length*.5,.9),.12,'DarkWood')
 finish(name)
(R/'tools/production/architecture_manifest.json').write_text(json.dumps(manifest,indent=2))
print('BH_ARCHITECTURE_KIT_COMPLETE',len(manifest['assets']),flush=True)
