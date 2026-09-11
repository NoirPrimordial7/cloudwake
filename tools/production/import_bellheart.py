"""Import editable Blender art and assemble over the tested Bellheart collision layout."""
import unreal as u, json, collections
from pathlib import Path
ROOT=Path('E:/Try')
data=json.loads((ROOT/'tools/production/art_manifest.json').read_text())
u.SystemLibrary.execute_console_command(None,'Interchange.FeatureFlags.Import.FBX 0')
u.AssetRegistryHelpers.get_asset_registry().search_all_assets(True)
AT=u.AssetToolsHelpers.get_asset_tools(); ML=u.MaterialEditingLibrary; EA=u.EditorAssetLibrary
palette={'Wood':(.32,.16,.065),'DarkWood':(.095,.047,.022),'Stone':(.57,.51,.37),'Rock':(.29,.32,.29),'Teal':(.025,.19,.20),'TealLight':(.05,.28,.28),'Bronze':(.48,.28,.08),'Iron':(.11,.13,.13),'Rope':(.5,.36,.17),'Cream':(.83,.77,.57),'Grass':(.23,.31,.07),'Leaf':(.29,.38,.065),'LeafLight':(.43,.46,.10),'LeafDark':(.055,.15,.035),'Water':(.018,.43,.46),'Foam':(.72,.94,.89),'Glow':(1,.49,.08),'Dirt':(.42,.29,.13)}
materials={}
def custom_input(name):
 value=u.CustomInput(); value.set_editor_property('input_name',name); return value
for name,color in palette.items():
 path='/Game/Cloudwake/Art/Materials/M_BH_'+name
 m=u.load_asset(path) or AT.create_asset('M_BH_'+name,'/Game/Cloudwake/Art/Materials',u.Material,u.MaterialFactoryNew())
 ML.delete_all_material_expressions(m)
 m.set_editor_property('two_sided',True)
 m.set_editor_property('used_with_instanced_static_meshes',True)
 tint=ML.create_material_expression(m,u.MaterialExpressionVectorParameter,-450,0)
 tint.set_editor_property('parameter_name','Tint'); tint.set_editor_property('default_value',u.LinearColor(*color,1))
 ML.connect_material_property(tint,'',u.MaterialProperty.MP_BASE_COLOR)
 if name in ('Wood','DarkWood','Stone','Rock','Grass','Dirt','Water'):
  pos=ML.create_material_expression(m,u.MaterialExpressionWorldPosition,-900,300)
  time=ML.create_material_expression(m,u.MaterialExpressionTime,-900,450)
  variation=ML.create_material_expression(m,u.MaterialExpressionCustom,-600,300)
  variation.set_editor_property('inputs',[custom_input('P'),custom_input('T')])
  variation.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT1)
  code='return .82 + .18*sin(P.x*.017+sin(P.y*.022))*sin(P.y*.011+P.z*.008);'
  if name in ('Wood','DarkWood'): code='return .9+.07*sin(P.x*.15+sin(P.z*.014)*3)+.03*sin(P.x*.49+P.z*.023);'
  if name=='Grass': code='return .78+.2*sin(P.x*.007+sin(P.y*.02))*sin(P.y*.013);'
  if name=='Water': code='return .87+.08*sin(P.x*.011+T*.3+sin(P.y*.017))*sin(P.y*.014-T*.25)+.05*pow(abs(sin(P.x*.045+sin(P.y*.025+T*.2))),12);'
  variation.set_editor_property('code',code)
  ML.connect_material_expressions(pos,'',variation,'P'); ML.connect_material_expressions(time,'',variation,'T')
  multiply=ML.create_material_expression(m,u.MaterialExpressionMultiply,-200,0)
  ML.connect_material_expressions(tint,'',multiply,'A'); ML.connect_material_expressions(variation,'',multiply,'B'); ML.connect_material_property(multiply,'',u.MaterialProperty.MP_BASE_COLOR)
  if name=='Water':
   normal=ML.create_material_expression(m,u.MaterialExpressionCustom,-350,450)
   normal.set_editor_property('inputs',[custom_input('P'),custom_input('T')]); normal.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT3)
   normal.set_editor_property('code','return normalize(float3(.045*sin(P.x*.025+sin(P.y*.014)+T*.5),.045*cos(P.y*.021+sin(P.x*.017)-T*.4),1));')
   ML.connect_material_expressions(pos,'',normal,'P'); ML.connect_material_expressions(time,'',normal,'T'); ML.connect_material_property(normal,'',u.MaterialProperty.MP_NORMAL)
 rough=ML.create_material_expression(m,u.MaterialExpressionScalarParameter,-200,150)
 rough.set_editor_property('parameter_name','Roughness'); rough.set_editor_property('default_value',.22 if name=='Water' else .72)
 ML.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 if name in ('Bronze','Iron'):
  metal=ML.create_material_expression(m,u.MaterialExpressionConstant,-200,220); metal.set_editor_property('r',.65)
  ML.connect_material_property(metal,'',u.MaterialProperty.MP_METALLIC)
 if name=='Glow': ML.connect_material_property(tint,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 if name=='Water':
  m.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT)
  m.set_editor_property('translucency_lighting_mode',u.TranslucencyLightingMode.TLM_SURFACE_PER_PIXEL_LIGHTING)
  fade=ML.create_material_expression(m,u.MaterialExpressionDepthFade,-150,550)
  fade.set_editor_property('opacity_default',.8); fade.set_editor_property('fade_distance_default',160)
  ML.connect_material_property(fade,'',u.MaterialProperty.MP_OPACITY)
 ML.recompile_material(m); materials[name]=m; EA.save_loaded_asset(m)
