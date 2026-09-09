"""Build navigable documentation from the manifest; never approves art automatically."""
import json,pathlib,hashlib,html
B=pathlib.Path(__file__).resolve().parents[1]/'Docs/Art/Bellheart'
m=json.loads((B/'reference_manifest.json').read_text(encoding='utf-8'))
refs=m['references']
for r in refs:
 p=B/r['file']
 if p.exists():
  r['sha256']=hashlib.sha256(p.read_bytes()).hexdigest()
  if r['status']=='planned':r['status']='candidate'
  r['provenance']='deterministic diagram' if r['views'] in ['TOPDOWN','GAMEPLAY_FLOW','ELEVATION'] else 'OpenAI image generation'
(B/'reference_manifest.json').write_text(json.dumps(m,indent=2),encoding='utf-8')
index=['# Bellheart reference index','', 'Phase 0: generated concept targets and measured design diagrams. No Unreal production assets are implied.','', 'Style authority: [Style bible](BELLHEART_STYLE_BIBLE.md) · [Materials](MATERIAL_PALETTE.md) · [Scale](SCALE_GUIDE.md) · [Locks](LOCKED_REFERENCES.md) · [Asset mapping](ASSET_REFERENCE_MAP.md)','', '| ID | Reference | Status | Purpose |','|---|---|---|---|']
for r in refs:index.append(f"| {r['id']} | [{r['name']}]({r['file']}) | {r['status']} | {r['purpose']} |")
(B/'REFERENCE_INDEX.md').write_text('\n'.join(index),encoding='utf-8')
assets={}
for r in refs:
 for asset in r['used_by']:assets.setdefault(asset,[]).append(r)
lines=['# Asset to reference mapping','','These are **planned production destinations**, not claims of existing Blender or Unreal assets. Read all linked references before authoring. Existing browser meshes are not approved Phase 0 assets.','','| Planned asset | Reference IDs | Planned Blender source | Planned export | Planned Unreal asset |','|---|---|---|---|---|']
for a,rs in assets.items():
 category=rs[0]['category'];source=f'Tools/Blender/Source/{a}.blend';export=f'Content/Cloudwake/Art/{category}/Source/{a}.fbx';unreal=f'/Game/Cloudwake/Art/{category}/{a}'
 if not a.startswith(('SM_','SK_')):source='Not a mesh';export='Not applicable';unreal=f'/Game/Cloudwake/{category}/{a}'
 lines.append(f"| {a} | "+', '.join(f"[{r['id']}]({r['file']})" for r in rs)+f' | {source} | {export} | {unreal} |')
(B/'ASSET_REFERENCE_MAP.md').write_text('\n'.join(lines),encoding='utf-8')
cards=[]
for r in refs:
 p=B/r['file'];cards.append(f'<article data-category="{r["category"]}"><a href="{r["file"]}">{f"<img loading=lazy src={r[chr(102)+chr(105)+chr(108)+chr(101)]}>" if p.exists() else "<div class=missing>Planned · not generated</div>"}</a><small>{r["id"]} · {r["status"]}</small><h2>{html.escape(r["name"])}</h2><p>{html.escape(r["purpose"])}</p></article>')
doc='''<!doctype html><meta charset=utf-8><title>Cloudwake · Bellheart reference library</title><style>body{margin:0;background:#eae2ce;color:#173b42;font:16px system-ui}header{padding:45px;background:#173b42;color:#e9dfc2}h1{font:48px Georgia;margin:10px 0}header p{max-width:850px}nav{padding:20px 45px;position:sticky;top:0;background:#eae2ce;z-index:2}button{padding:9px 15px;margin:3px;border:1px solid #b58a48;background:#fff8e7;color:#173b42;cursor:pointer}main{padding:20px 45px;display:grid;grid-template-columns:repeat(auto-fit,minmax(340px,1fr));gap:24px}article{background:#fbf7eb;border:1px solid #c7b58f;padding:14px}img{width:100%;height:240px;object-fit:contain;background:#e9e1cd}h2{font:25px Georgia}small{display:block;margin-top:12px}p{line-height:1.5;font-size:14px}.missing{height:240px;display:grid;place-items:center;background:#d5c6a1}a{color:inherit}</style><header><small>CLOUDWAKE / PHASE 0</small><h1>Bellheart Isle</h1><p>Visual development library. Concept images are art targets, not game screenshots. Use the style bible, measured maps, and scale guide together. Only explicitly reviewed masters may guide production.</p><a href="BELLHEART_STYLE_BIBLE.md">Style bible</a> · <a href="REFERENCE_INDEX.md">Reference index</a> · <a href="LOCKED_REFERENCES.md">Review and locks</a></header><nav><button onclick="filter('all')">All</button>'''
for cat in sorted(set(r['category']for r in refs)):doc+=f'<button onclick="filter(\'{cat}\')">{cat}</button>'
doc+='</nav><main>'+''.join(cards)+'</main><script>function filter(c){document.querySelectorAll("article").forEach(e=>e.style.display=c==="all"||e.dataset.category===c?"":"none")}</script>'
(B/'index.html').write_text(doc,encoding='utf-8')
print(sum((B/r['file']).exists()for r in refs),'/',len(refs),'files present; no automatic approval')
