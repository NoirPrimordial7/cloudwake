"""Read only product-library metadata; never read authentication stores."""
import sqlite3,json
db=sqlite3.connect('file:C:/ProgramData/Epic/EpicGamesLauncher/VaultCache/FabLibrary/listings_v1.db?mode=ro',uri=True)
for name,sql in db.execute("SELECT name,sql FROM sqlite_master WHERE type='table'"):
 print(json.dumps({'table':name,'schema':sql,'count':db.execute('SELECT COUNT(*) FROM "'+name.replace('"','""')+'"').fetchone()[0]}))
