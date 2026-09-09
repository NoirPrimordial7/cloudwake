"""Executed inside Unreal before the map builder; no final art assets."""
import unreal
unreal.AssetRegistryHelpers.get_asset_registry().search_all_assets(True)
tools = unreal.AssetToolsHelpers.get_asset_tools()
root = '/Game/Greybox'
mat = unreal.load_asset(root + '/M_Greybox')
if not mat:
    mat = tools.create_asset('M_Greybox', root, unreal.Material, unreal.MaterialFactoryNew())
    color = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, -300, 0)
    color.set_editor_property('parameter_name', 'Tint')
    color.set_editor_property('default_value', unreal.LinearColor(0.5,0.5,0.5,1))
    unreal.MaterialEditingLibrary.connect_material_property(color, '', unreal.MaterialProperty.MP_BASE_COLOR)
    rough = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionConstant, -300, 180)
    rough.set_editor_property('r', 0.8)
    unreal.MaterialEditingLibrary.connect_material_property(rough, '', unreal.MaterialProperty.MP_ROUGHNESS)
    unreal.MaterialEditingLibrary.recompile_material(mat)
    unreal.EditorAssetLibrary.save_loaded_asset(mat)
palette = {'Stone':(0.48,0.53,0.53), 'Path':(0.65,0.43,0.15), 'Teal':(0.06,0.27,0.3), 'Wood':(0.28,0.16,0.07), 'Water':(0.03,0.5,0.65), 'Tree':(0.18,0.32,0.09), 'Bronze':(0.6,0.35,0.08), 'Restored':(0.2,0.9,0.65)}
for name, rgb in palette.items():
    mi = unreal.load_asset(root + '/MI_' + name)
    if not mi:
        mi = tools.create_asset('MI_'+name, root, unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
    unreal.MaterialEditingLibrary.set_material_instance_parent(mi, mat)
    unreal.MaterialEditingLibrary.set_material_instance_vector_parameter_value(mi,'Tint',unreal.LinearColor(*rgb,1))
    unreal.EditorAssetLibrary.save_loaded_asset(mi)
task = unreal.AssetImportTask()
task.set_editor_property('filename', 'E:/Try/art/greybox/S_Bell.wav')
task.set_editor_property('destination_path', '/Game/Audio')
task.set_editor_property('automated', True)
task.set_editor_property('replace_existing', True)
task.set_editor_property('save', True)
tools.import_asset_tasks([task])
unreal.log('BH_GREYBOX_MATERIALS_SAVED')
