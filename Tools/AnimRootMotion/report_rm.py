# -*- coding: utf-8 -*-
"""列出目录下所有 AnimSequence 的 root motion 状态"""
import unreal, os

ANIM_DIR = "/Game/Mesh/YinHU/Animtions"
OUT = "D:/Warrior/Saved/rm_report.txt"
L = []
def log(m):
    L.append(str(m)); unreal.log(str(m)); print(m, flush=True)
def dump():
    open(OUT, "w", encoding="utf-8").write("\n".join(L))

AL = unreal.AnimationLibrary
try:
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    seqs = [a.get_asset() for a in ar.get_assets_by_path(ANIM_DIR, recursive=False)
            if str(a.asset_class_path.asset_name) == "AnimSequence" and a.get_asset()]
    seqs.sort(key=lambda o: o.get_name())
    on = off = 0
    for s in seqs:
        rm = AL.is_root_motion_enabled(s)
        if rm: on += 1
        else:  off += 1
        log("%-6s %-52s add=%s" % (rm, s.get_name(), AL.get_additive_animation_type(s)))
    log("\nTOTAL=%d  root_motion_ON=%d  OFF=%d" % (len(seqs), on, off))
except Exception:
    import traceback
    log("FATAL:\n" + traceback.format_exc())
finally:
    dump()
