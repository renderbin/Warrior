# -*- coding: utf-8 -*-
"""
诊断：YinHU 动画的根骨骼 / 根位移情况
在 UE 编辑器里运行： Tools -> Execute Python Script...  选择本文件
输出： Output Log + D:/Warrior/Saved/AnimRootMotionInspect.txt
"""
import unreal
import os

SKEL_PATH = "/Game/Mesh/YinHU/SK_HFM_Hu_Wind_01_Skeleton"
MESH_PATH = "/Game/Mesh/YinHU/SK_HFM_Hu_Wind_01"
ANIM_DIR = "/Game/Mesh/YinHU/Animtions"

OUT_PATH = "D:/Warrior/Saved/AnimRootMotionInspect.txt"
_lines = []


def log(msg):
    _lines.append(str(msg))
    unreal.log(str(msg))


def api(name):
    """在几个候选模块里找函数，返回第一个存在的"""
    for mod in ("AnimationLibrary", "AnimationBlueprintLibrary"):
        m = getattr(unreal, mod, None)
        if m is not None and hasattr(m, name):
            return getattr(m, name)
    return None


def bone_names_of_skeleton(skel):
    for fn_name in ("get_bone_names",):
        fn = api(fn_name)
        if fn:
            try:
                return list(fn(skel)), "%s(skeleton)" % fn_name
            except Exception as e:
                pass
    # 退路：用 AnimPose 拿
    try:
        pose_ext = unreal.AnimPoseExtensions
        for seq in list_anim_sequences():
            pose = pose_ext.get_anim_pose(seq, 0.0)
            return list(pose_ext.get_bone_names(pose)), "AnimPoseExtensions"
    except Exception as e:
        pass
    return None, None


def list_anim_sequences():
    ar = unreal.AssetRegistryHelpers.get_asset_registry()
    assets = ar.get_assets_by_path(ANIM_DIR, recursive=False)
    out = []
    for a in assets:
        if str(a.asset_class_path.asset_name) in ("AnimSequence",):
            obj = a.get_asset()
            if obj:
                out.append(obj)
    return sorted(out, key=lambda o: o.get_name())


def main():
    log("=" * 78)
    log("YinHU root-motion inspection")
    log("=" * 78)

    # ---------- 1. 骨骼层级 ----------
    skel = unreal.load_asset(SKEL_PATH)
    mesh = unreal.load_asset(MESH_PATH)
    log("\n[1] SKELETON")
    log("  skeleton  = %s" % (skel.get_path_name() if skel else "LOAD FAILED"))
    log("  previewmesh = %s" % (mesh.get_path_name() if mesh else "LOAD FAILED"))

    names, how = bone_names_of_skeleton(skel)
    if names:
        log("  bone count = %d   (via %s)" % (len(names), how))
        log("  --- 最顶层 20 根骨骼（UE 顺序 = 父在子前，index 0 就是 skeleton root）---")
        for i, n in enumerate(names[:20]):
            log("    [%3d] %s" % (i, n))
    else:
        log("  !! 拿不到骨骼列表，尝试列出可用 API")
        for mod in ("AnimationLibrary", "AnimationBlueprintLibrary", "AnimPoseExtensions", "SkeletonLibrary"):
            m = getattr(unreal, mod, None)
            if m is not None:
                log("    unreal.%s -> %s" % (mod, [x for x in dir(m) if "one" in x or "keleton" in x or "ose" in x][:25]))

    # ---------- 2. 逐条动画 ----------
    seqs = list_anim_sequences()
    log("\n[2] ANIM SEQUENCES (%d found in %s)" % (len(seqs), ANIM_DIR))

    is_rm = api("is_root_motion_enabled")
    get_lock = api("get_root_motion_lock_type")
    is_forced = api("is_root_motion_lock_forced")
    get_frames = api("get_num_frames")

    if names:
        b0 = names[0]
        b1 = names[1] if len(names) > 1 else None
    else:
        b0 = b1 = None

    get_pose = api("get_bone_pose_for_frame")

    log("  %-46s %6s %5s %5s %6s  %s" % ("name", "rm", "lock", "frcd", "frames", "bone0/bone1 位移变化"))
    for s in seqs:
        nm = s.get_name()
        try:
            rm = bool(is_rm(s)) if is_rm else "?"
            lk = str(get_lock(s)) if get_lock else "?"
            fc = bool(is_forced(s)) if is_forced else "?"
            fr = int(get_frames(s)) if get_frames else -1
        except Exception as e:
            log("  %-46s  ERR %s" % (nm, e))
            continue

        # 骨骼 0 / 1 的局部位移在整个动画里是否变化
        info = ""
        if get_pose and b0 and fr > 0:
            try:
                def span(bone):
                    vals = []
                    for f in (0, fr // 2, fr):
                        t = get_pose(s, bone, f, False)
                        vals.append((t.translation.x, t.translation.y, t.translation.z))
                    d = max(max(abs(v[i] - vals[0][i]) for i in range(3)) for v in vals)
                    return d, vals[0]
                d0, t0 = span(b0)
                s0 = "b0=%.3f" % d0
                if b1:
                    d1, t1 = span(b1)
                    s1 = "b1=%.3f" % d1
                else:
                    s1 = ""
                info = "%s %s  t0=(%.2f,%.2f,%.2f)" % (s0, s1, t0[0], t0[1], t0[2])
            except Exception as e:
                info = "pose ERR %s" % e

        log("  %-46s %6s %5s %5s %6d  %s" % (nm, rm, lk, fc, fr, info))

    os.makedirs(os.path.dirname(OUT_PATH), exist_ok=True)
    with open(OUT_PATH, "w", encoding="utf-8") as f:
        f.write("\n".join(_lines))
    unreal.log("\n>>> 报告已写入: %s" % OUT_PATH)


main()
