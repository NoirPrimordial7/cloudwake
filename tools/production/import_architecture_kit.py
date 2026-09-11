"""Import the construction kit without rebuilding or modifying Bellheart's level."""
import json
from pathlib import Path
import unreal as u

ROOT = Path('E:/Try')
data = json.loads((ROOT / 'tools/production/architecture_manifest.json').read_text())
u.SystemLibrary.execute_console_command(None, 'Interchange.FeatureFlags.Import.FBX 0')
u.AssetRegistryHelpers.get_asset_registry().search_all_assets(True)
at = u.AssetToolsHelpers.get_asset_tools()
report = []
for asset in data['assets']:
    dest = '/Game/Cloudwake/Art/' + asset['category']
    task = u.AssetImportTask()
    task.filename = asset['fbx']
    task.destination_path = dest
    task.destination_name = asset['name']
    task.automated = True
    task.save = True
    task.replace_existing = True
    task.replace_existing_settings = True
    opts = u.FbxImportUI()
    opts.import_mesh = True
    opts.import_materials = False
    opts.import_textures = False
    opts.import_as_skeletal = False
    opts.automated_import_should_detect_type = False
    opts.mesh_type_to_import = u.FBXImportType.FBXIT_STATIC_MESH
    imp = opts.static_mesh_import_data
    imp.combine_meshes = True
    imp.auto_generate_collision = False
    imp.convert_scene = False
    imp.convert_scene_unit = False
    imp.import_uniform_scale = .01
    task.options = opts
    at.import_asset_tasks([task])
    mesh = u.load_asset(dest + '/' + asset['name'])
    if not mesh:
        raise RuntimeError('Import failed: ' + asset['name'])
    for i, slot in enumerate(mesh.get_editor_property('static_materials')):
        name = str(slot.material_slot_name).split('.')[0]
        material = u.load_asset('/Game/Cloudwake/Art/Materials/M_BH_' + name)
        if not material:
            raise RuntimeError('Missing shared material: ' + name)
        mesh.set_material(i, material)
    u.EditorAssetLibrary.save_loaded_asset(mesh)
    bounds = mesh.get_bounds()
    extent = bounds.box_extent
    dimensions = [round(v * 2, 2) for v in (extent.x, extent.y, extent.z)]
    if max(dimensions) > 1000 or min(dimensions) <= 0:
        raise RuntimeError('Unexpected centimeter bounds: ' + asset['name'] + str(dimensions))
    report.append({'name': asset['name'], 'unreal_asset': dest + '/' + asset['name'],
                   'dimensions_cm': dimensions, 'collision': 'none; decorative kit',
                   'placement_scale': [1, -1, 1]})
out = ROOT / 'Docs/Art/Bellheart/architecture_import_report.json'
out.write_text(json.dumps({'assets': report, 'level_modified': False}, indent=2))
u.log('BH_ARCHITECTURE_IMPORT_COMPLETE assets=' + str(len(report)))
