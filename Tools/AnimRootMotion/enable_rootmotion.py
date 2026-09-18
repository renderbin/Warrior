# -*- coding: utf-8 -*-
"""批量开启 root motion：遍历目录内所有 AnimSequence，设置 bEnableRootMotion=True 并保存"""
import unreal, os

ANIM_DIR = "/Game/Mesh/YinHU/Animtions"
OUT = "D:/Warrior/Saved/batch_result.txt"
L = []
def log(m):
    L.append(str(m)); unreal.log(str(m)); print(m, flush=True)
def dump():
    open(OUT, "w", encoding="utf-8").write("\n".join(L))

AL = unreal.AnimationLibrary
EAL = unreal.EditorAssetLibrary

ok_list, fail_list, skip_list = [], [], []

try:
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    entries = [a for a in ar.get_assets_by_path(ANIM_DIR, recursive=False)
               if str(a.asset_class_path.asset_name) == "AnimSequence"]
    entries.sort(key=lambda a: str(a.asset_name))
    log("found %d AnimSequence" % len(entries))

    for i, e in enumerate(entries, 1):
        name = str(e.asset_name)
        path = str(e.package_name)
        try:
            s = unreal.load_asset(path)
            if not s:
                fail_list.append((name, "load failed")); continue
            if AL.is_root_motion_enabled(s):
                skip_list.append(name)
                log("[%3d/%d] %-52s already ON" % (i, len(entries), name))
                continue
            s.modify()
            AL.set_root_motion_enabled(s, True)
            if not AL.is_root_motion_enabled(s):
                fail_list.append((name, "set failed")); continue
            saved = EAL.save_asset(path, only_if_is_dirty=False)
            if saved:
                ok_list.append(name)
                log("[%3d/%d] %-52s OK" % (i, len(entries), name))
            else:
                fail_list.append((name, "save returned False"))
                log("[%3d/%d] %-52s SAVE FAILED" % (i, len(entries), name))
        except Exception as ex:
            fail_list.append((name, repr(ex)))
            log("[%3d/%d] %-52s ERR %s" % (i, len(entries), name, ex))

    log("\n" + "=" * 70)
    log("changed : %d" % len(ok_list))
    log("skipped (already on) : %d" % len(skip_list))
    log("failed  : %d" % len(fail_list))
    for n, r in fail_list:
        log("   FAIL %s : %s" % (n, r))
except Exception:
    import traceback
    log("FATAL:\n" + traceback.format_exc())
finally:
    dump()