cloudmat=u.load_asset('/Game/Cloudwake/Art/Materials/M_BH_Cloudsea') or AT.create_asset('M_BH_Cloudsea','/Game/Cloudwake/Art/Materials',u.Material,u.MaterialFactoryNew())
ML.delete_all_material_expressions(cloudmat); cloudmat.set_editor_property('material_domain',u.MaterialDomain.MD_VOLUME); cloudmat.set_editor_property('blend_mode',u.BlendMode.BLEND_ADDITIVE)
cloudmat.set_editor_property('used_with_volumetric_cloud',True)
white=ML.create_material_expression(cloudmat,u.MaterialExpressionConstant3Vector,-300,0); white.set_editor_property('constant',u.LinearColor(.92,.96,1,1)); ML.connect_material_property(white,'',u.MaterialProperty.MP_BASE_COLOR)
pos=ML.create_material_expression(cloudmat,u.MaterialExpressionWorldPosition,-700,200)
density=ML.create_material_expression(cloudmat,u.MaterialExpressionCustom,-350,200); density.set_editor_property('inputs',[custom_input('P')]); density.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT1)
density.set_editor_property('code','float3 q=P/6000; float3 i=floor(q), f=frac(q); f=f*f*(3-2*f); float n=0; for(int z=0;z<2;z++) for(int y=0;y<2;y++) for(int x=0;x<2;x++){float3 o=float3(x,y,z); float3 w=lerp(1-f,f,o); n+=frac(sin(dot(i+o,float3(127.1,311.7,74.7)))*43758.5453)*w.x*w.y*w.z;} float h=saturate(1-abs(P.z+21000)/9500); return .055*saturate((n-.28)*3)*h*h;')
ML.connect_material_expressions(pos,'',density,'P'); ML.connect_material_property(density,'',u.MaterialProperty.MP_SUBSURFACE_COLOR); ML.recompile_material(cloudmat)
if not EA.save_loaded_asset(cloudmat): raise RuntimeError('Cloudsea material save failed; close editors holding this package before retrying')
meshes={}
for asset in data['assets']:
 dest='/Game/Cloudwake/Art/'+asset['category']
 mesh=u.load_asset(dest+'/'+asset['name'])
 if not mesh or '-BHReimport' in u.SystemLibrary.get_command_line():
  task=u.AssetImportTask(); task.filename=asset['fbx']; task.destination_path=dest; task.destination_name=asset['name']; task.automated=True; task.save=True; task.replace_existing=True
  opts=u.FbxImportUI(); opts.import_mesh=True; opts.import_materials=False; opts.import_textures=False; opts.import_as_skeletal=False
  opts.set_editor_property('automated_import_should_detect_type',False); opts.mesh_type_to_import=u.FBXImportType.FBXIT_STATIC_MESH
  imp=opts.static_mesh_import_data; imp.combine_meshes=True; imp.auto_generate_collision=False; imp.convert_scene=False; imp.convert_scene_unit=False; imp.import_uniform_scale=.01
  task.replace_existing_settings=True; task.options=opts; AT.import_asset_tasks([task]); mesh=u.load_asset(dest+'/'+asset['name'])
 if not mesh: raise RuntimeError('Missing mesh '+asset['name'])
 for i,slot in enumerate(mesh.get_editor_property('static_materials')):
  name=str(slot.material_slot_name).split('.')[0]
  if name not in materials: raise RuntimeError('Unknown material '+name)
  mesh.set_material(i,materials[name])
 if asset['collision']:
  mesh.get_editor_property('body_setup').set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
 EA.save_loaded_asset(mesh); meshes[asset['name']]=mesh
 u.log('BH_ART_IMPORTED '+asset['name']+' bounds='+str(mesh.get_bounds().box_extent))
