"""Original temporary synthesized bell cue; replace during audio production."""
import math, struct, wave
from pathlib import Path
out=Path(__file__).resolve().parents[1]/'art'/'greybox'/'S_Bell.wav'
out.parent.mkdir(parents=True, exist_ok=True)
rate=22050
with wave.open(str(out),'wb') as w:
    w.setnchannels(1); w.setsampwidth(2); w.setframerate(rate)
    for i in range(rate*4):
        t=i/rate
        signal=sum(a*math.sin(2*math.pi*220*f*t)*math.exp(-t*d) for f,a,d in [(1,.5,1),(2.71,.2,1.4),(4.1,.1,2)])
        w.writeframesraw(struct.pack('<h',int(signal*28000*min(t*100,1))))
print(out)
