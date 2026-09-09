"""Measured concept diagrams rendered from fixed coordinates; no AI artwork editing."""
from PIL import Image,ImageDraw,ImageFont
import pathlib,json
B=pathlib.Path(__file__).resolve().parents[1]/'Docs/Art/Bellheart'
W,H=2000,1600
fontpath='C:/Windows/Fonts/segoeui.ttf'
def font(n):return ImageFont.truetype(fontpath,n)
sites=[('Dock',0,-86,0),('Arrival Path',0,-65,3),('Central Pond',0,-20,6),('Giant Tree',-34,5,10),('Fishing Shop',-24,30,14),('Blacksmith',27,30,14),('Elder House',-20,58,20),('Workshop',46,10,10),('Village Plaza',0,30,14),('Bell Tower',0,76,26),('Waterfall Fishing Area',-47,-24,6),('Bellcrab Arena',26,-11,7),('Lower Fishing Bank',-10,-43,7),('Hidden Path',-51,17,11),('Cloud Skiff Area',22,-83,0)]
def pt(x,y):return (650+x*6,790-y*6)
def setup(title,subtitle):
 im=Image.new('RGB',(W,H),'#f0e9d9');d=ImageDraw.Draw(im)
 d.rectangle((0,0,W,145),fill='#173B42');d.text((65,30),title,font=font(47),fill='#E9DFC2');d.text((65,98),subtitle,font=font(22),fill='#B8CCBF')
 return im,d
outline=[(-12,-99),(-27,-76),(-42,-59),(-60,-48),(-72,-19),(-68,23),(-52,51),(-36,70),(-22,92),(15,94),(35,75),(55,57),(70,26),(72,-8),(57,-37),(40,-66),(35,-90),(15,-97)]
main=[(0,-86),(0,-65),(0,-49),(17,-35),(27,-21),(29,5),(12,22),(0,30),(-10,44),(-20,58),(0,65),(0,76)]
side=[(-10,-43),(-29,-36),(-47,-24),(-53,-2),(-51,17),(-34,5),(-24,30),(0,30),(27,30),(46,10)]
def line(d,points,color,width):d.line([pt(*p) for p in points],fill=color,width=width,joint='curve')
def base(d):
 d.polygon([pt(*p) for p in outline],fill='#bac497',outline='#6B7C54',width=6)
 for y in [-60,-20,20,60]:d.line((pt(-70,y),pt(70,y)),fill='#adba8e',width=1)
 # terrace edge lines indicate areas, not traversable walls
 line(d,[(-66,10),(-43,20),(-6,19),(22,21),(67,10)],'#8e9d6d',4)
 line(d,[(-45,52),(-26,48),(6,54),(38,63)],'#8e9d6d',4)
 line(d,[(-22,73),(0,63),(25,74)],'#8e9d6d',4)
 a=pt(-23,-3);b=pt(23,-37);d.ellipse((a[0],a[1],b[0],b[1]),fill='#36B2B1',outline='#D5C6A1',width=10)
 line(d,[(-20,-22),(-35,-22),(-47,-24),(-68,-30)],'#36B2B1',22)
 a=pt(15,0);b=pt(37,-22);d.rounded_rectangle((a[0],a[1],b[0],b[1]),radius=18,fill='#d8bd86',outline='#ab754b',width=4)
 line(d,main,'#e5cf9e',20);line(d,side,'#e5cf9e',11)
 for i in [4,5,6,7,9]:
  name,x,y,z=sites[i];a=pt(x-5,y+4);b=pt(x+5,y-4);d.rectangle((*a,*b),fill='#286C70',outline='#483321',width=3)
 x,y=pt(-34,5);d.ellipse((x-60,y-60,x+60,y+60),fill='#56743D',outline='#3E633D',width=5)
 line(d,[(0,-78),(0,-97)],'#997043',26);line(d,[(0,-86),(22,-86)],'#997043',20)
 d.text((1140,200),'N',font=font(36),fill='#173B42');d.line((1155,275,1155,240),fill='#173B42',width=5);d.polygon([(1155,221),(1145,241),(1165,241)],fill='#173B42')
 d.line((100,1410,400,1410),fill='#173B42',width=5);d.text((160,1420),'50 meters',font=font(24),fill='#173B42')
def marks(d):
 for i,(name,x,y,z) in enumerate(sites,1):
  x,y=pt(x,y);d.ellipse((x-22,y-22,x+22,y+22),fill='#173B42',outline='#E9DFC2',width=2);d.text((x,y),str(i).zfill(2),font=font(19),fill='#ffffff',anchor='mm')
def legend(d):
 d.text((1280,195),'CANONICAL LOCATIONS',font=font(29),fill='#173B42')
 for i,(name,x,y,z) in enumerate(sites,1):
  yy=255+(i-1)*68;d.text((1280,yy),f'BH-{i:02}  {name}',font=font(23),fill='#173B42');d.text((1360,yy+30),f'({x}, {y}) m  /  elevation {z} m',font=font(18),fill='#647164')
 d.text((1280,1320),'North +Y · East +X · Z up',font=font(22),fill='#173B42')
 d.text((1280,1360),'Main path 3 m · Side path 1.8 m',font=font(22),fill='#173B42')
 d.text((1280,1400),'Concept proportions; blockout validates travel.',font=font(19),fill='#647164')
