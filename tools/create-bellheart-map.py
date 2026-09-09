"""Run with UnrealEditor-Cmd -run=pythonscript -script=... (after C++ build)."""
import unreal
unreal.AssetRegistryHelpers.get_asset_registry().search_all_assets(True)
asset = '/Game/Maps/Bellheart'
sub = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
rebuild = '-BHRebuild' in unreal.SystemLibrary.get_command_line()
if unreal.EditorAssetLibrary.does_asset_exist(asset) and not rebuild:
    unreal.log('Bellheart map already exists; preserving designer edits.')
else:
    if rebuild:
        unreal.EditorLoadingAndSavingUtils.new_blank_map(False)
    else:
        sub.new_level(asset)
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    world = actors.spawn_actor_from_class(unreal.load_class(None, '/Script/Cloudwake.BHWorld'), unreal.Vector(0, 0, 0))
    world.set_actor_label('Bellheart Greybox - canonical reference layout')
    world.build()
    start = actors.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(0, -8600, 110), unreal.Rotator(0, 90, 0))
    start.set_actor_label('Arrival - 180cm player, 168cm eyes')
    unreal.EditorLoadingAndSavingUtils.save_map(unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world(), asset)
    unreal.log('BH_MAP_SAVED /Game/Maps/Bellheart')
