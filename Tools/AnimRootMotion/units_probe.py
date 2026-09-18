# -*- coding: utf-8 -*-
"""确认：root 骨骼的参考姿势/缩放，以及几条关键动画的原始根位移数值"""
import unreal, os, traceback

ANIM_DIR = "/Game/Mesh/YinHU/Animtions"
OUT = "D:/Warrior/Saved/units_probe.txt"
L = []
def log(m):
    L.append(str(m)); unreal.log(str(m)); print(m, flush=True)
def dump():
    open(OUT, "w", encoding="utf-8").write("\n".join(L))

AL = unreal.AnimationLibrary
AP = unreal.AnimPoseExtensions

try:
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    seqs = {}
    for a in ar.get_assets_by_path(ANIM_DIR, recursive=False):
        if str(a.asset_class_path.asset_name) == "AnimSequence":
            o = a.get_asset()
            if o:
                seqs[o.get_name()] = o

    any_seq = sorted(seqs.values(), key=lambda o: o.get_name())[0]

    # --- 参考姿势 ---
    log("=== 参考姿势 (reference pose) ===")
    try:
        rp = AP.get_reference_pose(any_seq)
        for bn in ["root", "ref", "pelvis", "Bip", "spine_01", "Head_M"]:
            try:
                t = AP.get_bone_pose(rp, bn, True)
                tr, sc = t.translation, t.scale3d
                log("  %-12s  T=(%9.3f,%9.3f,%9.3f)  S=(%.4f,%.4f,%.4f)" % (
                    bn, tr.x, tr.y, tr.z, sc.x, sc.y, sc.z))
            except Exception as e:
                log("  %-12s  ERR %s" % (bn, e))
    except Exception:
        log("  get_reference_pose failed:\n" + traceback.format_exc())

    # --- 关键动画的原始 root 局部位移 ---
    log("\n=== root 骨骼的原始局部位移 (每帧采样) ===")
    for nm in ["AS_HFM_Hu_Wind_01_lock_walk_loop_fwd", "AS_HFM_Hu_Wind_01_free_run_loop_fwd",
               "AS_HFM_Hu_Wind_01_Turn_L_01", "AS_HFM_Hu_Wind_01_Jump_B",
               "AS_player_hfm_hu_rush_loop_fwd", "AS_HFM_Hu_Wind_01_idle_01"]:
        s = seqs.get(nm)
        if not s:
            log("  %s : NOT FOUND" % nm); continue
        nf = AL.get_num_frames(s)
        ln = AL.get_sequence_length(s)
        log("\n  %s   frames=%d  length=%.3fs  fps=%.1f" % (nm, nf, ln, (nf - 1) / ln if ln else 0))
        idx = sorted(set([0, nf // 4, nf // 2, 3 * nf // 4, max(0, nf - 1)]))
        for f in idx:
            t = AL.get_bone_pose_for_frame(s, "root", f, False)
            tr = t.translation
            log("     f%-5d T=(%9.3f,%9.3f,%9.3f)" % (f, tr.x, tr.y, tr.z))

except Exception:
    log("FATAL:\n" + traceback.format_exc())
finally:
    dump()