levels=u.get_editor_subsystem(u.LevelEditorSubsystem); levels.load_level('/Game/Maps/Bellheart')
actors=u.get_editor_subsystem(u.EditorActorSubsystem)
for a in actors.get_all_level_actors():
 atmosphere=a.get_component_by_class(u.SkyAtmosphereComponent)
 if atmosphere:
  atmosphere.set_editor_property('transform_mode',u.SkyAtmosphereTransformMode.PLANET_TOP_AT_COMPONENT_TRANSFORM)
  a.set_actor_location(u.Vector(0,0,-100000),False,False)
 if a.actor_has_tag('BH_Art'): actors.destroy_actor(a); continue
 if isinstance(a,u.StaticMeshActor):
  c=a.static_mesh_component
  c.set_visibility(False); a.set_actor_hidden_in_game(True)
 elif a.get_class().get_name()=='Actor' and a.get_component_by_class(u.TextRenderComponent):
  a.set_actor_hidden_in_game(True)
groups=collections.defaultdict(list)
for ins in data['instances']: groups[ins['name']].append(ins)
cluster=u.load_class(None,'/Script/Cloudwake.BHArtCluster')
for name,instances in groups.items():
 mesh=meshes[name]
 if len(instances)>2:
  a=actors.spawn_actor_from_class(cluster,u.Vector()); a.set_actor_label('Art instances - '+name)
  comp=a.get_editor_property('instances'); comp.set_static_mesh(mesh)
  for ins in instances:
   s=ins['scale']; t=u.Transform(location=u.Vector(*[v*100 for v in ins['position']]),rotation=u.Rotator(0,ins['yaw'],0),scale=u.Vector(s[0],-s[1],s[2]))
   comp.add_instance(t,True)
 else:
  for ins in instances:
   a=actors.spawn_actor_from_class(u.StaticMeshActor,u.Vector(*[v*100 for v in ins['position']]),u.Rotator(0,ins['yaw'],0))
   a.set_actor_label('Art - '+name); a.tags=['BH_Art']; c=a.static_mesh_component; c.set_static_mesh(mesh); c.set_mobility(u.ComponentMobility.STATIC)
   s=ins['scale']; a.set_actor_scale3d(u.Vector(s[0],-s[1],s[2]))
   collision=next(v['collision'] for v in data['assets'] if v['name']==name)
   c.set_collision_enabled(u.CollisionEnabled.QUERY_AND_PHYSICS if collision else u.CollisionEnabled.NO_COLLISION)
for a in actors.get_all_level_actors():
 if isinstance(a,u.DirectionalLight):
  a.light_component.set_mobility(u.ComponentMobility.MOVABLE)
  a.light_component.set_editor_property('intensity',5.0); a.set_actor_rotation(u.Rotator(-48,-35,0),False)
 if isinstance(a,u.SkyLight):
  a.light_component.set_mobility(u.ComponentMobility.MOVABLE); a.light_component.set_editor_property('intensity',1.1); a.light_component.set_editor_property('real_time_capture',True)
cloud=actors.spawn_actor_from_class(u.VolumetricCloud,u.Vector()); cloud.tags=['BH_Art']; cloud.set_actor_label('Bellheart atmospheric clouds')
cc=cloud.get_component_by_class(u.VolumetricCloudComponent)
cc.set_editor_property('material',cloudmat)
cc.set_editor_property('layer_bottom_altitude',.7); cc.set_editor_property('layer_height',.2)
EA.save_directory('/Game/Cloudwake',True,True)
world=u.get_editor_subsystem(u.UnrealEditorSubsystem).get_editor_world()
u.EditorLoadingAndSavingUtils.save_map(world,'/Game/Maps/Bellheart')
if '-BHCleanImports' in u.SystemLibrary.get_command_line():
 for category in {asset['category'] for asset in data['assets']}:
  for temporary in ('Meshes','MeshesV2'):
   old='/Game/Cloudwake/Art/'+category+'/'+temporary
   if EA.does_directory_exist(old): EA.delete_directory(old)
u.log('BH_ART_ASSEMBLY_COMPLETE assets='+str(len(meshes))+' instances='+str(len(data['instances'])))
