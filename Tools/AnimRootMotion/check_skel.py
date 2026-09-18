# -*- coding: utf-8 -*-
"""确认骨骼层级完好"""
import unreal, os
OUT = "D:/Warrior/Saved/skel_check.txt"
L = []
def log(m):
    L.append(str(m)); unreal.log(str(m)); print(m, flush=True)

AL = unreal.AnimationLibrary
AP = unreal.AnimPoseExtensions
try:
    skel = unreal.load_asset("/Game/Mesh/YinHU/SK_HFM_Hu_Wind_01_Skeleton")
    seq = unreal.load_asset("/Game/Mesh/YinHU/Animtions/AS_HFM_Hu_Wind_01_idle_01")
    names = [str(x) for x in AP.get_bone_names(AP.get_anim_pose_at_frame(seq, 0, unreal.AnimPoseEvaluationOptions()))]
    log("skeleton  : %s" % skel.get_path_name())
    log("bone count: %d" % len(names))
    for i in range(6):
        log("  [%2d] %-24s path: %s" % (i, names[i], " < ".join(str(p) for p in AL.find_bone_path_to_root(seq, names[i]))))
except Exception:
    import traceback
    log("FATAL:\n" + traceback.format_exc())
finally:
    open(OUT, "w", encoding="utf-8").write("\n".join(L))
