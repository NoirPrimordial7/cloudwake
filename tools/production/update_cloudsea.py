"""Repair the existing cloud material without rebuilding the gameplay level."""
import unreal as u
from pathlib import Path

# Use the same authoring code as the assembly script to keep the two workflows equal.
source = Path('E:/Try/tools/production/import_bellheart.py').read_text(encoding='utf-8-sig')
block = source[source.index("cloudmat=u.load_asset"):source.index('meshes={}')]
review = '-BHCloudReview' in u.SystemLibrary.get_command_line()
if review:
    block = block.replace('M_BH_Cloudsea', 'M_BH_Cloudsea_Review')
def custom_input(name):
    value = u.CustomInput()
    value.set_editor_property('input_name', name)
    return value
AT = u.AssetToolsHelpers.get_asset_tools()
ML = u.MaterialEditingLibrary
EA = u.EditorAssetLibrary
exec(block)
if review:
    levels = u.get_editor_subsystem(u.LevelEditorSubsystem)
    levels.load_level('/Game/Maps/Bellheart')
    actors = u.get_editor_subsystem(u.EditorActorSubsystem)
    count = 0
    for actor in actors.get_all_level_actors():
        component = actor.get_component_by_class(u.VolumetricCloudComponent)
        if component:
            component.set_editor_property('material', cloudmat)
            count += 1
    if count != 1:
        raise RuntimeError('Expected exactly one cloud layer, found ' + str(count))
    world = u.get_editor_subsystem(u.UnrealEditorSubsystem).get_editor_world()
    if not u.EditorLoadingAndSavingUtils.save_map(world, '/Game/Cloudwake/Art/Reviews/Bellheart_CloudReview'):
        raise RuntimeError('Cloud review map save failed')
u.log('BH_CLOUDSEA_MATERIAL_UPDATED extinction=SubsurfaceColor')