im,d=setup('BELLHEART ISLE / TOP-DOWN','CW_BH_ISLAND_TOPDOWN_V001  •  Layout authority  •  All distances in meters');base(d);marks(d);legend(d);im.save(B/'01_Island/CW_BH_ISLAND_TOPDOWN_V001.png')
im,d=setup('BELLHEART ISLE / GAMEPLAY FLOW','CW_BH_ISLAND_GAMEPLAY_FLOW_V001  •  Same coordinates as top-down  •  Quest revisits are intentional');base(d)
line(d,main,'#CD713E',10);line(d,side,'#7A59A3',6)
quest=[(0,-86),(-20,58),(-24,30),(-10,-43),(-24,30),(27,30),(0,76),(-10,-43),(-20,58),(-24,30),(26,-11),(0,76),(22,-83)]
for a,b in zip(quest,quest[1:]):
 p,q=pt(*a),pt(*b)
 for k in range(0,20,2):d.line((p[0]+(q[0]-p[0])*k/20,p[1]+(q[1]-p[1])*k/20,p[0]+(q[0]-p[0])*(k+1)/20,p[1]+(q[1]-p[1])*(k+1)/20),fill='#b59637',width=3)
marks(d)
keys=[('MAIN PATH','#CD713E'),('SIDE PATH','#7A59A3'),('QUEST LINKS (not walking routes)','#b59637'),('FISHING AREA','#36B2B1'),('SHOP / teal footprint','#286C70'),('NPC / Orin07 Mira05 Bram06 Tavi09','#173B42'),('BOSS AREA /12','#ab754b'),('LANDMARK /04 and10','#56743D'),('SPAWN /01','#997043'),('EXIT /15','#483321')]
for i,(s,c) in enumerate(keys):d.rectangle((1260,205+i*53,1290,235+i*53),fill=c);d.text((1310,203+i*53),s,font=font(21),fill='#173B42')
steps=['01 Dock →07 Orin: accept quest','05 Mira →13 bank: learn and catch','05 Sell →06 Bram: equip knife','10 Inspect tower →03 catch three','07 Return fragment →05 special bait','12 Reel Bellcrab → fight in arena','10 Carry Bellheart → install → ring','15 Cloud Skiff: next-island exit']
d.text((1260,815),'QUEST ORDER',font=font(28),fill='#173B42')
for i,s in enumerate(steps):d.text((1260,870+i*52),s,font=font(23),fill='#173B42')
d.text((1260,1330),'Boss bay: two retreat routes to plaza',font=font(21),fill='#173B42');d.text((1260,1360),'and south bank; never gate both.',font=font(21),fill='#173B42');im.save(B/'01_Island/CW_BH_ISLAND_GAMEPLAY_FLOW_V001.png')
im,d=setup('BELLHEART ISLE / ELEVATION','CW_BH_ISLAND_ELEVATION_V001  •  South–north section plus western waterfall inset')
def ep(y,z):return (150+(y+100)*7,1050-z*11)
for z in range(-40,61,10):d.line((100,ep(0,z)[1],1560,ep(0,z)[1]),fill='#cecbbb',width=2);d.text((30,ep(0,z)[1]-14),f'{z}m',font=font(20),fill='#647164')
profile=[(-100,0),(-86,0),(-65,3),(-48,7),(-35,7),(-3,7),(15,10),(30,14),(48,18),(58,20),(69,26),(83,26),(92,31),(98,25)]
d.polygon([ep(*p) for p in profile]+[ep(75,-10),ep(30,-35),ep(-20,-45),ep(-75,-15)],fill='#b1b1a0',outline='#647164',width=4);d.line([ep(*p) for p in profile],fill='#778844',width=10)
for name,y,z in [('Dock',-86,0),('Lower Pond',-20,6),('Village',30,14),('Tower Hill',62,26),('Upper Ruins',92,31)]:
 x,yy=ep(y,z);d.line((x,yy,x,yy-65),fill='#173B42',width=2);d.text((x-60,yy-100),f'{name}\n+{z}m',font=font(20),fill='#173B42')
x,y=ep(76,26);d.rectangle((x-25,y-352,x+25,y),fill='#D5C6A1',outline='#997043',width=4);d.text((x-125,y-410),'Tower: 32m above hill\nTotal top elevation +58m',font=font(22),fill='#173B42')
d.line((ep(-37,6),ep(-3,6)),fill='#36B2B1',width=10)
d.text((1640,240),'WEST FALL',font=font(27),fill='#173B42');d.rectangle((1720,450,1870,1100),fill='#b1b1a0');d.line((1650,450,1720,450,1720,714),fill='#36B2B1',width=15);d.text((1610,385),'Pond surface +6m',font=font(20),fill='#173B42');d.text((1610,735),'First drop to −18m',font=font(20),fill='#173B42')
d.text((180,1470),'Traversal: use winding paths / switchbacks on slopes. Section exaggerates height for readability; use listed dimensions.',font=font(23),fill='#173B42');im.save(B/'01_Island/CW_BH_ISLAND_ELEVATION_V001.png')
(B/'18_Blockout/layout_coordinates.json').write_text(json.dumps({'units':'meters','axes':{'x':'east','y':'north','z':'up'},'sites':[dict(id=f'BH-{i:02}',name=n,x=x,y=y,z=z)for i,(n,x,y,z) in enumerate(sites,1)],'main_path':main,'side_path':side,'pond_size':[46,34],'arena_size':[22,18]},indent=2))
print('Three measured diagrams created.')
