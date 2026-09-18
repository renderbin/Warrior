# -*- coding: utf-8 -*-
"""完整诊断：骨骼层级 + 每条动画的根运动状态与位移来源"""
import unreal, os, traceback

SKEL_PATH = "/Game/Mesh/YinHU/SK_HFM_Hu_Wind_01_Skeleton"
ANIM_DIR = "/Game/Mesh/YinHU/Animtions"
OUT = "D:/Warrior/Saved/inspect_full.txt"

L = []
def log(m):
    L.append(str(m)); unreal.log(str(m)); print(m, flush=True)

def dump():
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    open(OUT, "w", encoding="utf-8").write("\n".join(L))

AL = unreal.AnimationLibrary
AP = unreal.AnimPoseExtensions

def make_opts():
    return unreal.AnimPoseEvaluationOptions()


def bone_names(seq):
    pose = AP.get_anim_pose_at_frame(seq, 0, make_opts())
    return [str(x) for x in AP.get_bone_names(pose)]

def span(seq, bone, nf):
    """返回 (位移最大变化量, 旋转最大变化角(度), 首帧位移)"""
    if nf <= 0:
        return None
    idx = sorted(set([0, nf // 4, nf // 2, 3 * nf // 4, nf]))
    ts = []
    for f in idx:
        try:
            ts.append(AL.get_bone_pose_for_frame(seq, bone, f, False))
        except Exception as e:
            return None
    t0 = ts[0].translation
    d = max(max(abs(t.translation.x - t0.x), abs(t.translation.y - t0.y),
                abs(t.translation.z - t0.z)) for t in ts)
    q0 = ts[0].rotation
    ad = 0.0
    for t in ts:
        q = t.rotation
        dot = abs(q0.x * q.x + q0.y * q.y + q0.z * q.z + q0.w * q.w)
        dot = min(1.0, max(-1.0, dot))
        import math
        ad = max(ad, math.degrees(2.0 * math.acos(dot)))
    return (d, ad, (t0.x, t0.y, t0.z))

try:
    skel = unreal.load_asset(SKEL_PATH)
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    seqs = [a.get_asset() for a in ar.get_assets_by_path(ANIM_DIR, recursive=False)
            if str(a.asset_class_path.asset_name) == "AnimSequence" and a.get_asset()]
    seqs.sort(key=lambda o: o.get_name())

    sample = seqs[0]
    names = bone_names(sample)
    nf0 = AL.get_num_frames(sample)

    log("=" * 100)
    log("SKELETON: %s" % skel.get_path_name())
    log("bone count = %d      sample anim = %s (%d frames)" % (len(names), sample.get_name(), nf0))
    log("=" * 100)
    log("\n--- 骨骼层级（index 0 就是 UE 认定的 skeleton root）---")
    for i, b in enumerate(names[:30]):
        try:
            path = [str(p) for p in AL.find_bone_path_to_root(sample, b)]
            path = " < ".join(path)
        except Exception as e:
            path = "(path err: %s)" % e
        log("  [%3d] %-32s  %s" % (i, b, path))

    log("\n--- 顶层 6 根骨骼在每条动画里的位移/旋转变化量（度）---")
    log("  %-46s %5s %5s %4s  %-22s %-22s" % ("anim", "rm", "lock", "frcd", names[0][:20], names[1][:20] if len(names) > 1 else ""))
    log("  " + "-" * 108)
    nb = min(3, len(names))
    for s in seqs:
        nm = s.get_name()
        try:
            nf = AL.get_num_frames(s)
            rm = AL.is_root_motion_enabled(s)
            lk = str(AL.get_root_motion_lock_type(s)).replace("RootMotionRootLock.", "")
            fc = AL.is_root_motion_lock_forced(s)
        except Exception as e:
            log("  %-46s ERR %s" % (nm, e)); continue
        cells = []
        for i in range(nb):
            r = span(s, names[i], nf)
            if r is None:
                cells.append("n/a")
            else:
                cells.append("t=%.2f r=%.0f" % (r[0], r[1]))
        log("  %-46s %5s %5s %4s  %-22s %-22s %-22s" % (nm, rm, lk, fc, cells[0], cells[1] if nb > 1 else "", cells[2] if nb > 2 else ""))

except Exception:
    log("FATAL:\n" + traceback.format_exc())
finally:
    dump()
